/*
 * SideBar.cpp
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

#include <QToolButton>
#include <QButtonGroup>
#include "FileSidebar.h"


FileSidebar::FileSidebar(QWidget *parent) :
    QWidget(parent)
{
    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 5);
    stack = new QStackedWidget;

    auto tool_layout = new QHBoxLayout;
    tool_layout->setContentsMargins(0, 0, 0, 0);

    auto group = new QButtonGroup;
	group->setExclusive(true);

	btn_corpus = makeButton(group, this, QIcon(":/icons/32x32/corpus.png"), tr("Corpus"));
	btn_bookmarks = makeButton(group, this, QIcon(":/icons/32x32/bookmark.png"), tr("Bookmarks"));
//    btn_scripts = makeButton(group, this, QIcon(":/icons/32x32/console.png"), tr("Scripts"));
#ifdef Q_OS_MAC
    tool_layout->addSpacing(5);
#endif
    tool_layout->addWidget(btn_corpus);
    tool_layout->addWidget(btn_bookmarks);
//    tool_layout->addWidget(btn_scripts);
    tool_layout->addStretch();
    btn_corpus->toggle();

    label = new QLabel(this);
    label->setContentsMargins(5, 5, 0, 0);
    auto font = label->font();
    font.setBold(true);
    label->setFont(font);
    label->setText("Corpus");

    layout->addWidget(label);
    layout->addWidget(stack);
    layout->addLayout(tool_layout);
    setLayout(layout);

    //TODO: update file browser on mac
//#ifdef Q_OS_MAC
//    this->setContentsMargins(0,0,0,0);
//    label_project = new QLabel(tr("PROJECT"));
//    label_project->setContentsMargins(10,5,0,0);
//    label_project->setStyleSheet("QLabel { color:#738192; font-weight:bold; font-family: Lucida Grande; font-size: 11pt; };");
//    QVBoxLayout *vl = new QVBoxLayout;
//    vl->setContentsMargins(0,0,0,0);
//    vl->addWidget(label_project);
//#endif


}

QToolButton *FileSidebar::makeButton(QButtonGroup *group, QWidget *parent, const QIcon &icon, const QString &tip)
{
    auto button = new QToolButton(parent);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIcon(icon);
    button->setToolTip(tip);
    QSize size(32, 32);
    button->setIconSize(QSize(24, 24));
    button->setFixedSize(size);
    button->setAutoRaise(true);
    button->setCheckable(true);
    group->addButton(button);

    return button;
}

void FileSidebar::setCorpus(CorpusBrowser *value)
{
    corpus = value;
    addItem(value);
    connect(btn_corpus, SIGNAL(toggled(bool)), this, SLOT(toggleCorpus(bool)));
}

void FileSidebar::setBookmarks(BookmarkBrowser *value)
{
    bookmarks = value;
    addItem(value);
    connect(btn_bookmarks, SIGNAL(toggled(bool)), this, SLOT(toggleBookmarks(bool)));
}

void FileSidebar::setScripts(ScriptBrowser *value)
{
    scripts = value;
    addItem(value);
    connect(btn_scripts, SIGNAL(toggled(bool)), this, SLOT(toggleScripts(bool)));
}

void FileSidebar::showCorpus()
{
    if (!btn_corpus->isChecked()) {
        btn_corpus->setChecked(true);
    }
    stack->setCurrentWidget(corpus);
}

void FileSidebar::addItem(QWidget *w)
{
    stack->addWidget(w);
}

void FileSidebar::toggleCorpus(bool checked)
{
    if (checked)
    {
        showCorpus();
        label->setText("Corpus");
    }
}

void FileSidebar::toggleBookmarks(bool checked)
{
    if (checked)
    {
        stack->setCurrentWidget(bookmarks);
        label->setText("Bookmarks");
    }
}

void FileSidebar::toggleScripts(bool checked)
{
    if (checked)
    {
        stack->setCurrentWidget(scripts);
        label->setText("Scripts");
    }
}



