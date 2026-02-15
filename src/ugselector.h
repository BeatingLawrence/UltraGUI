#ifndef GUI_UGSELECTOR_H
#define GUI_UGSELECTOR_H

// QPalette roles used by UGSelector:
// - Accent: selected section background.
// - ButtonText (Normal/Disabled): normal and grayed entry text.

#include <QTimer>
#include <QWidget>

#include "types.h"
#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UGSelector : public QWidget
    {
        Q_OBJECT

       private:
        QPoint m_currentMouseCursor;

        QVector<UltraEntry> m_entries;

        int32_t m_selector;  //-1 = no items selected

        uint32_t m_animation;

        QTimer m_timer;

        bool m_clicked;

        uint32_t m_sectionHeight;

        uint32_t m_maxValue;

        QRectF _animationToRect();

        uint32_t _selectorToAnimation();

        void _computeAnimationBoundaries();

        void _select(uint32_t idx);

        void _notify();

        void _resetSelector();

       private slots:
        void _tick();

       protected:
        virtual void paintEvent(QPaintEvent* event) override;

        virtual void mouseMoveEvent(QMouseEvent* event) override;

        virtual void mousePressEvent(QMouseEvent* event) override;

        virtual void mouseReleaseEvent(QMouseEvent* event) override;

        virtual void leaveEvent(QEvent* event) override;

       public:
        explicit UGSelector(QWidget* parent = nullptr);

        virtual ~UGSelector();

        // check if ID is valid and adds a single entry
        void addEntry(const UltraEntry& entry, bool active = false);

        // check the validity of the given structure (IDs must be incremental)
        // and set the structure.
        // Return true if ID is valid.
        // all current entries will be lost
        bool setEntries(const QVector<UltraEntry>& entries);

        // delete all the entries and cleans the widget
        void clearEntries();

        // set an entry as grayed (not clickable) or not (clickable).
        // Returns true if ID is valid
        bool setGrayed(int32_t entryID, bool grayed = true);

        // set all the entries as clickable or not
        void setAllGrayed(bool grayed = true);

        // set the active (selected) entry, doing the animation.
        // This method will not cause the widget to send any signal
        void setActiveEntry(int32_t entryID);

        // return the currently selected entry.
        // return nullptr if no entry is selected ()
        const UltraEntry* current();

       signals:
        // emitted when click happens
        void onClick(int32_t id, void*);

        // emitted when click happens, only if relative data has been set and is not a nullptr
        // this signal is made specially to connect directly to stacked widget slots
        void onClickForStacked(QWidget*);
    };
}  // namespace gui

#endif  // GUI_UGSELECTOR_H
