#include "ultraslider.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>

using namespace hci;

//=========================================================
uint32_t UltraSlider::_sliderToPixel(uint16_t slider)
{
    uint16_t margin = height() / 2;
    return (uint32_t)(((float)slider / ((float)m_max) * ((float)width() - (float)(margin * 2))) + margin);
}
//=========================================================
void UltraSlider::_updateCursorDimensionTick()
{
    if(m_touchScreenMode)
    {
        m_cursorDimension = height();
    }
    else
    {
        if(m_hovering)
        {
            if(m_cursorDimension != (uint32_t)height())
            {
                m_cursorDimension++;
            }
        }
        else
        {
            if(m_cursorDimension != 0)
            {
                m_cursorDimension--;
            }
        }
    }
}
//=========================================================
void UltraSlider::_tick()
{
    uint32_t setpoint = _sliderToPixel(m_slider);
    float toSum = (float)labs((int)setpoint - (int)m_pixelPos) / 15.0f;

    if(setpoint > m_pixelPos)
    {
        m_pixelPos += ceil(toSum);
    }
    else if(setpoint < m_pixelPos)
    {
        m_pixelPos -= ceil(toSum);
    }

    _updateCursorDimensionTick();

    if((setpoint == m_pixelPos) && (m_cursorDimension == (uint32_t)height() || m_cursorDimension == 0))
    {
        m_timer.stop();
    }

    repaint();
}
//=========================================================
void UltraSlider::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    uint16_t margin = height() / 2;
    uint16_t pixelZeroPos = _sliderToPixel(m_zero);

    if(!m_pixelPos)
    {
        m_pixelPos = _sliderToPixel(m_slider);
    }

    //
    QPainterPath  path;
    path.moveTo(margin, 5);
    path.lineTo(m_pixelPos, 5);
    path.lineTo(m_pixelPos, height() - 6);
    path.lineTo(margin, height() - 6);
    path.lineTo(margin, 5);
    //
    QPainterPath background;
    background.moveTo(margin, 5);
    background.lineTo(width() - margin, 5);
    background.lineTo(width() - margin, height() - 6);
    background.lineTo(margin, height() - 6);
    background.lineTo(margin, 5);
    //
    QPainterPath cursor;

    if(m_cursorDimension)
    {
        QRect rect(0, 0, m_cursorDimension, m_cursorDimension);
        rect.moveCenter(QPoint(m_pixelPos - 1, (height() - 1) / 2));
        cursor.addEllipse(rect);
    }

    //
    QPainterPath zeroNotch;
    QRect notch(0, 0, 4, height() - 1);
    notch.moveLeft(pixelZeroPos - 2);
    zeroNotch.addRoundedRect(notch, 2, 2);
    //
    painter.fillPath(background, palette().color(QPalette::Dark));
    painter.fillPath(path, palette().color(QPalette::Light));
    painter.fillPath(zeroNotch, palette().color(QPalette::Dark));

    if(m_cursorDimension)
    {
        painter.fillPath(cursor, palette().color(QPalette::Base));
    }
}
//=========================================================
void UltraSlider::mouseMoveEvent(QMouseEvent* event)
{
    if((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
        uint32_t pos = event->x()  - 1;
        uint32_t maxPos = width() - (height() / 2);
        uint32_t minPos = height() / 2;

        if(pos <= maxPos && pos >= minPos)
        {
            m_slider = ceil((((float)pos - (float)minPos) / ((float)maxPos - (float)minPos)) * ((float)m_max));
            emit onChange(m_slider);

            if(!m_timer.isActive())
            {
                m_timer.start();
            }
        }
    }
    else
    {
        m_hovering = true;

        if(!m_timer.isActive())
        {
            m_timer.start();
        }
    }

    QWidget::mouseMoveEvent(event);
}
//=========================================================
void UltraSlider::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::MiddleButton)
    {
        m_slider = m_zero;
        emit onChange(m_slider);

        if(!m_timer.isActive())
        {
            m_timer.start();
        }
    }
    else if(event->button() == Qt::LeftButton)
    {
        uint32_t pos = event->x()  - 1;
        uint32_t maxPos = width() - (height() / 2);
        uint32_t minPos = height() / 2;

        if(pos <= maxPos && pos >= minPos)
        {
            m_slider = ceil((((float)pos - (float)minPos) / ((float)maxPos - (float)minPos)) * ((float)m_max));
            emit onChange(m_slider);

            if(!m_timer.isActive())
            {
                m_timer.start();
            }
        }
    }
}
//=========================================================
void UltraSlider::wheelEvent(QWheelEvent* event)
{
    if(event->angleDelta().y() > 0)
    {
        if(m_slider < m_max)
        {
            m_slider++;
            emit onChange(m_slider);
        }

        if(!m_timer.isActive())
        {
            m_timer.start();
        }
    }
    else if(event->angleDelta().y() < 0)
    {
        if(m_slider > 0)
        {
            m_slider--;
            emit onChange(m_slider);
        }

        if(!m_timer.isActive())
        {
            m_timer.start();
        }
    }

    QWidget::wheelEvent(event);
}
//=========================================================
void UltraSlider::leaveEvent(QEvent* event)
{
    (void)event;
    m_hovering = false;

    if(!m_timer.isActive())
    {
        m_timer.start();
    }
}
//=========================================================
UltraSlider::UltraSlider(QWidget* parent) : QWidget(parent),
    m_slider(0),
    m_zero(0),
    m_max(100),
    m_pixelPos(0),
    m_pixelZeroPos(0),
    m_cursorDimension(0),
    m_hovering(false),
    m_touchScreenMode(false)
{
    setMouseTracking(true);
    m_timer.setInterval(10);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//=========================================================
