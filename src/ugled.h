#ifndef GUI_UGLED_H
#define GUI_UGLED_H

// QPalette roles used by UGLed:
// - Base: LED body when off.
// - Accent: LED body when on.
// - WindowText: LED ring/border.

#include <QWidget>

#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UGLed : public QWidget
    {
        Q_OBJECT

       private:
        bool m_active;

        uint8_t m_ledBorderThickness;

       protected:
        virtual void paintEvent(QPaintEvent* event);

       public:
        explicit UGLed(QWidget* parent = nullptr);

        void ledBorderThickness(uint8_t ledBorderThickness);

       public slots:
        void activate(bool active = true);

        void deactivate();
    };
}  // namespace gui

#endif  // GUI_UGLED_H
