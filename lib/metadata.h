/*
 * metadata.h
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
 * Purpose: implements classes related to objects metadata (properties, comments, etc.)
 * Created: 01/09/10
 */

#ifndef METADATA_H_
#define METADATA_H_

#include <QObject>
#include <QDate>
#include <QString>
#include <QHash>
#include <QList>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "dolmen.h"
#include "utils/routines.h"

namespace metadata
{

/**************************************************************************************************
 ***                                   CLASS PROPERTY                                          ****
 **************************************************************************************************/

class Property : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString category READ category WRITE setCategory)
	Q_PROPERTY(QString value READ value WRITE setValue)

public:
    Property(const QString &label, const QString &category = "");

	QString category();
	void setCategory(const QString &);

	QString value();
	void setValue(const QString &);

	// labels and categories can't contain the character ':'
    // since it's used to display the property (as "category : label")
	bool isValid(const QString &);
    bool equals(Property *p);

	QString toString();
	void writeToXml(QXmlStreamWriter *);

    // global variable containing all properties
    static QHash<QString, Property*> properties;

private:
    QString m_category, m_value;
    // properties are registered in their stringified form.

};

typedef QList<Property*> PropertyList;
typedef QSet<Property*> PropertySet;

/**************************************************************************************************
 ***                                   CLASS AUTHOR                                            ****
 **************************************************************************************************/
class Author: public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(QString email READ email WRITE setEmail)

public:
	Author(QString name, QString email = QString());

	QString name() const;
	void setName(const QString &name);

	QString email() const;
	void setEmail(const QString &email);

	static Author* readFromXml(QXmlStreamReader *reader);
	QString toString() const;

private:
	QString m_name, m_email;
};

/**************************************************************************************************
 ***                                   CLASS VERSION                                           ****
 **************************************************************************************************/
class Version: public QObject
{
	Q_OBJECT

	Q_PROPERTY(QDate date READ date WRITE setDate)
	Q_PROPERTY(QString number READ number WRITE setNumber) // representation as a string (e.g. "1.0")
	Q_PROPERTY(QString description READ description WRITE setDescription)
	//TODO: add authors as properties
public:
	Version(QString description, Author *mainAuthor = NULL, QDate date = QDate::currentDate(), QString number = QString());
	QDate date() const;
	void setDate(const QDate &date);
	QString number() const;
	void setNumber(const QString &number);
	QString description() const;
	void setDescription(const QString &desc);
	QList<Author*> authors() const;
	void addAuthor(Author *author);
	void removeAuthor(Author *author);
	void removeAuthor(int index);

	static Version* readFromXml(QXmlStreamReader *reader);
	QString toString() const;
	QString toHtml() const;

private:
	QList<Author*> m_authors;
	QString m_description, m_number;
	QDate m_date;
};


/**************************************************************************************************
 ***                                   CLASS CHANGELOG                                         ****
 **************************************************************************************************/
class Changelog : public QObject
{
	Q_OBJECT

signals:
	void changelog_changed();

public:
	Changelog();
	QList<Version*> versions() const;
	Version* version(int index) const;
	void addVersion(Version *version);
	void removeVersion(Version *version);
	void removeVersion(int index);
	void readFromXml(QXmlStreamReader *reader);

private:
	QList<Version*> m_versions;
};

/**************************************************************************************************
 ***                                   CLASS COMMENT                                           ****
 **************************************************************************************************/

class Comment : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString text READ text WRITE setText)
	//TODO: add Author property
	//Q_PROPERTY(QString author READ author WRITE setAuthor)
	Q_PROPERTY(QDate date READ date WRITE setDate)

public:
	Comment();
	Comment(const QString &text, const QString &author = "", const QDate &date = QDate::currentDate());

	QString text() const;
	void setText(const QString &text);

	QString author() const;
	void setAuthor(const QString &author);

	QDate date();
	void setDate(QDate &date);



private:
    QString m_text, m_author;
    QDate m_date;
};


/**************************************************************************************************
 ***                                   FUNCTIONS                                               ****
 **************************************************************************************************/


// return a hash { category: [label1, label2...], ... }
QMultiHash<QString, QString> hashProperties(QHash<QString, metadata::Property*>);
QMultiHash<QString, QString> hashProperties(QSet<metadata::Property*>);

QString str2tag(QString &cat, QString &label);

QStringList getAllCategories();
QStringList getAllValues(QString category);


} /* end namespace */

#endif /* METADATA_H_ */
