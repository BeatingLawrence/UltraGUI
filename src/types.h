#ifndef ULTRAGUI_TYPES_H
#define ULTRAGUI_TYPES_H

#include <QString>
#include <QVariant>
#include <QVector>

#include "./ultragui_global.h"

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

    // UltraEntry structure
    struct ULTRAGUI_EXPORT UltraEntry
    {
        QString entryText;
        int32_t entryId;
        bool grayed;
        QVariant variant;
        void* data;
    };

    static const int32_t ULTRAGUI_EXPORT InvalidID = -1;

    enum AnimationSpeed
    {
        AS_Slow,
        AS_Normal,
        AS_Fast,
        AS_Superfast
    };

}  // namespace gui

#endif  // ULTRAGUI_TYPES_H
