#pragma once

#include <QString>
#include <QStringList>

/**
 * @brief Класс-валидатор для IPv4 / CIDR / шлюза / DNS.
 *
 * Все методы статические и не используют QtNetwork,
 * только строковую проверку.
 */
class Validator
{
public:
    /**
     * @brief Проверка корректности IPv4-адреса, например "192.168.1.1".
     */
    static bool isValidIpv4(const QString &ip);

    /**
     * @brief Проверка корректности CIDR, например "192.168.1.10/24".
     */
    static bool isValidCidr(const QString &cidr);

    /**
     * @brief Проверка корректности шлюза (обычный IPv4).
     */
    static bool isValidGateway(const QString &gateway);

    /**
     * @brief Проверка списка DNS-адресов.
     *
     * Требования (можешь адаптировать под свои тесты):
     *  - список не пустой;
     *  - не больше 5 адресов;
     *  - каждый элемент непустой и является корректным IPv4.
     */
    static bool isValidDnsList(const QStringList &dnsList);
};
