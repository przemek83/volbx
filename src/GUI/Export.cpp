#include "Export.h"

#include <eible/ExportDsv.h>
#include <eible/ExportXlsx.h>
#include <qwtble/PlotBase.h>
#include <wble/ProgressBarCounter.h>
#include <QDir>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>

#include "Common/Configuration.h"
#include "Common/Constants.h"
#include "Export/ExportImage.h"
#include "ModelsAndViews/DataView.h"
#include "Shared/Logger.h"

#include "PlotDock.h"

Export::Export(QWidget* tab, QWidget* parent)
    : QDialog(parent), ui_(std::make_unique<Ui::Export>()), tab_(tab)
{
    ui_->setupUi(this);

    connect(ui_->save, &QPushButton::clicked, this, &Export::saveClicked);
    connect(ui_->locationSearch, &QPushButton::clicked, this,
            &Export::locationSearchClicked);

    ui_->locationLineEdit->setText(
        Configuration::getInstance().getImportFilePath());
    auto* validator{new QRegularExpressionValidator(
        QRegularExpression(QStringLiteral("[\\w]*")), ui_->prefix)};
    ui_->prefix->setValidator(validator);
    ui_->prefix->setText(tab_->windowTitle().replace(
        QRegularExpression(QStringLiteral("[^\\w]")), QLatin1String("")));

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void Export::saveClicked()
{
    if (!locationIsValid(ui_->locationLineEdit->text()))
    {
        QMessageBox::warning(
            this, QObject::tr("Error"),
            QObject::tr("Can not create file in given location."));
        return;
    }

    saveOnDisk();

    accept();
}

void Export::locationSearchClicked()
{
    QFileDialog dialog;
    dialog.setDirectory(ui_->locationLineEdit->text());
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    dialog.exec();

    ui_->locationLineEdit->setText(dialog.directory().absolutePath());
}

void Export::saveOnDisk()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    const QString fileName{getFileName()};
    exportPlots(fileName);

    QElapsedTimer performanceTimer;
    performanceTimer.start();

    if (exportData(fileName))
        LOG(LogTypes::IMPORT_EXPORT,
            "Data exported in " +
                Constants::timeFromTimeToSeconds(performanceTimer) +
                " seconds.");
    else
        LOG(LogTypes::IMPORT_EXPORT, QStringLiteral("Exporting data failed."));

    QApplication::restoreOverrideCursor();
}

bool Export::locationIsValid(const QString& location) const
{
    const QDir dir(location);
    return !ui_->locationLineEdit->text().isEmpty() && dir.exists() &&
           QFile::permissions(dir.path()).testFlag(QFile::WriteUser);
}

bool Export::exportData(const QString& fileName)
{
    const auto* view{tab_->findChild<DataView*>()};
    Q_ASSERT(view != nullptr);

    const QString barTitle{
        Constants::getProgressBarTitle(Constants::BarTitle::SAVING)};
    ProgressBarCounter bar(barTitle, Constants::getProgressBarFullCounter(),
                           nullptr);
    bar.showDetached();

    bool exportSucceed{false};
    if (ui_->xlsx->isChecked())
        exportSucceed = exportToXlsx(fileName, view, bar);
    else
        exportSucceed = exportToCsv(fileName, view, bar);
    return exportSucceed;
}

bool Export::exportToXlsx(const QString& fileName, const DataView* view,
                          ProgressBarCounter& bar)
{
    QFile file(fileName + "_" + tr("data") + ".xlsx");
    ExportXlsx exportXlsx;
    connect(&exportXlsx, &ExportData::progressPercentChanged, &bar,
            &ProgressBarCounter::updateProgress);
    return exportXlsx.exportView(*view, file);
}

bool Export::exportToCsv(const QString& fileName, const DataView* view,
                         ProgressBarCounter& bar)
{
    QFile file(fileName + "_" + tr("data") + ".csv");
    file.open(QIODevice::WriteOnly);
    ExportDsv exportDsv(',');
    connect(&exportDsv, &ExportData::progressPercentChanged, &bar,
            &ProgressBarCounter::updateProgress);
    QLocale locale;
    locale.setNumberOptions(locale.numberOptions() |
                            QLocale::OmitGroupSeparator);
    exportDsv.setNumbersLocale(locale);
    return exportDsv.exportView(*view, file);
}

void Export::exportPlots(const QString& fileName)
{
    const QList<PlotDock*> plotDocks{tab_->findChildren<PlotDock*>()};
    for (const auto* plotDock : plotDocks)
    {
        const QList<PlotBase*> plots{plotDock->getPlots()};
        for (auto* plot : plots)
        {
            const QString name(fileName + "_" + plot->windowTitle() + ".png");
            ExportImage::exportAsImage(plot, name);
        }
    }
}

QString Export::getFileName()
{
    const QString dateString(
        QDate::currentDate().toString(exportFilesDateFormat_));
    QString fileName(ui_->locationLineEdit->text() + "/" + ui_->prefix->text() +
                     "_" + dateString);
    return fileName;
}
