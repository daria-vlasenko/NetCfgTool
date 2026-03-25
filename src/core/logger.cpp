#include "logger.h"
#include "paths.h"

#include <QDateTime>
#include <QTextStream>

namespace NetCfgTool {

namespace {

// 5 МБ = 5 * 1024 * 1024 байт
constexpr qint64 MAX_LOG_SIZE_BYTES = 5 * 1024 * 1024;
constexpr int MAX_LOG_ROTATIONS = 3;

QString levelToString(LogLevel level)
{
    switch (level) {
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info:  return "INFO";
    case LogLevel::Warn:  return "WARN";
    case LogLevel::Error: return "ERROR";
    }
    return "INFO";
}

QString formatTimestampUtc()
{
    // ТЗ: YYYY-MM-DDTHH:MM:SSZ (UTC)
    const auto nowUtc = QDateTime::currentDateTimeUtc();
    QString iso = nowUtc.toString(Qt::ISODate); // обычно уже содержит Z
    if (!iso.endsWith('Z')) {
        iso.append('Z');
    }
    return iso;
}

} // namespace

FileLogger::FileLogger()
{
    openLogFile();
}

FileLogger::~FileLogger()
{
    if (m_file.isOpen()) {
        m_file.flush();
        m_file.close();
    }
}

void FileLogger::openLogFile()
{
    const QString path = NetCfgTool::Paths::logFile();
    m_file.setFileName(path);
    if (!m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        // можно вывести qWarning() при желании
    }
}

void FileLogger::rotateIfNeeded()
{
    if (!m_file.isOpen()) {
        return;
    }

    if (m_file.size() <= MAX_LOG_SIZE_BYTES) {
        return;
    }

    m_file.flush();
    m_file.close();

    const QString baseLogPath = NetCfgTool::Paths::logFile();

    // Удаляем самый старый: app.log.3
    const QString oldest = baseLogPath + ".3";
    if (QFile::exists(oldest)) {
        QFile::remove(oldest);
    }

    // Сдвигаем .2 -> .3, .1 -> .2, основной -> .1
    for (int i = MAX_LOG_ROTATIONS - 1; i >= 1; --i) {
        const QString src = QString("%1.%2").arg(baseLogPath).arg(i);
        const QString dst = QString("%1.%2").arg(baseLogPath).arg(i + 1);
        if (QFile::exists(src)) {
            QFile::rename(src, dst);
        }
    }

    // Основной лог -> app.log.1
    if (QFile::exists(baseLogPath)) {
        QFile::rename(baseLogPath, baseLogPath + ".1");
    }

    // Открываем новый app.log
    openLogFile();
}

void FileLogger::log(LogLevel level,
                     const QString &component,
                     const QString &message)
{
    QMutexLocker locker(&m_mutex);

    if (static_cast<int>(level) < static_cast<int>(m_minLevel)) {
        return;
    }

    if (!m_file.isOpen()) {
        openLogFile();
        if (!m_file.isOpen()) {
            return;
        }
    }

    rotateIfNeeded();

    QTextStream out(&m_file);

    const QString line =
        QString("%1 [%2] [%3] %4\n")
            .arg(formatTimestampUtc(),
                 levelToString(level),
                 component,
                 message);

    out << line;
    out.flush();
}

void FileLogger::setMinimumLevel(LogLevel level)
{
    QMutexLocker locker(&m_mutex);
    m_minLevel = level;
}

LogLevel FileLogger::minimumLevel() const
{
    QMutexLocker locker(&m_mutex);
    return m_minLevel;
}

} // namespace NetCfgTool
