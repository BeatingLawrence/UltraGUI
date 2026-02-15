#include "ugselector.h"

#include <math.h>

#include <QMouseEvent>
#include <QPainter>

using namespace gui;

//=========================================================
QRectF UGSelector::_animationToRect()
{
    uint32_t selectorWidth  = 4;
    uint32_t selectorHeight = m_sectionHeight;
    uint32_t maxValue       = m_entries.size() * 100;
    uint32_t y              = (uint32_t)(((float)m_animation / (float)maxValue) * (float)(height()));
    QRect toReturn(0, y, selectorWidth, selectorHeight);
    return toReturn;
}
//=========================================================
uint32_t UGSelector::_selectorToAnimation() { return (m_selector * 100); }
//=========================================================
void UGSelector::_computeAnimationBoundaries()
{
    m_maxValue = m_entries.size() * 100;
    if (m_animation > m_maxValue) m_animation = m_maxValue;
}
//=========================================================
void UGSelector::_select(uint32_t idx)
{
    m_selector = idx;
    m_timer.start();
}
//=========================================================
void UGSelector::_notify()
{
    if (m_entries.empty() || m_selector == -1) return;
    emit onClick(m_entries[m_selector].id, m_entries[m_selector].data);
    if (m_entries[m_selector].data != nullptr) emit onClickForStacked((QWidget*)m_entries[m_selector].data);
}
//=========================================================
void UGSelector::_resetSelector() { m_selector = -1; }
//=========================================================
void UGSelector::_tick()
{
    uint32_t targetAnimation = _selectorToAnimation();
    float toSum              = (float)labs((int)targetAnimation - (int)m_animation) / 5.0f;

    if (targetAnimation > m_animation)
        m_animation += ceil(toSum);
    else if (targetAnimation < m_animation)
        m_animation -= ceil(toSum);
    else if (m_timer.isActive())
        m_timer.stop();

    repaint();
}
//=========================================================
void UGSelector::paintEvent(QPaintEvent* event)
{
    (void)event;

    if (m_entries.empty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //=================================MATH
    m_sectionHeight = uint32_t((float)height() / (float)m_entries.size());
    QRect hoveringRect(0, 0, width(), m_sectionHeight);
    //=================================DRAW
    painter.setFont(font());

    for (int i = 0; i < m_entries.size(); i++)
    {
        QPen painterPen(QBrush(), 1);

        if (!m_currentMouseCursor.isNull() && hoveringRect.contains(m_currentMouseCursor, true))
        {
            QColor color = palette().buttonText().color();
            color.setAlpha(30);
            painter.setBrush(QBrush(color));
            painter.setPen(painterPen);
            painter.drawRoundedRect(hoveringRect, 5, 5);
            painter.setBrush(QBrush());
        }

        painterPen.setColor(palette().color(m_entries[i].grayed ? QPalette::Disabled : QPalette::Normal,
                                            QPalette::ButtonText));
        painter.setPen(painterPen);
        painter.drawText(hoveringRect.adjusted(10, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft,
                         m_entries[i].text);
        hoveringRect.translate(0, m_sectionHeight);
    }

    if (m_selector == -1) return;
    painter.setPen(QPen(QBrush(), 1));
    painter.setBrush(QBrush(palette().accent()));
    painter.drawRoundedRect(_animationToRect(), 2, 2);
    painter.end();
}
//=========================================================
void UGSelector::mouseMoveEvent(QMouseEvent* event)
{
    if (event->type() == QEvent::Type::MouseMove)
    {
        QPoint point = event->pos();

        if (point.x() > 0 && point.y() > 0)
        {
            m_currentMouseCursor = event->pos();
            repaint();
        }
    }

    QWidget::mouseMoveEvent(event);
}
//=========================================================
void UGSelector::mousePressEvent(QMouseEvent* event)
{
    m_clicked = true;
    QWidget::mousePressEvent(event);
}
//=========================================================
void UGSelector::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_clicked)
    {
        m_clicked = false;
        QRect trackingArea(0, 0, width(), m_sectionHeight);

        for (qsizetype i = 0; i < m_entries.size(); i++)
        {
            QPoint point = event->pos();

            if (trackingArea.contains(point, true))
            {
                if ((m_selector != i) && !m_entries[i].grayed)
                {
                    _select(i);
                    _notify();
                }

                break;
            }

            trackingArea.translate(0, (int)m_sectionHeight);
        }
    }

    QWidget::mouseReleaseEvent(event);
}
//=========================================================
void UGSelector::leaveEvent(QEvent* event)
{
    m_currentMouseCursor = QPoint();
    m_clicked            = false;
    repaint();
    QWidget::leaveEvent(event);
}
//=========================================================
UGSelector::UGSelector(QWidget* parent)
    : QWidget(parent),
      m_currentMouseCursor(),
      m_selector(-1),
      m_animation(0),
      m_timer(this),
      m_clicked(false),
      m_sectionHeight(0),
      m_maxValue(0)
{
    m_timer.setInterval(10);
    setMouseTracking(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//=========================================================
UGSelector::~UGSelector() { m_timer.stop(); }
//=========================================================
void UGSelector::addEntry(const UltraEntry& entry, bool active)
{
    m_entries.push_back(entry);
    _computeAnimationBoundaries();

    if (active) m_selector = m_entries.size() - 1;
    update();
}
//=========================================================
bool UGSelector::setEntries(const QVector<UltraEntry>& entries)
{
    if (entries.size() == 0) return false;

    for (auto& el : entries)
        if (el.id == InvalidID) return false;

    clearEntries();
    m_entries = entries;
    _resetSelector();
    _computeAnimationBoundaries();
    update();
    return true;
}
//=========================================================
void UGSelector::clearEntries()
{
    m_entries.clear();
    _resetSelector();
    _computeAnimationBoundaries();
    update();
}
//=========================================================
bool UGSelector::setGrayed(int32_t entryID, bool grayed)
{
    if (entryID == InvalidID) return false;

    for (uint8_t i = 0; i < m_entries.size(); i++)
        if (m_entries[i].id == entryID)
        {
            m_entries[i].grayed = grayed;
            if (grayed && i == m_selector)
            {
                _select(0);
                _notify();
            }
            update();
            return true;
        }

    return false;
}
//=========================================================
void UGSelector::setAllGrayed(bool grayed)
{
    for (auto& el : m_entries) el.grayed = grayed;

    if (grayed) _select(0);

    update();
}
//=========================================================
void UGSelector::setActiveEntry(int32_t entryID)
{
    for (uint32_t i = 0; i < m_entries.size(); i++)
        if (m_entries[i].id == entryID)
        {
            if (m_entries[i].grayed) return;
            _select(i);
            return;
        }
}
//=========================================================
const UltraEntry* UGSelector::current()
{
    if (m_entries.empty() || m_selector == -1) return nullptr;
    return &m_entries[m_selector];
}
//=========================================================
