#include <QApplication>
#include <QFile>

#include <Common/Configuration.h>
#include <Common/Constants.h>
#include <GUI/VolbxMain.h>
#include <Shared/Application.h>
#include <Shared/Logger.h>

#include "VolbxProperties.h"

static void overwriteUpdaterfIfNeeded()
{
    const QString updaterFile{QCoreApplication::applicationDirPath() + "/" +
                              constants::getUpdaterName() +
                              constants::getExeFileSuffix()};

    if (QFile::exists(updaterFile + constants::getTemporaryFileSuffix()))
    {
        LOG(LogTypes::APP, "Overwriting " + updaterFile + " by file " +
                               updaterFile +
                               constants::getTemporaryFileSuffix());

        QFile::remove(updaterFile);
        QFile::rename(updaterFile + constants::getTemporaryFileSuffix(),
                      updaterFile);
    }
}

int main(int argc, char* argv[])
{
    const QApplication a(argc, argv);
    Application::setAdditionalApplicatioInfo(VER_PRODUCTNAME_STR);
    Application::initStyle(Configuration::getInstance().getStyleName());

    LOG(LogTypes::APP, QStringLiteral("Application start"));

    overwriteUpdaterfIfNeeded();

    // Create new or load existing configuration.
    Configuration::getInstance();

    VolbxMain w;
    LOG(LogTypes::APP, QStringLiteral("Showing main window"));
    w.show();
    w.checkForUpdates();
    return QApplication::exec();
}
