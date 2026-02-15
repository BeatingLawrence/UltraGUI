#include "ugpicker.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

#include "ultragui.h"

#define MARGIN_LEFT 5
#define MARGIN_RIGHT 5

using namespace gui;

//=========================================================
void UGPickerSelector::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);

    painter.save();
    painter.setBrush(m_palette.base());
    painter.setPen(QPen(m_palette.base().color()));
    painter.drawRect(rect());
    painter.restore();

    QRect r(MARGIN_LEFT, 0, width() - MARGIN_RIGHT, m_step);
    int offset = m_offset % m_step;

    painter.save();
    painter.translate(QPoint(0, -offset));
    painter.setPen(QPen(m_palette.text(), 1));

    for (int i = m_offset / m_step; i < m_entries->size(); i++)
    {
        if (r.translated(QPoint(0, -offset)).contains(m_cursor))
            painter.fillRect(r, UltraGui::faded(m_palette.accent().color()));

        painter.drawText(r, Qt::AlignVCenter | Qt::AlignLeft, (*m_entries)[i].text);
        r.translate(0, m_step);

        if (r.translated(QPoint(0, (-offset) - m_step)).bottom() > height()) break;
    }

    painter.restore();
    QColor color(m_palette.accent().color());

    if (m_newEntryAnimTimer.isActive())
    {
        // new entry animation in progress

        QRect animation(0, 0, width(), m_step);
        animation.moveBottom(height());

        QLinearGradient gradient(animation.topLeft(), animation.bottomLeft());
        color.setAlpha(0);
        gradient.setColorAt(0, color);
        color.setAlpha(m_newEntryAnimation * 5);
        gradient.setColorAt(1, color);
        painter.fillRect(animation, QBrush(gradient));
    }
}
//=========================================================
void UGPickerSelector::mousePressEvent(QMouseEvent* event)
{
    hide();
    event->accept();

    if (!rect().contains(event->pos(), true)) return;

    int idx = (event->pos().y() + m_offset) / m_step;
    emit onSelect(idx);
    QWidget::mousePressEvent(event);
}
//=========================================================
void UGPickerSelector::mouseMoveEvent(QMouseEvent* event)
{
    m_cursor = event->pos();
    repaint();
    QWidget::mouseMoveEvent(event);
}
//=========================================================
void UGPickerSelector::wheelEvent(QWheelEvent* event)
{
    auto m = event->pixelDelta();

    if (m.y() > 0)
        decOffset(qAbs(m.y()));
    else
        incOffset(qAbs(m.y()));

    repaint();
}
//=========================================================
void UGPickerSelector::hideEvent(QHideEvent* event)
{
    emit onHide();
    QWidget::hideEvent(event);
}
//=========================================================
void UGPickerSelector::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::FontChange) m_step = height() / font().pixelSize();
    QWidget::changeEvent(event);
}
//=========================================================
QRect UGPickerSelector::_cappedToScreen(const QRect& r)
{
    QRect screen = QApplication::screenAt(pos())->geometry();
    QRect ret(r);
    if (ret.bottom() > screen.bottom()) ret.setBottom(screen.bottom());
    return ret;
}
//=========================================================
void UGPickerSelector::incOffset(float x)
{
    int max = maxOffset();
    x       = ceil(x);

    if (m_offset + x > max)
    {
        m_offset       = max;
        m_snapToBottom = true;  // snap
    }
    else
        m_offset += x;
}
//=========================================================
void UGPickerSelector::decOffset(float x)
{
    if (m_offset - x < 0)
        m_offset = 0;
    else
        m_offset -= x;

    m_snapToBottom = false;  // unsnap
}
//=========================================================
int UGPickerSelector::maxOffset() { return (m_entries->size() * m_step) - height(); }
//=========================================================
void UGPickerSelector::_onTimerTick()
{
    if (m_newEntryAnimation)
        m_newEntryAnimation--;
    else
        m_newEntryAnimTimer.stop();

    if (m_snapToBottom)
        if (m_offset < maxOffset()) incOffset((float)m_step / 25.0f);

    repaint();
}
//=========================================================
UGPickerSelector::UGPickerSelector(QList<UltraEntry>* list, QWidget* parent)
    : QWidget(parent, Qt::Popup),
      m_entries(list),
      m_maxLines(20),
      m_step(0),
      m_offset(0),
      m_newEntryAnimTimer(),
      m_newEntryAnimation(0),
      m_snapToBottom(false),
      m_palette()
{
    m_newEntryAnimTimer.setInterval(10);
    m_newEntryAnimTimer.setSingleShot(false);
    setMouseTracking(true);

    connect(&m_newEntryAnimTimer, SIGNAL(timeout()), this, SLOT(_onTimerTick()));
}
//=========================================================
void UGPickerSelector::move_resize(QRect r)
{
    m_step = r.height();

    r.setHeight(r.height() * (m_entries->size() > m_maxLines ? m_maxLines : m_entries->size()));
    r = _cappedToScreen(r);

    move(r.topLeft());
    resize(r.size());

    m_offset       = 0;
    m_snapToBottom = false;
}
//=========================================================
void UGPickerSelector::setMaxLines(uint32_t lines) { m_maxLines = lines; }
//=========================================================
void UGPickerSelector::newEntry()
{
    if (!isVisible()) return;

    m_newEntryAnimation = 25;
    m_newEntryAnimTimer.start();
}
//=========================================================
void UGPickerSelector::set_palette(const QPalette& p)
{
    m_palette = p;
    update();
}
//=========================================================
void UGPicker::_computeGeometry()
{
    m_frame.clear();
    QRect r = rect();

    if (m_border)
        r.adjust(1, 1, -1, -1);
    else
        r.adjust(-1, -1, 1, 1);

    if (m_radius)
        m_frame.addRoundedRect(r, m_radius, m_radius);
    else
        m_frame.addRect(r);

    m_geometryRequired = false;
}
//=========================================================
void UGPicker::_updateSelectorPos()
{
    m_selector.move_resize(QRect(mapToGlobal(QPoint(0, height())), size()));
}
//=========================================================
void UGPicker::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);

    if (m_geometryRequired) _computeGeometry();

    painter.save();
    if (m_border)
        painter.setPen(QPen(palette().buttonText(), 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
    else
        painter.setPen(QPen(Qt::NoPen));

    painter.setBrush(QBrush(palette().button()));
    painter.drawPath(m_frame);
    painter.restore();

    QString text = "-";

    if (m_index != -1 && !m_entries.empty()) text = m_entries[m_index].text;

    painter.drawText(rect().adjusted(MARGIN_LEFT, 0, -(height() / 2), 0), Qt::AlignVCenter | Qt::AlignLeft,
                     text);

    QRectF icon(0, 0, (float)height() / 4.0f, (float)height() / 4.0f);

    icon.moveCenter(QPointF((float)width() - icon.width(), (float)height() / 2.0f));

    auto p = painter.pen();
    p.setWidth(2);
    p.setColor(palette().accent().color());
    painter.setPen(p);

    if (m_open) painter.setBrush(p.color());

    painter.drawEllipse(icon);
}
//=========================================================
void UGPicker::moveEvent(QMoveEvent* event)
{
    _updateSelectorPos();
    QWidget::moveEvent(event);
}
//=========================================================
void UGPicker::resizeEvent(QResizeEvent* event)
{
    _updateSelectorPos();
    m_geometryRequired = true;
    QWidget::resizeEvent(event);
}
//=========================================================
void UGPicker::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::FontChange)
        m_selector.setFont(font());
    else if (event->type() == QEvent::PaletteChange)
        m_selector.set_palette(palette());

    QWidget::changeEvent(event);
}
//=========================================================
void UGPicker::mousePressEvent(QMouseEvent* event)
{
    _updateSelectorPos();
    m_selector.show();
    m_open = true;
    repaint();
    QWidget::mousePressEvent(event);
}
//=========================================================
void UGPicker::_selected(uint32_t index)
{
    if (index >= m_entries.size()) return;

    m_index = index;
    emit onPick(&m_entries[index]);
    update();
}
//=========================================================
void UGPicker::_selectorHide()
{
    m_open = false;
    repaint();
}
//=========================================================
UGPicker::UGPicker(QWidget* parent)
    : QWidget(parent),
      m_entries(),
      m_selector(&m_entries, this),
      m_index(-1),
      m_open(false),
      m_border(false),
      m_radius(0),
      m_geometryRequired(true),
      m_frame()
{
    setMouseTracking(false);

    m_selector.hide();
    connect(&m_selector, SIGNAL(onSelect(uint32_t)), this, SLOT(_selected(uint32_t)));
    connect(&m_selector, SIGNAL(onHide()), this, SLOT(_selectorHide()));
}
//=========================================================
UGPicker::~UGPicker() {}
//=========================================================
void UGPicker::addEntry(const UltraEntry& entry)
{
    m_entries.push_back(entry);
    if (m_entries.size() == 1) update();

    m_selector.newEntry();
}
//=========================================================
void UGPicker::setMaxLines(uint32_t lines) { m_selector.setMaxLines(lines); }
//=========================================================
void UGPicker::setBorder(bool border)
{
    m_border = border;
    update();
}
//=========================================================
void UGPicker::setRadius(uint8_t radius)
{
    m_radius = radius;
    update();
}
//=========================================================
int32_t UGPicker::currentID() const
{
    auto e = current();
    if (!e) return UG_INVALID_ID;
    return e->id;
}
//=========================================================
const UltraEntry* UGPicker::current() const
{
    if (m_index == -1 || m_index >= m_entries.size()) return nullptr;
    return &m_entries[m_index];
}
//=========================================================
void UGPicker::clear()
{
    m_entries.clear();
    m_index = -1;
}
//=========================================================
