#ifndef GUI_BUTTONLOGIC_H
#define GUI_BUTTONLOGIC_H

#include <QPushButton>

namespace gui
{
    class ButtonLogic : public QPushButton
    {
        Q_OBJECT

       private:
        enum ButtonMachineState
        {
            Inactive,
            Hovering,
            Pressed,
            Active,
            ActivePressed,
        } m_state;

        enum ButtonMachineEvent
        {
            Press,    // mouse press the button
            Release,  // mouse release the button
            Enter,    // mouse enters the button perimeter
            Leave,    // mouse leaves the button perimeter
        };

        struct Configuration
        {
            bool toggle;    // toggle mode
            bool hovering;  // use hovering (false if using touchscreen)
            bool automode;

            Configuration()  // default configuration
                : toggle(false),
                  hovering(false),
                  automode(true)
            {
            }
        } m_conf;

        bool m_active, m_hovering;  // used just for reporting to derived class

        void _fadeIn();

        void _fadeOut();

        void _resetStateMachine();

        void _notifyChange(bool state);

        void _notifyAspectChange(bool state);

        void _notifyAll(bool state)
        {
            _notifyAspectChange(state);
            _notifyChange(state);
        }

        void _onEnter();

        void _onLeave();

        void _onPress();

        void _onRelease();

        void _printCurrentState();

       protected:
        // Called when the state changes
        virtual void stateChange(bool newState);

        // Called when the cursor enters the widget
        virtual void fadeIn();

        // Called when the cursor leaves the widget
        virtual void fadeOut();

        // Called when the button is clicked
        virtual void click();

        void configureHovering(bool enable);

        void configureToggle(bool enable);

        // In Automode (the default) the button changes its state
        // on its own, when it's pressed/clicked.
        // If AutoMode is turned off, the button emits a signal when it's clicked
        // but does not change its state. The application have to set the new state explicitly.
        // In this case the emitted signal contains the WANTED state (!current).
        // NOTE: disabling automode is available only if toggle mode is active, thus, disabling
        // automode will automatically enable toggle mode
        void configureAutoMode(bool enable);

        bool isActive() const;

        bool isHovering() const;

        // force the state (only in toggle mode)
        void forceState(bool state);

       private:
        void transition(ButtonMachineEvent event);

        void notify(ButtonMachineState oldState, ButtonMachineState newState);

        virtual void mousePressEvent(QMouseEvent* event) override;

        virtual void mouseReleaseEvent(QMouseEvent* event) override;

        virtual void leaveEvent(QEvent* event) override;

        virtual void enterEvent(QEnterEvent* event) override;

        virtual void mouseMoveEvent(QMouseEvent* event) override;

        virtual void hideEvent(QHideEvent* event) override;

        virtual bool hitButton(const QPoint& pos) const override;

       public:
        explicit ButtonLogic(QWidget* parent = nullptr);

       signals:
        // Emitted when button is activated
        void onEnable();

        // Emitted when button is deactivated
        void onDisable();

        // Emitted when button state changes.
        // The bool parameter contains the NEW state
        void onChange(bool);

        // Emitted when the button is clicked
        void onClick();
    };

}  // namespace gui

#endif  // GUI_BUTTONLOGIC_H
