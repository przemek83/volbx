#include <cstddef>
#include <windows.h>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

#include "Common/Configuration.h"
#include "Common/Constants.h"
#include "GUI/VolbxMain.h"
#include "Shared/Application.h"
#include "Shared/CommonProperties.h"
#include "Shared/Logger.h"

#include "VolbxProperties.h"

void overwriteUpdaterfIfNeeded()
{
    QString updaterFile(QCoreApplication::applicationDirPath() + "/" +
                        QString::fromLatin1(Constants::updaterName) +
                        QString::fromLatin1(Constants::exeFileSuffix));

    if (QFile::exists(updaterFile +
                      QString::fromLatin1(Constants::tmpFileSuffix)))
    {
        LOG(LogTypes::APP, "Overwriting " + updaterFile + " by file " +
            updaterFile + Constants::tmpFileSuffix);

        QFile::remove(updaterFile);
        QFile::rename(updaterFile + Constants::tmpFileSuffix,
                      updaterFile);
    }
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    Application::setAdditionalApplicatioInfo(VER_PRODUCTNAME_STR);

    Application::initStyle(Configuration::getInstance().getStyle());

    LOG(LogTypes::APP, "Application start");

    overwriteUpdaterfIfNeeded();

    //Create new or load existing config.
    Configuration::getInstance();

    VolbxMain w;

    LOG(LogTypes::APP, "Showing main window");

    w.show();
    w.checkForUpdates();

    return QApplication::exec();
}
