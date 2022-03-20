#include "ultraloadingcircle.h"
#include <QPainter>
#include <QEvent>
#include <QtMath>

using namespace hci;

//=========================================================
void UltraLoadingCircle::_adaptColors()
{
    QColor tempColor = palette().color(QPalette::WindowText);
    QImage defaultIcon = m_pixmap.toImage();

    for(int y = 0; y < defaultIcon.height(); y++)
    {
        for(int x = 0; x < defaultIcon.width(); x++)
        {
            tempColor.setAlpha(defaultIcon.pixelColor(x, y).alpha());
            defaultIcon.setPixelColor(x, y, tempColor);
        }
    }

    m_pixmap = QPixmap::fromImage(defaultIcon); //overwrite!
}
//=========================================================
void UltraLoadingCircle::_tick()
{
    if(m_ccw)
    {
        m_animation -= m_toSum;

        if(m_animation <= 0)
        {
            m_animation = 359;
        }
    }
    else
    {
        m_animation += m_toSum;

        if(m_animation >= 359)
        {
            m_animation = 0;
        }
    }

    repaint();
}
//=========================================================
void UltraLoadingCircle::paintEvent(QPaintEvent* event)
{
    (void) event;

    if(!m_loading || m_pixmap.isNull())
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPointF center(width() / 2, height() / 2);  //center of the widget
    painter.translate(center);
    painter.rotate(m_animation);
    painter.translate(-center);
    float allowedDimension = (float)m_pixmap.width() / qSqrt(2);
    QRectF allowedRect(0, 0, allowedDimension, allowedDimension);
    allowedRect.moveCenter(center);
    painter.drawPixmap(allowedRect, m_pixmap, m_pixmap.rect());
    painter.end();
}
//=========================================================
void UltraLoadingCircle::changeEvent(QEvent* event)
{
    if(event->type() == QEvent::ApplicationPaletteChange || event->type() == QEvent::PaletteChange)
    {
        if(m_adaptColors)
        {
            _adaptColors();
        }
    }

    QWidget::changeEvent(event);
}
//=========================================================
UltraLoadingCircle::UltraLoadingCircle(QWidget* parent) :
    QWidget(parent),
    m_timer(this),
    m_loading(false),
    m_animation(0),
    m_toSum(4),
    m_ccw(false),
    m_adaptColors(true)
{
    m_timer.setInterval(10);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//=========================================================
void UltraLoadingCircle::setLoading(bool state)
{
    m_loading = state;

    if(state)
    {
        m_timer.start();
    }
    else
    {
        m_timer.stop();
    }
}
//=========================================================
void UltraLoadingCircle::setPixmap(const QPixmap& pixmap)
{
    //check if pixmap is square
    if(pixmap.height() == pixmap.width())
    {
        m_pixmap = pixmap;
    }

    if(m_adaptColors)
    {
        _adaptColors();
    }
}
//=========================================================
void UltraLoadingCircle::setSpeed(uint32_t speed)
{
    m_toSum = ceil((float)speed / 100.0f);
}
//=========================================================
