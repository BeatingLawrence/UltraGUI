#ifndef GUI_ULTRASCROLL_H
#define GUI_ULTRASCROLL_H

#include <QPainterPath>
#include <QTimer>
#include <QVector>
#include <QWidget>

#include "types.h"
#include "ultragui_global.h"

/*
 *  This class constructs and draws a scroll object
 *
 *  A scroll object is made up of 3 parts: left button, right button and the central monitor.
 *
 *  The buttons serve as selectors to switch the entry showed in the monitor at the center.
 *
 *  The entry list is stored as a vector of UltraEntry, which may be setted
 *  in the component using setEntries or addEntry methods.
 *
 *  Grayed entries will be displayed using the grayed color (see QPalette usage below) in default configuration.
 *  If hideGrayed property has been set, grayed entries will be hidden instead of showed with grayed color.
 *
 *  Every time a button is clicked and the monitor changes entry, the signal onChange(uint)
 *  is emitted (if the new selected entry is not a grayed entry).
 *  Only if the new current entry has a valid data pointer, also the onDataChange(void*) signal is emitted.
 *
 *  Setting the component in touchScreenMode will prevent it from detecting hovering events.
 *
 *  QPalette colors used by this component:
 *
 *      -QPalette::Button
 *          Used for the background of the buttons (not clicked and not in hovering)
 *
 *      -QPalette::Midlight
 *          Used for the hovering color
 *
 *      -QPalette::Light
 *          Used for the button clicked color
 *
 *      -QPalette::ButtonText (Disabled)
 *          Used as grayed color
 *
 *      -QPalette::ButtonText (Enabled)
 *          Used as entry text color
 *
 *
 */

namespace gui
{
    class ULTRAGUI_EXPORT UltraScroll : public QWidget
    {
        Q_OBJECT

       private:
        enum HoveringElement
        {
            HE_NoHovering,
            HE_LeftButtonIsHovering,
            HE_RightButtonIsHovering
        } m_hovering;

        enum PressedElement
        {
            PE_NoPressing,
            PE_LeftButtonIsPressed,
            PE_RightButtonIsPressed
        } m_pressed;

        enum Label
        {
            L_Previous,
            L_Current,
            L_Next
        };

        QVector<UltraEntry> m_entries;

        uint32_t m_selector;

        uint32_t m_animation;

        bool m_hideGrayed;

        bool m_isRounded;

        bool m_geometryRequired;

        bool m_touchScreenMode;

        bool m_animationDirection;

        QPainterPath m_leftButton;
        QPainterPath m_rightButton;
        QPainterPath m_centerMonitor;
        QRect m_leftButtonClickArea;
        QRect m_rightButtonClickArea;
        uint32_t m_labelWidth;
        QPainterPath m_leftArrow;
        QPainterPath m_rightArrow;

        QTimer m_timer;

        void _computeGeometry();

        void _incrementSelector();

        void _decrementSelector();

        uint32_t _getSelectorMoved(uint32_t selector, bool decrement = false);

        uint32_t _lastIndex();

        void _choosePenColor(QColor& color, QPainter& painter, int32_t x);

        uint32_t _next();

        uint32_t _prev();

       private slots:
        void _tick();

       protected:
        virtual void paintEvent(QPaintEvent* event) override;

        virtual void mousePressEvent(QMouseEvent* event) override;

        virtual void mouseReleaseEvent(QMouseEvent* event) override;

        virtual void mouseMoveEvent(QMouseEvent* event) override;

        virtual void leaveEvent(QEvent* event) override;

        virtual void resizeEvent(QResizeEvent* event) override;

       public:
        explicit UltraScroll(QWidget* parent = nullptr);

        virtual ~UltraScroll();

        // checks if ID is valid and adds a single entry. Returns true if ID is valid
        void addEntry(const UltraEntry& entry);

        // checks the validity of the given structure (IDs must be incremental)
        // and sets the structure. Returns true if ID is valid.
        // all current entries will be lost
        bool setEntries(const QVector<UltraEntry>& entries);

        // deletes all the entries and cleans the widget
        void clearEntries();

        // sets an entry as grayed or not.
        // Returns true if ID is valid
        bool setGrayed(int32_t entryID, bool grayed = true);

        // sets all the entries as clickable or not
        void setAllGrayed(bool grayed = true);

        // gets the current selected entry ID
        int32_t getCurrentEntryID() const;

        // sets wether hide grayed entry or show them grayed
        void hideGrayedEntries(bool hide = true);

        // sets the touch screen mode state
        void setTouchScreenMode(bool state = true);

        // sets the rounded mode
        void setRounded(bool isRounded);

       signals:
        // emitted when user changes the current entry, if entry is not grayed
        void onChange(int32_t id);

        // emitted when user changes the current entry, if entry is not grayed and if data has been set
        void onDataChange(void*);
    };
}  // namespace gui

#endif  // GUI_ULTRASCROLL_H
