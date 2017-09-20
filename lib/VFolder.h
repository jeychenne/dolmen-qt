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
 * Purpose: implements virtual folders in the project's file system
 * Created: 07/09/2010
 */

#ifndef VFOLDER_H
#define VFOLDER_H

#include <QList>
#include <QSet>

#include "lib/VFileNode.h"

class DFile;

class VFolder : public VFileNode
{
	Q_OBJECT
	Q_PROPERTY(QString label READ label WRITE setLabel)

public:
	VFolder(VFolder *parent, QString label);
	~VFolder();

	QString label() const;
	void setLabel(const QString &value);

	VFolder* parentFolder();
	void setParentFolder(VFolder *);
	void moveTo(VFolder *, int pos = -1);
	QStringList properties() const;

	void addFile(VFileNode *, int pos = -1);
	void removeFile(VFileNode *);

	// the content of the folder is visible to the GUI
    const QList<VFileNode*> &content() const;
	// request a path for a subfile (only used by the root since it's
	// the only file connected to the GUI)
	void requestFilePath(DFile*);
    void clear();

signals:
	void file_needsPath(DFile *);


private:
	QString m_label;
	VFolder *m_parent;
	QSet<Property*> m_properties;
    QList<VFileNode*> m_content;

};

#endif // VFOLDER_H
