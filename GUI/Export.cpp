#include "Export.h"

#include <EibleUtilities.h>
#include <ExportXlsx.h>
#include <PlotBase.h>
#include <ProgressBarCounter.h>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>

#include "Common/Configuration.h"
#include "Common/Constants.h"
#include "Common/ExportData.h"
#include "Common/ExportImage.h"
#include "ModelsAndViews/DataView.h"
#include "Shared/Logger.h"

#include "DockWidget.h"
#include "PlotDockWidget.h"
#include "ui_Export.h"

Export::Export(QMainWindow* tab, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Export),
    tab_(tab)
{
    ui->setupUi(this);

    connect(ui->save, &QPushButton::clicked, this, &Export::saveClicked);
    connect(ui->locationSearch, &QPushButton::clicked, this, &Export::locationSearchClicked);

    ui->locationLineEdit->setText(Configuration::getInstance().getImportFilePath());
    ui->prefix->setValidator(new QRegExpValidator(QRegExp(QLatin1String("[\\w]*")), ui->prefix));
    ui->prefix->setText(tab_->windowTitle().replace(QRegExp(QLatin1String("[^\\w]")), QLatin1String("")));

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

Export::~Export()
{
    delete ui;
}

void Export::saveClicked()
{
    QDir dir(ui->locationLineEdit->text());

    if (ui->locationLineEdit->text().isEmpty() ||
        !dir.exists() ||
        !QFile::permissions(dir.path()).testFlag(QFile::WriteUser))
    {
        QMessageBox::warning(this,
                             QObject::tr("Error"),
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

    const QString dateString(QDate::currentDate().toString(exportFilesDateFormat_));
    const QString fileName(ui->locationLineEdit->text() + "/" +
                           ui->prefix->text() + "_" + dateString);
    QList<PlotDockWidget*> docks = tab_->findChildren<PlotDockWidget*>();
    for (PlotDockWidget* dock : docks)
    {
        QList<PlotBase*> list = dock->exportContent();
        for (PlotBase* plot : list)
        {
            QString name(fileName + "_" + plot->windowTitle() + ".png");
            ExportImage::exportAsImage(plot, name);
        }

    }
    auto view = tab_->findChild<DataView*>();
    Q_ASSERT(view != nullptr);

    if (ui->xlsx->isChecked())
    {
        const QString barTitle =
            Constants::getProgressBarTitle(Constants::BarTitle::SAVING);
        ProgressBarCounter bar(barTitle, view->model()->rowCount(), nullptr);
        bar.showDetached();

        QTime performanceTimer;
        performanceTimer.start();

        QFile file(fileName + "_" + tr("data") + ".xlsx");
        ExportXlsx exportXlsx;
        connect(&exportXlsx, &ExportXlsx::updateProgress,
                &bar, &ProgressBarCounter::updateProgress);
        if (exportXlsx.exportView(*view, file))
            LOG(LogTypes::IMPORT_EXPORT, "Data exported in " +
                QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
                " seconds.");
        else
            LOG(LogTypes::IMPORT_EXPORT, "Can not open XLSX template file.");
    }
    else
    {
        ExportData::asCsv(view,
                          fileName + "_" + tr("data") + ".csv",
                          false); //false = not inner format
    }

    QApplication::restoreOverrideCursor();
}
