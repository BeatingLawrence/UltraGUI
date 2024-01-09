#ifndef GUI_ULTRATOGGLE_H
#define GUI_ULTRATOGGLE_H

#include <QPainterPath>
#include <QTimer>

#include "buttonbehavior.h"
#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UltraToggle : public ButtonBehavior
    {
        Q_OBJECT

       private:
        uint32_t m_slideAnimation, m_animationMax;

        QTimer m_timer;

        QPainterPath m_path;

        void _mouseClicked();

        virtual void paintEvent(QPaintEvent* event) override;

        virtual void resizeEvent(QResizeEvent* event) override;

        virtual void stateChange(bool newState) override;

       private slots:
        void _onTimerTick();

       public:
        UltraToggle(QWidget* parent = nullptr);

        virtual ~UltraToggle();

        inline bool getState() const { return isActive(); }

        // Set the button as a toggle or a momentary button
        inline void setToggleMode(bool toggleMode) { configureToggle(toggleMode); }

        // Set wether the hovering (mouse tracking) must be used or not
        inline void useHovering(bool use) { configureHovering(use); };

       public slots:
        void setState(bool state);
    };
}  // namespace gui

#endif  // GUI_ULTRATOGGLE_H
