#pragma once

#include <QString>
#include <QStringList>

/**
 * @brief Структура профиля сетевых настроек.
 *
 * name    - имя профиля (уникальное, без пробелов)
 * mode    - "DHCP" или "STATIC"
 * ipCidr  - "192.168.10.50/24"
 * gateway - "192.168.10.1"
 * dns     - список DNS-адресов ("1.1.1.1", "8.8.8.8", ...)
 */
struct NetProfile
{
    QString     name;
    QString     mode;    // "DHCP" / "STATIC"
    QString     ipCidr;
    QString     gateway;
    QStringList dns;
};

/**
 * @brief Набор вспомогательных функций для работы с профилями.
 *
 * Хранение:
 *   ~/.config/NetCfgTool/profiles.json
 */
namespace Profiles {

/**
 * @brief Сохранить или обновить профиль по имени.
 * @return true при успехе, false при ошибке (текст в *error).
 */
bool save(const NetProfile &profile, QString *error = nullptr);

/**
 * @brief Загрузить профиль по имени.
 * @return true при успехе, false при ошибке (или если не найден).
 */
bool load(const QString &name, NetProfile &outProfile, QString *error = nullptr);

/**
 * @brief Удалить профиль по имени.
 * @return true при успехе, false при ошибке.
 */
bool remove(const QString &name, QString *error = nullptr);

/**
 * @brief Получить список имён всех профилей.
 */
QStringList list(QString *error = nullptr);

} // namespace Profiles
