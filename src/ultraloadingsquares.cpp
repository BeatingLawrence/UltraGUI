#include "ultraloadingsquares.h"

#include <math.h>

#include <QCoreApplication>
#include <QEvent>
#include <QPainter>

using namespace gui;

//============================================================
UltraLoadingSquares::UltraLoadingSquares(QWidget* parent)
    : QWidget(parent),
      m_loading(false),
      m_animation(0),
      m_timer(),
      m_speed(AnimationSpeed::AS_Normal),
      m_tinyRectSize(),
      m_bottomLeftCorner(0),
      m_bottomRightCorner(0),
      m_topRightCorner(0),
      m_topLeftCorner(0)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_onTimerTick()));
}
//============================================================
void UltraLoadingSquares::setLoading(bool state)
{
    if (!m_loading && state)
    {
        if (!m_tinyRectSize.isValid())
        {
            m_tinyRectSize.setWidth(width() / 3);
            m_tinyRectSize.setHeight(height() / 3);
        }

        m_bottomLeftCorner  = height() - m_tinyRectSize.height();
        m_bottomRightCorner = m_bottomLeftCorner + width() - m_tinyRectSize.width();
        m_topRightCorner    = m_bottomRightCorner + height() - m_tinyRectSize.height();
        m_topLeftCorner     = m_topRightCorner + width() - m_tinyRectSize.width();

        switch (m_speed)
        {
            case AS_Slow:
                m_timer.start(60);
                break;

            case AS_Normal:
                m_timer.start(40);
                break;

            case AS_Fast:
                m_timer.start(20);
                break;

            case AS_Superfast:
                m_timer.start(10);
                break;
        }
    }
    else if (m_loading && !state)
    {
        m_timer.stop();
    }

    m_loading = state;
}
//============================================================
void UltraLoadingSquares::setSquaresSize(uint8_t size)
{
    m_tinyRectSize.setWidth(size);
    m_tinyRectSize.setHeight(size);

    if (m_loading)
    {
        setLoading(false);
        setLoading(true);
    }
}
//============================================================
void UltraLoadingSquares::_increment(unsigned int& toIncrement, unsigned int end)
{
    unsigned int diff      = end - toIncrement;
    float toSum            = (float)diff / 6.6666f;
    unsigned int increment = ceil(toSum);
    toIncrement += increment == 0 ? 1 : increment;
}
//============================================================
void UltraLoadingSquares::_onTimerTick()
{
    if (m_animation <= m_bottomLeftCorner)
    {
        _increment(m_animation, m_bottomLeftCorner);
    }
    else if (m_animation > m_bottomLeftCorner && m_animation <= m_bottomRightCorner)
    {
        _increment(m_animation, m_bottomRightCorner);
    }
    else if (m_animation > m_bottomRightCorner && m_animation <= m_topRightCorner)
    {
        _increment(m_animation, m_topRightCorner);
    }
    else if (m_animation > m_topRightCorner && m_animation <= m_topLeftCorner)
    {
        _increment(m_animation, m_topLeftCorner);
    }

    if (m_animation >= m_topLeftCorner)
    {
        m_animation = 0;
    }

    repaint();
}
//============================================================
void UltraLoadingSquares::paintEvent(QPaintEvent* event)
{
    (void)event;

    if (!m_loading)
    {
        return;
    }

    QPainter painter(this);
    int xPos = 0;
    int yPos = 0;

    if (m_animation <= m_bottomLeftCorner)
    {
        yPos = m_animation;
    }
    else if (m_animation > m_bottomLeftCorner && m_animation <= m_bottomRightCorner)
    {
        xPos = m_animation - m_bottomLeftCorner;
        yPos = m_bottomLeftCorner;
    }
    else if (m_animation > m_bottomRightCorner && m_animation <= m_topRightCorner)
    {
        xPos = m_bottomLeftCorner;
        yPos = m_topRightCorner - m_animation;
    }
    else if (m_animation > m_topRightCorner && m_animation <= m_topLeftCorner)
    {
        xPos = m_topLeftCorner - m_animation;
    }

    painter.fillRect(QRect(QPoint(xPos, yPos), m_tinyRectSize), QBrush(palette().color(QPalette::Light)));
    painter.fillRect(QRect(QPoint(width() - xPos - m_tinyRectSize.width(), height() - yPos - m_tinyRectSize.height()), m_tinyRectSize), QBrush(palette().color(QPalette::Light)));
    painter.end();
}
//============================================================
