#ifndef GUI_ULTRATOGGLE_H
#define GUI_ULTRATOGGLE_H

#include <QPushButton>
#include <QTimer>

#include "UltraGUI_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UltraToggle : public QPushButton
    {
        Q_OBJECT

       private:
        bool m_state;

        bool m_hovering;

        unsigned int m_slideAnimation;

        QTimer m_timer;

        QColor _computeTransient(const QColor& first, const QColor& second, uint8_t selector);

       private slots:
        void _mouseClicked();

        void _onTimerTick();

       protected:
        virtual void paintEvent(QPaintEvent* event) override;

        virtual void enterEvent(QEnterEvent* event) override;

        virtual void leaveEvent(QEvent* event) override;

        virtual void hideEvent(QHideEvent* event) override;

       public:
        UltraToggle(QWidget* parent = nullptr);

        virtual ~UltraToggle();

        inline bool getState() const { return m_state; }

       public slots:

        void setState(bool state, bool useAnimation = true);

       signals:
        void onEnable();

        void onDisable();

        void onChange(bool newState);
    };
}  // namespace gui

#endif  // GUI_ULTRATOGGLE_H
