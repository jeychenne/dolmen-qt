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
 * Created: 03/09/10
 */

#include "Project.h"


Project::Project(sol::state_view lua) :
    m_lua(lua)
{
	m_path = "";
	m_name = "";
    file_system = new VFolder(NULL, "");
    Global::IdMap[file_system->id()] = file_system;
    unsaved_changes = false;
	m_changelog = new Changelog;

	connect(m_changelog, SIGNAL(changelog_changed()), this, SLOT(setChanged()));
}

QString Project::path() const
{
	return m_path;
}

void Project::setPath(const QString &value)
{
	if (value != m_path)
	{
		QFileInfo info(value);
		m_name = info.baseName();
        file_system->setLabel(m_name);
		m_path = nat(value);
		emit project_pathSet(m_path);
	}
}

QString Project::name() const
{
	return m_name;
}

QString Project::screenName() const
{
#ifdef Q_OS_MAC
	if (this->hasUnsavedChanges())
		return tr("PROJECT %1*").arg(m_name).toUpper();
	else
		return tr("PROJECT %1").arg(m_name).toUpper();
#else
	if (this->hasUnsavedChanges())
		return tr("Project %1*").arg(m_name);
	else
		return tr("Project %1").arg(m_name);
#endif
}

Changelog * Project::changelog()
{
	return m_changelog;
}

/*
VFileNode* Project::vnodeFromId(int id) const
{
	if (Global::idMap.contains(id))
		return Global::idMap[id];
	else
		return NULL;
}
*/

DFile* Project::file(const QString &path) const
{
	return Global::FileHash[path];
}

QList<DFile*> Project::files()
{
	return Global::FileHash.values();
}

void Project::watchModifications()
{
    unsaved_changes = true;
	emit project_filesModified();
}

void Project::renameFolder(VFolder *folder, const QString &newLabel)
{
	folder->setLabel(newLabel);
	watchModifications();
}

void Project::open(const QString &path)
{
	this->clear();
	this->setPath(path);

	QFile file(path);

	if (! file.open(QFile::ReadOnly | QFile::Text))
	{
		QString msg = QString("Cannot read file %1").arg(path);
		emit file_error(msg); return;
	}

	QXmlStreamReader reader(&file);

	reader.readNext();

	while (! reader.atEnd())
	{
		if (reader.isStartElement())
		{
			if (reader.name() == "Metadata")
			{
				this->readXmlMetadata(&reader);
			}
			else if (reader.name() == "FileSystem")
			{
				int size = reader.attributes().value("size").toString().toInt();
				emit project_startImportingFolder(size);
                readXmlVNode(&reader, file_system);
			}
            else if (reader.name() == "Scripts")
            {
                readXmlScripts(&reader);
            }
			else
				reader.readNext();
		}
		else
			reader.readNext();
	}

	file.close();

	if (reader.hasError())
	{
		QString msg = tr("%1 is not a valid project").arg(path);
		emit file_error(msg); return;
	}
	else if (file.error() != QFile::NoError)
	{
		QString msg = tr("Cannot read %1").arg(path);
		emit file_error(msg); return;
	}

	this->bindAllAnnotations();
	this->registerProject(path);

    //FIXME: the project is modified somewhere
    //unsavedChanges = false;
    emit project_modified(file_system);

    emit information(tr("Project %1 has been loaded").arg(name()));
}

void Project::bindAllAnnotations()
{
	// try to bind annotations now that all files are loaded
	foreach (DFile *f, Global::FileHash.values())
	{
		if (isInstance(f, Annotation))
		{
			Annotation *annot = qobject_cast<Annotation*>(f);
			annot->setSoundFile();
		}
    }
}

void Project::emitBookmarksModified()
{
    QList<IBrowserElement*> bookmarks;
    std::copy(m_bookmarks.begin(), m_bookmarks.end(), std::back_inserter(bookmarks));
    emit project_bookmarksModified(std::move(bookmarks));
}

void Project::emitScriptsModified()
{
    QList<IBrowserElement*> scripts;
    std::copy(m_scripts.begin(), m_scripts.end(), std::back_inserter(scripts));
    emit project_scriptsModified(std::move(scripts));
}

void Project::registerProject(const QString &path)
{
	Global::RecentProjects.removeAll(path);
	Global::RecentProjects.insert(0, path);
}

void Project::addMetadataToFile(const QString &filename, const QString &cat, const QString &val)
{
    foreach (const QString &path, Global::FileHash.keys())
    {
        // Only check the end of a path. This allows tagging multiple files ending in the same string
        // e.g. "tg.TextGrid" tagged as "Task:Text"
        if (path.endsWith(filename))
        {
            DFile *df = Global::FileHash[path];
            Property *p = new Property(val, cat);
            df->addProperty(p);
        }
    }
}

void Project::readXmlMetadata(QXmlStreamReader *reader)
{
	while (! (reader->isEndElement() && reader->name() == "Metadata"))
	{
		if (reader->isStartElement() && reader->name() == "Changelog")
			m_changelog->readFromXml(reader);
		else if (reader->isStartElement() && reader->name() == "Bookmarks")
			this->readXmlBookmarks(reader);

		reader->readNext();
	}
}

void Project::readXmlBookmarks(QXmlStreamReader *reader)
{
	while (! (reader->isEndElement() && reader->name() == "Bookmarks"))
	{
		if (reader->isStartElement() && reader->name() == "Bookmark")
		{
			SearchMatch *match = new SearchMatch(NULL, NULL, -1, 0.); // invalid match

            if (match->readFromXml(reader)) {
				m_bookmarks << match;
            }
            else {
                emit file_error("Invalid bookmark in project");
            }
		}

		reader->readNext();
	}

    emitBookmarksModified();
}

void Project::readXmlScripts(QXmlStreamReader *reader)
{
    while (! (reader->isEndElement() && reader->name() == "Scripts"))
    {
        if (reader->isStartElement() && reader->name() == "Script")
        {
            auto script = new Script;

            if (script->readFromXml(reader)) {
                m_scripts.append(std::move(script));
            }
            else {
                emit file_error("Invalid script element in project");
            }
        }

        reader->readNext();
    }

    emitScriptsModified();
}

void Project::readXmlVNode(QXmlStreamReader *reader, VFolder *folder)
{
	int counter = -1; // correct for the count of FileSystem

	while (! (reader->isEndElement() && (reader->name() == "VFolder" || reader->name() == "FileSystem")))
	{
		if (reader->isStartElement())
		{
			++counter;

			if (reader->name() == "VFolder")
			{
				QString label = reader->attributes().value("label").toString();
				VFolder *subfolder = new VFolder(folder, label);
				Global::IdMap[subfolder->id()] = subfolder;
				folder->addFile(subfolder);
				reader->readNext();
				readXmlVNode(reader, subfolder);
			}
			else if (reader->name() == "VFile")
			{
				QString classname = reader->attributes().value("class").toString();

				// use the DMM file instead of the actual file, if available.
				// The actual file name is not read
                //qDebug() << "XML VFile before:" << unsavedChanges;
				if (reader->attributes().hasAttribute("metadata"))
				{
					QString metadataPath = interpolatePath(reader->attributes().value("metadata").toString());
					addFile(metadataPath, folder, classname);

					reader->readNext();
				}
				else
				{
					QString path = interpolatePath(reader->readElementText());
					addFile(path, folder, classname);
				}
                //qDebug() << "XML VFile after:" << unsavedChanges;
			}
			else if (reader->name() == "FileSystem")
				/* pass */;
			else
				emit error(QString("Unknown FileSystem element \"%1\" in Dolmen Project").arg(reader->name().toString()));

            if (folder == file_system && reader->name() != "FileSystem")
				emit project_fileImported(counter);
		}

		reader->readNext();
	}
}

VFolder* Project::fileSystemRoot()
{
    return file_system;
}

void Project::setChanged()
{
    unsaved_changes = true;
    emit project_modified(file_system);
}

QList<Script *> Project::scripts() const
{
    return m_scripts;
}

bool Project::hasUnsavedChanges() const
{
    return unsaved_changes;
}

bool Project::isEmpty() const
{
	return Global::FileHash.isEmpty();
}

// high-level folder import function
void Project::importFolder(const QString &path)
{
	this->addFolder(path);
	this->bindAllAnnotations();
	this->setChanged();
}

// recursive folder import function
void Project::addFolder(const QString &path, VFolder *vfolder)
{
	if (vfolder == NULL)
        vfolder = file_system;


	QDir folder(path);
	QString subpath = nat(folder.absolutePath());
	if (! subpath.endsWith(QDir::separator()))
		subpath += QDir::separator();

    if (vfolder == file_system)
		emit project_startImportingFolder(folder.count());

	QStringList filters;
	filters << "*";

	QString f; int counter = 0;
	foreach (f, folder.entryList(filters))
	{
		++counter;

		if (! f.startsWith('.'))
		{
			subpath = path + QDir::separator() + f;
			QFileInfo subfolder(subpath);

			if (subfolder.isDir())
			{
				VFolder *newVFolder = new VFolder(vfolder, subfolder.fileName());
				Global::IdMap[newVFolder->id()] = newVFolder;
				vfolder->addFile(newVFolder);
				this->addFolder(subpath, newVFolder);
			}
			else
				this->addFile(subpath, vfolder);
		}

        if (vfolder == file_system)
			emit project_fileImported(counter);
	}
}

void Project::addFiles(const QStringList &files)
{
	QString file;
	foreach(file, files)
		addFile(file);
	this->setChanged();
}

void Project::addFile(const QString &path, VFolder *folder, QString file_class)
{
    DFile *df = NULL;

    if (Global::fileExists(path)) return;
	QString err = "";

	QFileInfo info(path);
	QString ext = info.suffix().toLower();

	if (folder == NULL)
        folder = file_system;

    if (file_class == "Annotation" || ext == DM_FILE_EXT || ext == "textgrid"
		|| ext == "lab")
	{
		Annotation *f = new Annotation(path);
		err = f->error(); if (err != "") { emit file_error(err); return; }
		registerFile(folder, f);

		if (Global::ApplicationMode == "PFC")
			f->parsePfcPath();
		else if (Global::ApplicationMode == "PAC")
			f->parsePacPath();
		else
			f->setSoundFile();

        // inform database
        if (!f->isNative()) {
            emit project_newFileAdded(f);
        }

        // Let users handle annotation
        sol::function callback = m_lua.script("return signal.emit");
        int id = m_lua["EVENT_ANNOTATION_LOADED"];
        callback(id, "hello world");

        df = f;
	}
    else if (ext == DM_TEXT_EXT)
    {
        Document *d = new Document(path);
        err = d->error(); if (err != "") { emit file_error(err); return; }
        registerFile(folder, d);

        // for now, no native documents
        emit project_newFileAdded(d);

        df = d;
    }
    else if (file_class == "DataTable" || ext == DM_TABLE_EXT)
    {
        DataTable *t = new DataTable(path);
        err = t->error(); if (err != "") { emit file_error(err); return; }
        registerFile(folder, t);

        df = t;
    }
    else if (file_class == "Sound" || isValidSoundExt(ext))
	{
		Sound *f = new Sound(path);
		err = f->error(); if (err != "") { emit file_error(err); return; }
		registerFile(folder, f);

        df = f;
	}
	else
	{
        QString msg = tr("File \"%1\" not recognized").arg(path);
		emit error(msg);
	}

    // the project will forward signals to the database
    if (df)
    {
        connect(df, SIGNAL(file_saveMetadata(DFile*)), this, SIGNAL(saveFileMetadata(DFile*)));
        connect(df, SIGNAL(file_setMetadata(DFile*)), this, SIGNAL(setFileMetadata(DFile*)));
    }
}

void Project::removeFile(VFile *vf)
{
	QString path = vf->object()->path();
	decrementFile(path);
	delete vf;
}

void Project::removeFolder(VFolder *folder)
{
    foreach(auto vf, folder->content())
	{
		if (isInstance(vf, VFolder))
		{
			VFolder *subfolder = qobject_cast<VFolder*>(vf);
			removeFolder(subfolder);
		}
		else if (isInstance(vf, VFile))
			removeFile(qobject_cast<VFile*>(vf));
	}

	delete folder;
}

void Project::addBookmark(Bookmark *bm)
{
	m_bookmarks << bm;
    emitBookmarksModified();
	this->setChanged();
}

void Project::removeBookmarks(QList<IBrowserElement*> lst)
{
    for (auto elem: lst) {
        removeBookmark(dynamic_cast<Bookmark*>(elem));
    }
}

void Project::addDataTable(DataTable *table)
{
    this->registerFile(file_system, table);
    this->setChanged();
}

void Project::createAnnotation(const QString &filename, const QString &sound_ref, QStringList tier_names, QList<tier_type_t> tier_types)
{
    Annotation *annot = new Annotation(filename, true);
    annot->setSoundRef(sound_ref);
    annot->setSoundFile(); //TODO: improve the way sound references are set

    for (int i = 0; i < tier_names.size(); ++i)
    {
        annot->addNewTier(tier_names[i], tier_types[i], -1);
    }

    this->registerFile(file_system, annot);
    this->setChanged();
}

void Project::removeBookmark(Bookmark *bm)
{
	if (m_bookmarks.removeOne(bm))
	{
        emitBookmarksModified();
        this->setChanged();
	}
}

void Project::tagFile(Property *tag, VFile *vf)
{
	vf->object()->addProperty(tag);
}

void Project::untagFile(Property *tag, VFile *vf)
{
	vf->object()->removeProperty(tag);
}

void Project::deleteProperty(Property *tag)
{
	foreach(DFile *file, Global::FileHash.values())
		file->removeProperty(tag);

	Property::properties.remove(tag->toString());
	delete tag;
}

DFile* Project::fileFromPath(const QString &path)
{
    if (Global::fileExists(path))
		return Global::FileHash[path];
	else
		return NULL;
}


void Project::registerFile(VFolder *folder, DFile *file)
{
	VFile *vf = new VFile(folder, file);
	Global::IdMap[vf->id()] = vf;
	Global::FileHash[file->path()] = file;
	folder->addFile(vf);
	incrementFile(file->path());
	connect(file, SIGNAL(file_modified()), this, SLOT(watchModifications()));
}

void Project::unregisterFile(VFile *vf)
{
	disconnect(vf->object(), SIGNAL(file_modified()), this, SLOT(watchModifications()));
}

int Project::incrementFile(const QString &path)
{
    if (file_ref_count.contains(path))
        file_ref_count[path]++;
	else
        file_ref_count[path] = 1;

    return file_ref_count[path];
}

int Project::decrementFile(const QString &path)
{
    int count = --file_ref_count[path];

	if (count == 0)
	{
        file_ref_count.remove(path);
		delete Global::FileHash[path];
		Global::FileHash.remove(path);
	}

	return count;
}

void Project::mergeFilesIntoFolder(QList<VFileNode*> files, QString &label)
{
	VFolder *root = files[0]->parentFolder();
	VFolder *newFolder = new VFolder(root, label);
	Global::IdMap[newFolder->id()] = newFolder;

	foreach (VFileNode *file, files)
		file->moveTo(newFolder);

	root->addFile(newFolder);
	this->setChanged();
}

void Project::saveFiles()
{
	foreach(DFile *f, Global::FileHash.values())
		saveFile(f);
}

void Project::onSaveFile(DFile *f) /* [slot] */
{
	saveFile(f);
	checkFileModifications();
	emit project_filesModified();
}

void Project::saveFile(DFile *f)
{
    if (! f->path().startsWith("/"))
        qDebug() << f->path() << "VALID:" << f->pathIsValid();

    if (! f->pathIsValid())
        file_system->requestFilePath(f);
	else
		f->save();

    inform(QString("Saved file %1").arg(f->path()));
}

void Project::saveFileWithPath(DFile *f, const QString &path) /* [slot] */
{
	f->setPath(path);
	f->save();
}

void Project::writeToDisk(QString &path) /* [slot] */
{
	setPath(path);
	saveFiles();

	QFile file(path);

	if (! file.open(QFile::WriteOnly | QFile::Text))
	{
		QString msg = QString("The file '%1' cannot be written: %2").arg(path, file.errorString());
		emit file_error(msg); return;
	}

	QXmlStreamWriter writer(&file);


	writer.setAutoFormatting(true);
	writer.writeStartDocument();
	writer.writeStartElement("DolmenProject");

	writer.writeStartElement("FileSystem");
    writer.writeAttribute("size", QString::number(file_system->content().size())); // hint for the QProgressDialog

    foreach(auto node, file_system->content())
		writeXmlVNode(&writer, node);

	writer.writeEndElement(); // </FileSystem>

	// write metadata after the file system so that paths in bookmarks can be set
	writer.writeStartElement("Metadata");
	this->writeXmlChangelog(&writer);

	writer.writeStartElement("Bookmarks");
    for (Bookmark *bm : m_bookmarks)
		bm->writeToXml(&writer);
	writer.writeEndElement(); // </Bookmarks>

    writer.writeStartElement("Scripts");
    for (auto &script : m_scripts) {
        script->writeToXml(&writer);
    }
    writer.writeEndElement(); // </Scripts>

	writer.writeEndElement(); // </Metadata>

	writer.writeEndDocument(); // </DolmenProject>
	file.close();

	if (file.error())
	{
		QString msg = QString("The file '%1' cannot be written: %2").arg(path, file.errorString());
		emit file_error(msg); return;
	}

	registerProject(path);
    unsaved_changes = false;
	emit project_saved();
}


void Project::writeXmlVNode(QXmlStreamWriter *writer, VFileNode *vf)
{
	if (isInstance(vf, VFolder))
	{
		VFolder *folder = qobject_cast<VFolder*>(vf);

		writer->writeStartElement("VFolder");
		writer->writeAttribute("label", folder->label());

        foreach(VFileNode *subnode, folder->content())
			writeXmlVNode(writer, subnode);

		writer->writeEndElement(); // </VFolder>
	}
	else if (isInstance(vf, VFile))
	{
		VFile *file = qobject_cast<VFile*>(vf);

		writer->writeStartElement("VFile");
		writer->writeAttribute("class", CLASSNAME(file->object()));
        //TODO: remove
//		if (file->object()->metadataPath() != "")
//		{
//			QString metadata = dm_compressPath(file->object()->metadataPath());
//			writer->writeAttribute("metadata", metadata);
//		}
		writer->writeCharacters(compressPath(file->object()->path()));
		writer->writeEndElement(); // </VFile>
	}
}

void Project::writeXmlChangelog(QXmlStreamWriter *writer)
{
	QList<Version*> versions = m_changelog->versions();
	if (versions.isEmpty()) return;

	writer->writeStartElement("Changelog");
	foreach(Version *version, versions)
	{
		writer->writeStartElement("Version");

		writer->writeStartElement("Description");
		writer->writeCharacters(version->description());
		writer->writeEndElement(); // </Description>

		writer->writeStartElement("Date");
		writer->writeAttribute("format", DM_DATE_FORMAT);
		writer->writeCharacters(version->date().toString(DM_DATE_FORMAT));
		writer->writeEndElement(); // </Date>

		writer->writeStartElement("NumberString");
		writer->writeCharacters(version->number());
		writer->writeEndElement(); // </NumberString>

		QList<Author*> authors = version->authors();
		if (! authors.isEmpty())
		{
			writer->writeStartElement("Authors");
            for (Author *author : authors)
			{
				writer->writeStartElement("Author");

				writer->writeStartElement("Name");
				writer->writeCharacters(author->name());
				writer->writeEndElement(); // </Name>

				writer->writeStartElement("Email");
				writer->writeCharacters(author->email());
				writer->writeEndElement(); // </Email>

				writer->writeEndElement(); // </Author>
			}
			writer->writeEndElement(); // </Authors>
		}
		writer->writeEndElement(); // </Version>
	}
	writer->writeEndElement(); // </Changelog>
}


void Project::checkFileModifications()
{
	if (m_path == "")
	{
        unsaved_changes = true;
		return;
	}
	else
	{
        unsaved_changes = false;

		foreach(DFile *f, Global::FileHash.values())
		{
			if (f->hasUnsavedChanges())
			{
                unsaved_changes = true;
				return;
			}
		}
	}
}

void Project::nativizeAnnotations(QStringList &paths)
{
	foreach(QString path, paths)
	{
		Annotation *f = new Annotation(path);
		f->nativizePath();
		f->copyMetadata(qobject_cast<Annotation*>(Global::FileHash[path]));
		f->writeToDmf();
        registerFile(file_system, f);
	}

    emit project_modified(file_system);
}


void Project::removeVFiles(QList<VFileNode*> &files) /* [slot] */
{
    for (VFileNode *vf : files)
	{
		if (isInstance(vf, VFile))
			removeFile(qobject_cast<VFile*>(vf));

		else if (isInstance(vf, VFolder))
			removeFolder(qobject_cast<VFolder*>(vf));
	}

	this->setChanged();
}

void Project::clear()
{
    for (DFile *f : Global::FileHash.values())
		delete f;

	Global::FileHash.clear();
	Property::properties.clear();
    file_ref_count.clear();
	delete m_changelog;
	m_changelog = NULL;
	//TODO: cleanly delete bookmarks (multiple inheritance)
	m_bookmarks.clear();

    for (auto script : m_scripts) {
        delete script;
    }
    m_scripts.clear();

    file_system->clear();
    Global::IdMap[file_system->id()] = file_system;
	m_changelog = new Changelog;
	connect(m_changelog, SIGNAL(changelog_changed()), this, SLOT(setChanged()));
    unsaved_changes = false;

}
