/*
 * Project.h
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
 * Purpose: physical files are organized into projects.
 * Created: 03/09/10
 */

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "dolmen.h"
#include "lib/VFile.h"
#include "lib/VFolder.h"
#include "lib/Annotation.h"
#include "lib/Document.h"
#include "lib/DataTable.h"
#include "lib/Sound.h"
#include "lib/metadata.h"
#include "lib/Bookmark.h"
#include "runtime/scripting.h"
#include "utils/routines.h"

using namespace metadata;

using namespace std;

class Project : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString path READ path WRITE setPath)
	Q_PROPERTY(QString name READ name)

public:
    Project(sol::state_view lua);

	QString path() const;
	void setPath(const QString &value);

	QString name() const;
	QString screenName() const; // "Project name"


	DFile* file(const QString &path) const;
	QList<DFile*> files();

	bool hasUnsavedChanges() const;
	VFolder* fileSystemRoot();
	bool isEmpty() const;

//	VFileNode* vnodeFromId(int id) const;

	DFile* fileFromPath(const QString &path);
	void registerProject(const QString &path);

    void addMetadataToFile(const QString &filename, const QString &cat, const QString &val);

	Changelog * changelog();

signals:
	void error(QString); // non-critical errors (e.g. file not recognized)
	void information(QString);
	void project_modified(VFolder *vf);
    void project_cleared();
	void project_filesModified(); // files in the project were modified but the file system remains unchanged
	void project_saved();
	void file_error(QString &msg); // critical errors (e.g. badly formatted annotation)
	void project_startImportingFolder(int);
	void project_fileImported(int);
    void project_newFileAdded(DFile *f);
	void project_pathSet(QString);
	void project_bookmarksModified(QList<Bookmark*>);
    void saveFileMetadata(DFile *f);
    void setFileMetadata(DFile *f);


public slots:
    void open(const QString &path);
    void addFiles(const QStringList &);
	void importFolder(const QString &);
	void mergeFilesIntoFolder(QList<VFileNode*>, QString &label);
	void tagFile(Property *, VFile *);
	void untagFile(Property *, VFile *);
	void deleteProperty(Property *);
	// write project to native project format (*.DMPR)
	void writeToDisk(QString &path);
	void nativizeAnnotations(QStringList &);
	void removeVFiles(QList<VFileNode*>&);
	void watchModifications(); // monitor modifications from files
	void renameFolder(VFolder *, const QString &);
	void saveFileWithPath(DFile *, const QString &);
	void onSaveFile(DFile *);
	void addBookmark(Bookmark *bm);
    void removeBookmarks(QList<IBrowserElement*> lst);
    void addDataTable(DataTable*);
    void createAnnotation(const QString &filename, const QString &sound_ref, QStringList tier_names, QList<tier_type_t> tier_types);

	//TODO: sort "public" slots that are actually private
private slots:
	void setChanged();

private:
	QString m_path, m_name;
    bool unsaved_changes;

	// reference-counting for physical files. For a given file, it is incremented
	// every time a virtual file is added. When a virtual file is removed, if the
	// count of the real file is 0, the physical file is deleted from the project.
    QHash<QString, int>  file_ref_count;
    VFolder             *file_system; // virtual file system displayed in the file browser
    QList<Bookmark*>     m_bookmarks;
    Changelog           *m_changelog;
    sol::state_view      m_lua;

	void clear(); // clean up memory
	int incrementFile(const QString &path);
	int decrementFile(const QString &path);
	void registerFile(VFolder *folder, DFile *file);
	void unregisterFile(VFile *vf);

	// add each file received from the GUI
	void addFolder(const QString &, VFolder *vfolder = NULL);
    void addFile(const QString &path, VFolder *folder = NULL, QString file_class = "");
	void removeFile(VFile *);
	void removeFolder(VFolder *);

	void removeBookmark(Bookmark *bm);

	void writeXmlChangelog(QXmlStreamWriter *writer);
	void writeXmlVNode(QXmlStreamWriter *writer, VFileNode *vf);
	void readXmlVNode(QXmlStreamReader *reader, VFolder *folder);
	void readXmlMetadata(QXmlStreamReader *reader);
	void readXmlBookmarks(QXmlStreamReader *reader);

	void saveFiles(); // save all unsaved changes in the project
	void saveFile(DFile *);
	void checkFileModifications(); // sync 'unsavedChanges' with the current state of the file system;

	void bindAllAnnotations();


};

#endif // PROJECT_H
