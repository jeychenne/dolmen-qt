/*
 * SideBar.h
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
 * Purpose: hold browsers on the left hand side of the main window
 * Created: 12/04/2011
 */

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QToolBar>
#include <QLabel>

class QToolButton;
class QButtonGroup;

#include "gui/CorpusBrowser.h"
#include "gui/BookmarkBrowser.h"
#include "gui/ScriptBrowser.h"
#include "lib/DFile.h"

// Left-side bar for file browsing

class FileSidebar : public QWidget
{
	Q_OBJECT

public:
    FileSidebar(QWidget *parent = NULL);
    void setCorpus(CorpusBrowser *value);
    void setBookmarks(BookmarkBrowser *value);
    void setScripts(ScriptBrowser *value);

public slots:
    void showCorpus();

signals:
    void updateUi();

private slots:
    void toggleCorpus(bool);
    void toggleBookmarks(bool);
    void toggleScripts(bool);

private:
    QLabel *label;
    QStackedWidget *stack;
    CorpusBrowser *corpus;
    BookmarkBrowser *bookmarks;
    ScriptBrowser *scripts;

    QToolButton *btn_corpus, *btn_bookmarks, *btn_scripts;

    void addItem(QWidget *w);
    QToolButton *makeButton(QButtonGroup *group, QWidget *parent, const QIcon &icon, const QString &tip);
};



#endif // SIDEBAR_H
