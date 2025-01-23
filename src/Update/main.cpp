#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>

#include "Shared/Application.h"
#include "Shared/Logger.h"

#include "Update.h"

namespace
{
void showOtherInstanceRunnningError()
{
    QMessageBox::critical(nullptr, QObject::tr("Error"),
                          QObject::tr("Different instance of ") +
                              QApplication::applicationName() +
                              QObject::tr(" already running, exiting."));
}

bool isUniqueInstance(QSharedMemory& sharedMemory)
{
    LOG(LogTypes::APP, QStringLiteral("Setting shared memory key named ") +
                           QApplication::applicationName() + u'.');

    if (sharedMemory.attach())
    {
        LOG(LogTypes::APP,
            QStringLiteral("Attached to shared memory. Different "
                           "instance already running, exiting."));

        return false;
    }

    LOG(LogTypes::APP,
        QStringLiteral("Attaching to shared memory successful. Continue."));

    if (!sharedMemory.create(1))
    {
        LOG(LogTypes::APP,
            QStringLiteral("Creating shared memory failed. Different instance "
                           "already running, exiting."));

        return false;
    }

    LOG(LogTypes::APP, QStringLiteral("Unique instance, continue."));

    return true;
}
}  // namespace

int main(int argc, char* argv[])
{
    const QApplication a(argc, argv);

    const QString appName{QStringLiteral("VersionCheck")};

    application::setAdditionalApplicatioInfo(appName);
    application::setQtStyle(QStringLiteral("Fusion"));

    if (QSharedMemory sharedMemory(appName); isUniqueInstance(sharedMemory))
    {
        Update w;
        w.show();

        return QApplication::exec();
    }

    showOtherInstanceRunnningError();

    return EXIT_FAILURE;
}
