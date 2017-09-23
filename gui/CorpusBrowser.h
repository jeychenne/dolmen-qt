/*
 * CorpusBrowser.h
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
 * Created: 01/05/2010
 */

#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QObject>
#include <QFileInfo>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDropEvent>
#include <QQueue>
#include <QStatusBar>

#include "dolmen.h"
#include "lib/Project.h"
#include "lib/VFolder.h"
#include "lib/VFile.h"
#include "lib/metadata.h"
#include "lib/praat.h"
#include "lib/Bookmark.h"
#include "gui/ContextMenu.h"
#include "gui/actions.h"
#include "gui/MetadataEditor.h"

#define GET_VNODE(ITEM) Global::IdMap[ITEM -> data(0, Qt::UserRole).toInt()]
//class BrowserItem;


typedef struct struct_Movement
{
	QTreeWidgetItem *item, *newParent;
	int newPos;
} Movement;


using namespace metadata;


// Functions
bool allTextgrids(QList<QTreeWidgetItem*> items);

void init_treeItem(QTreeWidgetItem *item, VFileNode *vfile);


/**************************************************************************************************
 ***                               CLASS FILE_BROWSER                                          ****
 **************************************************************************************************/

class CorpusBrowser : public QTreeWidget
{
	Q_OBJECT

public:
    CorpusBrowser(Project *, PraatInstance *);

public slots:
	void updateTree(VFolder *vfolder); // keep in sync with the project
	void onRightClick(const QPoint &);
	void onCreateFolder(QString &);
	void forwardProperty(Property *); // add property to files
	void deleteProperty(Property *); // remove property from project
	void dropProperty(Property *); // remove property from files
	void resetLabels();
    void setFileColor(); // modified files are shown with a star

	QTreeWidgetItem* childFromParentIndex(const QModelIndex &parent, int index);

	/* instead of using a custom model, we monitor the following methods to keep
	   the project file system and the tree in sync. */
	void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
	void rowsInserted(const QModelIndex &parent, int start, int end);
	void dropEvent(QDropEvent *event);

    void clearProject();

private slots:
	void onItemDoubleClicked(QTreeWidgetItem *item);
    void onItemSelected(QTreeWidgetItem *item);
	void editSelectedFile();
	void openFileInPraat();
    void createAnnotation();


signals:
	void tree_modified();
	void tree_mergeIntoFolder(QList<VFileNode*> files, QString &name);
	void tree_tagFile(Property *, VFile *);
	void tree_untagFile(Property *, VFile *);
	void tree_deleteProperty(Property *);
    //void tree_sendMetadata(QString meta);
	void tree_openFile(DFile *);
    void tree_fileSelected(DFile *);


private:
    Project          *m_project;
    PraatInstance    *m_praat;
    QLabel           *label_project; // On mac, display title in a custom label

    QQueue<Movement*> mv_buffer; // intercept items being moved in the tree
    QQueue<Movement*> drop_queue; // queue from buffer (items are processed in dropEvent())


	void buildNode(VFolder *vfolder, QTreeWidgetItem *node);
	void buildNode(VFolder *vfolder);

	void setExportSubmenu(ContextMenu &menu, DFile *f);
	void addSubmenuHeader(QMenu *submenu, QString header);
	void setHeaderStar();
    void setSubfileColor(QTreeWidgetItem*);
	void updateToolTip(QTreeWidgetItem *item);
    DFile * getSelectedDFile() const;

};

#endif // FILEBROWSER_H
