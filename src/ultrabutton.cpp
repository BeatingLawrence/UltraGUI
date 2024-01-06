#include "ultrabutton.h"

#include <QEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPathStroker>
#include <QResizeEvent>

using namespace gui;

//=========================================================
UltraButton::UltraButton(QWidget* parent)
    : QPushButton(parent),
      m_state(Inactive),
      m_configuration(),  // default constructor
      m_ledState(false),
      m_leftJustify(false),
      m_geometryRequired(true),
      m_iconLeftJustify(false),
      m_shrinkToFit(false),
      m_blinking(false),
      m_adaptIconsColor(true),
      m_animationFlag(false),
      m_leftPadding(5),
      m_iconLeftPadding(1),
      m_shrinkPadding(0),
      m_hoveringAnimation(0),
      m_animationToSum(0),
      m_activeText(""),
      m_defaultIcon(),
      m_activeIcon(),
      m_timer(this)
{
    _configureTiming(AS_Normal);
    useHovering(m_configuration.hovering);
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
        // TODO make the text dimension adapting to the button size and not vice versa
    }

    //=====================================================COLOR
    QColor fillColor;

    if (m_ledState)
        fillColor = palette().color(QPalette::Light);
    else
        fillColor = _computeTransient(palette().color(QPalette::Button), palette().color(QPalette::Midlight), m_hoveringAnimation);

    //=====================================================DRAW

    if (m_configuration.border)
        painter.setPen(QPen(QBrush(palette().color(QPalette::ButtonText)), 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
    else
        painter.setPen(QPen(Qt::NoPen));

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
    transition(Press);
    QPushButton::mousePressEvent(event);
}
//=========================================================
void UltraButton::mouseReleaseEvent(QMouseEvent* event)
{
    transition(Release);
    QPushButton::mouseReleaseEvent(event);
}
//=========================================================
void UltraButton::leaveEvent(QEvent* event)
{
    transition(Leave);
    QPushButton::leaveEvent(event);
}
//=========================================================
void UltraButton::enterEvent(QEnterEvent* event)
{
    transition(Enter);
    QPushButton::enterEvent(event);
}
//=========================================================
void UltraButton::mouseMoveEvent(QMouseEvent* event)
{
    QPushButton::mouseMoveEvent(event);

    bool hit = false;
    if (underMouse()) hit = hitButton(event->position().toPoint());

    if (!hit && (m_state == ActivePressed || m_state == Pressed)) transition(Leave);
}
//=========================================================
void UltraButton::transition(ButtonMachineEvent event)
{
    ButtonMachineState newState = Invalid;

    switch (m_state)
    {
        case Inactive:
            if (event == Enter && m_configuration.hovering)
            {
                newState = Hovering;
                animate(true);
            }
            else if (event == Press && !m_configuration.hovering)
            {
                if (m_configuration.toggle)
                {
                    newState = Pressed;
                    animate(true);
                }
                else
                    newState = ActivePressed;
            }
            else if (event == Release || event == Leave)
            {
                newState = Inactive;  // do not change
                animate(false);
            }
            break;

        case Hovering:
            if (event == Leave)
            {
                newState = Inactive;
                animate(false);
            }
            else if (event == Press)
            {
                if (m_configuration.toggle)
                    newState = Pressed;
                else
                    newState = ActivePressed;
            }
            break;

        case Pressed:
            if (event == Release)
                newState = Active;
            else if (event == Leave)
                newState = Inactive;
            break;

        case Active:
            if (event == Press)
                newState = ActivePressed;
            else if (event == Enter || event == Leave || event == Release)
                newState = Active;  // do not change
            break;

        case ActivePressed:
            if (event == Leave)
            {
                if (m_configuration.toggle)
                    newState = Active;
                else
                    newState = Inactive;
            }
            else if (event == Release)
            {
                if (m_configuration.hovering)
                    newState = Hovering;
                else
                {
                    newState = Inactive;
                    if (m_configuration.toggle) animate(false);
                }
            }
            break;
        default:
            break;
    }

    // qDebug("old state: %s, new state: %s, event: %s", debugState(m_state), debugState(newState), debugEvent(event));

    if (newState != Invalid && m_state != newState)
    {
        notify(m_state, newState);
        m_state = newState;
    }

    update();
}
//=========================================================
void UltraButton::notify(ButtonMachineState oldState, ButtonMachineState newState)
{
    if ((newState == Active && oldState == Pressed) || (newState == ActivePressed && oldState != Active))
    {
        // notify activation
        emit onEnable();
        emit onChange(true);
        if (m_configuration.sync) m_ledState = true;
    }
    if ((newState == Inactive && oldState != Hovering && oldState != Pressed) || (newState == Hovering && oldState == ActivePressed))
    {
        // notify deactivation
        emit onDisable();
        emit onChange(false);
        if (m_configuration.sync) m_ledState = false;
    }
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
const char* UltraButton::debugState(ButtonMachineState state)
{
    switch (state)
    {
        case Inactive:
            return "Inactive";
        case Hovering:
            return "Hovering";
        case Pressed:
            return "Pressed";
        case Active:
            return "Active";
        case ActivePressed:
            return "ActivePressed";
        case Invalid:
            return "Invalid";
    }
}
//=========================================================
const char* UltraButton::debugEvent(ButtonMachineEvent event)
{
    switch (event)
    {
        case Press:
            return "Press";
        case Release:
            return "Release";
        case Enter:
            return "Enter";
        case Leave:
            return "Leave";
    }
}
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
void UltraButton::useHovering(bool use)
{
    setMouseTracking(use);
    m_configuration.hovering = use;
    m_state                  = Inactive;  // force state machine reset
    if (m_configuration.sync) m_ledState = false;
    update();
}
//=========================================================
void UltraButton::blink()
{
    if (m_state != Hovering)
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
void UltraButton::animateHovering(bool animate, AnimationSpeed speed)
{
    m_configuration.animateHovering = animate;
    _configureTiming(speed);
}
//=========================================================
