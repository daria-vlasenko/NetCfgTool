#pragma once

#include <QString>
#include <QFile>
#include <QMutex>

namespace NetCfgTool {

/**
 * @brief Уровни логирования по ТЗ.
 */
enum class LogLevel {
    Debug = 0,
    Info,
    Warn,
    Error
};

/**
 * @brief Интерфейс логгера.
 */
class ILogger
{
public:
    virtual ~ILogger() = default;

    /**
     * @brief Записать сообщение в лог.
     * @param level     Уровень (DEBUG/INFO/WARN/ERROR).
     * @param component Компонент (например, "NmcliService").
     * @param message   Текст сообщения.
     */
    virtual void log(LogLevel level,
                     const QString &component,
                     const QString &message) = 0;

    /**
     * @brief Установить минимальный уровень логирования.
     *        Сообщения ниже этого уровня игнорируются.
     */
    virtual void setMinimumLevel(LogLevel level) = 0;

    /**
     * @brief Текущий минимальный уровень логирования.
     */
    virtual LogLevel minimumLevel() const = 0;
};

/**
 * @brief Реализация ILogger, пишущая в файл с ротацией.
 *
 * Лог-файл: ~/.local/share/NetCfgTool/logs/app.log
 * Формат строки:
 *   YYYY-MM-DDTHH:MM:SSZ [LEVEL] [Component] message
 * Ротация:
 *   > 5 МБ -> app.log -> app.log.1 -> app.log.2 -> app.log.3
 */
class FileLogger : public ILogger
{
public:
    FileLogger();
    ~FileLogger() override;

    void log(LogLevel level,
             const QString &component,
             const QString &message) override;

    void setMinimumLevel(LogLevel level) override;
    LogLevel minimumLevel() const override;

private:
    void openLogFile();
    void rotateIfNeeded();

private:
    QFile m_file;
    mutable QMutex m_mutex;
    LogLevel m_minLevel{LogLevel::Info};
};

} // namespace NetCfgTool
