#include "validator.h"

#include <QStringList>

namespace {

/**
 * @brief Проверка IPv4 "a.b.c.d" с числами 0..255.
 */
bool checkIpv4(const QString &ip)
{
    const QString trimmed = ip.trimmed();
    if (trimmed.isEmpty())
        return false;

    const QStringList parts = trimmed.split('.', Qt::SkipEmptyParts);
    if (parts.size() != 4)
        return false;

    for (const QString &part : parts) {
        bool ok = false;
        // запрещаем пробелы, +, - и т.п.
        if (part.isEmpty())
            return false;

        int value = part.toInt(&ok);
        if (!ok)
            return false;
        if (value < 0 || value > 255)
            return false;
    }

    return true;
}

} // namespace

bool Validator::isValidIpv4(const QString &ip)
{
    return checkIpv4(ip);
}

bool Validator::isValidCidr(const QString &cidr)
{
    const QString trimmed = cidr.trimmed();
    if (trimmed.isEmpty())
        return false;

    const int slashPos = trimmed.indexOf('/');
    if (slashPos <= 0 || slashPos == trimmed.size() - 1)
        return false;

    const QString ipPart   = trimmed.left(slashPos);
    const QString maskPart = trimmed.mid(slashPos + 1);

    if (!checkIpv4(ipPart))
        return false;

    bool ok = false;
    int prefix = maskPart.toInt(&ok);
    if (!ok)
        return false;

    // допустимые значения маски: 0..32 (чаще 1..32; возьмём 0..32)
    if (prefix < 0 || prefix > 32)
        return false;

    return true;
}

bool Validator::isValidGateway(const QString &gateway)
{
    // по сути это тот же IPv4, можно добавить доп. ограничения при желании
    return checkIpv4(gateway);
}

bool Validator::isValidDnsList(const QStringList &dnsList)
{
    if (dnsList.isEmpty())
        return false;

    // по ТЗ 1..5 DNS
    if (dnsList.size() > 5)
        return false;

    for (const QString &dns : dnsList) {
        if (dns.trimmed().isEmpty())
            return false;
        if (!checkIpv4(dns))
            return false;
    }

    return true;
}
