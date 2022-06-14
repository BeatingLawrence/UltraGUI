#ifndef ULTRAGUI_H
#define ULTRAGUI_H

#include <QString>
#include "./UltraGUI_global.h"

namespace hci
{
    class ULTRAGUI_EXPORT UltraGui
    {
        public:
            struct Version_t
            {
                uint16_t maj;
                uint16_t min;
                uint16_t pat;
                QString completeText;
            };

            static Version_t version();
    };
}

#endif // ULTRAGUI_H
