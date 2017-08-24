/*
 * dolmen.h
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

#include "BottomPane.h"



BottomPane::BottomPane(QWidget *parent, sol::state_view lua) : QTabWidget(parent)
{
    // QUERY TAB
    this->setContentsMargins(0,0,0,0);
	queries_tab = new QWidget();
	QVBoxLayout *vlayoutSearches = new QVBoxLayout(queries_tab);
	QVBoxLayout *vlayoutSearchList = new QVBoxLayout();
	vlayoutSearches->setContentsMargins(0,0,0,0);
	searches_list = new QListWidget(queries_tab);
	searches_list->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(searches_list, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displaySearchContextMenu(QPoint)));
	connect(searches_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClickQuery(QModelIndex)));

	searches_list->setFrameShadow(QFrame::Sunken);
	vlayoutSearchList->addWidget(searches_list);

	vlayoutSearches->addLayout(vlayoutSearchList);
	addTab(queries_tab, tr("Queries"));

	// OUTPUT TAB
//    QTextEdit *ed = new QTextEdit;
//    Global::Output.setEditor(this, ed);
//    addTab(ed, tr("Output"));

    console = new LuaConsole(lua);
    addTab(console, tr("Console"));
    setCurrentWidget(console);
}

void BottomPane::appendQuery(Query *query)
{
	QListWidgetItem *item = new QListWidgetItem(tr("Query %1: %2").arg(searches_list->count()+1).arg(query->toString()));
	// Lazy trick: use the whatsthis property to store the query
	item->setWhatsThis(query->toString());
    searches_list->addItem(item);
}

void BottomPane::setHandler(PluginHandler *handler)
{
    console->setHandler(handler);
}

void BottomPane::onDoubleClickQuery(QModelIndex index)
{
	emit reopenQuery(index.row());
}

void BottomPane::displaySearchContextMenu(const QPoint &pos)
{
	if (searches_list->count() == 0 || searches_list->selectedItems().count() != 1)
		return;

	ContextMenu *menu = new ContextMenu(this);

	QAction *dummyAction = new QAction(this);
	dummyAction->setText(tr("Queries:"));
	menu->addAction(dummyAction);
	dummyAction->setEnabled(false);
	menu->addSeparator();

	QAction *saveAction = new QAction(this);
	saveAction->setText(tr("Save query..."));
	menu->addAction(saveAction);

	connect(saveAction, SIGNAL(triggered()), this, SLOT(saveQuery()));
	menu->display(pos);
}

void BottomPane::saveQuery()
{
	if (Global::CurrentProjectPath == "")
	{
		QMessageBox::warning(this, tr("Error"), tr("You must save your project first."));
		return;
	}

	QString path = nat(QFileDialog::getSaveFileName(this, tr("Save query...")));

	if (path == "") return;
	if (! path.endsWith(DM_DOT_QUERY_EXT))
		path += DM_DOT_QUERY_EXT;

	QString query = searches_list->selectedItems()[0]->whatsThis();

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;


	QXmlStreamWriter writer(&file);

	writer.setAutoFormatting(true);
	writer.writeStartDocument();
	writer.writeStartElement("DolmenQuery");

	writer.writeStartElement("Project");
	// compress paths to make queries more portable
	writer.writeCharacters(compressPath(Global::CurrentProjectPath));
	writer.writeEndElement(); // </Project>

	writer.writeStartElement("QueryString");
	writer.writeCharacters(query);
	writer.writeEndElement(); // </QueryString>

	writer.writeEndDocument();
	file.close();

	if (file.error())
		emit file_error(tr("The file '%1' could not be written: %2").arg(path, file.errorString()));
}




