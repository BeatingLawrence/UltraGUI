#include "ugframe.h"

#include <algorithm>
#include <QFontMetrics>
#include <QPainter>

using namespace gui;

//===============================================
void UGFrame::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QColor c;

    if (m_timer.isActive())
        c = palette().accent().color();
    else
        c = palette().midlight().color();

    const bool hasTitle = !m_title.isEmpty();
    const QFontMetrics metrics(font());
    const int titleHeight = hasTitle ? metrics.height() : 0;
    const int topInset = hasTitle ? std::max(1, (titleHeight / 2) + 1) : 1;
    const QRect frameRect = rect().adjusted(1, topInset, -2, -2);
    constexpr int kRadius = 3;
    constexpr int kSideMargin = 10;
    constexpr int kGapPadding = 6;

    QString text;
    QRect textRect;
    bool hasVisibleTitle = false;

    if (hasTitle)
    {
        const int maxTitleWidth = frameRect.width() - (kSideMargin * 2);
        if (maxTitleWidth > 0)
        {
            text = metrics.elidedText(m_title, Qt::ElideRight, maxTitleWidth);
            const int textWidth = metrics.horizontalAdvance(text);
            if (textWidth > 0)
            {
                int titleX = frameRect.left() + kSideMargin;
                if (m_titleAlignment & Qt::AlignRight)
                    titleX = frameRect.right() - kSideMargin - textWidth;
                else if (m_titleAlignment & Qt::AlignHCenter)
                    titleX = frameRect.center().x() - (textWidth / 2);

                const int minX = frameRect.left() + kSideMargin;
                const int maxX = frameRect.right() - kSideMargin - textWidth;
                titleX = std::clamp(titleX, minX, maxX);

                const int titleY = std::max(0, frameRect.top() - (titleHeight / 2));
                textRect = QRect(titleX, titleY, textWidth, titleHeight);
                hasVisibleTitle = true;
            }
        }
    }

    c.setAlpha(m_alpha);
    painter.setPen(QPen(c, 1));
    painter.setBrush(Qt::NoBrush);
    if (!hasVisibleTitle)
    {
        painter.drawRoundedRect(frameRect, kRadius, kRadius);
    }
    else
    {
        const qreal left = frameRect.left();
        const qreal right = frameRect.right();
        const qreal top = frameRect.top();
        const qreal bottom = frameRect.bottom();
        const qreal radius = static_cast<qreal>(kRadius);
        const qreal topStart = left + radius;
        const qreal topEnd = right - radius;
        qreal gapLeft = static_cast<qreal>(textRect.left() - kGapPadding);
        qreal gapRight = static_cast<qreal>(textRect.right() + kGapPadding + 1);
        gapLeft = std::clamp(gapLeft, topStart, topEnd);
        gapRight = std::clamp(gapRight, topStart, topEnd);

        if (gapLeft > topStart)
            painter.drawLine(QPointF(topStart, top), QPointF(gapLeft, top));
        if (gapRight < topEnd)
            painter.drawLine(QPointF(gapRight, top), QPointF(topEnd, top));

        painter.drawLine(QPointF(left, top + radius), QPointF(left, bottom - radius));
        painter.drawLine(QPointF(right, top + radius), QPointF(right, bottom - radius));
        painter.drawLine(QPointF(left + radius, bottom), QPointF(right - radius, bottom));

        painter.drawArc(QRectF(left, top, 2 * radius, 2 * radius), 90 * 16, 90 * 16);
        painter.drawArc(QRectF(right - (2 * radius), top, 2 * radius, 2 * radius), 0, 90 * 16);
        painter.drawArc(QRectF(left, bottom - (2 * radius), 2 * radius, 2 * radius), 180 * 16, 90 * 16);
        painter.drawArc(QRectF(right - (2 * radius), bottom - (2 * radius), 2 * radius, 2 * radius), 270 * 16,
                        90 * 16);
    }

    if (hasVisibleTitle)
    {
        painter.setBackgroundMode(Qt::TransparentMode);
        painter.setPen(palette().windowText().color());
        painter.setFont(font());
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
    }
}
//===============================================
void UGFrame::timerTick()
{
    m_alpha -= 20;

    if (m_alpha <= 0)
    {
        if (m_blinksRemaining > 0)
        {
            m_blinksRemaining--;

            if (m_blinksRemaining == 0)
            {
                m_alpha = 255;
                stop();
                return;
            }
        }
        m_alpha = 255;
    }

    repaint();
}
//===============================================
UGFrame::UGFrame(QWidget* parent)
    : QFrame(parent),
      m_timer(),
      m_alpha(255),
      m_blinksRemaining(0),
      m_title(),
      m_titleAlignment(Qt::AlignHCenter)
{
    m_timer.setSingleShot(false);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerTick()));
}
//===============================================
UGFrame::~UGFrame() {}
//===============================================
void UGFrame::setTitle(const QString& title, Qt::Alignment align)
{
    if (align & Qt::AlignRight)
        m_titleAlignment = Qt::AlignRight;
    else if (align & Qt::AlignLeft)
        m_titleAlignment = Qt::AlignLeft;
    else
        m_titleAlignment = Qt::AlignHCenter;

    m_title = title.trimmed();
    repaint();
}
//===============================================
void UGFrame::setBorderColor(const QColor& color)
{
    QPalette borderPalette = palette();
    borderPalette.setColor(QPalette::Midlight, color);
    setPalette(borderPalette);
    repaint();
}
//===============================================
void UGFrame::blink(bool repeat)
{
    m_blinksRemaining = repeat ? -1 : 1;
    m_alpha           = 255;
    if (!m_timer.isActive()) m_timer.start(40);
}
//===============================================
void UGFrame::blink(uint8_t blinks)
{
    if (blinks == 0)
    {
        stop();
        return;
    }

    m_blinksRemaining = static_cast<int>(blinks);
    m_alpha           = 255;
    if (!m_timer.isActive()) m_timer.start(40);
}
//===============================================
void UGFrame::stop()
{
    m_timer.stop();
    m_blinksRemaining = 0;
    m_alpha           = 255;
    repaint();
}
//===============================================
