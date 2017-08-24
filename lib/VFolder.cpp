/*
 * dolmen.h
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
 * Purpose:
 * Created: //
 */

#include "VFolder.h"

VFolder::VFolder(VFolder *parent, QString label) :
        VFileNode()
{
	this->setLabel(label);
	m_parent = parent;
}

VFolder* VFolder::parentFolder()
{
	return m_parent;
}

void VFolder::setParentFolder(VFolder *f)
{
	m_parent = f;
}

QString VFolder::label() const
{
	return m_label;
}

void VFolder::setLabel(const QString &value)
{
	QString label = value;
	label.replace("_", " ");
	m_label = label;
}

void VFolder::addFile(VFileNode *f, int pos)
{
	if (pos == -1)
        m_content.append(f);
	else
        m_content.insert(pos, f);
}

void VFolder::removeFile(VFileNode *f)
{
    m_content.removeOne(f);
}

const QList<VFileNode *> &VFolder::content() const
{
    return m_content;
}


void VFolder::moveTo(VFolder *newFolder, int pos)
{
//	qDebug() << QString("Moving file to '%1' as pos %2").arg(newFolder->label()).arg(pos);
	this->parentFolder()->removeFile(this);
	newFolder->addFile(this, pos);
	setParentFolder(newFolder);
}

QStringList VFolder::properties() const
{
	QStringList result;

	foreach(Property *tag, m_properties)
		result << tag->toString();

	return result;
}

void VFolder::requestFilePath(DFile *f)
{
	emit file_needsPath(f);
}

void VFolder::clear()
{
    m_label.clear();
    m_parent = NULL;
    m_properties.clear();
    m_content.clear();
}

VFolder::~VFolder()
{
    foreach(VFileNode *f, m_content)
		delete f;

	if (parentFolder() != NULL)
		parentFolder()->removeFile(this);

	//TODO: see if there's more to delete
}
