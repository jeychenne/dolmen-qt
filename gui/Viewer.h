/*
 * Viewer
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

#ifndef VIEWER_H
#define VIEWER_H

#include <QObject>
#include <QTabWidget>

#include "StartView.h"
#include "View.h"
#include "AnnotationView.h"
#include "SoundView.h"
#include "TableView.h"
#include "ProjectView.h"

class Viewer : public QTabWidget
{
	Q_OBJECT

public:
    Viewer(QWidget *parent, QWidget *app);
	void setStartView();
	View* currentView() const;
	void openProjectView(Project *project);

	// overload to take care of all opened views
	bool close();

	bool contains(View *);
	bool contains(const QString &id);

	void setCurrentId(const QString &id); // focus widget based on its ID
    void displayHelp(const QString &page);

signals:
	void displayText(QString);

public slots:
	void closeTab(int index);
	void closeView(int index);
	void openFile(DFile *, double start = 0., double end = 0.);

private:
    QWidget *m_app;


};

#endif // VIEWER_H
