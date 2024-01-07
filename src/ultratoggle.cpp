#include "ultratoggle.h"

#include <math.h>

#include <QPainter>
#include <QPainterPath>

#include "ultragui.h"

using namespace gui;

//=========================================================
UltraToggle::UltraToggle(QWidget* parent)
    : ButtonBehavior(parent),
      m_slideAnimation(0),
      m_animationMax(0),
      m_timer(this),
      m_path()
{
    m_timer.setInterval(20);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_onTimerTick()));
}
//=========================================================
UltraToggle::~UltraToggle() { m_timer.stop(); }
//=========================================================
void UltraToggle::_onTimerTick()
{
    if (isActive())
    {
        if (m_slideAnimation == m_animationMax)
            m_timer.stop();
        else
        {
            unsigned int diff = labs((int)m_animationMax - (int)m_slideAnimation);
            float toSum       = (float)diff / 5.0f;
            m_slideAnimation += ceil(toSum);
        }
    }
    else
    {
        if (m_slideAnimation == 0)
            m_timer.stop();
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
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen());
    painter.fillPath(m_path, UltraGui::transient(palette().color(QPalette::Dark), palette().color(QPalette::Light), ((float)m_slideAnimation / (float)m_animationMax) * 255.0f));
    painter.setBrush(palette().color(QPalette::Base));
    float cursorDimension = (float)height() / (isHovering() ? 3.0f : 4.0f);
    painter.drawEllipse(QPointF((float)height() / 2.0f + (float)m_slideAnimation, (float)height() / 2.0f), cursorDimension, cursorDimension);
}
//=========================================================
void UltraToggle::resizeEvent(QResizeEvent* event)
{
    m_animationMax = width() - height();
    m_path         = QPainterPath(QPoint(height() / 2, 0));
    m_path.lineTo(width() - (height() / 2), 0);
    m_path.arcTo(QRect(width() - height(), 0, height(), height()), 90, -180);
    m_path.lineTo(height() / 2, height());
    m_path.arcTo(QRect(0, 0, height(), height()), -90, -180);
}
//=========================================================
void UltraToggle::stateChange(bool newState) { m_timer.start(); }
//=========================================================
void UltraToggle::setState(bool state, bool useAnimation)
{
    // m_state = state;

    // if (useAnimation)
    // {
    //     m_timer.start();
    //     _onTimerTick();
    // }
    // else
    // {
    //     m_slideAnimation = m_state ? m_animationMax : 0;
    //     repaint();
    // }
}
//=========================================================
