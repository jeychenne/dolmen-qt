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

#include "VFile.h"


VFile::VFile(VFolder *parent, DFile *f) :
        VFileNode()
{
	m_object = f;
	m_parent = parent;
}

VFolder* VFile::parentFolder()
{
	return m_parent;
}

void VFile::setParentFolder(VFolder *parent)
{
	m_parent = parent;
}

void VFile::moveTo(VFolder *newFolder, int pos)
{
    //qDebug() << QString("Moving file to '%1' as pos %2").arg(newFolder->label()).arg(pos);
	this->parentFolder()->removeFile(this);
	newFolder->addFile(this, pos);
	setParentFolder(newFolder);
}

DFile * VFile::object() const
{
	return m_object;
}

QStringList VFile::properties() const
{
	return m_object->propertiesAsStrings();
}

VFile::~VFile()
{
	parentFolder()->removeFile(this);
}
