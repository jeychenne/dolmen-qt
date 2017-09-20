/*
 * DFile.h
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
 * Purpose: base class to represent all physical files (sound, annotation, text).
 * Created: 03/09/10
 */

#ifndef DFile_H
#define DFile_H

#include <QObject>
#include <QSet>
#include <QStringList>

#include "dolmen.h"
#include "lib/metadata.h"
#include "search/SearchNode.h"
#include "utils/routines.h"

using namespace metadata;


class DFile : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString path READ path WRITE setPath)
	Q_PROPERTY(QString metadataPath READ metadataPath)
	Q_PROPERTY(QString description READ description WRITE setDescription)
	//Q_PROPERTY(int id READ id)

public:
	DFile();

	QString path() const;
	void setPath(const QString &);

    QString baseName() const;
    QString directory() const;
	QString description() const;
	void setDescription(const QString &);
    void setDescriptionSilently(const QString &);
	QString metadataPath() const;

	// Since exceptions are not used, the error is stored in an error() method to notify problems.
    // It returns an empty string if there is no error. The error is cleared after it is read.
	QString error();

	//int id() const;

	virtual void save() = 0;
    virtual void open() = 0;
	void addProperty(Property*);
    void addPropertySilently(Property *prop); // from database
    void removeProperty(Property *);
	void removeProperty(const QString &);
	QStringList propertiesAsStrings() const;
	virtual QString metadataAsHtml(bool withDescription = true) const = 0;
	QSet<Property*> properties() const;
	bool hasCategory(QString) const;
	bool hasLabel(QString) const;
	bool hasProperty(Property *) const;
    bool hasProperty(const QString &category, const QString &value) const;
    bool hasProperties() const;
	bool cmpLabel(SearchOpcode op, double value);
	bool hasUnsavedChanges() const;
    bool isOpen() const;
    bool isLoading() const;
    bool pathIsValid();

signals:
	void file_modified();
	void file_error(const QString &msg);
    void file_saveMetadata(DFile *df);
    void file_setMetadata(DFile *df);

public slots:
	void modifiedExternally(); // modified in the GUI

protected:
	QSet<Property*>	m_properties;
    QString		    m_path, m_description, m_error;
    QString		    m_metadata_path; // stores metadata in $APPDIR
    bool	     	unsaved_changes, m_open;
    bool		    content_modified;	// whether non-header data has been modified
    bool            path_is_bogus; // when a native annotation is created, the path only contains a display name
    int			    m_id;

	// if the content is stored in an external source (e.g. TextGrid, WAV file),
	// the metadata are stored in AppDataDir.
	QString getExternalMetadataPath();
	QString getMetadataPathFromFile();

    QString readXmlHeader(QXmlStreamReader*, QString element = "");
	void readXmlProperty(QXmlStreamReader*);

    void writeXmlHeader(QXmlStreamWriter *writer, QString klass, QString element = "", QString value = "");
    void notifyChanges();
    // derived classes should call the following method in save() to make sure they have a valid path




};

#endif // DFile_H
