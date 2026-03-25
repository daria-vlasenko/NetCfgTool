#include "network_manager.h"
#include <QProcess>

static bool run(const QStringList& args) {
    QProcess p;
    p.start("nmcli", args);
    p.waitForFinished();
    return p.exitCode() == 0;
}

bool NetworkManager::setDhcp(const QString& dev) {
    // Пример: nmcli con mod "Cudy-84AC-5G" ipv4.method auto
    return run({"con", "mod", dev, "ipv4.method", "auto"}) &&
           run({"con", "up", dev});
}

bool NetworkManager::setStatic(const QString& dev,
                               const QString& ipCidr,
                               const QString& gw,
                               const QStringList& dnsList) {
    QString dns = dnsList.join(",");
    return run({"con", "mod", dev, "ipv4.method", "manual"}) &&
           run({"con", "mod", dev, "ipv4.addresses", ipCidr}) &&
           run({"con", "mod", dev, "ipv4.gateway", gw}) &&
           run({"con", "mod", dev, "ipv4.dns", dns}) &&
           run({"con", "up", dev});
}
