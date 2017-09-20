/*
 * BookmarkBrowser.h
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
 * Purpose: browse bookmarks
 * Created: 12/04/2011
 */

#ifndef BOOKMARKBROWSER_H
#define BOOKMARKBROWSER_H

#include "lib/Bookmark.h"
#include "lib/DFile.h"
#include "search/SearchMatch.h"
#include "gui/ElementBrowser.h"


class BookmarkBrowser : public ElementBrowser
{
    Q_OBJECT

public:
	BookmarkBrowser(QWidget *parent = NULL);

signals:
	void openDFile(DFile *df, double start, double end);
    void removeElements(QList<IBrowserElement*>);
    void elementClicked(IBrowserElement *bm);

protected:
    void displayElements() override;
    void finalizeElements() override;

private slots:
	void onItemDoubleClicked(QTreeWidgetItem *);
    void onItemClicked(QTreeWidgetItem *);
	void showContextMenu(const QPoint &);
    void onOpenInView();
    void onOpenInPraat();
	void onRemoveBookmarks();

};

#endif // BOOKMARKBROWSER_H
