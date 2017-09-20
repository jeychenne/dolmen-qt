/*
 * dolmen.h
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
 * Created: //
 */

//#include <QPushButton>
#include "StartView.h"


StartView::StartView(QTabWidget *parent) : View(parent, tr("Start"))
{
    auto grid = new QGridLayout;
    auto new_folder = makeButton(this, QIcon(":/icons/100x100/new_folder.png"), tr("Add files to project"));
    auto folder = makeButton(this, QIcon(":/icons/100x100/folder.png"), tr("Open existing project"));
    auto doc = makeButton(this, QIcon(":/icons/100x100/help.png"), tr("Documentation"));
    auto settings = makeButton(this, QIcon(":/icons/100x100/settings.png"), tr("Settings"));

    connect(new_folder, SIGNAL(clicked(bool)), this, SLOT(onAddFiles()));
    connect(folder, SIGNAL(clicked(bool)), this, SLOT(onOpenProject()));
    connect(doc, SIGNAL(clicked(bool)), this, SLOT(onOpenDoc()));
    connect(settings, SIGNAL(clicked(bool)), this, SLOT(onOpenSettings()));

    auto wgt = new QWidget;
    wgt->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    wgt->setLayout(grid);

    //grid->setSpacing(100);
    grid->addWidget(new_folder, 0, 0);
    grid->addWidget(folder, 0, 1);
    grid->addWidget(doc, 1, 0);
    grid->addWidget(settings, 1, 1);

    layout->addWidget(wgt); //addLayout(grid);
}

QString StartView::id() const
{
    return QString("00");
}

void StartView::onAddFiles()
{
    emit add_files();
}

void StartView::onOpenProject()
{
    emit open_project();
}

void StartView::onOpenSettings()
{
    emit open_settings();
}

void StartView::onOpenDoc()
{
    emit open_doc();
}

void StartView::leave()
{

}

QToolButton *StartView::makeButton(QWidget *parent, const QIcon &icon, const QString &text)
{
    auto button = new QToolButton(parent);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIcon(icon);
    button->setText(text);
    button->setIconSize(QSize(100, 100));
    button->setFixedSize(300, 150);
    button->setAutoRaise(true);

    return button;
}
