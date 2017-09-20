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
 * Created: //
 */

#include "DFile.h"

#include <QFileDialog>

DFile::DFile()
{
	m_path			= "";
    m_properties	= QSet<Property*>();
	m_description	= "";
    m_error			= "";
    path_is_bogus   = false;
	unsaved_changes	= false;
    m_open          = false;
	content_modified	= false;
}


void DFile::addProperty(Property *prop)
{
    this->addPropertySilently(prop);
    notifyChanges();
}

void DFile::addPropertySilently(Property *prop)
{
    Property *toRemove = NULL;

    foreach (Property *existing_prop, m_properties)
    {
        if (existing_prop->toString() == prop->toString())
            return;

        // A file may only contain one value per category.
        // If a category is already present, its value is silently modified.
        else if (existing_prop->category() == prop->category())
        {
            toRemove = existing_prop;
            break;
        }
    }

    if (toRemove)
        m_properties.remove(toRemove);
    m_properties.insert(prop);
}

void DFile::removeProperty(Property *tag)
{

	removeProperty(tag->toString());
}

void DFile::removeProperty(const QString &value)
{
    foreach (Property *t, m_properties)
	{
		if (t->toString() == value)
		{
            m_properties.remove(t);
            notifyChanges();
		}
	}
}


QString DFile::description() const
{
	return m_description;
}


void DFile::setDescription(const QString &value)
{
    m_description = value;

    if (unsaved_changes == false)
	{
        notifyChanges();
	}
}

void DFile::setDescriptionSilently(const QString &value)
{
    m_description = value;
}

bool DFile::hasCategory(QString cat) const
{
    foreach (Property *t, m_properties)
	{
		if (t->category() == cat)
			return true;
	}

	return false;
}

bool DFile::hasLabel(QString label) const
{
    foreach (Property *t, m_properties)
	{
//		qDebug()<< QString("testing %1 with label %2 (repr = %3)").arg(myPath, label, t->toString());
		if (t->value() == label)
		{
			return true;
		}
	}

	return false;
}

bool DFile::hasProperty(const QString &category, const QString &value) const
{
    foreach (Property *t, m_properties)
    {
        if (t->category() == category && t->value() == value)
            return true;
    }

    return false;
}

bool DFile::hasProperty(Property *tag) const
{
    foreach (Property *t, m_properties)
	{
		if (t->toString() == tag->toString())
			return true;
	}

	return false;
}

bool DFile::hasProperties() const
{
    return !m_properties.isEmpty();
}

bool DFile::cmpLabel(SearchOpcode op, double value)
{
	switch (op) // TODO: check cat/label at once to ensure the right label is checked for each cat
	{
	case NumEqOperator:
        foreach (Property *t, m_properties)
		{
			if (t->value().toDouble() == value)
				return true;
		}
		break;

	case NumGeOperator:
        foreach (Property *t, m_properties)
		{
			if (t->value().toDouble() >= value)
				return true;
		}
		break;

	case NumGtOperator:
        foreach (Property *t, m_properties)
		{
			if (t->value().toDouble() > value)
				return true;
		}
		break;

	case NumLeOperator:
        foreach (Property *t, m_properties)
		{
			if (t->value().toDouble() <= value)
				return true;
		}
		break;

	case NumLtOperator:
        foreach (Property *t, m_properties)
		{
			if (t->value().toDouble() < value)
				return true;
		}
		break;

	case NumNeOperator:
        foreach (Property *t, m_properties)
		{
			if (t->value().toDouble() != value)
				return true;
		}
		break;

	default:
		qDebug("Unhandled default case in DFile::cmpLabel()");
	}

	return false;
}

bool DFile::hasUnsavedChanges() const
{
    return unsaved_changes || path_is_bogus;
}

bool DFile::isOpen() const
{
    return m_open;
}

QStringList DFile::propertiesAsStrings() const
{
	QStringList result;

    foreach(Property *prop, m_properties)
        result.append(prop->toString());

	return result;
}

QSet<Property*> DFile::properties() const
{
    return m_properties;
}
/*
int DFile::id() const
{
	return myId;
}
*/
QString DFile::path() const
{
	return m_path;
}

void DFile::setPath(const QString &value)
{
	m_path = value;
}

QString DFile::baseName() const
{
    QFileInfo info(m_path);

    return info.baseName();
}

QString DFile::directory() const
{
    QFileInfo info(m_path);

    return info.dir().absolutePath();
}


QString DFile::metadataPath() const
{
	return m_metadata_path;
}

QString DFile::getMetadataPathFromFile()
{
	QFileInfo info(m_path);
	QString newPath = info.absolutePath() + QDir::separator() + info.baseName() + DM_DOT_METADATA_EXT;
	return newPath;
}

QString DFile::getExternalMetadataPath()
{
	QString basename = QFileInfo(m_path).baseName();
	// for an external file "/path/to/myfile.TextGrid", create a metadata file
	// called "$DM_APPDIR/Metadata/myfile_N.dmm", where N is an int. This ensures we can have
	// different files with the same basename but located in different directories.
	int N = 1;
	QFileInfo newPathInfo(getMetadataDir() + QDir::separator()
						  + basename + QString("_%1.%2").arg(N).arg(DM_METADATA_EXT));

	while (newPathInfo.exists())
	{
		N++;
		newPathInfo = QFileInfo(getMetadataDir() + QDir::separator()
								+ basename + QString("_%1.%2").arg(N).arg(DM_METADATA_EXT));
	}

	m_metadata_path = newPathInfo.absoluteFilePath();
	//qDebug() << QString("DMM: %1").arg(myMetadataPath);

	return m_metadata_path;
}

QString DFile::readXmlHeader(QXmlStreamReader *reader, QString element)
{
    QString value = "";

    while (! (reader->isEndElement() && reader->name() == "Header"))
	{
		if (reader->isStartElement())
		{
            if (reader->name() == "Property" || reader->name() == "Tag")
				readXmlProperty(reader);

			else if (reader->name() == "Description")
				m_description = reader->readElementText();

            else if (reader->name() == element)
				value = nat(reader->readElementText()); // nativize (assume it's a path)

			else
				reader->readNext();
		}

		reader->readNext();
	}

    return value;
}

void DFile::readXmlProperty(QXmlStreamReader *reader)
{
	QString value, category;

    while (! (reader->isEndElement() && (reader->name() == "Property" || reader->name() == "Tag")))
	{
		if (reader->isStartElement() && reader->name() == "Category")
			category = reader->readElementText();

		else if (reader->isStartElement() && reader->name() == "Value")
		{
			value = reader->readElementText();
            m_properties.insert(new Property(value, category));
		}
		else
			reader->readNext();
	}
}

void DFile::writeXmlHeader(QXmlStreamWriter *writer, QString klass, QString element, QString value)
{
    // the optional element/value describe an optional element in the header (e.g. SoundReference in an annotation)
	writer->writeStartElement("Header");
	writer->writeStartElement("Class");
	writer->writeCharacters(klass);
	writer->writeEndElement();
    writer->writeStartElement("Properties");
    foreach(Property *tag, m_properties)
	{
		tag->writeToXml(writer);
	}
    writer->writeEndElement(); // </Properties>

    if (element != "")
    {
        writer->writeStartElement(element);
        writer->writeCharacters(value);
        writer->writeEndElement();
    }

	writer->writeStartElement("Description");
	writer->writeCharacters(this->description());
	writer->writeEndElement(); // </Description>

	writer->writeEndElement(); // </Header>

}

QString DFile::error()
{
	QString error = m_error;
	m_error = "";

	return error;
}

void DFile::modifiedExternally()
{
    content_modified = true;
    notifyChanges();
}

void DFile::notifyChanges()
{
    unsaved_changes = true;
    emit file_modified();
}

bool DFile::pathIsValid()
{
    return (! m_path.isEmpty()) && (! path_is_bogus);
}



