#ifndef UGPICKER_H
#define UGPICKER_H

// QPalette roles used by UGPicker (+ popup UGPickerSelector):
// - Button: picker background.
// - ButtonText: picker border/title.
// - Accent: indicator and row highlight.
// - Base: popup selector background.
// - Text: popup selector text.

#include <QPainterPath>
#include <QRandomGenerator>
#include <QTimer>
#include <QWidget>

#include "types.h"
#include "ultragui_global.h"

namespace gui
{
    class UGPickerSelector : public QWidget
    {
        Q_OBJECT

        QList<UltraEntry>* m_entries;
        uint32_t m_maxLines;
        int m_step;
        int m_offset;
        QPoint m_cursor;
        QTimer m_newEntryAnimTimer;
        int m_newEntryAnimation;
        bool m_snapToBottom;
        QPalette m_palette;

        virtual void paintEvent(QPaintEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseMoveEvent(QMouseEvent* event) override;
        virtual void wheelEvent(QWheelEvent* event) override;
        virtual void hideEvent(QHideEvent* event) override;
        virtual void changeEvent(QEvent* event) override;

        QRect _cappedToScreen(const QRect& r);

        void incOffset(float x);
        void decOffset(float x);

        int maxOffset();

       private slots:
        void _onTimerTick();

       public:
        UGPickerSelector(QList<UltraEntry>* list, QWidget* parent = nullptr);

        void move_resize(QRect r);

        void setMaxLines(uint32_t lines);

        void newEntry();

        void set_palette(const QPalette& p);

       signals:
        void onSelect(uint32_t);
        void onHide();
    };

    class ULTRAGUI_EXPORT UGPicker : public QWidget
    {
        Q_OBJECT

       private:
        QList<UltraEntry> m_entries;
        UGPickerSelector m_selector;
        int m_index;
        bool m_open;

        bool m_border;
        uint8_t m_radius;
        bool m_geometryRequired;
        QPainterPath m_frame;

        void _computeGeometry();

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
        explicit UGPicker(QWidget* parent = nullptr);

        virtual ~UGPicker();

        void addEntry(const UltraEntry& entry);

        void setMaxLines(uint32_t lines);

        void setBorder(bool border = true);

        // Set the radius of round corners. If set to zero, corners will not be rounded
        void setRadius(uint8_t radius);

        // Return UG_INVALID_ID if no entry is selected
        int32_t currentID() const;

        // Return nullptr if no entry is selected
        const UltraEntry* current() const;

       public slots:
        void clear();

       signals:
        void onPick(const UltraEntry*);
    };
}  // namespace gui
#endif  // UGPICKER_H
