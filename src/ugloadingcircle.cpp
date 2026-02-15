#include "ugloadingcircle.h"

#include <algorithm>
#include <cmath>
#include <QPainter>

using namespace gui;

void UGLoadingCircle::_tick()
{
    if (m_ccw)
    {
        m_animation -= m_toSum;

        if (m_animation <= 0)
        {
            m_animation = 359;
        }
    }
    else
    {
        m_animation += m_toSum;

        if (m_animation >= 359)
        {
            m_animation = 0;
        }
    }

    repaint();
}
//=========================================================
void UGLoadingCircle::paintEvent(QPaintEvent* event)
{
    (void)event;

    if (!m_loading)
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int minSide  = std::max(1, std::min(width(), height()));
    const int penWidth = std::max(1, minSide / 10);
    const qreal side   = std::max<qreal>(1.0, minSide - penWidth - 2);

    QRectF arcRect(0.0, 0.0, side, side);
    arcRect.moveCenter(QPointF(width() / 2.0, height() / 2.0));

    painter.setPen(QPen(palette().windowText().color(), penWidth, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(Qt::NoBrush);

    int spanDegrees = 1;
    if (m_progress >= 100)
        spanDegrees = 360;
    else
        spanDegrees = std::max(1, static_cast<int>(std::round((m_progress / 100.0f) * 359.0f)));

    const int startAngle16 = (m_ccw ? m_animation : -m_animation) * 16;
    const int spanAngle16  = (m_ccw ? spanDegrees : -spanDegrees) * 16;
    painter.drawArc(arcRect, startAngle16, spanAngle16);
}
//=========================================================
UGLoadingCircle::UGLoadingCircle(QWidget* parent)
    : QWidget(parent),
      m_timer(this),
      m_loading(false),
      m_progress(75),
      m_animation(0),
      m_toSum(4),
      m_ccw(false)
{
    m_timer.setInterval(10);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//=========================================================
void UGLoadingCircle::setLoading(bool state)
{
    m_loading = state;

    if (state)
    {
        m_timer.start();
    }
    else
    {
        m_timer.stop();
    }
}
//=========================================================
void UGLoadingCircle::setProgress(uint8_t progress)
{
    m_progress = std::min<uint8_t>(progress, 100);
    update();
}
//=========================================================
void UGLoadingCircle::setPixmap(const QPixmap& pixmap)
{
    (void)pixmap;
}
//=========================================================
void UGLoadingCircle::setSpeed(uint32_t speed)
{
    m_toSum = static_cast<uint8_t>(std::max<uint32_t>(1, static_cast<uint32_t>(std::ceil(speed / 100.0f))));
}
//=========================================================
