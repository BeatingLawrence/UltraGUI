#ifndef ULTRAPICKER_H
#define ULTRAPICKER_H

#include <QRandomGenerator>
#include <QTimer>
#include <QWidget>

#include "types.h"
#include "ultragui_global.h"

namespace gui
{
    class UltraPickerSelector : public QWidget
    {
        Q_OBJECT

        QList<UltraEntry>* m_entries;
        uint32_t m_maxLines;
        int m_step;
        int m_offset;
        QPoint m_cursor;
        QRandomGenerator m_generator;
        QTimer m_newEntryAnimTimer;
        int m_newEntryAnimation;

        virtual void paintEvent(QPaintEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseMoveEvent(QMouseEvent* event) override;
        virtual void wheelEvent(QWheelEvent* event) override;
        virtual void hideEvent(QHideEvent* event) override;
        virtual void changeEvent(QEvent* event) override;

        QRect _cappedToScreen(const QRect& r);

        QColor _randomColor();

        int ptToPx(int pt);

        void incOffset(int x);
        void decOffset(int x);

        int maxOffset();

       private slots:
        void _onTimerTick();

       public:
        UltraPickerSelector(QList<UltraEntry>* list, QWidget* parent = nullptr);

        void move_resize(QRect r);

        void setMaxLines(uint32_t lines);

        void newEntry();

       signals:
        void onSelect(uint32_t);
        void onHide();
    };

    class ULTRAGUI_EXPORT UltraPicker : public QWidget
    {
        Q_OBJECT

       private:
        QList<UltraEntry> m_entries;
        UltraPickerSelector m_selector;
        int m_index;
        bool m_open;

        void _updateSelectorPos();

        virtual void paintEvent(QPaintEvent* event) override;

        virtual void moveEvent(QMoveEvent* event) override;
        virtual void resizeEvent(QResizeEvent* event) override;
        virtual void changeEvent(QEvent* event) override;

        virtual void mousePressEvent(QMouseEvent* event) override;

       private slots:
        void _selected(uint32_t index);
        void _selectorHide();

       public:
        explicit UltraPicker(QWidget* parent = nullptr);

        virtual ~UltraPicker();

        void addEntry(const UltraEntry& entry);

        void setMaxLines(uint32_t lines);

       public slots:
        void clear();

       signals:
        void onPick(int32_t);
    };
}  // namespace gui
#endif  // ULTRAPICKER_H
