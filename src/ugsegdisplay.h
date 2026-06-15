#ifndef UGSEGDISPLAY_H
#define UGSEGDISPLAY_H

// QPalette roles used by UGSegDisplay:
// - Base: panel background.
// - Accent: active segments.
// - Text: labels/unit and border.

// 7 Seg display has its charm

#include <QWidget>

#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UGSegDisplay : public QWidget
    {
       private:
        float m_value;

        uint8_t m_int, m_dec;

        QString m_unit;

        bool m_border, m_autominus;

        struct Geometry
        {
            float centerX;
            float centerX1;
            float centerX2;

            float centerY;
            float centerY1;
            float centerY2;
            float centerY3;
            float centerY4;
        };

        virtual void paintEvent(QPaintEvent* event) override;

        bool _isNegative();

        uint8_t _countInt(const QString& str);
        uint8_t _countDec(const QString& str);

        void _paintMinus(QPainter& painter, float w);

        float _normalize(float value);

        QString _print();

        void _paintDigit(QPainter& painter, const QRectF& zone, uint8_t digit);

        void _paintSeg(QPainter& painter, uint8_t segment, const Geometry& geom, const QPainterPath& vs,
                       const QPainterPath& hs);

       public:
        UGSegDisplay(QWidget* parent = nullptr);

        virtual ~UGSegDisplay();

        void setValue(float value);

        void setDigitsNum(uint8_t integer, uint8_t decimal);

        void setUnit(const QString& unit);

        void drawBorder(bool state = true);

        void setAutoMinus(bool state = true);
    };

}  // namespace gui

#endif  // UGSEGDISPLAY_H
