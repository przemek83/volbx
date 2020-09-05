#include "Export.h"

#include <ExportDsv.h>
#include <ExportXlsx.h>
#include <PlotBase.h>
#include <ProgressBarCounter.h>
#include <QDir>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>

#include "Common/Configuration.h"
#include "Common/Constants.h"
#include "Export/ExportImage.h"
#include "ModelsAndViews/DataView.h"
#include "Shared/Logger.h"

#include "PlotDock.h"
#include "ui_Export.h"

Export::Export(QMainWindow* tab, QWidget* parent)
    : QDialog(parent), ui(new Ui::Export), tab_(tab)
{
    ui->setupUi(this);

    connect(ui->save, &QPushButton::clicked, this, &Export::saveClicked);
    connect(ui->locationSearch, &QPushButton::clicked, this,
            &Export::locationSearchClicked);

    ui->locationLineEdit->setText(
        Configuration::getInstance().getImportFilePath());
    ui->prefix->setValidator(
        new QRegExpValidator(QRegExp(QLatin1String("[\\w]*")), ui->prefix));
    ui->prefix->setText(tab_->windowTitle().replace(
        QRegExp(QLatin1String("[^\\w]")), QLatin1String("")));

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

Export::~Export() { delete ui; }

void Export::saveClicked()
{
    if (!locationIsValid(ui->locationLineEdit->text()))
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
    dialog.setDirectory(ui->locationLineEdit->text());
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    dialog.exec();

    ui->locationLineEdit->setText(dialog.directory().absolutePath());
}

void Export::saveOnDisk()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    const QString fileName{getFileName()};
    exportPlots(fileName);

    QTime performanceTimer;
    performanceTimer.start();

    if (exportData(fileName))
        LOG(LogTypes::IMPORT_EXPORT,
            "Data exported in " +
                QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
                " seconds.");
    else
        LOG(LogTypes::IMPORT_EXPORT, "Exporting data failed.");

    QApplication::restoreOverrideCursor();
}

bool Export::locationIsValid(const QString& location) const
{
    const QDir dir(location);
    return !ui->locationLineEdit->text().isEmpty() & dir.exists() &&
           QFile::permissions(dir.path()).testFlag(QFile::WriteUser);
}

bool Export::exportData(const QString& fileName)
{
    const auto view{tab_->findChild<DataView*>()};
    Q_ASSERT(view != nullptr);

    const QString barTitle{
        Constants::getProgressBarTitle(Constants::BarTitle::SAVING)};
    ProgressBarCounter bar(barTitle, 100, nullptr);
    bar.showDetached();

    bool exportSucceed{false};
    if (ui->xlsx->isChecked())
        exportSucceed = exportToXlsx(fileName, view, bar);
    else
        exportSucceed = exportToCsv(fileName, view, bar);
    return exportSucceed;
}

bool Export::exportToXlsx(const QString& fileName, DataView* view,
                          ProgressBarCounter& bar)
{
    QFile file(fileName + "_" + tr("data") + ".xlsx");
    ExportXlsx exportXlsx;
    connect(&exportXlsx, &ExportData::progressPercentChanged, &bar,
            &ProgressBarCounter::updateProgress);
    return exportXlsx.exportView(*view, file);
}

bool Export::exportToCsv(const QString& fileName, DataView* view,
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
    for (const auto plotDock : plotDocks)
    {
        const QList<PlotBase*> plots{plotDock->exportContent()};
        for (const auto plot : plots)
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
    const QString fileName(ui->locationLineEdit->text() + "/" +
                           ui->prefix->text() + "_" + dateString);
    return fileName;
}
