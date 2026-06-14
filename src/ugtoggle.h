#ifndef GUI_UGTOGGLE_H
#define GUI_UGTOGGLE_H

// QPalette roles used by UGToggle:
// - Base: OFF state background.
// - Accent: ON/transition background.
// - AlternateBase: inner knob/slider.
// - Text: OFF label on the Base background.
// - HighlightedText: ON label on the Accent background.

#include <QPainterPath>
#include <QString>
#include <QTimer>

#include "buttonlogic.h"
#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UGToggle : public ButtonLogic
    {
        Q_OBJECT

       private:
        uint32_t m_slideAnimation, m_animationMax;

        QTimer m_timer;

        QPainterPath m_path;
        QString m_onText;
        QString m_offText;

        void _mouseClicked();

        virtual void paintEvent(QPaintEvent* event) override;

        virtual void resizeEvent(QResizeEvent* event) override;

        virtual void stateChange(bool newState) override;

       private slots:
        void _onTimerTick();

       public:
        UGToggle(QWidget* parent = nullptr);

        virtual ~UGToggle();

        inline bool getState() const { return isActive(); }

        // Set the button as a toggle or a momentary button
        inline void setToggleMode(bool toggleMode) { configureToggle(toggleMode); }

        // In automode the toggle updates its own state on click.
        // With automode disabled the state is managed by application code.
        inline void setAutoMode(bool automode) { configureAutoMode(automode); }

        // Set wether the hovering (mouse tracking) must be used or not
        inline void useHovering(bool use) { configureHovering(use); };

        // Set the label shown in the colored area while the toggle is on.
        void setOnText(const QString& text);

        // Set the label shown in the dark area while the toggle is off.
        void setOffText(const QString& text);

       public slots:
        void setState(bool state);
    };
}  // namespace gui

#endif  // GUI_UGTOGGLE_H
