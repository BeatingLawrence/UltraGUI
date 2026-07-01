#include "ugbutton.h"

#include <QEvent>
#include <QFontMetrics>
#include <QIcon>
#include <QPainter>
#include <QPainterPathStroker>
#include <QPixmap>
#include <QResizeEvent>

#include "ultragui.h"

using namespace gui;

UGButton::UGButton(QWidget* parent)
    : ButtonLogic(parent),
      m_configuration(),  // default constructor
      m_ledState(false),
      m_animationFlag(false),
      m_blinking(false),
      m_hoveringAnimation(0),
      m_animationToSum(0),
      m_activeText(""),
      m_timer(this)
{
    _configureTiming(AS_Normal);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
    setSizePolicy({QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed, QSizePolicy::PushButton});
}
//=========================================================
UGButton::~UGButton() { m_timer.stop(); }
//=========================================================
void UGButton::paintEvent(QPaintEvent* event)
{
    (void)event;

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
    {
        const QColor idleColor = palette().button().color();
        const QColor hoverColor = idleColor.lighter(115);
        fillColor = UltraGui::transient(idleColor, hoverColor, m_hoveringAnimation);
    }

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

    const QIcon currentIcon = icon();
    if (!currentIcon.isNull())
    {
        QSize targetSize = iconSize();
        if (!targetSize.isValid() || targetSize.isEmpty())
        {
            const int side = qMax(1, qMin(width(), height()) - 4);
            targetSize = QSize(side, side);
        }

        const QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
        const QIcon::State state = m_ledState ? QIcon::On : QIcon::Off;
        const QPixmap pixmap = currentIcon.pixmap(targetSize, mode, state);
        const QSize drawSize = pixmap.deviceIndependentSize().toSize();
        QRect iconRect(QPoint(0, 0), drawSize);
        iconRect.moveCenter(rect().center());
        painter.drawPixmap(iconRect.topLeft(), pixmap);
    }
    else
    {
        painter.setPen(palette().buttonText().color());
        painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, str);
    }
}
//=========================================================
void UGButton::changeEvent(QEvent* event) { QPushButton::changeEvent(event); }
//=========================================================
void UGButton::resizeEvent(QResizeEvent* event)
{
    _computeGeometry();
    QPushButton::resizeEvent(event);
    update();
}
//=========================================================
void UGButton::animate(bool enter)
{
    m_animationFlag = enter;

    if (m_configuration.animateHovering)
        m_timer.start();
    else
        m_hoveringAnimation = enter ? 255 : 0;
}
//=========================================================
QFont UGButton::adjustedFont(const QString& text)
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
void UGButton::fadeIn() { animate(true); }
//=========================================================
void UGButton::fadeOut() { animate(false); }
//=========================================================
void UGButton::stateChange(bool newState) { emit onChangeEntry(newState, &m_entry); }
//=========================================================
void UGButton::click() { emit onClickEntry(&m_entry); }
//=========================================================
void UGButton::_tick()
{
    if (m_animationFlag || m_blinking)
        _sum(m_hoveringAnimation, m_animationToSum, 255);
    else
        _sum(m_hoveringAnimation, m_animationToSum, 255, true);

    if (m_blinking && m_hoveringAnimation == 255)
        m_blinking = false;
    else if (m_hoveringAnimation == 0 || m_hoveringAnimation == 255)
        m_timer.stop();

    repaint();
}
//=========================================================
void UGButton::_configureTiming(AnimationSpeed speed)
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
void UGButton::_sum(uint8_t& value, uint8_t toSum, uint8_t max, bool subtract)
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
void UGButton::_computeGeometry()
{
    m_frame.clear();
    QRect r = rect();

    if (m_configuration.border)
        r.adjust(1, 1, -1, -1);
    else
        r.adjust(-1, -1, 1, 1);

    if (m_configuration.radius)
        m_frame.addRoundedRect(r, m_configuration.radius, m_configuration.radius);
    else
        m_frame.addRect(r);
}
//=========================================================
void UGButton::blink()
{
    m_blinking = true;
    m_timer.start();
}
//=========================================================
void UGButton::setState(bool state) { forceState(state); }
//=========================================================
void UGButton::setLedState(bool state)
{
    m_ledState = state;
    update();
}
//=========================================================
void UGButton::animateHovering(bool animate, AnimationSpeed speed)
{
    m_configuration.animateHovering = animate;
    _configureTiming(speed);
}
//=========================================================
void UGButton::setEntry(const UltraEntry& entry)
{
    m_entry = entry;
    setText(entry.text);
    setEnabled(!entry.grayed);
    setVisible(!entry.hidden);
}
//=========================================================
const UltraEntry& UGButton::entry() const { return m_entry; }
//=========================================================
QSize UGButton::sizeHint() const { return QSize(40, 20); }
//=========================================================
QSize UGButton::minimumSizeHint() const { return QSize(20, 15); }
//=========================================================
