#ifndef UGBUTTONARRAY_H
#define UGBUTTONARRAY_H

// QPalette roles used by UGButtonArray:
// - No direct role in paint; colors are managed by child UGButton widgets.

#include <QBoxLayout>
#include <QWidget>

#include "ugbutton.h"

namespace gui
{
    class UGButtonArray : public QWidget
    {
        Q_OBJECT
       private:
        QBoxLayout m_layout;

        QVector<UGButton*> m_buttons;

        int32_t m_selector;

        virtual void paintEvent(QPaintEvent* event) override;

       public:
        explicit UGButtonArray(QWidget* parent = nullptr);

        virtual ~UGButtonArray();

        void setArrayDir(ArrayDirection dir);

        void addButton(const UltraEntry& entry, bool active = false);

        void clear();

        void setVisible(int32_t id, bool visible);

        // return the latest pressed button entry
        // return nullptr if no entry has been selected
        const UltraEntry* current();

       private slots:
        void onPress(const UltraEntry*);

       signals:
        // emitted when a button is clicked
        void onClick(const UltraEntry*);

        // emitted when the current entry changes
        void onChange(const UltraEntry*);
    };

}  // namespace gui

#endif  // UGBUTTONARRAY_H
