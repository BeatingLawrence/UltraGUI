#include "ultratoggle.h"

#include <math.h>

#include <QPainter>
#include <QPainterPath>

using namespace gui;

//=========================================================
UltraToggle::UltraToggle(QWidget* parent)
    : QPushButton(parent),
      m_state(false),
      m_hovering(false),
      m_slideAnimation(0),
      m_timer(this)
{
    setMouseTracking(true);
    m_timer.setInterval(20);
    connect(this, SIGNAL(clicked()), this, SLOT(_mouseClicked()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_onTimerTick()));
}
//=========================================================
UltraToggle::~UltraToggle() { m_timer.stop(); }
//=========================================================
QColor UltraToggle::_computeTransient(const QColor& first, const QColor& second, uint8_t selector)
{
    float mR  = second.redF() - first.redF();
    float mG  = second.greenF() - first.greenF();
    float mB  = second.blueF() - first.blueF();
    float mA  = second.alphaF() - first.alphaF();
    uint8_t R = ((float)selector * mR) + first.red();
    uint8_t G = ((float)selector * mG) + first.green();
    uint8_t B = ((float)selector * mB) + first.blue();
    uint8_t A = ((float)selector * mA) + first.alpha();
    return QColor(R, G, B, A);
}
//=========================================================
void UltraToggle::_mouseClicked()
{
    if (m_state)
    {
        emit onDisable();
        emit onChange(false);
    }
    else
    {
        emit onEnable();
        emit onChange(true);
    }

    m_state = !m_state;
    m_timer.start();
}
//=========================================================
void UltraToggle::_onTimerTick()
{
    uint32_t animationMaxValue = width() - height();

    if (m_state)
    {
        if (m_slideAnimation == animationMaxValue)
        {
            if (m_timer.isActive())
            {
                m_timer.stop();
            }
        }
        else
        {
            unsigned int diff = labs((int)animationMaxValue - (int)m_slideAnimation);
            float toSum       = (float)diff / 5.0f;
            m_slideAnimation += ceil(toSum);
        }
    }
    else
    {
        if (m_slideAnimation == 0)
        {
            if (m_timer.isActive())
            {
                m_timer.stop();
            }
        }
        else
        {
            unsigned int diff = m_slideAnimation;
            float toSum       = (float)diff / 5.0f;
            m_slideAnimation -= ceil(toSum);
        }
    }

    repaint();
}
//=========================================================
void UltraToggle::paintEvent(QPaintEvent* event)
{
    (void)event;
    uint32_t animationMaxValue = width() - height();
    //==============================DRAW
    QPainterPath path(QPoint(height() / 2, 0));
    path.lineTo(width() - (height() / 2), 0);
    path.arcTo(QRect(width() - height(), 0, height(), height()), 90, -180);
    path.lineTo(height() / 2, height());
    path.arcTo(QRect(0, 0, height(), height()), -90, -180);
    //==============================
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(0, 0, 0, 0), 1));
    painter.fillPath(path, _computeTransient(palette().color(QPalette::Dark), palette().color(QPalette::Light), ((float)m_slideAnimation / (float)animationMaxValue) * 255.0f));
    painter.setBrush(palette().color(QPalette::Base));
    float cursorDimension = (float)height() / (m_hovering ? 3.0f : 4.0f);
    painter.drawEllipse(QPointF((float)height() / 2.0f + (float)m_slideAnimation, (float)height() / 2.0f), cursorDimension, cursorDimension);
    painter.end();
}
//=========================================================
void UltraToggle::enterEvent(QEnterEvent* event)
{
    (void)event;
    m_hovering = true;
}
//=========================================================
void UltraToggle::leaveEvent(QEvent* event)
{
    (void)event;
    m_hovering = false;
}
//=========================================================
void UltraToggle::hideEvent(QHideEvent* event)
{
    QPushButton::hideEvent(event);
    m_hovering       = false;
    m_slideAnimation = m_state ? (width() - height()) : 0;
    m_timer.stop();
}
//=========================================================
void UltraToggle::setState(bool state, bool useAnimation)
{
    m_state = state;

    if (useAnimation)
    {
        m_timer.start();
        _onTimerTick();
    }
    else
    {
        m_slideAnimation = m_state ? (width() - height()) : 0;
        repaint();
    }
}
//=========================================================
