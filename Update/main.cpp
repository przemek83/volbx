#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>

#include "Shared/Application.h"
#include "Shared/Logger.h"
#include "Update/UpdateProperties.h"

#include "Update.h"

static void showDifferentInstanceRunnningError()
{
    QMessageBox::critical(nullptr,
                          QObject::tr("Error"),
                          QObject::tr("Different instance of ") +
                          QApplication::applicationName() +
                          QObject::tr(" already running, exiting."));

}

/**
 * @brief check if different updater is working.
 * @return true if unique, false if already working.
 */
static bool isUniqueInstance()
{
    //Only one instance can run. Intentionally leak.
    QSharedMemory* sharedMemory =
        new QSharedMemory(QStringLiteral(VER_PRODUCTNAME_STR));

    LOG(LogTypes::APP,
        QLatin1String("Setting shared memory key named ") +
        QApplication::applicationName() + QLatin1Char('.'));

    if (sharedMemory->attach())
    {
        LOG(LogTypes::APP,
            QLatin1String("Attached to shared memory. Different instance already running, exiting."));

        return false;
    }

    LOG(LogTypes::APP, QLatin1String("Attaching to shared memory successful. Continue."));

    if (!sharedMemory->create(1))
    {
        LOG(LogTypes::APP,
            QLatin1String("Creating shared memory failed. Different instance already running, exiting."));

        return false;
    }

    LOG(LogTypes::APP, QLatin1String("Unique instance, continue."));

    return true;
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    Application::setAdditionalApplicatioInfo(VER_PRODUCTNAME_STR);
    Application::setQtStyle(QStringLiteral("Fusion"));

    if (!isUniqueInstance())
    {
        showDifferentInstanceRunnningError();

        return -1;
    }

    Update w;
    w.show();

    return QApplication::exec();
}
