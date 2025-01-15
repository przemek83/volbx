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
                              Constants::getUpdaterName() +
                              Constants::getExeFileSuffix()};

    if (QFile::exists(updaterFile + Constants::getTemporaryFileSuffix()))
    {
        LOG(LogTypes::APP, "Overwriting " + updaterFile + " by file " +
                               updaterFile +
                               Constants::getTemporaryFileSuffix());

        QFile::remove(updaterFile);
        QFile::rename(updaterFile + Constants::getTemporaryFileSuffix(),
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
