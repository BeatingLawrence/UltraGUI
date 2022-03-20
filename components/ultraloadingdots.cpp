#include "ultraloadingdots.h"
#include <QEvent>
#include <QPainter>
#include <QtMath>

using namespace hci;

//=========================================================
void UltraLoadingDots::_computeGeometry(uint8_t n, uint16_t r)
{
    m_points.clear();
    float stepAngle = (M_PI * 2.0f) / (float)n;

    for(uint8_t i = 0; i < n; i++)
    {
        float currentAngle = stepAngle * (float)i;
        QPointF toPush(((float)r * qSin(currentAngle)), ((float)r * qCos(currentAngle)));
        m_points.push_back(qMakePair(toPush, false));
    }
}
//=========================================================
void UltraLoadingDots::_tick()
{
    for(int i = 0; i < m_points.size(); i++)
    {
        if(m_points[i].second)
        {
            m_points[i].second = false;

            if(i == (m_points.size() - 1)) //last element
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
void UltraLoadingDots::paintEvent(QPaintEvent* event)
{
    (void) event;

    if(!m_loading)
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(palette().color(QPalette::WindowText)));
    painter.setBrush(palette().color(QPalette::WindowText));
    QPointF center(width() / 2, height() / 2);  //center of the widget
    painter.translate(center);

    for(auto& el : m_points)
    {
        if(!el.second)
        {
            QPointF point(el.first.x(), -el.first.y());
            m_dot.moveCenter(point);
            painter.drawEllipse(m_dot);
        }
    }

    painter.end();
}
//=========================================================
UltraLoadingDots::UltraLoadingDots(QWidget* parent) :
    QWidget(parent),
    m_dot(0.0f, 0.0f, 2.0f, 2.0f),
    m_loading(false)
{
    m_timer.setInterval(200);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//=========================================================
void UltraLoadingDots::setDots(uint8_t size, uint8_t numberOfDots, uint16_t radiusFromCenter)
{
    if(size == 0 || numberOfDots == 0 || radiusFromCenter == 0)
    {
        return;
    }

    m_dot.setWidth((float)size);
    m_dot.setHeight((float)size);
    _computeGeometry(numberOfDots, radiusFromCenter);
}
//=========================================================
void UltraLoadingDots::setLoading(bool state)
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
void UltraLoadingDots::setSpeed(BlinkingSpeed speed)
{
    m_timer.stop();

    switch(speed)
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

    if(m_loading)
    {
        m_timer.start();
    }
}
//=========================================================
