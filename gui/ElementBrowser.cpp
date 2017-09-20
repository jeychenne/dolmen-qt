
#include "ElementBrowser.h"

ElementBrowser::ElementBrowser(QWidget *parent) :
    QTreeWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);

#ifdef Q_OS_MAC // color the sidebar (as in Mail or Finder)
    QPalette p(this->palette());
    p.setColor(QPalette::Base, "#dce4eb");
    setPalette(p);
    setStyleSheet("QTreeWidget { border: 0px; }");
#endif

    //setDragDropMode(QAbstractItemView::InternalMove);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    //setDropIndicatorShown(true);

    setEditTriggers(QAbstractItemView::SelectedClicked);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(sizePolicy.hasHeightForWidth());
    setSizePolicy(sizePolicy);
    setFrameShadow(QFrame::Plain);
}

IBrowserElement *ElementBrowser::elementFromId(int id) const
{
    for (auto elem: m_content)
    {
        if (elem->id() == id)
            return elem;
    }

    return NULL;
}

IBrowserElement *ElementBrowser::elementFromTreeItem(QTreeWidgetItem *item)
{
    bool ok;
    int id = item->data(0, Qt::UserRole).toInt(&ok);

    return ok ? elementFromId(id) : nullptr;
}

void ElementBrowser::redraw(QList<IBrowserElement*> elements)
{
    this->clear();
    finalizeElements();
    m_content = std::move(elements);
    displayElements();
}
