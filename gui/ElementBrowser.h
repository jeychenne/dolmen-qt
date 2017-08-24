#ifndef ELEMENTBROWSER_H
#define ELEMENTBROWSER_H

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QList>
#include "gui/ContextMenu.h"


// Base class for all tabs that can be displayed in the left
// sidebar, other than the corpus tab

class ElementBrowser : public QTreeWidget
{
    Q_OBJECT

public:
    ElementBrowser(QWidget *parent);

};

#endif // ELEMENTBROWSER_H
