#pragma once
#include <QMainWindow>

class QListWidget;
class QTextEdit;
class QRadioButton;
class QLineEdit;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    // UI
    QListWidget*  overviewList = nullptr;
    QTextEdit*    detailsText  = nullptr;

    QRadioButton* dhcpRadio    = nullptr;
    QRadioButton* staticRadio  = nullptr;

    QLineEdit*    ipCidrEdit   = nullptr;
    QLineEdit*    gwEdit       = nullptr;
    QLineEdit*    dnsEdit      = nullptr;

    QPushButton*  applyDhcpBtn   = nullptr;
    QPushButton*  applyStaticBtn = nullptr;
    QPushButton*  enableBtn      = nullptr;
    QPushButton*  disableBtn     = nullptr;
    QPushButton*  restartBtn     = nullptr;

    QPushButton*  saveProfileBtn = nullptr;
    QPushButton*  loadProfileBtn = nullptr;
    QPushButton*  delProfileBtn  = nullptr;

    // состояние
    QString currentDevice;

private:
    void buildUi();
    void refreshOverview();
    void refreshDetails(const QString& dev);

    // actions
    void onEnableInterface();
    void onDisableInterface();
    void onRestartInterface();

    void onSaveProfile();
    void onLoadProfile();
    void onDeleteProfile();

    static QString currentConnectionId(const QString& dev);
};
