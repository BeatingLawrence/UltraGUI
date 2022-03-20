#ifndef ULTRATYPES_H
#define ULTRATYPES_H

#include<QString>
#include<QVector>
#include "./UltraGUI_global.h"

namespace hci
{
    class ULTRAGUI_EXPORT UltraTypes
    {
        public:
            //UltraEntry structure
            struct UltraEntry
            {
                QString entryText;
                int32_t entryId;
                bool grayed;
                void* data;
            };

            static const int32_t UltraEntryInvalidID = -1;

            //Creates a single UltraEntry with an ID and a string
            static UltraEntry createUltraEntry(QString string, int32_t id = UltraEntryInvalidID)
            {
                UltraEntry entry;
                entry.entryId = id;
                entry.entryText = string;
                entry.grayed = false;
                entry.data = nullptr;
                return entry;
            }

            //generates a vector of UltraEntry from a vector af strings. IDs are sequentials.
            static QVector<UltraEntry> createUltraEntryVector(QVector<QString> strings)
            {
                QVector<UltraEntry> toReturn;
                int32_t i = 0;

                for(auto& el : strings)
                {
                    toReturn.push_back(createUltraEntry(el, i++));
                }

                return toReturn;
            }
    };
}

#endif // ULTRATYPES_H
