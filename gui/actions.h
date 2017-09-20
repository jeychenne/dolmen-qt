/*
 * actions.h
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
 * Purpose: implement menu actions
 * Created: 20/09/2010
 */

#ifndef ACTIONS_H
#define ACTIONS_H

#include <QAction>
#include <QKeySequence>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QList>
#include <QXmlStreamReader>
#include <QMessageBox>

#include "dolmen.h"
#include "lib/Project.h"
#include "gui/CorpusBrowser.h"
#include "gui/PropertyEditor.h"
#include "utils/Global.h"
#include "utils/routines.h"
#include "gui/MetadataEditor.h"

class CorpusBrowser;

/**************************************************************************************************
 ***                               CLASS ACTION_OPEN_PROJECT                                   ****
 **************************************************************************************************/

class ActionOpenProject : public QAction
{
	Q_OBJECT

public:
	ActionOpenProject(QWidget *parent, Project *project);

public slots:
	void execute();

signals:
	void menu_openProject(const QString path);
};


/**************************************************************************************************
 ***                               CLASS ACTION_SAVE_PROJECT                                   ****
 **************************************************************************************************/

class ActionSaveProject : public QAction
{
	Q_OBJECT

public:
	ActionSaveProject(QWidget *parent, Project *project);

public slots:
	void execute();

signals:
	void menu_saveProject(QString &path);

private:
	Project *project;
};


/**************************************************************************************************
 ***                               CLASS ACTION_SAVE_PROJECT_AS                                ****
 **************************************************************************************************/

class ActionSaveProjectAs : public QAction
{
	Q_OBJECT

public:
	ActionSaveProjectAs(QWidget *parent, Project *project);

public slots:
	void execute();

signals:
	void menu_saveProject(QString &path);

private:
	Project *project;
};


/**************************************************************************************************
 ***                            CLASS ACTION_SAVE_SINGLE_FILE                                  ****
 **************************************************************************************************/

class ActionSaveSingleFile : public QAction
{
	Q_OBJECT

public:
	ActionSaveSingleFile(QWidget *parent, Project *project);

public slots:
	void execute();

signals:
	void menu_saveSingleFile(DFile *);
private:
	CorpusBrowser *fb;
};


/**************************************************************************************************
 ***                            CLASS ACTION_REMOVE_VNODE                                      ****
 **************************************************************************************************/

class ActionRemoveVNode : public QAction
{
	Q_OBJECT

public:
	ActionRemoveVNode(QWidget *parent, Project *project);

public slots:
	void execute();

signals:
	void menu_removeVFiles(QList<VFileNode*> &);

private:
	CorpusBrowser *fb;
};


/**************************************************************************************************
 ***                            CLASS ACTION_CONVERT_TO_DMF                                    ****
 **************************************************************************************************/

class ActionConvertToDmf : public QAction
{
	Q_OBJECT

public:
	ActionConvertToDmf(QWidget *parent, Project *project);

public slots:
	void execute();

signals:
	void menu_textgrid2dmf(QStringList &);

private:
	CorpusBrowser *fb;
};


/**************************************************************************************************
 ***                               CLASS ACTION_ADD_FILE                                       ****
 **************************************************************************************************/

class ActionAddFile : public QAction
{
	Q_OBJECT

public:
	ActionAddFile(QWidget *parent, Project *project);

public slots:
	void execute();

signals:
	void menu_addFilesToProject(QStringList paths);
};

/**************************************************************************************************
 ***                               CLASS ACTION_ADD_DIR                                        ****
 **************************************************************************************************/

class ActionAddFolder : public QAction
{
	Q_OBJECT

public:
	ActionAddFolder(QWidget *parent, Project *project);

public slots:
	void execute();

signals:
	void menu_addFolderToProject(const QString &);
};

/**************************************************************************************************
 ***                               CLASS ACTION_RENAME_FOLDER                                  ****
 **************************************************************************************************/

class ActionRenameFolder : public QAction
{
	Q_OBJECT

public:
	ActionRenameFolder(QWidget *parent, Project *project);

public slots:
	void execute();

signals:
	void menu_renameFolder(VFolder *, const QString &);

private:
	CorpusBrowser *fb;
};


/**************************************************************************************************
 ***                           CLASS ACTION_MOVE_FILES_TO_FOLDER                               ****
 **************************************************************************************************/

class ActionMoveFilesToFolder : public QAction
{
	Q_OBJECT

public:
	ActionMoveFilesToFolder(QWidget *parent);

public slots:
	void execute();

signals:
	void menu_moveFilesToFolder(QString &folder);
};


/**************************************************************************************************
 ***                              CLASS ACTION_ADD_TAG                                         ****
 **************************************************************************************************/

class ActionAddProperty : public QAction
{
	Q_OBJECT

public:
	ActionAddProperty(QWidget *parent, QString &tag, QString &name);

public slots:
	void execute();

signals:
	void menu_addProperty(Property *);


private:
	QString tag;
};


/**************************************************************************************************
 ***                              CLASS ACTION_NEW_TAG                                         ****
 **************************************************************************************************/
class ActionNewProperty : public QAction
{
	Q_OBJECT

public:
	ActionNewProperty(QWidget *parent);

public slots:
	void execute();

private:
	QString label;

};


/**************************************************************************************************
 ***                            CLASS ACTION_REMOVE_TAG                                        ****
 **************************************************************************************************/
class ActionRemoveProperty : public QAction
{
	Q_OBJECT

public:
	ActionRemoveProperty(QWidget *parent, QString &tag, QString &name);

public slots:
	void execute();

signals:
	void menu_removeProperty(Property *);

private:
	QString tag;
};


/**************************************************************************************************
 ***                            CLASS ACTION_DELETE_TAG                                        ****
 **************************************************************************************************/

// remove a property permanently from project
class ActionDeleteProperty : public QAction
{
	Q_OBJECT

public:
	ActionDeleteProperty(QWidget *parent);

public slots:
	void execute();

signals:
	void menu_delProperty(Property *);

};

/**************************************************************************************************
 ***                            CLASS ACTION_BIND_SOUND                                        ****
 **************************************************************************************************/

// remove a property permanently from project
class ActionBindSound : public QAction
{
	Q_OBJECT

public:
	ActionBindSound(Annotation *annot, Sound *sound, QWidget *parent);

public slots:
	void execute();

private:
	Annotation *annot;
	Sound *snd;
};


/**************************************************************************************************
 ***                               CLASS ACTION_OPEN_QUERY                                     ****
 **************************************************************************************************/

class ActionOpenQuery : public QAction
{
	Q_OBJECT

public:
	ActionOpenQuery(QWidget *parent, Project *project);

public slots:
	void execute();

signals:
	void menu_openQuery(const QString &query);

private:
	Project *project;
};

/**************************************************************************************************
 ***                            CLASS ACTION_EXPORT_ANNOT_TXT                                  ****
 **************************************************************************************************/

class ActionExportAnnotTxt : public QAction
{
	Q_OBJECT

public:
	ActionExportAnnotTxt(DFile *f, QWidget *parent);

public slots:
	void execute();

private:
	DFile *file;
};


/**************************************************************************************************
 ***                            CLASS ACTION_EXPORT_ANNOT_TEXTGRID                             ****
 **************************************************************************************************/

class ActionExportAnnotTextGrid : public QAction
{
	Q_OBJECT

public:
	ActionExportAnnotTextGrid(Annotation *annot, QWidget *parent);

public slots:
	void execute();

private:
	Annotation *annot;
};


/**************************************************************************************************
 ***                            CLASS ACTION_EXPORT_TABLE_EXCEL                                ****
 **************************************************************************************************/

class ActionExportTableExcel : public QAction
{
    Q_OBJECT

public:
    ActionExportTableExcel(DataTable *table, QWidget *parent);

public slots:
    void execute();

private:
    DataTable *table;
};

/**************************************************************************************************
 ***                            CLASS ACTION_EXPORT_TABLE_CSV                                  ****
 **************************************************************************************************/

class ActionExportTableCsv : public QAction
{
    Q_OBJECT

public:
    ActionExportTableCsv(DataTable *table, QWidget *parent);

public slots:
    void execute();

private:
    DataTable *table;
};

/**************************************************************************************************
 ***                            CLASS ACTION_IMPORT_META_CSV                                   ****
 **************************************************************************************************/

class ActionImportMetaCsv : public QAction
{
    Q_OBJECT

signals:
    void sendFileName(const QString &);

public:
    ActionImportMetaCsv(QWidget *parent);

public slots:
    void execute();
};




/**************************************************************************************************
 ***                            CLASS ACTION_EDIT_DFILE_METADATA                               ****
 **************************************************************************************************/

class ActionEditDFileMetadata : public QAction
{
	Q_OBJECT

public:
	ActionEditDFileMetadata(DFile *df, QWidget *parent);
    ActionEditDFileMetadata(QList<DFile*> files, QWidget *parent);
public slots:
	void execute();

private:
    QList<DFile*> m_files;
};


#endif // ACTIONS_H
