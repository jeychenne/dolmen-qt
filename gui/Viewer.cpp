/*
 * Viewer.cpp
 *
 * Copyright (C) 2010-2013 Julien Eychenne 
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
 * Created: 02/09/10
 */

#include <QDesktopServices>
#include "Viewer.h"
#include "utils/routines.h"

Viewer::Viewer(QWidget *parent, QWidget *app) :
    QTabWidget(parent)
{
	//this->setContentsMargins(0,0,0,0);
//	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    sizePolicy.setHorizontalStretch(5);
//	sizePolicy.setVerticalStretch(0);
//	sizePolicy.setHeightForWidth(sizePolicy.hasHeightForWidth());
//	setSizePolicy(sizePolicy);
    m_app = app;
	setTabsClosable(true);
	setMovable(true);

	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

    setDocumentMode(true);
}

void Viewer::setStartView()
{
    auto view = new StartView(this);
    connect(view, SIGNAL(add_files()), m_app, SLOT(onAddFiles()));
    connect(view, SIGNAL(open_project()), m_app, SLOT(onOpenProject()));
    connect(view, SIGNAL(open_doc()), m_app, SLOT(openDocumentation()));
    connect(view, SIGNAL(open_settings()), m_app, SLOT(editPreferences()));
}

void Viewer::displayHelp(const QString &page)
{
    QString url(getHelpDir());
    url.append(page);
    url.append("/index.html");

    if (! QFileInfo(url).exists())
    {
        QMessageBox dlg(QMessageBox::Critical, tr("Documentation not found"),
                    tr("Cannot find the documentation on disk.\nPlease adjust the path in the preferences."));
        dlg.exec();
    }
    else
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(url));
    }
}

void Viewer::openProjectView(Project *project)
{
	QString id = project->screenName();

	if (this->contains(id))
		setCurrentId(id);
	else
	{
		ProjectView *view = new ProjectView(this, project);
		setCurrentId(view->id());
	}
}

View* Viewer::currentView() const
{
	return qobject_cast<View*>(currentWidget());
}

void Viewer::closeTab(int i)
{
	this->closeView(i);

	if (this->count() == 0)
		setStartView();
}

void Viewer::closeView(int index)
{
	View *view = qobject_cast<View *>(this->widget(index));
	view->leave();
	removeTab(index);
	delete view;
}


void Viewer::openFile(DFile *dfile, double start, double end)
{
	if (this->contains(dfile->path()))
		this->setCurrentId(dfile->path());

	else if (isInstance(dfile, Annotation))
	{
		Annotation *annot = qobject_cast<Annotation*>(dfile);
		AnnotationView *view = new AnnotationView(this, annot, start, end);
		connect(view, SIGNAL(displayText(QString)), this, SIGNAL(displayText(QString)));
		this->setCurrentWidget(view);
	}
	else if (isInstance(dfile, Sound))
	{
		Sound *snd = qobject_cast<Sound*>(dfile);
		SoundView *view = new SoundView(this, snd);
		connect(view, SIGNAL(displayText(QString)), this, SIGNAL(displayText(QString)));
		Q_UNUSED(view);
		//this->setCurrentWidget(view);
	}

    else if (isInstance(dfile, DataTable))
    {
        DataTable *table = qobject_cast<DataTable*>(dfile);
        TableView *view = new TableView(this, table);
        connect(view, SIGNAL(displayText(QString)), this, SIGNAL(displayText(QString)));
        this->setCurrentWidget(view);
    }
}

bool Viewer::close()
{
	for (int i = 0; i < this->count(); ++i)
	{
		View *view = qobject_cast<View*>(this->widget(i));
		//view->close();
		Q_UNUSED(view); // TODO close views properly
	}

	return QWidget::close();
}

bool Viewer::contains(View *view)
{
	for (int i = 0; i < this->count(); ++i)
	{
		View *currentView = qobject_cast<View*>(this->widget(i));

		if (view->id() == currentView->id())
			return true;
	}

	return false;
}

bool Viewer::contains(const QString &id)
{
	for (int i = 0; i < this->count(); ++i)
	{
		View *currentView = qobject_cast<View*>(this->widget(i));

		if (id == currentView->id())
			return true;
	}

	return false;
}

void Viewer::setCurrentId(const QString &id)
{
	for (int i = 0; i < this->count(); ++i)
	{
		View *currentView = qobject_cast<View*>(this->widget(i));

		if (id == currentView->id())
		{
			this->setCurrentIndex(i); return;
		}
	}
}
