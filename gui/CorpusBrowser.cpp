/*
 * CorpusBrowser.cpp
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
 * Created: 02/09/2010
 */

#include "CorpusBrowser.h"
#include "AnnotationCreator.h"

// Functions
bool allTextgrids(QList<QTreeWidgetItem *> items)
{
	if (items.size() == 0)
		return false;

	foreach(QTreeWidgetItem *item, items)
	{
		VFileNode *vnode = GET_VNODE(item);
		if (isInstance(vnode, VFile))
		{
			VFile *vf = qobject_cast<VFile*>(vnode);

			if (isInstance(vf->object(), Annotation))
			{
				Annotation *annot = qobject_cast<Annotation*>(vf->object());

				if (annot->isNative())
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}

	return true;
}


/* Returns true if all annotations in a folder have the same Speaker/Subject property */
bool isFolderSpeaker(VFolder *vfolder)
{
	QString value;

    foreach (auto vfile, vfolder->content())
	{
		if (isInstance(vfile, VFile))
		{
			VFile *file = qobject_cast<VFile*>(vfile);
			DFile *df = file->object();
			bool hasDifferentValue = false; // in case there are several speaker/subject properties per file, for some reason...

            foreach (Property *tag, df->properties())
			{
				if (tag->category() == QCoreApplication::tr("Speaker") || tag->category() == QCoreApplication::tr("Subject"))
				{
					if (value.isEmpty())
						value = tag->value();
					else if (value != tag->value())
						hasDifferentValue = true;
				}
			}

			if (hasDifferentValue)
				return false;
		}
	}

    // if value is empty, no property was found and it can't be a speaker folder
    return ! value.isEmpty();
}

/* Returns true if all the sub-folders in a folder are Speaker/Subject folders */
bool isFolderSurvey(VFolder *vfolder)
{
    foreach (auto vfile, vfolder->content())
	{
		if (isInstance(vfile, VFolder))
		{
			if (!isFolderSpeaker(qobject_cast<VFolder*>(vfile)))
				return false;
		}
	}

	return true;
}

void init_treeItem(QTreeWidgetItem *item, VFileNode *vfile)
{
	QIcon icon;

	if (isInstance(vfile, VFolder))
	{
		VFolder *folder = qobject_cast<VFolder*>(vfile);
		item->setText(0, folder->label());

        if (QIcon::hasThemeIcon("folder"))
			icon = QIcon::fromTheme("folder");
		else
		{
            icon = QIcon(":/icons/16x16/folder.png");
		}
	}
	else if (isInstance(vfile, VFile))
	{
		VFile *file = qobject_cast<VFile*>(vfile);
		// disable drop target
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

		QFileInfo info(file->object()->path());
		item->setText(0, info.fileName());

        if (isInstance(file->object(), Annotation))
            icon = QIcon(":/icons/16x16/annotation.png");
        else if (isInstance(file->object(), Document))
            icon = QIcon(":/icons/16x16/document.png");
        else if (isInstance(file->object(), DataTable))
            icon = QIcon(":/icons/16x16/datatable.png");
		else if (isInstance(file->object(), Sound))
            icon = QIcon(":/icons/16x16/sound.png");
	}

	item->setIcon(0, icon);
}




/**************************************************************************************************
 ***                               CLASS FILE_BROWSER                                          ****
 **************************************************************************************************/

CorpusBrowser::CorpusBrowser(Project * project, PraatInstance *praat, QLabel *project_label) :
	QTreeWidget()
{
	m_project		= project;
	m_praat			= praat;
	label_project	= project_label;

	setFocusPolicy(Qt::NoFocus);

#ifdef Q_OS_MAC // color the sidebar (as in Mail or Finder)
	QPalette p(this->palette());
	p.setColor(QPalette::Base, "#dce4eb");
	setPalette(p);
	setStyleSheet("QTreeWidget { border: 0px; }");
#endif

	mv_buffer = QQueue<Movement*>();
	drop_queue = QQueue<Movement*>();

	setDragDropMode(QAbstractItemView::InternalMove);
	setContextMenuPolicy(Qt::CustomContextMenu);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setDropIndicatorShown(true);

	setEditTriggers(QAbstractItemView::SelectedClicked);

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(sizePolicy.hasHeightForWidth());
	setSizePolicy(sizePolicy);
	setFrameShadow(QFrame::Plain);
	QString text = "The file browser keeps all the files in your project organized. "
				"You can add/remove files, move them around, or merge them into new folders."
				"The way files are organized in the file browser does not depend on the way they"
				"are stored on your hard drive.";

	setWhatsThis(text);

	connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemSelected(QTreeWidgetItem*)));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(onRightClick(const QPoint &)));

#ifdef Q_OS_MAC
	headerItem()->setHidden(true);
	connect(m_project, SIGNAL(project_saved()), this, SLOT(updateTitle()));
#else
	headerItem()->setText(0, "");
#endif
}

void CorpusBrowser::onItemSelected(QTreeWidgetItem *item)
{
    VFileNode *vnode = GET_VNODE(item);
    if (isInstance(vnode, VFile))
    {
        VFile *vf = qobject_cast<VFile*>(vnode);
        if (vf->object() != NULL)
            emit tree_fileSelected(vf->object());
    }
}

void CorpusBrowser::onItemDoubleClicked(QTreeWidgetItem *item)
{
	VFileNode *vnode = GET_VNODE(item);
	if (isInstance(vnode, VFile))
	{
		VFile *vf = qobject_cast<VFile*>(vnode);
		if (vf->object() != NULL)
			emit tree_openFile(vf->object());
	}
}

void CorpusBrowser::editSelectedFile()
{
    DFile *df = this->getSelectedDFile();

    if (df != NULL)
        emit tree_openFile(df);
}

void CorpusBrowser::onRightClick(const QPoint &pos)
{
    QSet<QString> existingProperties = QSet<QString>::fromList(Property::properties.keys());
	QList<QTreeWidgetItem*> items = selectedItems();
	ContextMenu menu(this);
	bool openProject = true;

	ActionSaveSingleFile *saveFile = NULL;

	if (findItems(QString(".*"), Qt::MatchRegExp).size() == 0)
		openProject = false;

	// get common properties from selected files
	foreach(QTreeWidgetItem *item, items)
	{
		VFileNode *vnode = GET_VNODE(item);

        QSet<QString> itemProperties = QSet<QString>::fromList(vnode->properties());
        existingProperties.intersect(itemProperties);
	}
   // QMultiHash<QString, QString> tagHash = hashProperties(Property::properties);


	/* adapt menu depending on the # of items selected and their type */
	if (items.size() == 1)
	{
        VFileNode *vnode = GET_VNODE(items[0]);

        if (isInstance(vnode, VFile))
		{
			VFile *vf = qobject_cast<VFile*>(vnode);

            if (isInstance(vf->object(), Sound))
            {
                QAction *action_create_annotation = new QAction(tr("Create annotation..."), this);
                menu.addAction(action_create_annotation);
                connect(action_create_annotation, SIGNAL(triggered()), this, SLOT(createAnnotation()));
                menu.addSeparator();
            }

			QAction *editAction = new QAction(tr("Open as view"), this);
#ifdef Q_OS_UNIX
            editAction->setIcon(QIcon(":/icons/16x16/annotation.png"));
#endif
			menu.addAction(editAction);
			connect(editAction, SIGNAL(triggered()), this, SLOT(editSelectedFile()));

            if (Global::PraatIsInstalled && (isInstance(vf->object(), Annotation) || isInstance(vf->object(), Sound)))
            {
                QAction *praatAction = new QAction(tr("Open file in Praat"), this);
#ifdef Q_OS_UNIX
                praatAction->setIcon(QIcon(":/icons/praat.png"));
#endif
                menu.addAction(praatAction);
                connect(praatAction, SIGNAL(triggered()), this, SLOT(openFileInPraat()));
            }
            menu.addSeparator();

			ActionEditDFileMetadata *metaAction = new ActionEditDFileMetadata(vf->object(), this);
			menu.addAction(metaAction);
			// temporarily disable when there's no sound file
			if (isInstance(vf->object(), Annotation))
			{
				Annotation *a = qobject_cast<Annotation*>(vf->object());
				editAction->setEnabled(a->soundFile() != NULL);
			}

			saveFile = new ActionSaveSingleFile(this, m_project);

			if (! vf->object()->hasUnsavedChanges())
				saveFile->setDisabled(true);

			this->setExportSubmenu(menu, vf->object());

		}
		else if (isInstance(vnode, VFolder))
		{
			menu.addAction(new ActionRenameFolder(this, m_project));
		}

		menu.addSeparator();
		menu.addAction(new ActionRemoveVNode(this, m_project));
	}
	else if (items.size() > 1)
	{
        // bind annotation to sound
		if (items.size() == 2)
		{
            VFileNode *vfile1 = GET_VNODE(items[0]);
            VFileNode *vfile2 = GET_VNODE(items[1]);

            if (isInstance(vfile1, VFile) && isInstance(vfile2, VFile))
            {
                VFile *file1 = qobject_cast<VFile*>(vfile1);
                VFile *file2 = qobject_cast<VFile*>(vfile2);

				if (isInstance(file1->object(), Annotation) && isInstance(file2->object(), Sound))
                {
					Annotation *annot = qobject_cast<Annotation*>(file1->object());
					Sound *snd = qobject_cast<Sound*>(file2->object());
                    // display bind action unless the annotation is already bound to this sound file
                    if (annot->soundFile() != snd)
                    {
                        menu.addAction(new ActionBindSound(annot, snd, this));
                        menu.addSeparator();
                    }
                }
				else if (isInstance(file2->object(), Annotation) && isInstance(file1->object(), Sound))
                {
					Annotation *annot = qobject_cast<Annotation*>(file2->object());
					Sound *snd = qobject_cast<Sound*>(file1->object());
                    if (annot->soundFile() != snd)
                    {
                        menu.addAction(new ActionBindSound(annot, snd, this));
                        menu.addSeparator();
                    }
                }
            }
		}

        // get DFile's
        QList<DFile*> files;
        foreach(QTreeWidgetItem *item, items)
        {
            VFileNode *vnode = GET_VNODE(item);
            if (isInstance(vnode, VFile))
            {
                VFile *vf = qobject_cast<VFile*>(vnode);
                files << vf->object();
            }
        }

        ActionEditDFileMetadata *metaAction = new ActionEditDFileMetadata(files, this);
        menu.addAction(metaAction);
		menu.addAction(new ActionMoveFilesToFolder(this));

		menu.addSeparator();
		menu.addAction(new ActionRemoveVNode(this, m_project));
	}

	if (allTextgrids(items))
	{
		menu.addSeparator();
		menu.addAction(new ActionConvertToDmf(this, m_project));
	}


	if (! openProject)
	{
		menu.addAction(new ActionOpenProject(this, m_project));
		menu.addAction(new ActionAddFile(this, m_project));
		menu.addAction(new ActionAddFolder(this, m_project));
	}
	else
	{
		//menu.addSeparator();
		menu.addAction(new ActionAddFile(this, m_project));
	}

	//menu.addSeparator();

    //BUG: project not saved when the file is saved
//	if (saveFile != NULL)
//		menu.addAction(saveFile);

	ActionSaveProject *actionSave = new ActionSaveProject(this, m_project);
	menu.addAction(actionSave);
	if (! m_project->hasUnsavedChanges())
		actionSave->setDisabled(true);

	menu.display(pos);
}

void CorpusBrowser::setExportSubmenu(ContextMenu &menu, DFile *f)
{
	if (isInstance(f, Annotation))
	{
		Annotation *annot = qobject_cast<Annotation*>(f);
		menu.addSeparator();
		QMenu *submenu = new QMenu(tr("Export..."));
		menu.addMenu(submenu);

		submenu->addAction(new ActionExportAnnotTxt(annot, this));
		submenu->addAction(new ActionExportAnnotTextGrid(annot, this));

        menu.addSeparator();
	}
    else if (isInstance(f, DataTable))
    {
        DataTable *table = qobject_cast<DataTable*>(f);

        menu.addSeparator();
        QMenu *submenu = new QMenu(tr("Export..."));
        menu.addMenu(submenu);

        submenu->addAction(new ActionExportTableExcel(table, this));
        submenu->addAction(new ActionExportTableCsv(table, this));

        menu.addSeparator();
    }
}

void CorpusBrowser::dropEvent(QDropEvent *event)
{
	QTreeWidget::dropEvent(event);
	Q_ASSERT(mv_buffer.isEmpty());

	while (! drop_queue.isEmpty())
	{
		Movement *mv = drop_queue.dequeue();
		VFileNode *itemVNode = GET_VNODE(mv->item);
		QTreeWidgetItem *parentItem = mv->newParent;
		VFolder *newFolder;

		if (parentItem)
		{
			VFileNode *vnode = GET_VNODE(parentItem);
			Q_ASSERT(isInstance(vnode, VFolder));
			newFolder = qobject_cast<VFolder*>(vnode);
		}
		else
			newFolder = m_project->fileSystemRoot();

		itemVNode->moveTo(newFolder, mv->newPos);
	}

	emit tree_modified();
	setHeaderStar();
}

void CorpusBrowser::openFileInPraat()
{
    DFile *df = this->getSelectedDFile();

    if (! df)
		return;

    if (isInstance(df, Sound))
        m_praat->openSound(df->path());

    else if (isInstance(df, Annotation))
	{
        Annotation *annot = qobject_cast<Annotation*>(df);

		if (annot->soundFile())
			m_praat->openTextGrid(annot->path(), annot->soundFile()->path());
		else
			m_praat->openTextGrid(annot->path());
	}
}


void CorpusBrowser::createAnnotation()
{
   DFile *df = this->getSelectedDFile();
   QString suggested_name = df->baseName();
   QString path = df->directory() + QDir::separator() + suggested_name + DM_DOT_FILE_EXT;

   while (Global::fileExists(path))
   {
       suggested_name += "-2";
       path = df->directory() + QDir::separator() + suggested_name + DM_DOT_FILE_EXT;
   }
   suggested_name += DM_DOT_FILE_EXT;

   AnnotationCreator *ed = new AnnotationCreator(suggested_name, df->path(), this);
   connect(ed, SIGNAL(createAnnotation(QString,QString,QStringList,QList<tier_type_t>)),
           m_project, SLOT(createAnnotation(QString,QString,QStringList,QList<tier_type_t>)));

   ed->show();
}

void CorpusBrowser::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
	QTreeView::rowsAboutToBeRemoved(parent, start, end);

	Movement *mv = new Movement;
	mv->item = childFromParentIndex(parent, start);

	mv_buffer.enqueue(mv);
}

void CorpusBrowser::rowsInserted (const QModelIndex &parent, int start, int end )
{
	QTreeView::rowsInserted(parent, start, end);

	if (! mv_buffer.isEmpty())
	{
		Movement *mv = mv_buffer.dequeue();
		mv->newParent = itemFromIndex(parent);
		mv->newPos = start;
		drop_queue.enqueue(mv);
	}
}

QTreeWidgetItem* CorpusBrowser::childFromParentIndex(const QModelIndex &parent, int index)
{
	QTreeWidgetItem *parentItem  = this->itemFromIndex(parent);

	if (! parentItem)
		return this->topLevelItem(index); /* root */
	else
		return parentItem->child(index); /* vfolder */
}


void CorpusBrowser::onCreateFolder(QString &name)
{
	QList<VFileNode*> files;
	QList<QTreeWidgetItem*> items = this->selectedItems();

	foreach(QTreeWidgetItem *item, items)
		files << GET_VNODE(item);

	emit tree_mergeIntoFolder(files, name);
	setHeaderStar();
}

void CorpusBrowser::updateTree(VFolder *vfolder)
{
	this->clear();
//	QString label = vfolder->label();
//	if (m_project->hasUnsavedChanges())
//		label += "*";
#ifdef Q_OS_MAC
	label_project->setText(m_project->screenName());
#else
	this->headerItem()->setText(0, m_project->screenName());
#endif
	buildNode(vfolder);
	//this->expandToDepth(0);
    resetLabels();
}

void CorpusBrowser::updateTitle()
{
#ifdef Q_OS_MAC
	label_project->setText(m_project->screenName());
#endif
}

void CorpusBrowser::clearProject()
{
    this->clear();
}

void CorpusBrowser::buildNode(VFolder *vfolder, QTreeWidgetItem *node)
{
	/* attach items to tree items */
    foreach(auto vf, vfolder->content())
	{
		QTreeWidgetItem *newNode;

		if (isInstance(vf, VFolder))
		{
			VFolder *newFolder = qobject_cast<VFolder*>(vf);
			newNode = new QTreeWidgetItem(node);
			newNode->setData(0, Qt::UserRole, newFolder->id());
			init_treeItem(newNode, newFolder);

			buildNode(newFolder, newNode);
		}
		else
		{
			newNode = new QTreeWidgetItem(node);
			newNode->setData(0, Qt::UserRole, vf->id());
			this->updateToolTip(newNode);
			init_treeItem(newNode, vf);
		}
	}
}


void CorpusBrowser::buildNode(VFolder *vfolder)
{
	/* attach items to the root */
    foreach(auto vf, vfolder->content())
	{
		QTreeWidgetItem *newNode;

		if (isInstance(vf, VFolder))
		{
			VFolder *newFolder = qobject_cast<VFolder*>(vf);
			newNode = new QTreeWidgetItem(this);
			newNode->setData(0, Qt::UserRole, newFolder->id());
			init_treeItem(newNode, newFolder);

			buildNode(newFolder, newNode);
		}
		else
		{
			newNode = new QTreeWidgetItem(this);
			newNode->setData(0, Qt::UserRole, vf->id());
			this->updateToolTip(newNode);
			init_treeItem(newNode, vf);
		}
	}
}

void CorpusBrowser::addSubmenuHeader(QMenu *submenu, QString header)
{
	QFont font;
	font.setWeight(QFont::Bold);

	QAction *subtitle = new QAction(header, this);
	subtitle->setFont(font);
	subtitle->setEnabled(false);
	submenu->addAction(subtitle);
	submenu->addSeparator();

}

void CorpusBrowser::forwardProperty(Property *tag)
{
	foreach(QTreeWidgetItem *item, this->selectedItems())
	{
		VFileNode *vnode = GET_VNODE(item);

		if (isInstance(vnode, VFile))
		{
			VFile *vf = qobject_cast<VFile*>(vnode);
			emit tree_tagFile(tag, vf);
		}
	}
}

void CorpusBrowser::dropProperty(Property *tag)
{
	foreach(QTreeWidgetItem *item, this->selectedItems())
	{
		VFileNode *vnode = GET_VNODE(item);

		if (isInstance(vnode, VFile))
		{
			VFile *vf = qobject_cast<VFile*>(vnode);
			emit tree_untagFile(tag, vf);
		}
	}
}

void CorpusBrowser::deleteProperty(Property *tag)
{
	emit tree_deleteProperty(tag);
}


void CorpusBrowser::setHeaderStar()
{
#ifdef Q_OS_MAC
	label_project->setText(m_project->screenName());
	label_project->setToolTip(m_project->path());
#else
	headerItem()->setText(0, m_project->screenName());
	headerItem()->setToolTip(0, m_project->path());
#endif
}


void CorpusBrowser::resetLabels()
{
	setHeaderStar();
    setFileColor();
}


void CorpusBrowser::setFileColor()
{
//    QFont font_unsaved("", 12 , QFont::Bold);
//    QBrush brush_unsaved(Qt::red);
//    QFont font_saved("", 12 , QFont::Normal);
//    QBrush brush_saved(Qt::black);
    QFont font;
    QBrush brush;

	setHeaderStar();


	for (int i = 0; i < topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *item = topLevelItem(i);
		VFileNode *vnode = GET_VNODE(item);

		if (isInstance(vnode, VFolder))
            setSubfileColor(item);

		else if (isInstance(vnode, VFile))
		{
			this->updateToolTip(item);
			VFile *vf = qobject_cast<VFile*>(vnode);
            //QString label = item->text(0);

            font = item->font(0);

            if (vf->object()->hasUnsavedChanges())
            {
                font.setBold(true);
                brush = QBrush(Qt::red);
            }
            else
            {
                font.setBold(false);
                brush = QBrush(Qt::black);
            }

            item->setForeground(0, brush);
            item->setFont(0, font);

           // item->setText(0, QFileInfo(vf->object()->path()).fileName());
		}
	}
}

void CorpusBrowser::setSubfileColor(QTreeWidgetItem *subfolder)
{
    QFont font;
    QBrush brush;

	for (int i = 0; i < subfolder->childCount(); ++i)
	{
		QTreeWidgetItem *item = subfolder->child(i);
		VFileNode *vnode = GET_VNODE(item);

		if (isInstance(vnode, VFolder))
            setSubfileColor(item);

		else if (isInstance(vnode, VFile))
		{
			this->updateToolTip(item);
			VFile *vf = qobject_cast<VFile*>(vnode);
            //QString label = item->text(0);

            font = item->font(0);

            if (vf->object()->hasUnsavedChanges())
            {
                font.setBold(true);
                brush = QBrush(Qt::red);
            }
            else
            {
                font.setBold(false);
                brush = QBrush(Qt::black);
            }

            item->setForeground(0, brush);
            item->setFont(0, font);

           // item->setText(0, QFileInfo(vf->object()->path()).fileName());
		}
	}
}

void CorpusBrowser::updateToolTip(QTreeWidgetItem *item)
{
	VFileNode *vnode = GET_VNODE(item);

	if (isInstance(vnode, VFile))
	{
		VFile *vf = qobject_cast<VFile*>(vnode);
		item->setToolTip(0, vf->object()->metadataAsHtml());
	}
}


DFile * CorpusBrowser::getSelectedDFile() const
{
    QList<QTreeWidgetItem*> items = selectedItems();
    VFileNode *vnode = GET_VNODE(items[0]);
    VFile *vf = qobject_cast<VFile*>(vnode);

    return vf->object();
}
