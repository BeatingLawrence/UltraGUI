#ifndef ULTRAGUI_TYPES_H
#define ULTRAGUI_TYPES_H

#include <QString>
#include <QVariant>
#include <QVector>

#include "./ultragui_global.h"

#define UG_INVALID_ID gui::InvalidID

namespace gui
{
    struct ULTRAGUI_EXPORT UltraGUIVersion
    {
        enum VersionType
        {
            Alpha,
            Beta,
            Release,
        };

        uint16_t major;
        uint16_t minor;
        uint16_t patch;
        VersionType type;
        std::string text;
    };

    static const int32_t InvalidID = -1;

    // UltraEntry structure
    struct ULTRAGUI_EXPORT UltraEntry
    {
        QString text;
        int32_t id;
        bool grayed, hidden;
        QVariant variant;

        // internal (do not use)
        void* data;
        uint32_t index;

        UltraEntry(const UltraEntry&) = default;

        UltraEntry(int32_t id)
            : text(),
              id(id),
              grayed(false),
              hidden(false),
              variant(),
              data(),
              index() {};

        UltraEntry(const char* str = nullptr, int32_t id = UG_INVALID_ID, bool grayed = false,
                   const QVariant& variant = QVariant())
            : text(str),
              id(id),
              grayed(grayed),
              hidden(false),
              variant(variant),
              data(),
              index() {};

        UltraEntry(const QString& str, int32_t id = UG_INVALID_ID, bool grayed = false,
                   const QVariant& variant = QVariant())
            : text(str),
              id(id),
              grayed(grayed),
              hidden(false),
              variant(variant),
              data(),
              index() {};
    };

    typedef QVector<UltraEntry> UGEntrySet;

    enum AnimationSpeed
    {
        AS_Slow,
        AS_Normal,
        AS_Fast,
        AS_Superfast
    };

    enum ArrayDirection
    {
        AD_Horizontal,
        AD_Vertical,
    };

}  // namespace gui

#endif  // ULTRAGUI_TYPES_H
