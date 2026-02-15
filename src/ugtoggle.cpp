#include "ugtoggle.h"

#include <math.h>

#include <QPainter>
#include <QPainterPath>

#include "ultragui.h"

using namespace gui;

//=========================================================
UGToggle::UGToggle(QWidget* parent)
    : ButtonLogic(parent),
      m_slideAnimation(0),
      m_animationMax(0),
      m_timer(this),
      m_path()
{
    m_timer.setInterval(20);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_onTimerTick()));
}
//=========================================================
UGToggle::~UGToggle() { m_timer.stop(); }
//=========================================================
void UGToggle::setState(bool state) { forceState(state); }
//=========================================================
void UGToggle::_onTimerTick()
{
    if (isActive())
    {
        if (!isVisible())
        {
            m_timer.stop();
            m_slideAnimation = m_animationMax;
        }
        else if (m_slideAnimation == m_animationMax)
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
        if (!isVisible())
        {
            m_timer.stop();
            m_slideAnimation = 0;
        }
        else if (m_slideAnimation == 0)
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
void UGToggle::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillPath(m_path, UltraGui::transient(palette().base().color(), palette().accent().color(),
                                                 ((float)m_slideAnimation / (float)m_animationMax) * 255.0f));

    painter.setPen(QPen(Qt::PenStyle::NoPen));
    painter.setBrush(palette().alternateBase());
    float cursorDimension = (float)height() / (isHovering() ? 3.0f : 4.0f);
    painter.drawEllipse(QPointF((float)height() / 2.0f + (float)m_slideAnimation, (float)height() / 2.0f),
                        cursorDimension, cursorDimension);
}
//=========================================================
void UGToggle::resizeEvent(QResizeEvent* event)
{
    m_animationMax = width() - height();
    m_path         = QPainterPath(QPoint(height() / 2, 0));
    m_path.lineTo(width() - (height() / 2), 0);
    m_path.arcTo(QRect(width() - height(), 0, height(), height()), 90, -180);
    m_path.lineTo(height() / 2, height());
    m_path.arcTo(QRect(0, 0, height(), height()), -90, -180);
}
//=========================================================
void UGToggle::stateChange(bool newState) { m_timer.start(); }
//=========================================================
