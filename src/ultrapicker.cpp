#include "ultrapicker.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

using namespace gui;

//=========================================================
void UltraPickerSelector::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
    painter.fillRect(rect(), palette().base());

    QRect r(0, 0, width(), m_step);

    int offset = m_offset % m_step;
    painter.save();
    painter.translate(QPoint(0, -offset));

    for (int i = m_offset / m_step; i < m_entries->size(); i++)
    {
        if (r.translated(QPoint(0, -offset)).contains(m_cursor)) painter.fillRect(r, palette().light());

        painter.drawText(r, Qt::AlignVCenter | Qt::AlignLeft, (*m_entries)[i].entryText);
        r.translate(0, m_step);

        if (r.translated(QPoint(0, (-offset) - m_step)).bottom() > height()) break;
    }

    painter.restore();

    if (m_newEntryAnimTimer.isActive())
    {
        // new entry animation in progress

        QRect animation(0, 0, width(), m_step);
        animation.moveBottom(height());

        QLinearGradient gradient(animation.topLeft(), animation.bottomLeft());
        gradient.setColorAt(0, QColor(0, 255, 0, 0));
        gradient.setColorAt(1, QColor(0, 255, 0, m_newEntryAnimation * 5));
        painter.fillRect(animation, QBrush(gradient));
    }
}
//=========================================================
void UltraPickerSelector::mousePressEvent(QMouseEvent* event)
{
    hide();
    event->accept();

    if (!rect().contains(event->pos(), true)) return;

    int idx = (event->pos().y() + m_offset) / m_step;
    emit onSelect(idx);
    QWidget::mousePressEvent(event);
}
//=========================================================
void UltraPickerSelector::mouseMoveEvent(QMouseEvent* event)
{
    m_cursor = event->pos();
    repaint();
    QWidget::mouseMoveEvent(event);
}
//=========================================================
void UltraPickerSelector::wheelEvent(QWheelEvent* event)
{
    auto m = event->pixelDelta();

    if (m.y() > 0)
        decOffset(qAbs(m.y()));
    else
        incOffset(qAbs(m.y()));

    repaint();
}
//=========================================================
void UltraPickerSelector::hideEvent(QHideEvent* event)
{
    emit onHide();
    QWidget::hideEvent(event);
}
//=========================================================
void UltraPickerSelector::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::FontChange) m_step = height() / font().pixelSize();
    QWidget::changeEvent(event);
}
//=========================================================
QRect UltraPickerSelector::_cappedToScreen(const QRect& r)
{
    QRect screen = QApplication::screenAt(pos())->geometry();
    QRect ret(r);
    if (ret.bottom() > screen.bottom()) ret.setBottom(screen.bottom());
    return ret;
}
//=========================================================
QColor UltraPickerSelector::_randomColor()
{
    return QColor(m_generator.bounded(0, 256), m_generator.bounded(0, 256), m_generator.bounded(0, 256), 100);
}
//=========================================================
int UltraPickerSelector::ptToPx(int pt)
{
    return (int)((double)pt / 72.0f * QApplication::screenAt(pos())->physicalDotsPerInch());
}
//=========================================================
void UltraPickerSelector::incOffset(int x)
{
    int max = maxOffset();

    if (m_offset + x > max)
        m_offset = max;
    else
        m_offset += x;
}
//=========================================================
void UltraPickerSelector::decOffset(int x)
{
    if (m_offset - x < 0)
        m_offset = 0;
    else
        m_offset -= x;
}
//=========================================================
int UltraPickerSelector::maxOffset() { return (m_entries->size() * m_step) - height(); }
//=========================================================
void UltraPickerSelector::_onTimerTick()
{
    if (m_newEntryAnimation)
        m_newEntryAnimation--;
    else
        m_newEntryAnimTimer.stop();

    repaint();
}
//=========================================================
UltraPickerSelector::UltraPickerSelector(QList<UltraEntry>* list, QWidget* parent)
    : QWidget(parent, Qt::Popup),
      m_entries(list),
      m_maxLines(20),
      m_step(0),
      m_offset(0),
      m_generator(),
      m_newEntryAnimTimer(),
      m_newEntryAnimation(0)
{
    m_newEntryAnimTimer.setInterval(10);
    m_newEntryAnimTimer.setSingleShot(false);
    setMouseTracking(true);

    connect(&m_newEntryAnimTimer, SIGNAL(timeout()), this, SLOT(_onTimerTick()));
}
//=========================================================
void UltraPickerSelector::move_resize(QRect r)
{
    m_step = r.height();

    r.setHeight(r.height() * (m_entries->size() > m_maxLines ? m_maxLines : m_entries->size()));
    r = _cappedToScreen(r);

    move(r.topLeft());
    resize(r.size());

    m_offset = 0;
}
//=========================================================
void UltraPickerSelector::setMaxLines(uint32_t lines) { m_maxLines = lines; }
//=========================================================
void UltraPickerSelector::newEntry()
{
    if (!isVisible()) return;

    m_newEntryAnimation = 25;
    m_newEntryAnimTimer.start();
}
//=========================================================
void UltraPicker::_updateSelectorPos()
{
    m_selector.move_resize(QRect(mapToGlobal(QPoint(0, height())), size()));
}
//=========================================================
void UltraPicker::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);

    painter.fillRect(rect(), palette().button());

    if (m_entries.empty()) return;

    QString text = "-";

    if (m_index != -1 && !m_entries.empty()) text = m_entries[m_index].entryText;

    painter.drawText(rect().adjusted(0, 0, -(height() / 2), 0), Qt::AlignVCenter | Qt::AlignLeft, text);

    QRectF icon(0, 0, (float)height() / 4.0f, (float)height() / 4.0f);

    icon.moveCenter(QPointF((float)width() - icon.width(), (float)height() / 2.0f));

    auto p = painter.pen();
    p.setWidth(2);
    p.setColor(palette().light().color());
    painter.setPen(p);

    if (m_open) painter.setBrush(p.color());

    painter.drawEllipse(icon);
}
//=========================================================
void UltraPicker::moveEvent(QMoveEvent* event)
{
    _updateSelectorPos();
    QWidget::moveEvent(event);
}
//=========================================================
void UltraPicker::resizeEvent(QResizeEvent* event)
{
    _updateSelectorPos();
    QWidget::resizeEvent(event);
}
//=========================================================
void UltraPicker::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::FontChange) m_selector.setFont(font());
    QWidget::changeEvent(event);
}
//=========================================================
void UltraPicker::mousePressEvent(QMouseEvent* event)
{
    _updateSelectorPos();
    m_selector.show();
    m_open = true;
    repaint();
    QWidget::mousePressEvent(event);
}
//=========================================================
void UltraPicker::_selected(uint32_t index)
{
    if (index >= m_entries.size()) return;

    m_index = index;
    emit onPick(m_entries[index].entryId);
    update();
}
//=========================================================
void UltraPicker::_selectorHide()
{
    m_open = false;
    repaint();
}
//=========================================================
UltraPicker::UltraPicker(QWidget* parent)
    : QWidget(parent),
      m_entries(),
      m_selector(&m_entries, this),
      m_index(-1),
      m_open(false)
{
    setMouseTracking(false);

    m_selector.hide();
    connect(&m_selector, SIGNAL(onSelect(uint32_t)), this, SLOT(_selected(uint32_t)));
    connect(&m_selector, SIGNAL(onHide()), this, SLOT(_selectorHide()));
}
//=========================================================
UltraPicker::~UltraPicker() {}
//=========================================================
void UltraPicker::addEntry(const UltraEntry& entry)
{
    m_entries.push_back(entry);
    if (m_entries.size() == 1) update();

    m_selector.newEntry();
}
//=========================================================
void UltraPicker::setMaxLines(uint32_t lines) { m_selector.setMaxLines(lines); }
//=========================================================
void UltraPicker::clear()
{
    m_entries.clear();
    m_index = -1;
}
//=========================================================
