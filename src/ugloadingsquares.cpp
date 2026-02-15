#include "ugloadingsquares.h"

#include <algorithm>
#include <math.h>

#include <QCoreApplication>
#include <QEvent>
#include <QPainter>

using namespace gui;

//============================================================
UGLoadingSquares::UGLoadingSquares(QWidget* parent)
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
void UGLoadingSquares::_updateGeometry()
{
    m_tinyRectSize = QSize(std::max(1, width() / 3), std::max(1, height() / 3));

    const unsigned int ySpan = static_cast<unsigned int>(std::max(0, height() - m_tinyRectSize.height()));
    const unsigned int xSpan = static_cast<unsigned int>(std::max(0, width() - m_tinyRectSize.width()));

    m_bottomLeftCorner  = ySpan;
    m_bottomRightCorner = m_bottomLeftCorner + xSpan;
    m_topRightCorner    = m_bottomRightCorner + ySpan;
    m_topLeftCorner     = m_topRightCorner + xSpan;

    if (m_animation > m_topLeftCorner) m_animation = 0;
}
//============================================================
void UGLoadingSquares::setLoading(bool state)
{
    if (!m_loading && state)
    {
        _updateGeometry();

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
        m_timer.stop();

    m_loading = state;
}
//============================================================
void UGLoadingSquares::resizeEvent(QResizeEvent* event)
{
    _updateGeometry();
    QWidget::resizeEvent(event);
}
//============================================================
void UGLoadingSquares::_increment(unsigned int& toIncrement, unsigned int end)
{
    unsigned int diff      = end - toIncrement;
    float toSum            = (float)diff / 6.6666f;
    unsigned int increment = ceil(toSum);
    toIncrement += increment == 0 ? 1 : increment;
}
//============================================================
void UGLoadingSquares::_onTimerTick()
{
    if (m_animation <= m_bottomLeftCorner)
        _increment(m_animation, m_bottomLeftCorner);
    else if (m_animation > m_bottomLeftCorner && m_animation <= m_bottomRightCorner)
        _increment(m_animation, m_bottomRightCorner);
    else if (m_animation > m_bottomRightCorner && m_animation <= m_topRightCorner)
        _increment(m_animation, m_topRightCorner);
    else if (m_animation > m_topRightCorner && m_animation <= m_topLeftCorner)
        _increment(m_animation, m_topLeftCorner);
    if (m_animation >= m_topLeftCorner) m_animation = 0;

    repaint();
}
//============================================================
void UGLoadingSquares::paintEvent(QPaintEvent* event)
{
    (void)event;

    if (!m_loading) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    int xPos = 0;
    int yPos = 0;

    if (m_animation <= m_bottomLeftCorner)
        yPos = m_animation;

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
        xPos = m_topLeftCorner - m_animation;

    const qreal side   = static_cast<qreal>(std::min(m_tinyRectSize.width(), m_tinyRectSize.height()));
    const qreal radius = side / 4.0;

    const QRectF firstRect(QPointF(xPos, yPos), QSizeF(m_tinyRectSize));
    const QRectF secondRect(QPointF(width() - xPos - m_tinyRectSize.width(),
                                    height() - yPos - m_tinyRectSize.height()),
                            QSizeF(m_tinyRectSize));

    painter.setPen(Qt::NoPen);
    painter.setBrush(palette().windowText());
    painter.drawRoundedRect(firstRect, radius, radius);
    painter.drawRoundedRect(secondRect, radius, radius);
    painter.end();
}
//============================================================
