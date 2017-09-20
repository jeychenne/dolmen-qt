/*
 * StartView.h
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
 * Created: 10/10/2010
 */

#ifndef STARTVIEW_H
#define STARTVIEW_H

#include <QWidget>
#include <QToolButton>
//#include <QTextBrowser>
#include "View.h"

//class QPushButton;


class StartView : public View
{
    Q_OBJECT
public:
	StartView(QTabWidget *parent);
	void leave();
	QString id() const;

signals:
    void open_project();
    void add_files();
    void open_settings();
    void open_doc();

private slots:
    void onAddFiles();
    void onOpenProject();
    void onOpenSettings();
    void onOpenDoc();

private:
    QToolButton *makeButton(QWidget *parent, const QIcon &icon, const QString &text);

};


#endif // STARTVIEW_H
