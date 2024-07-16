#include "ultrabutton.h"

#include <QEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPathStroker>
#include <QResizeEvent>

#include "ultragui.h"

using namespace gui;

//=========================================================
UltraButton::UltraButton(QWidget* parent)
    : ButtonBehavior(parent),
      m_configuration(),  // default constructor
      m_ledState(false),
      m_geometryRequired(true),
      m_animationFlag(false),
      m_blinking(false),
      m_adaptIconsColor(true),
      m_hoveringAnimation(0),
      m_animationToSum(0),
      m_activeText(""),
      m_defaultIcon(),
      m_activeIcon(),
      m_timer(this)
{
    _configureTiming(AS_Normal);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//=========================================================
UltraButton::~UltraButton() { m_timer.stop(); }
//=========================================================
void UltraButton::paintEvent(QPaintEvent* event)
{
    (void)event;

    if (m_geometryRequired) _computeGeometry();

    if (m_configuration.sync) m_ledState = isActive();

    QString str;

    if (m_activeText.isEmpty())
        str = text();
    else
        str = m_ledState ? m_activeText : text();

    //=====================================================COLOR

    QColor fillColor;

    if (m_ledState)
        fillColor = palette().accent().color();
    else
        fillColor = UltraGui::transient(palette().button().color(),
                                        UltraGui::faded(palette().accent().color()), m_hoveringAnimation);

    //=====================================================DRAW

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setFont(m_configuration.textFit ? adjustedFont(str) : font());

    painter.save();
    if (m_configuration.border)
        painter.setPen(QPen(palette().buttonText(), 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
    else
        painter.setPen(QPen(Qt::NoPen));

    painter.setBrush(QBrush(fillColor));
    painter.drawPath(m_frame);
    painter.restore();

    //=====================================================ICON & TEXT

    if (m_defaultIcon.isNull())
    {
        painter.setPen(palette().buttonText().color());

        // left justified:
        // painter.drawText(rect().adjusted(m_leftPadding, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter,
        // string); centered:
        painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, str);
    }
    else
    {
        int xPos = (width() / 2) - (m_defaultIcon.width() / 2);
        int yPos = (height() / 2) - (m_defaultIcon.height() / 2);
        QRect target(xPos, yPos, m_defaultIcon.width(), m_defaultIcon.height());

        if (m_ledState)
            painter.drawPixmap(target, m_activeIcon.isNull() ? m_defaultIcon : m_activeIcon,
                               m_defaultIcon.rect());
        else
            painter.drawPixmap(target, m_defaultIcon, m_defaultIcon.rect());
    }
}
//=========================================================
void UltraButton::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::ApplicationPaletteChange || event->type() == QEvent::PaletteChange)
    {
        if (!m_defaultIcon.isNull() && m_adaptIconsColor)
        {
            _adaptIconsColor();
        }
    }

    QPushButton::changeEvent(event);
}
//=========================================================
void UltraButton::resizeEvent(QResizeEvent* event)
{
    m_geometryRequired = true;
    update();
    QPushButton::resizeEvent(event);
}
//=========================================================
void UltraButton::animate(bool enter)
{
    m_animationFlag = enter;

    if (m_configuration.animateHovering)
        m_timer.start();
    else
        m_hoveringAnimation = enter ? 255 : 0;
}
//=========================================================
QFont UltraButton::adjustedFont(const QString& text)
{
    auto f = font();
    f.setPixelSize(1);
    int textW = QFontMetrics(f).horizontalAdvance(text);

    while (textW <= width() && f.pixelSize() <= height())
    {
        f.setPixelSize(f.pixelSize() + 1);
        textW = QFontMetrics(f).horizontalAdvance(text);
    }

    return f;
}
//=========================================================
void UltraButton::fadeIn() { animate(true); }
//=========================================================
void UltraButton::fadeOut() { animate(false); }
//=========================================================
void UltraButton::_tick()
{
    if (m_animationFlag || m_blinking)
    {
        _sum(m_hoveringAnimation, m_animationToSum, 255);
    }
    else
    {
        _sum(m_hoveringAnimation, m_animationToSum, 255, true);
    }

    if (m_blinking && m_hoveringAnimation == 255)
    {
        m_blinking = false;
    }
    else if (m_hoveringAnimation == 0 || m_hoveringAnimation == 255)
    {
        m_timer.stop();
    }

    repaint();
}
//=========================================================
void UltraButton::_configureTiming(AnimationSpeed speed)
{
    switch (speed)
    {
        case AS_Slow:
            m_timer.setInterval(10);
            m_animationToSum = 5;
            break;

        case AS_Normal:
            m_timer.setInterval(5);
            m_animationToSum = 5;
            break;

        case AS_Fast:
            m_timer.setInterval(5);
            m_animationToSum = 10;
            break;

        case AS_Superfast:
            m_timer.setInterval(5);
            m_animationToSum = 12;
            break;
    }
}
//=========================================================
void UltraButton::_sum(uint8_t& value, uint8_t toSum, uint8_t max, bool subtract)
{
    if (subtract)
    {
        if (toSum > value)
            value = 0;
        else
            value -= toSum;
    }
    else
    {
        if ((uint16_t)((uint16_t)value + (uint16_t)toSum) > (uint16_t)max)
            value = max;
        else
            value += toSum;
    }
}
//=========================================================
void UltraButton::_adaptIconsColor()
{
    QColor tempColor   = palette().buttonText().color();
    QImage defaultIcon = m_defaultIcon.toImage();

    for (int y = 0; y < defaultIcon.height(); y++)
    {
        for (int x = 0; x < defaultIcon.width(); x++)
        {
            tempColor.setAlpha(defaultIcon.pixelColor(x, y).alpha());
            defaultIcon.setPixelColor(x, y, tempColor);
        }
    }

    m_defaultIcon = QPixmap::fromImage(defaultIcon);
    //======================

    if (m_activeIcon.isNull())
    {
        return;
    }

    QImage offIcon = m_activeIcon.toImage();

    for (int y = 0; y < offIcon.height(); y++)
    {
        for (int x = 0; x < offIcon.width(); x++)
        {
            tempColor.setAlpha(offIcon.pixelColor(x, y).alpha());
            offIcon.setPixelColor(x, y, tempColor);
        }
    }

    m_activeIcon = QPixmap::fromImage(offIcon);
}
//=========================================================
void UltraButton::_computeGeometry()
{
    m_frame.clear();
    QRect r = rect();
    r.adjust(1, 1, -1, -1);

    if (m_configuration.radius)
        m_frame.addRoundedRect(r, m_configuration.radius, m_configuration.radius);
    else
        m_frame.addRect(r);

    m_geometryRequired = false;
}
//=========================================================
void UltraButton::setIcons(const QPixmap& defaultIcon, const QPixmap& activeIcon, bool colorAdapting)
{
    if (defaultIcon.isNull()) return;

    if (defaultIcon.width() > width() && defaultIcon.height() > height()) return;

    m_defaultIcon = defaultIcon.copy();

    if (!activeIcon.isNull())
    {
        if (defaultIcon.size() == activeIcon.size()) m_activeIcon = activeIcon.copy();
    }

    m_adaptIconsColor = colorAdapting;

    if (colorAdapting) _adaptIconsColor();

    update();
}
//=========================================================
void UltraButton::blink()
{
    m_blinking = true;
    m_timer.start();
}
//=========================================================
void UltraButton::setState(bool state) { forceState(state); }
//=========================================================
void UltraButton::setLedState(bool state)
{
    m_ledState = state;
    update();
}
//=========================================================
void UltraButton::animateHovering(bool animate, AnimationSpeed speed)
{
    m_configuration.animateHovering = animate;
    _configureTiming(speed);
}
//=========================================================
