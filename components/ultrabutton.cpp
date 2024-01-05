#include "ultrabutton.h"

#include <QEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPathStroker>
#include <QResizeEvent>

using namespace hci;

//=========================================================
UltraButton::UltraButton(QWidget* parent)
    : QPushButton(parent),
      m_ledState(false),
      m_buttonState(false),
      m_toggleMode(false),
      m_synchronous(true),
      m_hovering(false),
      m_leftJustify(false),
      m_iconLeftJustify(false),
      m_shrinkToFit(false),
      m_useHoveringAnimation(true),
      m_blinking(false),
      m_adaptIconsColor(true),
      m_hasBorder(false),
      m_geometryRequired(true),
      m_touchScreenMode(false),
      m_activeText(""),
      m_leftPadding(5),
      m_iconLeftPadding(1),
      m_shrinkPadding(0),
      m_hoveringAnimation(0),
      m_animationToSum(0),
      m_defaultIcon(),
      m_activeIcon(),
      m_timer(this)
{
    _configureTiming(UltraButton::AS_Normal);

    // TODO: add macro that set mouse tracking to off if the system is MACOS
    setMouseTracking(!m_touchScreenMode);
    //
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//=========================================================
UltraButton::~UltraButton() { m_timer.stop(); }
//=========================================================
void UltraButton::paintEvent(QPaintEvent* event)
{
    (void)event;

    if (m_geometryRequired) _computeGeometry();

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setFont(font());

    if (m_shrinkToFit && !text().isEmpty())
    {
        int widthOfText = painter.fontMetrics().horizontalAdvance(text());
        setFixedWidth(widthOfText + m_shrinkPadding);
    }

    //=====================================================COLOR
    QColor fillColor;

    if (m_ledState)
        fillColor = palette().color(QPalette::Light);
    else
        fillColor = _computeTransient(palette().color(QPalette::Button), palette().color(QPalette::Midlight), m_hoveringAnimation);

    //=====================================================DRAW

    if (m_hasBorder)
        painter.setPen(QPen(QBrush(palette().color(QPalette::ButtonText)), 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
    else
        painter.setPen(QPen());

    painter.setBrush(QBrush(fillColor));
    painter.drawPath(m_frame);

    //=====================================================ICON & TEXT

    if (m_defaultIcon.isNull())
    {
        if (m_ledState)
        {
            if (m_activeText.isEmpty() && !text().isEmpty())
                _writeString(painter, text(), palette().color(QPalette::ButtonText));
            else
                _writeString(painter, m_activeText, palette().color(QPalette::ButtonText));
        }
        else
        {
            if (text().isEmpty() && !m_activeText.isEmpty())
                _writeString(painter, m_activeText, palette().color(QPalette::ButtonText));
            else
                _writeString(painter, text(), palette().color(QPalette::ButtonText));
        }
    }
    else
    {
        int xPos = m_iconLeftJustify ? m_iconLeftPadding : (width() / 2) - (m_defaultIcon.width() / 2);
        int yPos = (height() / 2) - (m_defaultIcon.height() / 2);
        QRect target(xPos, yPos, m_defaultIcon.width(), m_defaultIcon.height());

        if (m_ledState)
            painter.drawPixmap(target, m_activeIcon.isNull() ? m_defaultIcon : m_activeIcon, m_defaultIcon.rect());
        else
            painter.drawPixmap(target, m_defaultIcon, m_defaultIcon.rect());
    }

    additionalPainting(painter);
}
//=========================================================
QColor UltraButton::_computeTransient(const QColor& first, const QColor& second, uint8_t selector)
{
    float mR  = second.redF() - first.redF();
    float mG  = second.greenF() - first.greenF();
    float mB  = second.blueF() - first.blueF();
    float mA  = second.alphaF() - first.alphaF();
    uint8_t R = ((float)selector * mR) + first.red();
    uint8_t G = ((float)selector * mG) + first.green();
    uint8_t B = ((float)selector * mB) + first.blue();
    uint8_t A = ((float)selector * mA) + first.alpha();
    return QColor(R, G, B, A);
}
//=========================================================
void UltraButton::additionalPainting(QPainter& painter)
{
    (void)painter;
    // noop
}
//=========================================================
void UltraButton::hideEvent(QHideEvent* event)
{
    m_hovering          = false;
    m_blinking          = false;
    m_hoveringAnimation = 0;
    m_timer.stop();
    QPushButton::hideEvent(event);
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
void UltraButton::mousePressEvent(QMouseEvent* event)
{
    if (!m_toggleMode)
    {
        changeTo(true);
    }
    else if (m_touchScreenMode)
    {
        m_hoveringAnimation = 255;
    }

    update();
    QPushButton::mousePressEvent(event);
}
//=========================================================
void UltraButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (!m_toggleMode)
    {
        changeTo(false);
    }
    else
    {
        if (m_touchScreenMode)
        {
            m_hoveringAnimation = 0;
            changeTo(!m_buttonState);
        }
        else if (m_hovering)
            changeTo(!m_buttonState);
    }

    update();

    QPushButton::mouseReleaseEvent(event);
}
//=========================================================
void UltraButton::mouseMoveEvent(QMouseEvent* event)
{
    bool hit = false;
    // if (underMouse()) hit = hitButton(event->position().toPoint());

    if (underMouse()) hit = m_inputRect.contains(event->position().toPoint());

    if (hit != m_hovering)
    {
        if (hit)
            cursorEntered();
        else
            cursorLeft();

        update();
        m_hovering = hit;
    }

    QPushButton::mouseMoveEvent(event);
}
//=========================================================
void UltraButton::leaveEvent(QEvent* event)
{
    cursorLeft();
    QPushButton::leaveEvent(event);
}
//=========================================================
void UltraButton::cursorEntered()
{
    if (!m_touchScreenMode)
    {
        m_blinking = false;

        if (m_useHoveringAnimation)
            m_timer.start();
        else
            m_hoveringAnimation = 255;
    }
}
//=========================================================
void UltraButton::cursorLeft()
{
    if (m_touchScreenMode || !m_useHoveringAnimation)
        m_hoveringAnimation = 0;
    else
        m_timer.start();

    if (!m_toggleMode) changeTo(false);

    update();
}
//=========================================================
void UltraButton::changeTo(bool newState)
{
    if (m_buttonState != newState)
    {
        m_buttonState = newState;
        if (newState)
            emit onEnable();
        else
            emit onDisable();

        emit onChange(newState);

        if (m_synchronous)
        {
            activate(newState);
        }
    }
}
//=========================================================
void UltraButton::_tick()
{
    if (m_timer.isActive())
    {
        if (m_hovering || m_blinking)
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
}
//=========================================================
void UltraButton::_writeString(QPainter& painter, const QString& string, const QColor& color)
{
    QPen pen = painter.pen();
    pen.setColor(color);
    painter.setPen(pen);

    if (m_leftJustify)
    {
        painter.drawText(rect().adjusted(m_leftPadding, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, string);
    }
    else
    {
        painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, string);
    }
}
//=========================================================
void UltraButton::_configureTiming(AnimationSpeed speed)
{
    switch (speed)
    {
        case UltraButton::AS_Slow:
            m_timer.setInterval(10);
            m_animationToSum = 5;
            break;

        case UltraButton::AS_Normal:
            m_timer.setInterval(5);
            m_animationToSum = 5;
            break;

        case UltraButton::AS_Fast:
            m_timer.setInterval(5);
            m_animationToSum = 10;
            break;

        case UltraButton::AS_Superfast:
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
        {
            value = 0;
        }
        else
        {
            value -= toSum;
        }
    }
    else
    {
        if ((uint16_t)((uint16_t)value + (uint16_t)toSum) > (uint16_t)max)
        {
            value = max;
        }
        else
        {
            value += toSum;
        }
    }
}
//=========================================================
void UltraButton::_adaptIconsColor()
{
    QColor tempColor   = palette().color(QPalette::ButtonText);
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
    auto r = rect();
    m_frame.clear();
    m_frame.addRect(r);
    int vMargins       = r.height() / 10;
    int hMargins       = r.width() / 10;
    m_inputRect        = r.marginsRemoved({hMargins, vMargins, hMargins, vMargins});
    m_geometryRequired = false;
}
//=========================================================
void UltraButton::setIcons(const QPixmap& defaultIcon, const QPixmap& activeIcon, bool colorAdapting)
{
    if (defaultIcon.isNull())
    {
        return;
    }

    if (defaultIcon.width() > width() && defaultIcon.height() > height())
    {
        return;
    }

    m_defaultIcon = defaultIcon.copy();

    if (!activeIcon.isNull())
    {
        if (defaultIcon.size() == activeIcon.size())
        {
            m_activeIcon = activeIcon.copy();
        }
    }

    m_adaptIconsColor = colorAdapting;

    if (colorAdapting)
    {
        _adaptIconsColor();
    }

    update();
}
//=========================================================
void UltraButton::blink()
{
    if (!m_hovering)
    {
        m_blinking = true;
        m_timer.start();
    }
}
//=========================================================
void UltraButton::activate(bool active)
{
    m_ledState = active;
    update();
}
//=========================================================
void UltraButton::deactivate() { activate(false); }
//=========================================================
void UltraButton::animateHovering(bool animate, const AnimationSpeed& speed)
{
    m_useHoveringAnimation = animate;
    _configureTiming(speed);
}
//=========================================================
