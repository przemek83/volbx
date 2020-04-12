#include "ExportImage.h"

#include <PlotBase.h>
#include <qwt_plot_renderer.h>
#include <QApplication>
#include <QClipboard>
#include <QImage>
#include <QPainter>

void ExportImage::exportAsImage(PlotBase* plot, const QString& fileName)
{
    QwtPlotRenderer rend;
    rend.setDiscardFlags(QwtPlotRenderer::DiscardNone);
    rend.setDiscardFlags(QwtPlotRenderer::DiscardBackground |
                         QwtPlotRenderer::DiscardCanvasBackground);

    QSize size(plot->widthMM(), plot->heightMM());

    rend.renderDocument(plot, fileName, QStringLiteral("png"), size);
}

void ExportImage::quickExportAsImage(const QList<PlotBase*>& list)
{
    QwtPlotRenderer rend;
    rend.setDiscardFlags(QwtPlotRenderer::DiscardNone);
    rend.setDiscardFlags(QwtPlotRenderer::DiscardBackground |
                         QwtPlotRenderer::DiscardCanvasBackground);

    switch (list.size())
    {
        case 1:
        {
            exportSingleImage(rend, list);
            break;
        }

        case 2:
        {
            exportTwinImage(list, rend);
            break;
        }

        case 4:
        {
            exportFourPlotsImage(list, rend);
            break;
        }

        default:
        {
            Q_ASSERT(false);
            break;
        }
    }
}

void ExportImage::exportSingleImage(const QwtPlotRenderer& rend,
                                    const QList<PlotBase*>& list)
{
    QImage img(list.front()->size(), QImage::Format_ARGB32);
    img.fill(QColor(Qt::white).rgb());
    QPainter painter(&img);

    rend.render(list.front(), &painter, list.front()->rect());

    QApplication::clipboard()->setImage(img);
}

void ExportImage::exportTwinImage(const QList<PlotBase*>& list,
                                  const QwtPlotRenderer& rend)
{
    PlotBase* left = list.first();
    PlotBase* right = list.at(1);
    QSize size(left->size().width() + right->size().width(),
               qMax(left->size().height(), right->size().height()));
    QImage img(size, QImage::Format_ARGB32);
    img.fill(QColor(Qt::white).rgb());
    QPainter painter(&img);

    QRect rect(0, 0, left->size().width(), left->size().height());
    rend.render(left, &painter, rect);

    rect.setRect(left->size().width(), 0, right->size().width(),
                 right->size().height());
    rend.render(right, &painter, rect);

    QApplication::clipboard()->setImage(img);
}

void ExportImage::exportFourPlotsImage(const QList<PlotBase*>& list,
                                       const QwtPlotRenderer& rend)
{
    int width = list.front()->size().width();
    int height = list.front()->size().height();
    QSize size(width * 2, height * 2);
    QImage img(size, QImage::Format_ARGB32);
    img.fill(QColor(Qt::white).rgb());
    QPainter painter(&img);

    QRect rect(0, 0, width, height);
    rend.render(list[0], &painter, rect);
    rect.adjust(width, 0, width, 0);
    rend.render(list[1], &painter, rect);
    rect.adjust(-width, height, -width, height);
    rend.render(list[2], &painter, rect);
    rect.adjust(width, 0, width, 0);
    rend.render(list[3], &painter, rect);

    QApplication::clipboard()->setImage(img);
}
