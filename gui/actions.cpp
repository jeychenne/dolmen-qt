/*
 * actions.cpp
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
 * Purpose: provide all actions used in menus.
 * Created: 03/09/10
 */

#include "actions.h"

// There is a bug with QT5 and GTK, and can no longer call QFileDialog twice as it hangs the second time.
// One workaround is to pass QFileDialog::DontUseNativeDialog, another one that seems to work better is to pass a NULL parent.
// We go with the second option.
// See: https://forum.qt.io/topic/49209/qfiledialog-getopenfilename-hangs-in-windows-when-using-the-native-dialog/15
#ifdef Q_OS_LINUX
#define ACTION_PARENT nullptr
#else
#define ACTION_PARENT this->parentWidget()
#endif

/**************************************************************************************************
 ***                               CLASS ACTION_OPEN_PROJECT                                   ****
 **************************************************************************************************/

ActionOpenProject::ActionOpenProject(QWidget *parent, Project *project) :
		QAction(tr("&Open project..."), parent)
{
	setShortcut(QKeySequence("Ctrl+o"));
	setStatusTip("Open a project file");

	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
	connect(this, SIGNAL(menu_openProject(const QString)), project, SLOT(open(const QString)));
}

void ActionOpenProject::execute()
{
    QString path = nat(QFileDialog::getOpenFileName(ACTION_PARENT, tr("Open project...")));

	if (path != "")
		emit menu_openProject(path);
}


/**************************************************************************************************
 ***                               CLASS ACTION_SAVE_PROJECT                                   ****
 **************************************************************************************************/

ActionSaveProject::ActionSaveProject(QWidget *parent, Project *project) :
		QAction(tr("&Save project"), parent)
{
	this->project = project;
#ifdef Q_OS_UNIX
	this->setIcon(QIcon(":/icons/filesave.png"));
#endif
	setShortcut(QKeySequence("Ctrl+Shift+s"));
	setStatusTip("Save project to a file");

	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
    connect(this, SIGNAL(menu_saveProject(QString&)), project, SLOT(writeToDisk(QString&)));
}

void ActionSaveProject::execute()
{
	QString path = project->path();

	if (path == "")
	{
        // FIXME: see ActionProjectSaveAs...
        path = nat(QFileDialog::getSaveFileName(ACTION_PARENT, tr("Save project...")));

		if (path == "")
			return;

		if (! path.toLower().endsWith(DM_DOT_PROJECT_EXT))
			path += DM_DOT_PROJECT_EXT;
	}

	emit menu_saveProject(path);
}



/**************************************************************************************************
 ***                               CLASS ACTION_SAVE_PROJECT_AS                                ****
 **************************************************************************************************/

ActionSaveProjectAs::ActionSaveProjectAs(QWidget *parent, Project *project) :
		QAction(tr("&Save project as..."), parent)
{
	this->project = project;

	setStatusTip("Save project to a file");

	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
    connect(this, SIGNAL(menu_saveProject(QString&)), project, SLOT(writeToDisk(QString&)));
}

void ActionSaveProjectAs::execute()
{
    QString path = QFileDialog::getSaveFileName(ACTION_PARENT, tr("Save project..."));
    path = nat(path);

	if (path == "")
		return;

	if (! path.toLower().endsWith(DM_DOT_PROJECT_EXT))
		path += DM_DOT_PROJECT_EXT;

	emit menu_saveProject(path);
}


/**************************************************************************************************
 ***                            CLASS ACTION_CONVERT_TO_DMF                                    ****
 **************************************************************************************************/

ActionConvertToDmf::ActionConvertToDmf(QWidget *parent, Project *project) :
		QAction(tr("Convert TextGrid(s) to Dolmen File(s)"), parent)
{
	fb = qobject_cast<CorpusBrowser*>(parent);
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
	connect(this, SIGNAL(menu_textgrid2dmf(QStringList&)), project, SLOT(nativizeAnnotations(QStringList&)));
}

void ActionConvertToDmf::execute()
{
	QList<QTreeWidgetItem*> items = fb->selectedItems();
	QStringList paths = QStringList();

	foreach(QTreeWidgetItem *item, items)
	{
		VFile *vf = static_cast<VFile*>(GET_VNODE(item));
		paths << vf->object()->path();
	}

	emit menu_textgrid2dmf(paths);
}


/**************************************************************************************************
 ***                            CLASS ACTION_SAVE_SINGLE_FILE                                  ****
 **************************************************************************************************/

ActionSaveSingleFile::ActionSaveSingleFile(QWidget *parent, Project *project) :
		QAction(tr("Save file"), parent)
{
	setShortcut(QKeySequence("Ctrl+s"));
	fb = qobject_cast<CorpusBrowser*>(parent);
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
	connect(this, SIGNAL(menu_saveSingleFile(DFile*)), project, SLOT(onSaveFile(DFile*)));
}

void ActionSaveSingleFile::execute()
{
	VFile *vf = qobject_cast<VFile*>(GET_VNODE(fb->selectedItems()[0]));
	emit menu_saveSingleFile(vf->object());
}


/**************************************************************************************************
 ***                            CLASS ACTION_REMOVE_VNODE                                      ****
 **************************************************************************************************/

ActionRemoveVNode::ActionRemoveVNode(QWidget *parent, Project *project) :
		QAction(tr("Remove file(s) from project"), parent)
{
#ifdef Q_OS_UNIX
    this->setIcon(QIcon(":/icons/trash.png"));
#endif
    fb = qobject_cast<CorpusBrowser*>(parent);
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
	connect(this, SIGNAL(menu_removeVFiles(QList<VFileNode*>&)), project, SLOT(removeVFiles(QList<VFileNode*>&)));
}

void ActionRemoveVNode::execute()
{
	QList<VFileNode*> files = QList<VFileNode*>();

	foreach(QTreeWidgetItem *item, fb->selectedItems())
		files << GET_VNODE(item);

	emit menu_removeVFiles(files);
}

/**************************************************************************************************
 ***                               CLASS ACTION_ADD_FILE                                       ****
 **************************************************************************************************/

ActionAddFile::ActionAddFile(QWidget *parent, Project *project) :
		QAction(tr("&Add file(s) to project..."), parent)
{
	setShortcut(QKeySequence("Ctrl+Shift+a"));
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
	connect(this, SIGNAL(menu_addFilesToProject(QStringList)), project, SLOT(addFiles(QStringList)));
}

void ActionAddFile::execute()
{
    QStringList oldpaths = QFileDialog::getOpenFileNames(ACTION_PARENT, tr("Add file(s) to project..."),
													  dm_home());

	QStringList paths;
	foreach (QString path, oldpaths)
		paths << nat(path);

	if (paths.size() != 0)
	{
		paths.sort();
		emit menu_addFilesToProject(paths);
	}
}

/**************************************************************************************************
 ***                               CLASS ACTION_ADD_DIR                                        ****
 **************************************************************************************************/

ActionAddFolder::ActionAddFolder(QWidget *parent, Project *project) :
		QAction(tr("Add content of folder to project..."), parent)
{
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
	connect(this, SIGNAL(menu_addFolderToProject(QString)), project, SLOT(importFolder(QString)));
}

void ActionAddFolder::execute()
{
    QString path = nat(QFileDialog::getExistingDirectory(ACTION_PARENT,
                                                         tr("Add directory to project..."), dm_home()));

	if (path != "")
		emit menu_addFolderToProject(path);
}


/**************************************************************************************************
 ***                               CLASS ACTION_RENAME_FOLDER                                  ****
 **************************************************************************************************/

ActionRenameFolder::ActionRenameFolder(QWidget *parent, Project *project) :
		QAction(tr("Rename folder..."), parent)
{
	fb = qobject_cast<CorpusBrowser*>(parent);

	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
	connect(this, SIGNAL(menu_renameFolder(VFolder*,const QString&)),
			project, SLOT(renameFolder(VFolder*,const QString&)));
}

void ActionRenameFolder::execute()
{
	bool ok;
	QString text = QInputDialog::getText(this->parentWidget(),
		tr("Rename folder"), tr("New name for the folder:"),
		QLineEdit::Normal, QString::null, &ok);

	if (ok && ! text.isEmpty())
	{
		QTreeWidgetItem *item = fb->selectedItems()[0];
		VFolder *folder = qobject_cast<VFolder*>(GET_VNODE(item));

		item->setText(0, text);
		emit menu_renameFolder(folder, text);
	}
}


/**************************************************************************************************
 ***                           CLASS ACTION_MOVE_FILES_TO_FOLDER                               ****
 **************************************************************************************************/
ActionMoveFilesToFolder::ActionMoveFilesToFolder(QWidget *parent) :
		QAction(tr("Move files to folder..."), parent)
{
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
	CorpusBrowser *fb = qobject_cast<CorpusBrowser*>(parent);
	connect(this, SIGNAL(menu_moveFilesToFolder(QString&)), fb, SLOT(onCreateFolder(QString&)));
}


void ActionMoveFilesToFolder::execute()
{
	bool ok;
    QString text = QInputDialog::getText(ACTION_PARENT,
		tr("Create new folder"), tr("Name of the new folder:"),
		QLineEdit::Normal, QString::null, &ok);

	if (ok && ! text.isEmpty())
		emit menu_moveFilesToFolder(text);
}


/**************************************************************************************************
 ***                              CLASS ACTION_ADD_TAG                                         ****
 **************************************************************************************************/

ActionAddProperty::ActionAddProperty(QWidget *parent, QString &tag, QString &name) :
		QAction(name, parent)
{
	this->tag = tag;
	QFont font("Monospace");
	font.setStyleHint(QFont::TypeWriter);
	this->setFont(font);
	CorpusBrowser *fb = qobject_cast<CorpusBrowser*>(parent);
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
	connect(this, SIGNAL(menu_addProperty(Property*)), fb, SLOT(forwardProperty(Property*)));
}

void ActionAddProperty::execute()
{
	emit menu_addProperty(Property::properties[tag]);
}


/**************************************************************************************************
 ***                              CLASS ACTION_NEW_TAG                                         ****
 **************************************************************************************************/
// TODO: Remove.........................
ActionNewProperty::ActionNewProperty(QWidget *parent) :
		QAction(tr("Create new tag..."), parent)
{
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
	label = tr("New tag...");
}

void ActionNewProperty::execute()
{
	AddPropertyEditor *editor = new AddPropertyEditor(this->parentWidget());
	CorpusBrowser *fb = qobject_cast<CorpusBrowser*>(this->parentWidget());
	connect(editor, SIGNAL(menu_newProperty(Property*)), fb, SLOT(forwardProperty(Property*)));

	editor->exec();
}


/**************************************************************************************************
 ***                            CLASS ACTION_REMOVE_TAG                                        ****
 **************************************************************************************************/

ActionRemoveProperty::ActionRemoveProperty(QWidget *parent, QString &tag, QString &name) :
		QAction(name, parent)
{
	this->tag = tag;
	QFont font("Monospace");
	font.setStyleHint(QFont::TypeWriter);
	this->setFont(font);
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
	CorpusBrowser *fb = qobject_cast<CorpusBrowser*>(this->parentWidget());
	connect(this, SIGNAL(menu_removeProperty(Property*)), fb, SLOT(dropProperty(Property*)));
}

void ActionRemoveProperty::execute()
{
	emit menu_removeProperty(Property::properties[tag]);
}


/**************************************************************************************************
 ***                            CLASS ACTION_DELETE_TAG                                        ****
 **************************************************************************************************/

ActionDeleteProperty::ActionDeleteProperty(QWidget *parent) :
		QAction(tr("Remove property from project..."), parent)
{
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));

}

void ActionDeleteProperty::execute()
{
	DelPropertyEditor *editor = new DelPropertyEditor(this->parentWidget());
	CorpusBrowser *fb = qobject_cast<CorpusBrowser*>(this->parentWidget());
	connect(editor, SIGNAL(menu_rmProperty(Property*)), fb, SLOT(deleteProperty(Property*)));

	editor->exec();
}

/**************************************************************************************************
 ***                            CLASS ACTION_BIND_SOUND                                        ****
 **************************************************************************************************/
ActionBindSound::ActionBindSound(Annotation *annot, Sound *sound, QWidget *parent) :
		QAction(tr("Bind annotation to sound file"), parent)
{
	this->annot = annot;
	snd = sound;
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
}

void ActionBindSound::execute()
{
	annot->setSoundFile(snd);
}



/**************************************************************************************************
 ***                            CLASS ACTION_OPEN_QUERY                                        ****
 **************************************************************************************************/
ActionOpenQuery::ActionOpenQuery(QWidget *parent, Project *project) :
		QAction(tr("Open query..."), parent)
{
	this->project = project;
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
}

void ActionOpenQuery::execute()
{
	QString projectPath, queryString;
    QString path = nat(QFileDialog::getOpenFileName(ACTION_PARENT, tr("Open query..."),
												dm_home()));

	if (path == "") return;

	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		 return; // TODO: raise some exception

	QXmlStreamReader reader(&file);
	reader.readNext();

	while (! reader.atEnd())
	{
		if (reader.isStartElement())
		{
			if (reader.name() == "Project")
				projectPath = interpolatePath(reader.readElementText());

			else if (reader.name() == "QueryString")
				queryString = reader.readElementText();
		}

		reader.readNext();
	}

	file.close();

	if (queryString == "" || projectPath == "" || reader.hasError())
	{
		QString msg = QString("Problem parsing query %1").arg(path);
		QMessageBox::warning(NULL, tr("Error"), msg);
	}
	else if (file.error() != QFile::NoError)
	{
		QString msg = QString ("Cannot read query %1").arg(path);
		QMessageBox::warning(NULL, tr("Error"), msg);
	}
	else if (projectPath != Global::CurrentProjectPath)
	{
		QString msg = tr("This query doesn't match the current project");
		QMessageBox::critical(NULL, tr("Query error"), msg);
	}
	else
		emit menu_openQuery(queryString);
}



/**************************************************************************************************
 ***                            CLASS ACTION_EXPORT_ANNOT_TXT                                  ****
 **************************************************************************************************/

ActionExportAnnotTxt::ActionExportAnnotTxt(DFile *f, QWidget *parent) :
		QAction(tr("To plain text file..."), parent)
{
	file = f;
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
}

void ActionExportAnnotTxt::execute()
{
    QString path = nat(QFileDialog::getSaveFileName(ACTION_PARENT, tr("Save as plain text...")));

	if (path == "")
		return;


	if (isInstance(file, Annotation))
	{
		Annotation *annot = qobject_cast<Annotation*>(file);
		annot->writeToTxt(path);
	}

	// TODO: export Document to txt?
}


/**************************************************************************************************
 ***                            CLASS ACTION_EXPORT_ANNOT_TEXTGRID                             ****
 **************************************************************************************************/
ActionExportAnnotTextGrid::ActionExportAnnotTextGrid(Annotation *annot, QWidget *parent) :
		QAction(tr("Tiers to TextGrid..."), parent)
{
	this->annot = annot;
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
}

void ActionExportAnnotTextGrid::execute()
{
    QString path = nat(QFileDialog::getSaveFileName(ACTION_PARENT, tr("Save as plain text...")));

	if (path == "")
		return;

	if (! path.toLower().endsWith(".textgrid"))
		path += ".TextGrid";

	annot->writeToTextGrid(path);
}

/**************************************************************************************************
 ***                            CLASS ACTION_EXPORT_TABLE_EXCEL                                ****
 **************************************************************************************************/

ActionExportTableExcel::ActionExportTableExcel(DataTable *table, QWidget *parent) :
        QAction(tr("To Excel (XML)..."), parent)
{
    this->table = table;
    connect(this, SIGNAL(triggered()), this, SLOT(execute()));
}

void ActionExportTableExcel::execute()
{
    QString path = nat(QFileDialog::getSaveFileName(ACTION_PARENT,
                                                    tr("Save as Excel XML file..."), dm_home(),
                                                    tr("Excel (*.xls *.xml)")));

    if (path == "")
        return;

    if (! path.toLower().endsWith(".xls") && !path.toLower().endsWith(".xml"))
        path += ".xls";

    table->writeToExcel(path);
}

/**************************************************************************************************
 ***                            CLASS ACTION_EXPORT_TABLE_CSV                                  ****
 **************************************************************************************************/

ActionExportTableCsv::ActionExportTableCsv(DataTable *table, QWidget *parent) :
        QAction(tr("To comma-separated value (CSV) file..."), parent)
{
    this->table = table;
    connect(this, SIGNAL(triggered()), this, SLOT(execute()));
}

void ActionExportTableCsv::execute()
{
    QString path = nat(QFileDialog::getSaveFileName(ACTION_PARENT,
                                                    tr("Save as CSV..."), dm_home(),
                                                    tr("Excel (*.csv *.txt)")));

    if (path == "")
        return;

    if (! path.toLower().endsWith(".csv") && !path.toLower().endsWith(".txt"))
        path += ".csv";

    table->writeToCsv(path);
}


/**************************************************************************************************
 ***                            CLASS ACTION_IMPORT_META_CSV                                   ****
 **************************************************************************************************/

ActionImportMetaCsv::ActionImportMetaCsv(QWidget *parent) :
    QAction(tr("Import metadata from CSV file..."), parent)
{
    connect(this, SIGNAL(triggered()), this, SLOT(execute()));
}

void ActionImportMetaCsv::execute()
{
    QString path = QFileDialog::getOpenFileName(ACTION_PARENT, tr("Open CSV file..."), "", tr("CSV (*.csv)"));

    if (path.isEmpty())
        return;

    emit sendFileName(path);
}


/**************************************************************************************************
 ***                            CLASS ACTION_EDIT_DFILE_METADATA                               ****
 **************************************************************************************************/
ActionEditDFileMetadata::ActionEditDFileMetadata(DFile *df, QWidget *parent) :
        QAction(tr("Edit properties..."), parent)
{
    m_files << df;
	connect(this, SIGNAL(triggered()), this, SLOT(execute()));
}

ActionEditDFileMetadata::ActionEditDFileMetadata(QList<DFile*> files, QWidget *parent) :
        QAction(tr("Edit properties..."), parent)
{
    m_files = files;
    connect(this, SIGNAL(triggered()), this, SLOT(execute()));
}

void ActionEditDFileMetadata::execute()
{
    MetadataEditor *editor = new MetadataEditor(this->parentWidget(), m_files);
	editor->show();
}
