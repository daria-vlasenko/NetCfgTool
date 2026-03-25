#include <QApplication>

#include "core/paths.h"
#include "core/logger.h"
#include "mainwindow.h"

using namespace NetCfgTool;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Гарантируем наличие нужных директорий:
    // ~/.config/NetCfgTool
    // ~/.local/share/NetCfgTool/logs
    Paths::ensureAppDirectories();

    // Создаём файловый логгер по ТЗ
    FileLogger logger;
    logger.setMinimumLevel(LogLevel::Info);
    logger.log(LogLevel::Info, "Main", "NetCfgTool started");

    MainWindow w;
    w.show();

    int ret = app.exec();

    logger.log(LogLevel::Info, "Main",
               "NetCfgTool exited with code " + QString::number(ret));

    return ret;
}
