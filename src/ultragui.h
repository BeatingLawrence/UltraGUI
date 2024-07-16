#ifndef ULTRAGUI_H
#define ULTRAGUI_H

#include <QColor>
#include <QString>

#include "types.h"
#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UltraGui
    {
       private:
        struct dual
        {
            std::string left;
            std::string right;
        };

        static std::string strPrint(std::string format, ...);
        static std::string substrUntil(const std::string& str, const std::string& token);
        static std::string substrFrom(const std::string& str, const std::string& token);
        static dual split(const std::string& str, const std::string& token);

       public:
        // Create a single UltraEntry with an ID and a string
        static UltraEntry createUltraEntry(QString string, int32_t id = InvalidID);

        // Generate a vector of UltraEntry from a vector af strings. IDs are sequential.
        static QVector<UltraEntry> createUltraEntryVector(QVector<QString> strings);

        static QColor transient(const QColor& first, const QColor& second, uint8_t selector);

        // Return a faded color (with apha set half the value)
        static QColor faded(const QColor& color, float alpha = 0.5f);

        static UltraGUIVersion version();
    };
}  // namespace gui

#endif  // ULTRAGUI_H
