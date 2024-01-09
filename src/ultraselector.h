#ifndef GUI_ULTRASELECTOR_H
#define GUI_ULTRASELECTOR_H

#include <QTimer>
#include <QWidget>

#include "types.h"
#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UltraSelector : public QWidget
    {
        Q_OBJECT

       private:
        QPoint m_currentMouseCursor;

        QVector<UltraEntry> m_entries;

        uint32_t m_selector;

        uint32_t m_animation;

        QTimer m_timer;

        bool m_clicked;

        uint32_t m_sectionHeight;

        uint32_t m_maxValue;

        QRectF _animationToRect();

        uint32_t _selectorToAnimation();

        void _computeAnimationBoundaries();

       private slots:
        void _tick();

       protected:
        virtual void paintEvent(QPaintEvent* event) override;

        virtual void mouseMoveEvent(QMouseEvent* event) override;

        virtual void mousePressEvent(QMouseEvent* event) override;

        virtual void mouseReleaseEvent(QMouseEvent* event) override;

        virtual void leaveEvent(QEvent* event) override;

       public:
        explicit UltraSelector(QWidget* parent = nullptr);

        virtual ~UltraSelector();

        // checks if ID is valid and adds a single entry
        void addEntry(const UltraEntry& entry);

        // checks the validity of the given structure (IDs must be incremental)
        // and sets the structure. Returns true if ID is valid.
        // all current entries will be lost
        bool setEntries(const QVector<UltraEntry>& entries);

        // deletes all the entries and cleans the widget
        void clearEntries();

        // sets an entry as grayed (not clickable) or not (clickable).
        // Returns true if ID is valid
        bool setGrayed(int32_t entryID, bool grayed = true);

        // sets all the entries as clickable or not
        void setAllGrayed(bool grayed = true);

       signals:
        // emitted when click happens
        void onClick(int32_t id, void*);

        // emitted when click happens, only if relative data has been set and is not a nullptr
        // this signal is made specially to connect directly to stacked widget slots
        void onClickForStacked(QWidget*);
    };
}  // namespace gui

#endif  // GUI_ULTRASELECTOR_H
