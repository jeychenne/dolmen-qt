/*
 * metadata.cpp
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
 * Created: 01/09/10
 */

#include "metadata.h"



namespace metadata
{

QMultiHash<QString, QString> hashProperties(QHash<QString, metadata::Property *> properties)
{
	QMultiHash<QString, QString> hash = QMultiHash<QString, QString>();

    foreach(metadata::Property *tag, properties.values())
		hash.insert(tag->category(), tag->value());

	return hash;
}

QMultiHash<QString, QString> hashProperties(QSet<metadata::Property *> properties)
{
	QMultiHash<QString, QString> hash = QMultiHash<QString, QString>();

    foreach(metadata::Property *tag, properties)
		hash.insert(tag->category(), tag->value());

	return hash;
}


// duplicated from Property::toString()
QString str2tag(QString &cat, QString &label)
{
	QString result;

	if (cat == "")
		result = label;
	else
		result = cat + " : " + label;

	return result;
}

/**************************************************************************************************
 ***                                       CLASS PROPERTY                                      ****
 **************************************************************************************************/

Property::Property(const QString &label, const QString &category)
{
	m_value = label;
	m_category = category;

    // the empty property is not registered in the property set.
    // It is used by the file browser to keep track of all properties
    if (! metadata::Property::properties.contains(this->toString()))
        metadata::Property::properties[this->toString()] = this;
}

QString Property::category()
{
	return m_category;
}

void Property::setCategory(const QString &value)
{
	m_category = value;
}

QString Property::value()
{
	return m_value;
}

void Property::setValue(const QString &value)
{
	m_value = value;
}


bool Property::isValid(const QString &s)
{
	if (s.contains(':'))
		return false;

	else
		return true;
}

bool Property::equals(Property *p)
{
    return (p == this) || (this->category() == p->category() && this->value() == p->value());
}

QString Property::toString()
{
	QString result;

	if (m_category == "")
		result = m_value;

	else
		result = m_category + " : " + m_value;

	return result;
}

void Property::writeToXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement("Property");

	writer->writeStartElement("Category");
	writer->writeCharacters(m_category);
	writer->writeEndElement(); // </Category>

	writer->writeStartElement("Value");
	writer->writeCharacters(m_value);
	writer->writeEndElement(); // </Value>

    writer->writeEndElement(); // </Property>
}


/**************************************************************************************************
 ***                                   CLASS AUTHOR                                            ****
 **************************************************************************************************/
Author::Author(QString name, QString email)
{
	m_name = name;
	m_email = email;
}

QString Author::name() const
{
	return m_name;
}

void Author::setName(const QString &name)
{
	m_name = name;
}

QString Author::email() const
{
	return m_email;
}

void Author::setEmail(const QString &email)
{
	m_email = email;
}

Author* Author::readFromXml(QXmlStreamReader *reader)
{
	Author *author = new Author("");

	while (! (reader->isEndElement() && reader->name() == "Author"))
	{
		if (reader->isStartElement())
		{
			if (reader->name() == "Name")
				author->setName(reader->readElementText());

			else if (reader->name() == "Email")
				author->setEmail(reader->readElementText());
		}

		reader->readNext();
	}

	if (author->name() == "")
		return NULL;
	else
		return author;
}

QString Author::toString() const
{
	return m_name;
}

/**************************************************************************************************
 ***                                   CLASS VERSION                                           ****
 **************************************************************************************************/

Version::Version(QString description, Author *mainAuthor, QDate date, QString number)
{
	m_authors = QList<Author*>();
	if (mainAuthor)
		m_authors << mainAuthor;
	m_date = date;
	m_description = description;
	m_number = number;
}

QDate Version::date() const
{
	return m_date;
}

void Version::setDate(const QDate &date)
{
	m_date = date;
}

QString Version::description() const
{
	return m_description;
}

void Version::setDescription(const QString &desc)
{
	m_description = desc;
}

QString Version::number() const
{
	return m_number;
}

void Version::setNumber(const QString &number)
{
	m_number = number;
}

void Version::addAuthor(Author *author)
{
	m_authors << author;
}

void Version::removeAuthor(Author *author)
{
	int index = -1;

	for (int i = 0; i < m_authors.size(); ++i)
	{
		if (m_authors.at(i)->toString() == author->toString())
		{
			index = i;
			break;
		}
	}

	if (index != -1)
		m_authors.removeAt(index);
}

void Version::removeAuthor(int index)
{
	m_authors.removeAt(index);
}

QList<Author*> Version::authors() const
{
	return m_authors;
}

Version* Version::readFromXml(QXmlStreamReader *reader)
{
	Version *version = new Version("");

	while (! (reader->isEndElement() && reader->name() == "Version") || reader->atEnd())
	{
		if (reader->isStartElement())
		{
			if (reader->name() == "Description")
				version->setDescription(reader->readElementText());

			else if (reader->name() == "NumberString")
				version->setNumber(reader->readElementText());

			else if (reader->name() == "Date")
			{
				QString format = reader->attributes().value("format").toString();
				QDate date = QDate::fromString(reader->readElementText(), format);
				version->setDate(date);
			}
			else if (reader->name() == "Author")
			{
				Author *a = Author::readFromXml(reader);
				if (a) version->addAuthor(a);
			}
		}
		reader->readNext();
	}

	if (version->description() == "")
		return NULL;
	else
		return version;
}

QString Version::toString() const
{
	QString str = "";

	if (m_number != "")
		str = " (" + m_number + ")";

	return m_date.toString(DM_DATE_FORMAT) + str;
}

QString Version::toHtml() const
{
	QString str = QString("<b>Description:</b>&nbsp;%1<br/>"
						  "<b>Date:</b>&nbsp;%2<br/>").arg(description()).arg(date().toString(DM_DATE_FORMAT));

	if (! number().isEmpty())
		str += QString("<b>Number:</b>&nbsp;%1<br/>").arg(number());

	if (! m_authors.isEmpty())
	{
		str += "<b>Authors:</b>&nbsp;";

		QStringList authors;
		foreach (Author *author, m_authors)
		{
			authors << QString("%1%2").arg(author->name())
			.arg(author->email().isEmpty()? "": "&nbsp;&lt;"+author->email()+"&gt;");
		}

		str += authors.join(", ");
	}

	return str;
}

/**************************************************************************************************
 ***                                   CLASS CHANGELOG                                         ****
 **************************************************************************************************/
Changelog::Changelog()
{
	m_versions = QList<Version*>();
}

QList<Version*> Changelog::versions() const
{
	return m_versions;
}

Version* Changelog::version(int index) const
{
	Q_ASSERT(index >= 0 && index < m_versions.size());
	return m_versions.at(index);
}

void Changelog::addVersion(Version *version)
{
	m_versions << version;
	emit changelog_changed();
}

void Changelog::removeVersion(Version *version)
{
	int key = -1;

	for (int i = 0; i < m_versions.size(); ++i)
	{
		if (m_versions.at(i)->toString() == version->toString())
		{
			key = i;
			break;
		}
	}

	if (key != -1)
		this->removeVersion(key);
}

void Changelog::removeVersion(int index)
{
	m_versions.removeAt(index);
	emit changelog_changed();
}

void Changelog::readFromXml(QXmlStreamReader *reader)
{
	while (! (reader->isEndElement() && reader->name() == "Changelog") || reader->atEnd())
	{
		if (reader->name() == "Version" && reader->isStartElement())
		{
			Version *v = Version::readFromXml(reader);
			if (v) m_versions << v; // by-pass method to avoid triggering signal
		}
		reader->readNext();
	}
}


/**************************************************************************************************
 ***                                   CLASS COMMENT                                           ****
 **************************************************************************************************/

Comment::Comment()
{
	m_date = QDate::currentDate();
}

Comment::Comment(const QString &text, const QString &author, const QDate &date)
{
	m_text = text;
	m_author = author;
	m_date = date;
}

QString Comment::author() const
{
	return m_author;
}

void Comment::setAuthor(const QString &author)
{
	m_author = author;
}

QString Comment::text() const
{
	return m_text;
}

void Comment::setText(const QString &text)
{
	m_text = text;
}

QDate Comment::date()
{
	return m_date;
}

void Comment::setDate(QDate &date)
{
	m_date = date;
}


/**************************************************************************************************
 ***                                   FUNCTIONS                                               ****
 **************************************************************************************************/

QStringList getAllCategories()
{
    QList<Property*> properties = Property::properties.values();
    QStringList cats;

    foreach (Property *property, properties)
    {
        if (! cats.contains(property->category()))
            cats.append(property->category());
    }

    cats.sort();

    return cats;
}

QStringList getAllValues(QString category)
{
    QList<Property*> properties = Property::properties.values();
    QStringList values;

    foreach (Property *property, properties)
    {
        if (property->category() == category && ! values.contains(property->value()))
            values.append(property->value());
    }

    values.sort();

    return values;
}


} /* end namespace */
