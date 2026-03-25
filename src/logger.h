#pragma once

#include <QString>
#include "core/logger.h"   // NetCfgTool::FileLogger, LogLevel

/**
 * @brief Фасад-одиночка для логгера.
 *
 * Использование:
 *   Logger::info("MainWindow", "something happened");   // компонент + сообщение
 *   Logger::info("Just a message");                     // компонент по умолчанию "App"
 */
class Logger
{
public:
    // ---- настройка уровня ----
    static void setMinimumLevel(NetCfgTool::LogLevel level);

    // ---- ДВУХАРГУМЕНТНЫЕ ВЕРСИИ ----
    static void debug(const QString& component, const QString& message);
    static void info (const QString& component, const QString& message);
    static void warn (const QString& component, const QString& message);
    static void error(const QString& component, const QString& message);

    // ---- ОДНОАРГУМЕНТНЫЕ ВЕРСИИ ----
    static void debug(const QString& message);
    static void info (const QString& message);
    static void warn (const QString& message);
    static void error(const QString& message);

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& instance();

private:
    NetCfgTool::FileLogger m_backend;
};
