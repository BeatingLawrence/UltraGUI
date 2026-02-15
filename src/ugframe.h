#ifndef GUI_UGFRAME_H
#define GUI_UGFRAME_H

// QPalette roles used by UGFrame:
// - Base: normal frame fill.
// - Accent: fill color during blink.

#include <cstdint>

#include <QFrame>
#include <QTimer>

namespace gui
{
    class UGFrame : public QFrame
    {
        Q_OBJECT

       private:
        QTimer m_timer;

        int m_alpha;

        int m_blinksRemaining;

        virtual void paintEvent(QPaintEvent* event) override;

       private slots:
        void timerTick();

       public:
        explicit UGFrame(QWidget* parent = nullptr);

        virtual ~UGFrame();

        void blink(bool repeat = false);
        void blink(uint8_t blinks);

        void stop();

       signals:
    };
}  // namespace gui

#endif  // GUI_UGFRAME_H
