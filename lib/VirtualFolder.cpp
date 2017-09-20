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

#include "VirtualFolder.h"

VirtualFolder::VirtualFolder(VirtualFile *parent, QString label) :
        VirtualFile(parent, NULL)
{
	m_label = label;
}

QString VirtualFolder::label() const
{
	return m_label;
}

void VirtualFolder::setLabel(const QString &value)
{
	m_label = value;
}

void VirtualFolder::addFile(VirtualFile *f)
{
	content.append(f);
}

void VirtualFolder::removeFile(VirtualFile *f)
{
	content.removeOne(f);
}
