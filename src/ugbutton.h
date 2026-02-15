#ifndef GUI_UGBUTTON_H
#define GUI_UGBUTTON_H

// QPalette roles used by UGButton:
// - Button: button background in idle state.
// - ButtonText: text and border color.
// - Accent: active state and hover transition.

/*  This class constructs and draws a button object.
 *
 *  The button consists of two separate
 *  parts: the button itself and the led.
 *
 *  Theese two sub-components may be connected (in-sync) or not.
 *  When they are in-sync, they share the same state,
 *  otherwise, they both can have an own state.
 *  The state of the button is controlled only by the mouse clicks, the
 *  state of the led may be controlled by the application instead (only if sync is off).
 *
 *  The toggle property decides the behavior of the button.
 *  Toggle-mode=ON means that one click activates the button and another click deactivates it.
 *  Toggle-mode=OFF means that the button remains active only while mouse is pressed.
 *  As soon as the user releases the mouse button, the component deactivates.
 *
 *  The text used for the button is the same as 'text' property of the QPushButton.
 *  The component tries to use the one setted as 'activeText' if no text has
 *  been set in the QPushButton interface.
 *  If both 'text' and 'activeText' has been set, the component will use the first
 *  while led is OFF and the second while led is ON.
 *  Text will not be used when an icon is set.
 *  The font of the base object of class QWidget will be used.
 *
 *  QPalette colors used by this component:
 *
 *      -QPalette::Button
 *          Used for the background of the button (idle state)
 *
 *      -QPalette::ButtonText
 *          Used for text and border color
 *
 *      -QPalette::Accent
 *          Used for active state and hovering transition
 */

#include <QPainterPath>
#include <QTimer>

#include "buttonlogic.h"
#include "types.h"
#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UGButton : public ButtonLogic
    {
        Q_OBJECT

       private:
        struct Configuration
        {
            bool animateHovering;  // use animation for hovering transitions
            bool sync;             // led and button share the same state
            bool textFit;          // adjust the text dimension to adjust the button size
            bool border;           // draw border
            uint8_t radius;        // border radius

            Configuration()  // default configuration
                : animateHovering(true),
                  sync(true),
                  textFit(false),
                  border(false)
            {
            }
        } m_configuration;

        bool m_ledState, m_animationFlag, m_blinking;
        uint8_t m_hoveringAnimation, m_animationToSum;
        QString m_activeText;

        UltraEntry m_entry;

        QTimer m_timer;

        //=====================geometry
        QPainterPath m_frame;
        //=============================

        void _configureTiming(AnimationSpeed speed);

        void _sum(uint8_t& value, uint8_t toSum, uint8_t max, bool subtract = false);

        void _computeGeometry();

        void animate(bool enter);

        QFont adjustedFont(const QString& text);

        virtual void fadeIn() override;

        virtual void fadeOut() override;

        virtual void stateChange(bool newState) override;
        virtual void click() override;

       private slots:
        void _tick();

       public:
        explicit UGButton(QWidget* parent = nullptr);

        virtual ~UGButton();

        // Get button state
        inline bool isButtonActive() const { return isActive(); }

        // Get LED state
        inline bool isLedActive() const { return m_ledState; }

        // Set the button as a toggle or a momentary button
        inline void setToggleMode(bool toggleMode) { configureToggle(toggleMode); }

        // set wether the hovering (mouse tracking) must be used or not
        inline void useHovering(bool use) { configureHovering(use); };

        // set the automod state
        inline void setAutoMode(bool automode) { configureAutoMode(automode); };

        // Set wether LED state must stay in sync with button state
        inline void setSync(bool sync = true) { m_configuration.sync = sync; }

        // Set text displayed while led is active
        inline void setActiveText(const QString& string) { m_activeText = string; }

        // Set whether draw border or not
        inline void drawBorder(bool drawBorder = true) { m_configuration.border = drawBorder; }

        // Set the border radius. 0 to disable
        inline void setBorderRadius(uint8_t radius) { m_configuration.radius = radius; }

        // Set wether the text size must be adjusted to match the button width
        inline void setTextFit(bool fit) { m_configuration.textFit = fit; }

        // set wether use animation while passing from non-hovering to hovering state and vice versa
        void animateHovering(bool animate = true, AnimationSpeed speed = AS_Normal);

        // virtual QSize sizeHint() const override;

        void setEntry(const UltraEntry& entry);

        const UltraEntry& entry() const;

        virtual QSize sizeHint() const override;

        virtual QSize minimumSizeHint() const override;

       private:
        virtual void paintEvent(QPaintEvent* event) override;

        virtual void changeEvent(QEvent* event) override;

        virtual void resizeEvent(QResizeEvent* event) override;

       public slots:
        void blink();

        // Set button state
        void setState(bool state);

        // Set LED state
        void setLedState(bool state);

       signals:
        void onClickEntry(const UltraEntry*);
        void onChangeEntry(bool, const UltraEntry*);
    };

}  // namespace gui

#endif  // GUI_UGBUTTON_H
