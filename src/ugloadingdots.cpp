#include "ugloadingdots.h"

#include <algorithm>
#include <QEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QtMath>

using namespace gui;

//=========================================================
void UGLoadingDots::_computeGeometry(uint8_t n, uint16_t r)
{
    m_points.clear();
    float stepAngle = (M_PI * 2.0f) / (float)n;

    for (uint8_t i = 0; i < n; i++)
    {
        float currentAngle = stepAngle * (float)i;
        QPointF toPush(((float)r * qSin(currentAngle)), ((float)r * qCos(currentAngle)));
        m_points.push_back(qMakePair(toPush, false));
    }
}
//=========================================================
void UGLoadingDots::_updateGeometry()
{
    const int minSide = std::max(1, std::min(width(), height()));
    constexpr uint8_t dots = 10;

    const uint8_t dotSize = static_cast<uint8_t>(std::max(1, minSide / 8));

    m_dot.setWidth(static_cast<float>(dotSize));
    m_dot.setHeight(static_cast<float>(dotSize));

    const uint16_t radius = static_cast<uint16_t>(std::max(0, (minSide / 2) - (dotSize / 2) - 1));

    _computeGeometry(dots, radius);
}
//=========================================================
void UGLoadingDots::_tick()
{
    for (int i = 0; i < m_points.size(); i++)
    {
        if (m_points[i].second)
        {
            m_points[i].second = false;

            if (i == (m_points.size() - 1))  // last element
            {
                m_points[0].second = true;
            }
            else
            {
                m_points[i + 1].second = true;
            }

            repaint();
            return;
        }
    }

    m_points[0].second = true;
    repaint();
}
//=========================================================
void UGLoadingDots::paintEvent(QPaintEvent* event)
{
    (void)event;

    if (!m_loading) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(palette().windowText().color()));
    painter.setBrush(palette().windowText().color());
    QPointF center(width() / 2, height() / 2);  // center of the widget
    painter.translate(center);

    for (auto& el : m_points)
    {
        if (!el.second)
        {
            QPointF point(el.first.x(), -el.first.y());
            m_dot.moveCenter(point);
            painter.drawEllipse(m_dot);
        }
    }

    painter.end();
}
//=========================================================
UGLoadingDots::UGLoadingDots(QWidget* parent)
    : QWidget(parent),
      m_dot(0.0f, 0.0f, 2.0f, 2.0f),
      m_loading(false)
{
    m_timer.setInterval(200);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
    _updateGeometry();
}
//=========================================================
void UGLoadingDots::setLoading(bool state)
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
void UGLoadingDots::setSpeed(BlinkingSpeed speed)
{
    m_timer.stop();

    switch (speed)
    {
        case BS_Slow:
            m_timer.setInterval(300);
            break;

        case BS_Normal:
            m_timer.setInterval(200);
            break;

        case BS_Fast:
            m_timer.setInterval(150);
            break;

        case BS_Superfast:
            m_timer.setInterval(100);
            break;
    }

    if (m_loading)
    {
        m_timer.start();
    }
}
//=========================================================
void UGLoadingDots::resizeEvent(QResizeEvent* event)
{
    _updateGeometry();
    QWidget::resizeEvent(event);
}
//=========================================================
gui::UGLoadingDots::~UGLoadingDots() {}
//=========================================================
