#ifndef GUI_UGFRAME_H
#define GUI_UGFRAME_H

// QPalette roles used by UGFrame:
// - Midlight: idle frame border.
// - Accent: blinking frame border.

#include <cstdint>

#include <QColor>
#include <QFrame>
#include <QString>
#include <QTimer>

#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UGFrame : public QFrame
    {
        Q_OBJECT

       private:
        QTimer m_timer;

        int m_alpha;

        int m_blinksRemaining;

        QString m_title;

        Qt::Alignment m_titleAlignment;

        virtual void paintEvent(QPaintEvent* event) override;

       private slots:
        void timerTick();

       public:
        explicit UGFrame(QWidget* parent = nullptr);

        virtual ~UGFrame();

        void setTitle(const QString& title, Qt::Alignment align = Qt::AlignHCenter);

        void setBorderColor(const QColor& color);

        void blink(bool repeat = false);
        void blink(uint8_t blinks);

        void stop();

       signals:
    };
}  // namespace gui

#endif  // GUI_UGFRAME_H
