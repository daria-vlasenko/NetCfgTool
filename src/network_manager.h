#pragma once
#include <QString>
#include <QStringList>

class NetworkManager {
public:
    static bool setDhcp(const QString& dev);
    static bool setStatic(const QString& dev,
                          const QString& ipCidr,
                          const QString& gw,
                          const QStringList& dnsList);
};
