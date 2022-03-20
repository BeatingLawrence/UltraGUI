#include "ultraled.h"
#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>

using namespace hci;

//=========================================================
UltraLed::UltraLed(QWidget* parent) : QWidget(parent),
    m_active(false),
    m_ledBorderThickness(1)
{
    //noop
}
//=========================================================
void UltraLed::paintEvent(QPaintEvent* event)
{
    (void) event;
    QPainter painter(this);
    painter.setPen(QPen(QColor(0, 0, 0, 0), 1));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath path;
    path.addEllipse(this->rect());

    if(m_active)
    {
        painter.fillPath(path, palette().color(QPalette::Light));
    }
    else
    {
        painter.fillPath(path, palette().color(QPalette::Dark));
    }

    if(m_ledBorderThickness != 0)
    {
        QPainterPathStroker stroker(QPen(QColor(), m_ledBorderThickness * 2));
        QPainterPath ring = stroker.createStroke(path).intersected(path);
        painter.fillPath(ring, palette().color(QPalette::WindowText));
    }

    painter.end();
}
//=========================================================
void UltraLed::ledBorderThickness(uint8_t ledBorderThickness)
{
    m_ledBorderThickness = ledBorderThickness;
    update();
}
//=========================================================
void UltraLed::activate(bool active)
{
    m_active = active;
    update();
}
//=========================================================
void UltraLed::deactivate()
{
    activate(false);
}
//=========================================================
