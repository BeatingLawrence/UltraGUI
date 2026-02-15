#ifndef GUI_UGLOADINGDOTS_H
#define GUI_UGLOADINGDOTS_H

// QPalette roles used by UGLoadingDots:
// - WindowText: pen and fill for animated dots.

#include <QTimer>
#include <QWidget>

#include "ultragui_global.h"

class QResizeEvent;

namespace gui
{
    class ULTRAGUI_EXPORT UGLoadingDots : public QWidget
    {
        Q_OBJECT

       private:
        QTimer m_timer;

        QRectF m_dot;

        QVector<QPair<QPointF, bool>> m_points;  // point, hidden

        bool m_loading;

        // converts the input to a series of points
        void _computeGeometry(uint8_t n, uint16_t r);
        void _updateGeometry();

       private slots:
        void _tick();

       protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void resizeEvent(QResizeEvent* event) override;

       public:
        explicit UGLoadingDots(QWidget* parent = nullptr);

        virtual ~UGLoadingDots();

        enum BlinkingSpeed
        {
            BS_Slow,
            BS_Normal,
            BS_Fast,
            BS_Superfast
        };

        // sets the loading state of the widget
        void setLoading(bool state = true);

        // sets the blinking speed
        void setSpeed(BlinkingSpeed speed);

       signals:
    };
}  // namespace gui

#endif  // GUI_UGLOADINGDOTS_H
