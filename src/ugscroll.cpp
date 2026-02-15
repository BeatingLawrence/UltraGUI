#include "ugscroll.h"

#include <math.h>

#include <QMouseEvent>
#include <QPainter>

#include "ultragui.h"

using namespace gui;

//=========================================================
void UGScroll::_computeGeometry()
{
    m_leftButtonClickArea  = QRect(0, 0, height(), height());
    m_rightButtonClickArea = QRect(width() - height(), 0, height(), height());
    m_leftArrow.clear();
    m_rightArrow.clear();
    m_leftButton.clear();
    m_rightButton.clear();
    m_centerMonitor.clear();

    if (m_isRounded)
    {
        uint32_t radius = height() / 2;
        //
        m_leftButton.moveTo(QPoint(height(), 0));
        m_leftButton.lineTo(QPoint(height(), height()));
        m_leftButton.lineTo(QPoint(radius, height()));
        m_leftButton.arcTo(QRect(0, 0, height(), height()), -90, -180);
        m_leftButton.lineTo(QPoint(height(), 0));
        //
        m_rightButton.moveTo(QPoint(width() - height(), 0));
        m_rightButton.lineTo(QPoint(width() - radius, 0));
        m_rightButton.arcTo(QRect(width() - height(), 0, height(), height()), 90, -180);
        m_rightButton.lineTo(QPoint(width() - height(), height()));
        m_rightButton.lineTo(QPoint(width() - height(), 0));
    }
    else
    {
        m_leftButton.addRect(m_leftButtonClickArea);
        m_rightButton.addRect(m_rightButtonClickArea);
    }

    m_centerMonitor.moveTo(QPoint(height(), 0));
    m_centerMonitor.lineTo(QPoint(width() - height(), 0));
    m_centerMonitor.lineTo(QPoint(width() - height(), height()));
    m_centerMonitor.lineTo(QPoint(height(), height()));
    m_centerMonitor.lineTo(QPoint(height(), 0));
    //
    m_labelWidth = width() - (height() * 2);
    //
    m_leftArrow.moveTo(QPoint(height() / 5, height() / 2));
    m_leftArrow.lineTo(QPoint(height() / 2, height() / 5));
    m_leftArrow.moveTo(QPoint(height() / 5, height() / 2));
    m_leftArrow.lineTo(QPoint(height() / 2, height() - (height() / 5)));
    m_leftArrow.translate(1, 0);
    //
    m_rightArrow.moveTo(QPoint(width() - (height() / 5), height() / 2));
    m_rightArrow.lineTo(QPoint(width() - (height() / 2), height() / 5));
    m_rightArrow.moveTo(QPoint(width() - (height() / 5), height() / 2));
    m_rightArrow.lineTo(QPoint(width() - (height() / 2), height() - height() / 5));
    m_rightArrow.translate(-1, 0);
    //
    m_geometryRequired = false;
}
//=========================================================
void UGScroll::_incrementSelector()
{
    uint32_t previous = m_selector;
    m_selector        = _next();

    if (m_selector != previous)
    {
        emit onChange(m_entries[m_selector].id);

        if (m_entries[m_selector].data != nullptr) emit onDataChange(m_entries[m_selector].data);
    }

    m_animation          = 0;
    m_animationDirection = true;
    m_timer.start();
}
//=========================================================
void UGScroll::_decrementSelector()
{
    uint32_t previous = m_selector;
    m_selector        = _prev();

    if (m_selector != previous)
    {
        emit onChange(m_entries[m_selector].id);

        if (m_entries[m_selector].data != nullptr) emit onDataChange(m_entries[m_selector].data);
    }

    m_animation          = m_labelWidth;
    m_animationDirection = false;
    m_timer.start();
}
//=========================================================
uint32_t UGScroll::_getSelectorMoved(uint32_t selector, bool decrement)
{
    if (decrement)
    {
        if (selector == 0)
            return _lastIndex();
        else
            return selector - 1;
    }
    else
    {
        if (selector == _lastIndex())
            return 0;
        else
            return selector + 1;
    }
}
//=========================================================
uint32_t UGScroll::_lastIndex()
{
    if (m_entries.size())
        return m_entries.size() - 1;
    else
        return 0;
}
//=========================================================
void UGScroll::_choosePenColor(QColor& color, QPainter& painter, int32_t x)
{
    int32_t mid  = (width() / 2) - (m_labelWidth / 2);
    float factor = abs((float)mid - (float)x) / (float)mid;

    if (factor > 1.0f)
        factor = 1.0f;
    else if (factor < 0)
        factor = 0.0f;

    color.setAlphaF(1.0f - factor);
    painter.setPen(color);
}
//=========================================================
uint32_t UGScroll::_next()
{
    if (m_hideGrayed)
    {
        uint32_t previous     = m_selector;
        uint32_t tempSelector = _getSelectorMoved(previous);

        while (tempSelector != previous && m_entries[tempSelector].grayed)
            tempSelector = _getSelectorMoved(tempSelector);

        return tempSelector;
    }
    else
        return _getSelectorMoved(m_selector);
}
//=========================================================
uint32_t UGScroll::_prev()
{
    if (m_hideGrayed)
    {
        uint32_t previous     = m_selector;
        uint32_t tempSelector = _getSelectorMoved(previous, true);

        while (tempSelector != previous && m_entries[tempSelector].grayed)
            tempSelector = _getSelectorMoved(tempSelector, true);

        return tempSelector;
    }
    else
        return _getSelectorMoved(m_selector, true);
}
//=========================================================
void UGScroll::_tick()
{
    float toSum = abs((float)(((int)m_labelWidth / 2) - (int)m_animation)) / ((float)m_labelWidth / 2.0f);
    toSum       = (1.0f - toSum) * 8;
    uint32_t intToSum = ceil(toSum);
    intToSum          = intToSum == 0 ? 1 : intToSum;

    if (m_animationDirection)
    {
        m_animation += intToSum;
    }
    else
    {
        m_animation -= intToSum;
    }

    if (m_animation == 0 || m_animation == m_labelWidth)
    {
        m_timer.stop();
    }

    repaint();
}
//=========================================================
void UGScroll::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (m_isRounded) painter.setRenderHint(QPainter::Antialiasing, true);

    if (m_geometryRequired) _computeGeometry();

    painter.fillPath(m_centerMonitor, QBrush(palette().button()));

    if (m_pressed == PE_LeftButtonIsPressed)
        painter.fillPath(m_leftButton, QBrush(palette().accent()));
    else
    {
        if (m_hovering == HE_LeftButtonIsHovering)
            painter.fillPath(m_leftButton, QBrush(UltraGui::faded(palette().accent().color())));
        else
            painter.fillPath(m_leftButton, QBrush(palette().button()));
    }

    if (m_pressed == PE_RightButtonIsPressed)
        painter.fillPath(m_rightButton, QBrush(palette().accent()));
    else
    {
        if (m_hovering == HE_RightButtonIsHovering)
            painter.fillPath(m_rightButton, QBrush(UltraGui::faded(palette().accent().color())));
        else
            painter.fillPath(m_rightButton, QBrush(palette().button()));
    }

    if (m_entries.empty()) return;

    QRect prevTextRect;
    QRect currTextRect(height(), 0, m_labelWidth, height());
    QRect nextTextRect;

    if (m_timer.isActive())
    {
        currTextRect.translate(m_animation, 0);
        prevTextRect          = currTextRect.translated(-(int32_t)m_labelWidth, 0);
        nextTextRect          = currTextRect.translated(m_labelWidth, 0);
        //
        uint32_t prevSelector = _prev();
        uint32_t nextSelector = _next();
        //
        QColor currColor;
        QColor prevColor;
        QColor nextColor;

        if (m_animationDirection)
        {
            currColor = palette().color(
                m_entries[prevSelector].grayed ? QPalette::Disabled : QPalette::Normal, QPalette::ButtonText);
            prevColor = palette().color(m_entries[m_selector].grayed ? QPalette::Disabled : QPalette::Normal,
                                        QPalette::ButtonText);
            nextColor = palette().color(
                m_entries[nextSelector].grayed ? QPalette::Disabled : QPalette::Normal, QPalette::ButtonText);
            //
            _choosePenColor(currColor, painter, currTextRect.x());
            painter.drawText(currTextRect, Qt::AlignCenter, m_entries[prevSelector].text);
            _choosePenColor(prevColor, painter, prevTextRect.x());
            painter.drawText(prevTextRect, Qt::AlignCenter, m_entries[m_selector].text);
            _choosePenColor(nextColor, painter, nextTextRect.x());
            painter.drawText(nextTextRect, Qt::AlignCenter, m_entries[nextSelector].text);
        }
        else
        {
            currColor = palette().color(m_entries[m_selector].grayed ? QPalette::Disabled : QPalette::Normal,
                                        QPalette::ButtonText);
            prevColor = palette().color(
                m_entries[nextSelector].grayed ? QPalette::Disabled : QPalette::Normal, QPalette::ButtonText);
            nextColor = palette().color(
                m_entries[prevSelector].grayed ? QPalette::Disabled : QPalette::Normal, QPalette::ButtonText);
            //
            _choosePenColor(currColor, painter, currTextRect.x());
            painter.drawText(currTextRect, Qt::AlignCenter, m_entries[m_selector].text);
            _choosePenColor(prevColor, painter, prevTextRect.x());
            painter.drawText(prevTextRect, Qt::AlignCenter, m_entries[nextSelector].text);
            _choosePenColor(nextColor, painter, nextTextRect.x());
            painter.drawText(nextTextRect, Qt::AlignCenter, m_entries[prevSelector].text);
        }
    }
    else
    {
        painter.setPen(palette().color(m_entries[m_selector].grayed ? QPalette::Disabled : QPalette::Normal,
                                       QPalette::ButtonText));
        painter.drawText(currTextRect, Qt::AlignCenter, m_entries[m_selector].text);
    }

    painter.setPen(QPen(QBrush(palette().buttonText()), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPath(m_leftArrow);
    painter.drawPath(m_rightArrow);
    painter.end();
}
//=========================================================
void UGScroll::mousePressEvent(QMouseEvent* event)
{
    if (m_leftButtonClickArea.contains(event->pos()))
        m_pressed = UGScroll::PE_LeftButtonIsPressed;
    else if (m_rightButtonClickArea.contains(event->pos()))
        m_pressed = UGScroll::PE_RightButtonIsPressed;

    update();
    QWidget::mousePressEvent(event);
}
//=========================================================
void UGScroll::mouseReleaseEvent(QMouseEvent* event)
{
    event->button();
    if (m_leftButtonClickArea.contains(event->pos()) && m_pressed == UGScroll::PE_LeftButtonIsPressed)
        _decrementSelector();

    else if (m_rightButtonClickArea.contains(event->pos()) && m_pressed == UGScroll::PE_RightButtonIsPressed)
        _incrementSelector();

    m_pressed = UGScroll::PE_NoPressing;
    update();
    QWidget::mouseReleaseEvent(event);
}
//=========================================================
void UGScroll::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_touchScreenMode)
    {
        if (m_leftButtonClickArea.contains(event->pos()))
            m_hovering = UGScroll::HE_LeftButtonIsHovering;
        else if (m_rightButtonClickArea.contains(event->pos()))
            m_hovering = UGScroll::HE_RightButtonIsHovering;
        else
            m_hovering = UGScroll::HE_NoHovering;

        update();
    }

    QWidget::mouseMoveEvent(event);
}
//=========================================================
void UGScroll::leaveEvent(QEvent* event)
{
    if (!m_touchScreenMode)
    {
        m_hovering = UGScroll::HE_NoHovering;
        m_pressed  = UGScroll::PE_NoPressing;
        update();
    }

    QWidget::leaveEvent(event);
}
//=========================================================
void UGScroll::resizeEvent(QResizeEvent* event)
{
    m_geometryRequired = true;
    update();
    QWidget::resizeEvent(event);
}
//=========================================================
UGScroll::UGScroll(QWidget* parent)
    : QWidget(parent),
      m_hovering(UGScroll::HE_NoHovering),
      m_pressed(UGScroll::PE_NoPressing),
      m_selector(0),
      m_hideGrayed(false),
      m_isRounded(true),
      m_geometryRequired(true),
      m_touchScreenMode(false),
      m_animationDirection(false)
{
    m_timer.setInterval(15);
    setMouseTracking(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//=========================================================
UGScroll::~UGScroll() { m_timer.stop(); }
//=========================================================
void UGScroll::addEntry(const UltraEntry& entry)
{
    m_entries.push_back(entry);
    update();
}
//=========================================================
bool UGScroll::setEntries(const QVector<UltraEntry>& entries)
{
    if (entries.size() == 0) return false;

    for (auto& el : entries)
        if (el.id == InvalidID) return false;

    clearEntries();
    m_entries  = entries;
    m_selector = 0;
    update();
    return true;
}
//=========================================================
void UGScroll::clearEntries()
{
    m_entries.clear();
    m_selector = 0;
    update();
}
//=========================================================
bool UGScroll::setGrayed(int32_t entryID, bool grayed)
{
    if (entryID == InvalidID) return false;

    for (auto& el : m_entries)
        if (el.id == entryID)
        {
            el.grayed = grayed;
            update();
            return true;
        }

    return false;
}
//=========================================================
void UGScroll::setAllGrayed(bool grayed)
{
    for (auto& el : m_entries) el.grayed = grayed;

    update();
}
//=========================================================
void UGScroll::hideGrayedEntries(bool hide)
{
    m_hideGrayed = hide;
    update();
}
//=========================================================
void UGScroll::setTouchScreenMode(bool state)
{
    m_touchScreenMode = state;
    setMouseTracking(!state);
    m_hovering = HE_NoHovering;
}
//=========================================================
void UGScroll::setRounded(bool isRounded)
{
    if (m_isRounded != isRounded)
    {
        m_geometryRequired = true;
        m_isRounded        = isRounded;
        update();
    }
}
//=========================================================
