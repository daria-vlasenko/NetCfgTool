#include "paths.h"
#include <QDir>

namespace NetCfgTool::Paths {

static QString home()
{
    return QDir::homePath();
}

QString profilesFile()
{
    return home() + "/.config/NetCfgTool/profiles.json";
}

QString logsDir()
{
    return home() + "/.local/share/NetCfgTool/logs";
}

QString logFile()
{
    return logsDir() + "/app.log";
}

void ensureAppDirectories()
{
    // ~/.config/NetCfgTool
    {
        QDir configRoot(home() + "/.config");
        if (!configRoot.exists("NetCfgTool"))
            configRoot.mkpath("NetCfgTool");
    }

    // ~/.local/share/NetCfgTool/logs
    {
        QDir shareRoot(home() + "/.local/share");
        if (!shareRoot.exists("NetCfgTool"))
            shareRoot.mkpath("NetCfgTool");

        QDir appRoot(shareRoot.filePath("NetCfgTool"));
        if (!appRoot.exists("logs"))
            appRoot.mkpath("logs");
    }
}

} // namespace NetCfgTool::Paths
