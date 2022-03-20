#ifndef ULTRAGUI_H
#define ULTRAGUI_H

#include <QString>
#include <cstdlib>
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

            static Version_t version()
            {
                Version_t version;
                version.maj = atoi(VERSION_MAJ);
                version.min = atoi(VERSION_MIN);
                version.pat = atoi(VERSION_PAT);
                version.completeText = QString("UltraGUI-%1.%2.%3%4").arg(VERSION_MAJ, VERSION_MIN, VERSION_PAT, VERSION_SUF);
                return version;
            }
    };
}

#endif // ULTRAGUI_H
