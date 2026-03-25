#include "logger.h"

using NetCfgTool::LogLevel;

namespace {
    const QString DEFAULT_COMPONENT = QStringLiteral("App");
}

Logger::Logger()
{
    m_backend.setMinimumLevel(LogLevel::Info);
}

Logger& Logger::instance()
{
    static Logger inst;
    return inst;
}

void Logger::setMinimumLevel(LogLevel level)
{
    instance().m_backend.setMinimumLevel(level);
}

// ---- две строки ----

void Logger::debug(const QString& component, const QString& message)
{
    instance().m_backend.log(LogLevel::Debug, component, message);
}

void Logger::info(const QString& component, const QString& message)
{
    instance().m_backend.log(LogLevel::Info, component, message);
}

void Logger::warn(const QString& component, const QString& message)
{
    instance().m_backend.log(LogLevel::Warn, component, message);
}

void Logger::error(const QString& component, const QString& message)
{
    instance().m_backend.log(LogLevel::Error, component, message);
}

// ---- одна строка ----

void Logger::debug(const QString& message)
{
    debug(DEFAULT_COMPONENT, message);
}

void Logger::info(const QString& message)
{
    info(DEFAULT_COMPONENT, message);
}

void Logger::warn(const QString& message)
{
    warn(DEFAULT_COMPONENT, message);
}

void Logger::error(const QString& message)
{
    error(DEFAULT_COMPONENT, message);
}
