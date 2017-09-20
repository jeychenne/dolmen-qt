/*
 * BookmarkBrowser.cpp
 *
 * Copyright (C) 2010-2017 Julien Eychenne
 *
 * This file is part of Dolmen.
 *
 * Dolmen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (GPL) as
 * published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * Dolmen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dolmen. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:  Julien Eychenne
 * Purpose:
 * Created: 12/04/2011
 */

#include <cassert>
#include "BookmarkBrowser.h"



BookmarkBrowser::BookmarkBrowser(QWidget *parent) :
    ElementBrowser(parent)
{
	connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClicked(QTreeWidgetItem*)));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));

#ifdef Q_OS_MAC
	headerItem()->setHidden(true);
#else
    headerItem()->setText(0, "Bookmarks");
#endif
}

void BookmarkBrowser::displayElements()
{
    QIcon icon(":/icons/32x32/bookmark.png");
    for (auto *elem : m_content)
    {
        Bookmark *bm = dynamic_cast<Bookmark*>(elem);
        QTreeWidgetItem *item = new QTreeWidgetItem(this);
        item->setText(0, bm->title());
        item->setToolTip(0, bm->tooltip());
        item->setIcon(0, icon);
        item->setData(0, Qt::UserRole, bm->id());

        if (bm->type() == "SearchMatch")
        {
            SearchMatch *qm = static_cast<SearchMatch*>(bm);
            connect(qm, SIGNAL(openDFile(DFile*,double,double)), this, SIGNAL(openDFile(DFile*,double,double)));
        }
    }
}

void BookmarkBrowser::finalizeElements()
{
    for (auto elem: m_content)
    {
        auto bm = dynamic_cast<Bookmark*>(elem);

        if (bm->type() == "SearchMatch")
        {
            SearchMatch *qm = static_cast<SearchMatch*>(bm);
            disconnect(qm, SIGNAL(openDFile(DFile*,double,double)), this, SIGNAL(openDFile(DFile*,double,double)));
        }
    }
}

void BookmarkBrowser::onItemDoubleClicked(QTreeWidgetItem *item)
{
    auto bm = m_content.at(indexFromItem(item).row());
	bm->open();
}

void BookmarkBrowser::onItemClicked(QTreeWidgetItem *item)
{
    auto bm = m_content.at(indexFromItem(item).row());
    emit elementClicked(bm);
}

void BookmarkBrowser::showContextMenu(const QPoint &pos)
{
	ContextMenu menu(this);

    auto open_view = new QAction(tr("Open in view"), this);
    auto open_praat = new QAction(tr("Open in Praat"), this);
    auto remove = new QAction(tr("Remove"), this);

    menu.addAction(open_view);
    menu.addAction(open_praat);
    menu.addSeparator();
    menu.addAction(remove);

    connect(remove, SIGNAL(triggered()), this, SLOT(onRemoveBookmarks()));
    connect(open_view, SIGNAL(triggered()), this, SLOT(onOpenInView()));
    connect(open_praat, SIGNAL(triggered()), this, SLOT(onOpenInPraat()));

    menu.display(pos);
}

void BookmarkBrowser::onOpenInView()
{
    auto bm = elementFromTreeItem(selectedItems().first());
    assert(bm);
    bm->open();
}

void BookmarkBrowser::onOpenInPraat()
{
    auto bm = dynamic_cast<Bookmark*>(elementFromTreeItem(selectedItems().first()));
    assert(bm);
    bm->openInPraat();
}

void BookmarkBrowser::onRemoveBookmarks()
{
    QList<IBrowserElement*> lst;

    for (auto item: selectedItems())
	{
        auto bm = elementFromTreeItem(item);
        if (bm) lst << bm;
	}

    emit removeElements(lst);
}










