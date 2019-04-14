#include <windows.h>
#include <cstddef>

#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <QApplication>

#include "GUI/VolbxMain.h"
#include "Common/Configuration.h"
#include "Shared/Logger.h"
#include "Common/Constants.h"
#include "Shared/CommonProperties.h"
#include "VolbxProperties.h"
#include "Shared/Application.h"

void overwriteUpdaterfIfNeeded()
{
    QString updaterFile(QCoreApplication::applicationDirPath() + "/" +
                        QString::fromLatin1(Constants::updaterName_) +
                        QString::fromLatin1(Constants::exeFileSuffix_));

    if (QFile::exists(updaterFile +
                      QString::fromLatin1(Constants::tmpFileSuffix_)))
    {
        LOG(LOG_APP, "Overwriting " + updaterFile + " by file " +
            updaterFile + Constants::tmpFileSuffix_);

        QFile::remove(updaterFile);
        QFile::rename(updaterFile + Constants::tmpFileSuffix_,
                      updaterFile);
    }
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    Application::setAdditionalApplicatioInfo(VER_PRODUCTNAME_STR);

    Application::initStyle(Configuration::getInstance().getStyle());

    LOG(LOG_APP, "Application start");

    overwriteUpdaterfIfNeeded();

    //Create new or load existing config.
    Configuration::getInstance();

    VolbxMain w;

    LOG(LOG_APP, "Showing main window");

    w.show();
    w.checkForUpdates();

    return QApplication::exec();
}
