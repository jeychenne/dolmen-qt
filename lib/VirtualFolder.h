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

#ifndef VIRTUALFOLDER_H
#define VIRTUALFOLDER_H

#include <QObject>
#include <QList>

#include "lib/VirtualFile.h"

class VirtualFolder: public VirtualFile
{
	Q_OBJECT
	Q_PROPERTY(QString label READ label WRITE setLabel)

public:
	VirtualFolder(VirtualFile *parent, QString label);

	QString label() const;
	void setLabel(const QString &value);

	void addFile(VirtualFile *);
	void removeFile(VirtualFile *);

	// the content of the folder is visible to the GUI
	QList<VirtualFile*> content;

private:
    QString m_label;


};

#endif // VIRTUALFOLDER_H
