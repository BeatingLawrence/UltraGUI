#include "ultralogs.h"

#include <QPaintEvent>
#include <QPainter>

using namespace gui;

unsigned int UltraLogs::Indentation(10);

//================================================================
uint8_t UltraLogs::_alpha(unsigned int index, unsigned int lastIndex)
{
    if (index > lastIndex)
    {
        return 0;
    }

    uint8_t toReturn = (uint8_t)(((float)index / (float)lastIndex) * 255.0f);
    return (255 - toReturn);
}
//================================================================
void UltraLogs::_tick()
{
    m_animation++;
    repaint();

    if (m_animation == 20)  // animation end
    {
        m_animation = 0;

        if (m_queue.empty())
        {
            m_timer.stop();
        }
        else
        {
            m_entries.push_front(qMakePair(m_queue.back().first, m_queue.back().second));
            m_queue.pop_back();

            if ((uint32_t)m_entries.size() > (m_numberOfLines + 1))
            {
                m_entries.pop_back();
            }
        }
    }
}
//================================================================
void UltraLogs::mouseDoubleClickEvent(QMouseEvent* event)
{
    QWidget::mouseDoubleClickEvent(event);
    emit onDoubleClick();
}
//================================================================
void UltraLogs::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    QColor standardColor = palette().color(QPalette::WindowText);
    painter.setPen(QPen(QBrush(standardColor), 1));
    QFont widgetFont = font();
    widgetFont.setPointSize((height() / m_numberOfLines) - 2);
    painter.setFont(widgetFont);

    //=====================================================BORDER

    if (m_border)
    {
        painter.drawLine(0, 0, 0, height());
        painter.drawLine(0, 0, width(), 0);
        painter.drawLine(width() - 1, 0, width() - 1, height());
    }

    //=====================================================MATH
    unsigned int step = height() / m_numberOfLines;
    unsigned int yPos = step;
    unsigned int diff = step - (unsigned int)(((float)m_animation / 20.0f) * (float)step);
    //=====================================================DRAW

    for (auto& el : m_entries)
    {
        QColor color;

        if (el.second.isValid())
        {
            color = el.second;
        }
        else
        {
            color = standardColor;
        }

        unsigned int y;

        if (m_timer.isActive())
        {
            y = yPos - diff;
        }
        else
        {
            y = yPos;
        }

        color.setAlpha(_alpha(y, height() + step));
        QPen pen(color);
        painter.setPen(pen);
        painter.drawText(UltraLogs::Indentation, y, el.first);
        yPos += step;
    }
}
//================================================================
UltraLogs::UltraLogs(QWidget* parent)
    : QWidget(parent),
      m_animation(0),
      m_timer(this),
      m_border(true),
      m_aura(true),
      m_numberOfLines(5)
{
    m_timer.setInterval(10);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//================================================================
UltraLogs::~UltraLogs() { m_timer.stop(); }
//================================================================
void UltraLogs::push(const QString& message, const QColor& textColor)
{
    if (message.isEmpty())
    {
        return;
    }

    if (m_timer.isActive())
    {
        m_queue.push_front(qMakePair(message, textColor));
    }
    else
    {
        m_entries.push_front(qMakePair(message, textColor));

        if (((uint32_t)m_entries.size()) > (m_numberOfLines + 1))
        {
            m_entries.pop_back();
        }

        m_timer.start();
    }
}
//================================================================
void UltraLogs::clear()
{
    m_entries.clear();
    m_queue.clear();
    update();
}
//================================================================
