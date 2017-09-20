/*
 * DataTable.h
 *
 * Copyright (C) 2010-2017 Julien Eychenne
 * Copyright (C) 2012 Japanese Society for the Promotion of Science (JSPS)
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
 * Purpose: implements a data table file
 * Created: 15/4/2012
 *
 * The development of this module has been supported by the Japanese
 * Society for the Promotion of Science (JSPS), Grant-in-Aid for Scientific
 * Research (B) n°23320121 (2011-2014).
 * Project title: A corpus-based longitudinal study of the interphonological
 * features of Japanese learners of French.
 * Project leader: Sylvain DETEY (Waseda University, Tokyo, Japan).
 */

#ifndef DATATABLE_H
#define DATATABLE_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTextStream>

#include "lib/DFile.h"
#include "lib/TableModel.h"

class DataTable : public DFile
{
    Q_OBJECT

public:
    DataTable(TableModel *model);
    DataTable(QList<QStringList> rows);
    DataTable(QString path);
    TableModel * model() const;
    void save();
    void open();
    QString metadataAsHtml(bool with_description) const;
    void writeToExcel(const QString &path);
    void writeToCsv(const QString &path);

signals:


private:
    TableModel *m_model;
    void writeToDmt(QString path = QString());
    void readFromDmt();
    void readXmlTable(QXmlStreamReader *reader);
    void readXmlColumn(QXmlStreamReader *reader);

};

#endif // DATATABLE_H
