#include "ExportImage.h"

#include <PlotBase.h>
#include <qwt_plot_renderer.h>
#include <QApplication>
#include <QClipboard>
#include <QImage>
#include <QPainter>

namespace
{
QImage createImage(QSize size)
{
    QImage image(size, QImage::Format_ARGB32);
    image.fill(QColor(Qt::white).rgb());
    return image;
}

void exportSingleImage(const QwtPlotRenderer& plotRenderer,
                       const QList<PlotBase*>& plotsList)
{
    PlotBase* plot{plotsList.front()};
    QImage image(createImage(plot->size()));
    QPainter painter(&image);
    plotRenderer.render(plot, &painter, plot->rect());

    QApplication::clipboard()->setImage(image);
}

void exportTwinImage(const QList<PlotBase*>& plotsList,
                     const QwtPlotRenderer& plotRenderer)
{
    PlotBase* leftPlot{plotsList.first()};
    PlotBase* rightPlot{plotsList.at(1)};
    const QSize leftSize{leftPlot->size()};
    const QSize rightSize{rightPlot->size()};

    QSize imageSize(leftSize.width() + rightSize.width(),
                    qMax(leftSize.height(), rightSize.height()));
    QImage image(createImage(imageSize));
    QPainter painter(&image);

    QRect rect(0, 0, leftSize.width(), leftSize.height());
    plotRenderer.render(leftPlot, &painter, rect);
    rect.setRect(leftSize.width(), 0, rightSize.width(), rightSize.height());
    plotRenderer.render(rightPlot, &painter, rect);

    QApplication::clipboard()->setImage(image);
}

void exportQuadImage(const QList<PlotBase*>& plotsList,
                     const QwtPlotRenderer& plotRenderer)
{
    const int width{plotsList.front()->size().width()};
    const int height{plotsList.front()->size().height()};
    const QSize imageSize(width * 2, height * 2);
    QImage image(createImage(imageSize));
    QPainter painter(&image);

    QRect rect(0, 0, width, height);
    plotRenderer.render(plotsList[0], &painter, rect);
    rect.adjust(width, 0, width, 0);
    plotRenderer.render(plotsList[1], &painter, rect);
    rect.adjust(-width, height, -width, height);
    plotRenderer.render(plotsList[2], &painter, rect);
    rect.adjust(width, 0, width, 0);
    plotRenderer.render(plotsList[3], &painter, rect);

    QApplication::clipboard()->setImage(image);
}
}  // namespace

namespace ExportImage
{
void exportAsImage(PlotBase* plot, const QString& fileName)
{
    QSize size(plot->widthMM(), plot->heightMM());

    QwtPlotRenderer plotRenderer;
    plotRenderer.setDiscardFlags(QwtPlotRenderer::DiscardNone);
    plotRenderer.setDiscardFlags(QwtPlotRenderer::DiscardBackground |
                                 QwtPlotRenderer::DiscardCanvasBackground);
    plotRenderer.renderDocument(plot, fileName, QStringLiteral("png"), size);
}

void quickExportAsImage(const QList<PlotBase*>& list)
{
    QwtPlotRenderer plotRenderer;
    plotRenderer.setDiscardFlags(QwtPlotRenderer::DiscardNone);
    plotRenderer.setDiscardFlags(QwtPlotRenderer::DiscardBackground |
                                 QwtPlotRenderer::DiscardCanvasBackground);

    switch (list.size())
    {
        case 1:
        {
            exportSingleImage(plotRenderer, list);
            break;
        }

        case 2:
        {
            exportTwinImage(list, plotRenderer);
            break;
        }

        case 4:
        {
            exportQuadImage(list, plotRenderer);
            break;
        }

        default:
        {
            Q_ASSERT(false);
            break;
        }
    }
}
}  // namespace ExportImage
