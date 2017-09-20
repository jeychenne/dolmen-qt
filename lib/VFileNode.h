/*
 * VFileNode.h
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
 * Purpose: VFileNode is an abstraction over files and folders
 * in the project's file system. Files are implemented by the class VFile
 * and folders by VFolder.
 * They are used in the file browser.
 * Created: 03/09/10
 */

#ifndef VFILENODE_H
#define VFILENODE_H

#include <QObject>
#include <QStringList>

#include "utils/routines.h"
#include "lib/metadata.h"
using namespace metadata;

class VFolder;

class VFileNode : public QObject
{
	Q_OBJECT

 public:
	VFileNode();

	virtual VFolder* parentFolder() = 0;
	virtual void setParentFolder(VFolder *) = 0;
	virtual void moveTo(VFolder *f, int pos = -1) = 0;
	virtual QStringList properties() const = 0;

	int id() const;
protected:
	int m_id;
};

#endif // VFILENODE_H

