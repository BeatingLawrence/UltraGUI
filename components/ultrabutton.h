#ifndef HCI_ULTRABUTTON_H
#define HCI_ULTRABUTTON_H

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
 *  The component may use icons to display its led state.
 *  The color adapting feature will overwrite existing icons,
 *  keeping just the alpha channel, and using the text color setted making the icons mono-chromatic.
 *  Be sure to de-activate this feature if colored icons are wanted.
 *  setIcons() will set only valid icons. Size of both icons must be the same and be less than widget size.
 *  activeIcon is not mandatory.
 *
 *  QPalette colors used by this component:
 *
 *      -QPalette::Button
 *          Used for the background of the led (OFF and not in hovering)
 *
 *      -QPalette::ButtonText
 *          Used for the color of the text and for the color-adapting feature of the icons
 *
 *      -QPalette::Light
 *          Used for the led while it is lit.
 *
 *      -QPalette::Midlight
 *          Used for the hovering color
 *
 *  Border can be:
 *      Rounded:
 *          In this mode, corners are rounded and anti-aliasing is enabled.
 *          To activate this, the application must set an edge radius.
 *
 *      Not-rounded (square):
 *          In this mode, corners are square and anti-aliasing is disabled.
 *          This is the default mode, if no edge radius has been set.
 */

#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QTimer>

#include "./UltraGUI_global.h"

namespace hci
{
    class ULTRAGUI_EXPORT UltraButton : public QPushButton
    {
        Q_OBJECT

       public:
        enum AnimationSpeed
        {
            AS_Slow,
            AS_Normal,
            AS_Fast,
            AS_Superfast
        };

       private:
        bool m_ledState, m_buttonState, m_toggleMode, m_synchronous, m_hovering, m_leftJustify;
        bool m_iconLeftJustify, m_shrinkToFit, m_useHoveringAnimation, m_blinking, m_adaptIconsColor;
        bool m_hasBorder, m_geometryRequired, m_touchScreenMode;
        QString m_activeText;
        uint8_t m_leftPadding, m_iconLeftPadding, m_shrinkPadding, m_hoveringAnimation, m_animationToSum;

        QPixmap m_defaultIcon;

        QPixmap m_activeIcon;

        QTimer m_timer;

        //=====================geometry
        QPainterPath m_frame;
        QRect m_inputRect;
        //=============================

        QColor _computeTransient(const QColor& first, const QColor& second, uint8_t selector);

        void _writeString(QPainter& painter, const QString& string, const QColor& color);

        void _configureTiming(AnimationSpeed speed);

        void _sum(uint8_t& value, uint8_t toSum, uint8_t max, bool subtract = false);

        void _adaptIconsColor();

        void _computeGeometry();

        void cursorEntered();

        void cursorLeft();

        void changeTo(bool newState);

       private slots:
        void _tick();

       public:
        explicit UltraButton(QWidget* parent = nullptr);

        virtual ~UltraButton();

        // sets icons
        void setIcons(const QPixmap& defaultIcon, const QPixmap& activeIcon = QPixmap(), bool colorAdapting = true);

        // gets button state
        inline bool isButtonActive() const { return m_buttonState; }

        // gets LED state
        inline bool isLedActive() const { return m_ledState; }

        // sets if the button is a toggle
        inline void setToggleMode(bool toggleMode) { m_toggleMode = toggleMode; }

        // sets wether LED state must stay in sync with button state
        inline void setSync(bool sync = true) { m_synchronous = sync; }

        // sets text displayed while led is active
        inline void setActiveText(const QString& string) { m_activeText = string; }

        // sets whether draw border or not
        inline void drawBorder(bool drawBorder = true) { m_hasBorder = drawBorder; }

        // sets left-justify of icon
        inline void iconLeftJustify(bool leftJustify = true) { m_iconLeftJustify = leftJustify; }

        // sets left padding of icon
        inline void setIconLeftPadding(uint8_t padding) { m_iconLeftPadding = padding; }

        // use adaptive colors for the icon
        inline void setIconColorAdapting(bool state = true) { m_adaptIconsColor = state; }

        // sets the touch screen mode state
        inline void setTouchScreenMode(bool state = true)
        {
            m_touchScreenMode = state;
            setMouseTracking(!state);
            m_hovering          = false;
            m_hoveringAnimation = 0;
            m_timer.stop();
        }

        // sets text left-justify
        inline void leftJustify(bool leftJustify = true) { m_leftJustify = leftJustify; }

        // sets a left padding (valid with left-justify enabled)
        inline void setLeftPadding(uint8_t padding) { m_leftPadding = padding; }

        // sets wether the component must tune its width based on text width
        inline void shrinkToFIt(bool shrinkToFit = true) { m_shrinkToFit = shrinkToFit; }

        // sets an offset to sum to shrink to fit calculations (left + right)
        inline void shrinkToFitPadding(uint8_t padding) { m_shrinkPadding = padding; }

        // sets wether use animation while passing from non-hovering to hovering state and vice versa
        void animateHovering(bool animate = true, const AnimationSpeed& speed = UltraButton::AS_Normal);

       protected:
        virtual void paintEvent(QPaintEvent* event) override;

        virtual void additionalPainting(QPainter& painter);

        virtual void hideEvent(QHideEvent* event) override;

        virtual void changeEvent(QEvent* event) override;

        virtual void resizeEvent(QResizeEvent* event) override;

        virtual void mousePressEvent(QMouseEvent* event) override;

        virtual void mouseReleaseEvent(QMouseEvent* event) override;

        virtual void mouseMoveEvent(QMouseEvent* event) override;

        virtual void leaveEvent(QEvent* event) override;

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

       public slots:
        void blink();

        // Set led state
        void activate(bool active = true);

        // Switch off led
        void deactivate();
    };

}  // namespace hci

#endif  // BUTTON_H
