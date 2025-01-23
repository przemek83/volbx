#include <QApplication>
#include <QFile>

#include <Common/Configuration.h>
#include <Common/Constants.h>
#include <GUI/VolbxMain.h>
#include <Shared/Application.h>
#include <Shared/Logger.h>

namespace
{
void overwriteUpdaterfIfNeeded()
{
    const QString updaterFile{QCoreApplication::applicationDirPath() + "/" +
                              constants::getUpdaterName() +
                              constants::getExeFileSuffix()};

    if (QFile::exists(updaterFile + constants::getTemporaryFileSuffix()))
    {
        LOG(LogTypes::APP, "Overwriting " + updaterFile + " by file " +
                               updaterFile +
                               constants::getTemporaryFileSuffix());

        bool success{QFile::remove(updaterFile)};
        if (!success)
            LOG(LogTypes::APP, "Cannot remove " + updaterFile);

        success = QFile::rename(
            updaterFile + constants::getTemporaryFileSuffix(), updaterFile);
        if (!success)
            LOG(LogTypes::APP, "Cannot rename file to " + updaterFile);
    }
}
}  // namespace

int main(int argc, char* argv[])
{
    const QApplication a(argc, argv);

    const QString appName{QStringLiteral("Volbx")};
    application::setAdditionalApplicatioInfo(appName);
    Configuration config;
    application::initStyle(config.getStyleName());

    LOG(LogTypes::APP, QStringLiteral("Application start"));

    overwriteUpdaterfIfNeeded();

    VolbxMain w(std::move(config));
    LOG(LogTypes::APP, QStringLiteral("Showing main window"));
    w.show();
    w.checkForUpdates();
    return QApplication::exec();
}
