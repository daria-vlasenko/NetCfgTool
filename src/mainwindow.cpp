#include "mainwindow.h"
#include "nmcli_service.h"
#include "network_manager.h"
#include "validator.h"
#include "profiles.h"
#include "logger.h"

#include <QListWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QWidget>
#include <QProcess>
#include <QMessageBox>
#include <QInputDialog>
#include <QStringList>

static NmcliService gSvc;

// Получение ID активного подключения для устройства (или пустая строка)
QString MainWindow::currentConnectionId(const QString &dev)
{
    QProcess p;
    p.start("nmcli", {"-t", "-f", "GENERAL.CONNECTION", "device", "show", dev});
    p.waitForFinished();

    QString out = QString::fromUtf8(p.readAllStandardOutput()).trimmed();
    if (out.startsWith("GENERAL.CONNECTION:"))
        out = out.section(':', 1);

    if (out == "--")
        out.clear();

    return out.trimmed();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    buildUi();
    resize(1050, 720);
    setWindowTitle("NetCfgTool");
    statusBar()->showMessage("Ready");

    Logger::info("MainWindow", "App started");

    refreshOverview();
    if (overviewList->count() > 0) {
        overviewList->setCurrentRow(0);
        currentDevice = overviewList->currentItem()->text().section(' ', 0, 0);
        refreshDetails(currentDevice);
    }

    // Переключение выделенного интерфейса в списке
    connect(overviewList, &QListWidget::currentRowChanged, this, [this](int) {
        if (!overviewList->currentItem())
            return;
        currentDevice = overviewList->currentItem()->text().section(' ', 0, 0);
        refreshDetails(currentDevice);
    });

    // Применение DHCP
    connect(applyDhcpBtn, &QPushButton::clicked, this, [this] {
        if (currentDevice.isEmpty())
            return;

        const QString connId = currentConnectionId(currentDevice);
        const QString target = connId.isEmpty() ? currentDevice : connId;

        Logger::info("MainWindow", "Apply DHCP to: " + target);

        if (NetworkManager::setDhcp(target)) {
            statusBar()->showMessage("DHCP applied", 3000);
            refreshOverview();
            refreshDetails(currentDevice);
        } else {
            Logger::warn("MainWindow", "DHCP failed for: " + target);
            QMessageBox::warning(this, "Error", "DHCP failed for: " + target);
        }
    });

    // Применение статической конфигурации
    connect(applyStaticBtn, &QPushButton::clicked, this, [this] {
        if (currentDevice.isEmpty())
            return;

        const QString ipCidr  = ipCidrEdit->text().trimmed();
        const QString gw      = gwEdit->text().trimmed();
        const QStringList dns = dnsEdit->text().split(',', Qt::SkipEmptyParts);

        if (!Validator::isValidCidr(ipCidr)) {
            QMessageBox::warning(this, "Validation", "Invalid IP/CIDR");
            return;
        }
        if (!Validator::isValidGateway(gw)) {
            QMessageBox::warning(this, "Validation", "Invalid Gateway");
            return;
        }
        if (!Validator::isValidDnsList(dns)) {
            QMessageBox::warning(this, "Validation", "Invalid DNS list");
            return;
        }

        const QString connId = currentConnectionId(currentDevice);
        const QString target = connId.isEmpty() ? currentDevice : connId;

        Logger::info(
            "MainWindow",
            "Apply STATIC to: " + target + " ip=" + ipCidr + " gw=" + gw + " dns=" + dns.join(",")
        );

        if (NetworkManager::setStatic(target, ipCidr, gw, dns)) {
            statusBar()->showMessage("Static applied", 3000);
            refreshOverview();
            refreshDetails(currentDevice);
        } else {
            Logger::warn("MainWindow", "Static failed for: " + target);
            QMessageBox::warning(this, "Error", "Static config failed for: " + target);
        }
    });
}

void MainWindow::buildUi()
{
    overviewList = new QListWidget(this);
    detailsText  = new QTextEdit(this);
    detailsText->setReadOnly(true);

    auto rightGrp = new QGroupBox("Details", this);
    auto rightLay = new QVBoxLayout(rightGrp);
    rightLay->addWidget(detailsText);

    auto split = new QSplitter(this);
    split->addWidget(overviewList);
    split->addWidget(rightGrp);
    split->setStretchFactor(0, 1);
    split->setStretchFactor(1, 2);

    // --- IPv4 вкладка ---
    auto ipv4Tab = new QWidget(this);
    auto ipv4Lay = new QVBoxLayout(ipv4Tab);

    auto rRow = new QHBoxLayout();
    dhcpRadio   = new QRadioButton("DHCP");
    staticRadio = new QRadioButton("Static");
    dhcpRadio->setChecked(true);
    rRow->addWidget(dhcpRadio);
    rRow->addWidget(staticRadio);
    rRow->addStretch();

    auto ipRow = new QHBoxLayout();
    ipRow->addWidget(new QLabel("IP/CIDR:"));
    ipCidrEdit = new QLineEdit();
    ipCidrEdit->setPlaceholderText("192.168.10.105/24");
    ipRow->addWidget(ipCidrEdit);

    auto gwRow = new QHBoxLayout();
    gwRow->addWidget(new QLabel("Gateway:"));
    gwEdit = new QLineEdit();
    gwEdit->setPlaceholderText("192.168.10.1");
    gwRow->addWidget(gwEdit);

    auto dnsRow = new QHBoxLayout();
    dnsRow->addWidget(new QLabel("DNS:"));
    dnsEdit = new QLineEdit();
    dnsEdit->setPlaceholderText("1.1.1.1,8.8.8.8");
    dnsRow->addWidget(dnsEdit);

    auto btnRowTop = new QHBoxLayout();
    applyDhcpBtn   = new QPushButton("Apply DHCP");
    applyStaticBtn = new QPushButton("Apply Static");
    btnRowTop->addWidget(applyDhcpBtn);
    btnRowTop->addWidget(applyStaticBtn);
    btnRowTop->addStretch();

    auto btnRowBottom = new QHBoxLayout();
    enableBtn      = new QPushButton("Enable Interface");
    disableBtn     = new QPushButton("Disable Interface");
    restartBtn     = new QPushButton("Restart Interface");
    saveProfileBtn = new QPushButton("Save Profile");
    loadProfileBtn = new QPushButton("Load Profile");
    delProfileBtn  = new QPushButton("Delete Profile");

    btnRowBottom->addWidget(enableBtn);
    btnRowBottom->addWidget(disableBtn);
    btnRowBottom->addWidget(restartBtn);
    btnRowBottom->addSpacing(20);
    btnRowBottom->addWidget(saveProfileBtn);
    btnRowBottom->addWidget(loadProfileBtn);
    btnRowBottom->addWidget(delProfileBtn);
    btnRowBottom->addStretch();

    connect(enableBtn,  &QPushButton::clicked, this, &MainWindow::onEnableInterface);
    connect(disableBtn, &QPushButton::clicked, this, &MainWindow::onDisableInterface);
    connect(restartBtn, &QPushButton::clicked, this, &MainWindow::onRestartInterface);

    connect(saveProfileBtn, &QPushButton::clicked, this, &MainWindow::onSaveProfile);
    connect(loadProfileBtn, &QPushButton::clicked, this, &MainWindow::onLoadProfile);
    connect(delProfileBtn,  &QPushButton::clicked, this, &MainWindow::onDeleteProfile);

    ipv4Lay->addLayout(rRow);
    ipv4Lay->addLayout(ipRow);
    ipv4Lay->addLayout(gwRow);
    ipv4Lay->addLayout(dnsRow);
    ipv4Lay->addLayout(btnRowTop);
    ipv4Lay->addLayout(btnRowBottom);

    auto tabs = new QTabWidget(this);
    tabs->addTab(ipv4Tab, "IPv4 Config");

    auto central = new QWidget(this);
    auto root    = new QVBoxLayout(central);
    root->addWidget(split, 1);
    root->addWidget(tabs, 0);
    setCentralWidget(central);
}

void MainWindow::refreshOverview()
{
    overviewList->clear();
    for (const auto &it : gSvc.list()) {
        const QString line =
            QString::fromStdString(it.device) + " (" +
            QString::fromStdString(it.type)   + ", " +
            QString::fromStdString(it.state)  + ")";
        overviewList->addItem(line);
    }
}

void MainWindow::refreshDetails(const QString &dev)
{
    const QString raw = QString::fromStdString(gSvc.deviceShow(dev.toStdString()));
    detailsText->setPlainText(raw);
}

// -------------------- Enable / Disable / Restart --------------------

void MainWindow::onEnableInterface()
{
    if (currentDevice.isEmpty())
        return;

    Logger::info("MainWindow", "Enable device: " + currentDevice);

    QProcess p;
    p.start("nmcli", {"device", "connect", currentDevice});
    p.waitForFinished();

    if (p.exitCode() == 0) {
        statusBar()->showMessage(currentDevice + " enabled", 3000);
        refreshOverview();
        refreshDetails(currentDevice);
    } else {
        Logger::warn("MainWindow", "Enable failed: " + currentDevice);
        QMessageBox::warning(this, "Error", QString::fromUtf8(p.readAllStandardError()));
    }
}

void MainWindow::onDisableInterface()
{
    if (currentDevice.isEmpty())
        return;

    Logger::info("MainWindow", "Disable device: " + currentDevice);

    QProcess p;
    p.start("nmcli", {"device", "disconnect", currentDevice});
    p.waitForFinished();

    if (p.exitCode() == 0) {
        statusBar()->showMessage(currentDevice + " disabled", 3000);
        refreshOverview();
        refreshDetails(currentDevice);
    } else {
        Logger::warn("MainWindow", "Disable failed: " + currentDevice);
        QMessageBox::warning(this, "Error", QString::fromUtf8(p.readAllStandardError()));
    }
}

void MainWindow::onRestartInterface()
{
    if (currentDevice.isEmpty()) {
        statusBar()->showMessage("No interface selected", 3000);
        return;
    }

    const QString connId = currentConnectionId(currentDevice);
    Logger::info("MainWindow", "Restart requested: dev=" + currentDevice + " connId=" + connId);

    if (!connId.isEmpty()) {
        QProcess p1, p2;
        p1.start("nmcli", {"connection", "down", connId});
        p1.waitForFinished();

        p2.start("nmcli", {"connection", "up", connId});
        p2.waitForFinished();

        if (p1.exitCode() == 0 && p2.exitCode() == 0) {
            statusBar()->showMessage("Restarted connection: " + connId, 3000);
            refreshOverview();
            refreshDetails(currentDevice);
            return;
        }
    }

    QProcess d, c;
    d.start("nmcli", {"device", "disconnect", currentDevice});
    d.waitForFinished();

    c.start("nmcli", {"device", "connect", currentDevice});
    c.waitForFinished();

    if (d.exitCode() == 0 && c.exitCode() == 0) {
        statusBar()->showMessage("Restarted device: " + currentDevice, 3000);
        refreshOverview();
        refreshDetails(currentDevice);
    } else {
        QMessageBox::warning(this, "Restart failed", "See logs for details");
        Logger::warn("MainWindow", "Restart failed for: " + currentDevice);
    }
}

// --------------------------- Profiles -----------------------------

void MainWindow::onSaveProfile()
{
    NetProfile p;
    p.name = QInputDialog::getText(
        this, "Save Profile", "Profile name (latin, no spaces):"
    );
    if (p.name.isEmpty())
        return;

    p.mode    = staticRadio->isChecked() ? "STATIC" : "DHCP";
    p.ipCidr  = ipCidrEdit->text().trimmed();
    p.gateway = gwEdit->text().trimmed();
    p.dns     = dnsEdit->text().split(',', Qt::SkipEmptyParts);

    if (p.mode == "STATIC") {
        if (!Validator::isValidCidr(p.ipCidr) ||
            !Validator::isValidGateway(p.gateway) ||
            !Validator::isValidDnsList(p.dns)) {
            QMessageBox::warning(this, "Validation", "Invalid static params");
            return;
        }
    }

    QString err;
    if (Profiles::save(p, &err)) {
        statusBar()->showMessage("Profile saved: " + p.name, 3000);
        Logger::info("MainWindow", "Profile saved: " + p.name);
    } else {
        Logger::warn("MainWindow", "Profile save failed: " + err);
        QMessageBox::warning(this, "Save failed", err);
    }
}

void MainWindow::onLoadProfile()
{
    const QStringList names = Profiles::list();
    if (names.isEmpty()) {
        QMessageBox::information(this, "Profiles", "No profiles");
        return;
    }

    bool ok = false;
    const QString name = QInputDialog::getItem(
        this, "Load Profile", "Choose:", names, 0, false, &ok
    );
    if (!ok || name.isEmpty())
        return;

    NetProfile p;
    QString err;
    if (!Profiles::load(name, p, &err)) {
        QMessageBox::warning(this, "Load failed", err);
        return;
    }

    if (p.mode == "DHCP") {
        dhcpRadio->setChecked(true);
        applyDhcpBtn->click();
    } else {
        staticRadio->setChecked(true);
        ipCidrEdit->setText(p.ipCidr);
        gwEdit->setText(p.gateway);
        dnsEdit->setText(p.dns.join(','));
        applyStaticBtn->click();
    }

    Logger::info("MainWindow", "Profile loaded/applied: " + name);
}

void MainWindow::onDeleteProfile()
{
    const QStringList names = Profiles::list();
    if (names.isEmpty()) {
        QMessageBox::information(this, "Profiles", "No profiles");
        return;
    }

    bool ok = false;
    const QString name = QInputDialog::getItem(
        this, "Delete Profile", "Choose:", names, 0, false, &ok
    );
    if (!ok || name.isEmpty())
        return;

    QString err;
    if (!Profiles::remove(name, &err)) {
        QMessageBox::warning(this, "Remove failed", err);
        return;
    }

    statusBar()->showMessage("Profile removed: " + name, 3000);
    Logger::info("MainWindow", "Profile removed: " + name);
}
