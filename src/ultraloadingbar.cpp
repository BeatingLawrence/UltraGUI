#include "ultraloadingbar.h"

#include <math.h>

#include <QFontDatabase>
#include <QPainter>
#include <QPainterPath>
#include <QStaticText>
#include <cmath>

using namespace gui;

const int UltraLoadingBar::RefreshPeriodMs(20);

//=========================================================
UltraLoadingBar::UltraLoadingBar(QWidget* parent)
    : QWidget(parent),
      m_timer(this),
      m_setPoint(0),
      m_progress(0),
      m_pixels(0),
      m_rounded(false),
      m_drawBackGround(true)
{
    setAutoFillBackground(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_onTimerTick()));
    m_timer.setInterval(UltraLoadingBar::RefreshPeriodMs);
}
//=========================================================
UltraLoadingBar::~UltraLoadingBar() { m_timer.stop(); }
//=========================================================
void UltraLoadingBar::setProgressBar(int status)
{
    m_setPoint = (uint32_t)status;

    if (m_setPoint > 1000)
    {
        m_setPoint = 1000;
    }

    if (!m_timer.isActive())
    {
        m_timer.start();
    }
}
//=========================================================
void UltraLoadingBar::setProgressBarToZero()
{
    m_progress = 0;
    m_setPoint = 0;
    m_pixels   = 0;
    update();
}
//=========================================================
void UltraLoadingBar::_onTimerTick()
{
    float toSum = (float)labs((int)m_setPoint - (int)m_progress) / 10.0f;

    if (m_setPoint > m_progress)
    {
        m_progress += ceil(toSum);
    }
    else if (m_setPoint < m_progress)
    {
        m_progress -= ceil(toSum);
    }
    else if (m_timer.isActive())
    {
        m_timer.stop();
    }

    float factor = (float)m_progress / 1000.0f;

    if (m_rounded)
    {
        m_pixels = ((float)width() - height()) * factor;
    }
    else
    {
        m_pixels = ((float)width()) * factor;
    }

    static uint8_t lastPercentage = 0;
    uint8_t percentage            = (uint8_t)(factor * 100.0f);

    if (lastPercentage != percentage)
    {
        lastPercentage = percentage;
        emit onPercentageChange(percentage);
    }

    repaint();
}
//=========================================================
void UltraLoadingBar::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (m_rounded)
    {
        painter.setRenderHint(QPainter::Antialiasing, true);
    }

    QPainterPath background;
    QPainterPath bar;

    if (m_drawBackGround)
    {
        if (m_rounded)
        {
            background.moveTo(QPoint(height() / 2, 0));
            background.lineTo(width() - 1 - (height() / 2), 0);
            background.arcTo(QRect(width() - 1 - height(), 0, height(), height()), 90, -180);
            background.lineTo(height() / 2, height());
            background.arcTo(QRect(0, 0, height(), height()), -90, -180);
        }
        else
        {
            background.addRect(QRect(0, 0, width(), height()));
        }

        painter.fillPath(background, QBrush(palette().color(QPalette::Dark)));
    }

    if (m_pixels != 0)
    {
        if (m_rounded)
        {
            bar.moveTo(QPoint(height() / 2, 0));
            bar.lineTo(m_pixels - 1 + height() / 2, 0);
            bar.arcTo(QRect(m_pixels - 1, 0, height(), height()), 90, -180);
            bar.lineTo(height() / 2, height());
            bar.arcTo(QRect(0, 0, height(), height()), -90, -180);
        }
        else
        {
            bar.addRect(QRect(0, 0, m_pixels, height()));
        }

        painter.fillPath(bar, QBrush(palette().color(QPalette::Light)));
    }

    painter.end();
}
//=========================================================
