#ifndef ELEMENTBROWSER_H
#define ELEMENTBROWSER_H

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QList>
#include "lib/BrowserElement.h"
#include "gui/ContextMenu.h"


// Base class for all tabs that can be displayed in the left
// sidebar, other than the corpus tab

class ElementBrowser : public QTreeWidget
{
    Q_OBJECT

public:
    ElementBrowser(QWidget *parent);

public slots:
    void redraw(QList<IBrowserElement*> elements);

protected:
    QList<IBrowserElement*> m_content;

    virtual void finalizeElements() = 0;
    virtual void displayElements() = 0;

    IBrowserElement *elementFromId(int id) const;
    IBrowserElement *elementFromTreeItem(QTreeWidgetItem *item);
};

#endif // ELEMENTBROWSER_H
