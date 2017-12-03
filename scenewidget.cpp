#include "scenewidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QRect>

SceneWidget::SceneWidget(QWidget *parent)
    : QFrame(parent)
{}

Core SceneWidget::core(int index) const
{
    Core core{};

    core.length   = cores->index(index, 0).data().toDouble();
    core.area     = cores->index(index, 1).data().toDouble();
    core.elastic  = cores->index(index, 2).data().toDouble();
    core.strength = cores->index(index, 3).data().toDouble();
    core.load     = cores->index(index, 4).data().toDouble();

    return core;
}

void SceneWidget::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    scale *= calcScale(calcParams());

    drawAxis(painter);
    drawConstruction(painter);
    drawConcentratedLoads(painter);
}

void SceneWidget::drawAxis(QPainter &painter)
{
    painter.setPen(QPen(Qt::gray, 2, Qt::DashDotLine));

    qreal yPos = rect().height() / 2;
    painter.drawLine(QPointF(0, yPos), QPointF(rect().width(), yPos));
}

void SceneWidget::drawConstruction(QPainter &painter)
{
    qreal pos = X_BEGIN_POS;

    if(hasLeftSupport && cores->rowCount() != 0)
        drawLeftSupport(painter, pos, core(0).area * scale);

    for(int i = 0; i < cores->rowCount(); ++ i)
    {
        Core core = this->core(i);
        drawCore(painter, core.length, core.area, pos);

        if(core.load != 0)
            drawDistributedLoad(painter, core.length * scale, core.load, pos);

        pos += core.length * scale;
    }

    if(hasRightSupport && cores->rowCount() != 0)
        drawRightSupport(painter, pos, core(cores->rowCount() - 1).area * scale);
}

void SceneWidget::drawCore(QPainter &painter, qreal length, qreal area, qreal xBeginPos)
{
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::black, 4, Qt::SolidLine));

    qreal yBeginPos = rect().height() / 2 - area * scale / 2;

    painter.drawRect(xBeginPos, yBeginPos, length * scale, area * scale);
}

void SceneWidget::drawDistributedLoad(QPainter &painter, qreal length,
                                      const double Q, qreal xBeginPos)
{
    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));

    qreal xEndPos = xBeginPos + length;

    qreal yAxisPos = rect().height() / 2;
    painter.drawLine(xBeginPos, yAxisPos, xEndPos, yAxisPos);

    qreal deltaX = 5;
    qreal deltaY = 5;
    qreal step = 10;

    qreal xPos = xBeginPos + step;
    while (xPos < xEndPos)
    {
        qreal xOffset =  xPos - (Q > 0 ? deltaX : -deltaX);

        QPointF arrow[3];
        arrow[0] = QPointF(xOffset, yAxisPos - deltaY);
        arrow[1] = QPointF(xPos, yAxisPos);
        arrow[2] = QPointF(xOffset, yAxisPos + deltaY);

        painter.drawPolyline(arrow, 3);

        xPos += step;
    }
}

void SceneWidget::drawConcentratedLoads(QPainter &painter)
{
    qreal pos = X_BEGIN_POS;
    for(int node = 0; node < cLoads->rowCount(); ++node)
    {
        const double F = cLoads->index(node, 0).data().toDouble();

        qreal endPos = pos;
        if(node < cores->rowCount())
            endPos += core(node).length * scale;

        if(F == 0)
        {
            pos = endPos;
            continue;
        }

        drawConcentratedLoad(painter, F, pos);
        pos = endPos;
    }
}

void SceneWidget::drawConcentratedLoad(QPainter &painter, const double F, qreal xBeginPos)
{
    painter.setBrush(Qt::SolidPattern);
    painter.setPen(QPen(Qt::black, 0, Qt::SolidLine));

    qreal yAxisPos = rect().height() / 2;

    qreal step = 15;

    QPointF triangle[3];
    triangle[0] = QPointF(xBeginPos, yAxisPos - step);
    triangle[1] = QPointF(xBeginPos + (F > 0 ? step : -step), yAxisPos);
    triangle[2] = QPointF(xBeginPos, yAxisPos + step);

    painter.drawPolygon(triangle, 3);
}

void SceneWidget::drawLeftSupport(QPainter &painter, qreal xPos , qreal height)
{
    painter.setPen(QPen(Qt::black, 4, Qt::SolidLine));

    qreal startYPos = rect().height() / 2 - height / 2;

    painter.drawLine(xPos, startYPos, xPos, startYPos + height);

    for(qreal curr = startYPos; curr <= startYPos + height; curr += 10)
        painter.drawLine(xPos, curr, xPos - 10, curr - 10);
}

void SceneWidget::drawRightSupport(QPainter &painter, qreal xPos, qreal height)
{
    painter.setPen(QPen(Qt::black, 4, Qt::SolidLine));

    qreal startYPos = rect().height() / 2 - height / 2;

    painter.drawLine(xPos, startYPos, xPos, startYPos + height);

    for(qreal curr = startYPos; curr <= startYPos + height; curr += 10)
        painter.drawLine(xPos, curr, xPos + 10, curr + 10);
}

qreal SceneWidget::calcScale(const Params &params) const
{
    qreal wScale = params.length > width()  ? width()  / params.length : 1;
    qreal hScale = params.heigth > height() ? height() / params.heigth : 1;

   return std::min(wScale, hScale);
}

SceneWidget::Params SceneWidget::calcParams() const
{
    Params p{};

    for(int i = 0; i < cores->rowCount(); ++i)
    {
        Core core = this->core(i);

        p.length = p.length + core.length * scale;

        qreal area = core.area * scale;
        if(p.heigth < area)
            p.heigth = area;
    }

    return p;
}
