#pragma once

#include <QString>

namespace NetCfgTool::Paths {

/**
 * @brief Полный путь к profiles.json
 * ~/.config/NetCfgTool/profiles.json
 */
QString profilesFile();

/**
 * @brief Папка для логов:
 * ~/.local/share/NetCfgTool/logs
 */
QString logsDir();

/**
 * @brief Путь к app.log
 * ~/.local/share/NetCfgTool/logs/app.log
 */
QString logFile();

/**
 * @brief Создать нужные директории:
 *   ~/.config/NetCfgTool
 *   ~/.local/share/NetCfgTool/logs
 */
void ensureAppDirectories();

} // namespace NetCfgTool::Paths
