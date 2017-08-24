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

#ifndef VFILE_H
#define VFILE_H

#include "lib/VFileNode.h"
#include "lib/VFolder.h"
#include "lib/DFile.h"
#include "utils/routines.h"


class VFile : public VFileNode
{
	Q_OBJECT

public:
	VFile(VFolder *parent, DFile *f);
	~VFile();

	VFolder* parentFolder();
	void setParentFolder(VFolder *);
	void moveTo(VFolder *f, int pos = -1);
	QStringList properties() const;

	DFile * object() const;

private:
	DFile *m_object;
	VFolder *m_parent;
};

#endif // VFILE_H

