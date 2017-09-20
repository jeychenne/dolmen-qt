/*
 * DataTable.cpp
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

#include "DataTable.h"

DataTable::DataTable(TableModel *model) : DFile()
{
    // created from a query view, not yet saved to disk
    m_model = model;
    unsaved_changes = true;
}

DataTable::DataTable(QList<QStringList> rows) : DFile()
{
    m_model = new TableModel(rows);
    unsaved_changes = true;
}

DataTable::DataTable(QString path) : DFile()
{
    m_path = path;
    m_model = NULL;
    unsaved_changes = false;

    readFromDmt();
}


TableModel * DataTable::model() const
{
    return m_model;
}

void DataTable::save()
{
    if (unsaved_changes || m_model->isModified())
    {
        writeToDmt();
        unsaved_changes = content_modified = false;
        m_model->setModified(false);
    }
}

void DataTable::writeToDmt(QString path)
{
    if (path == "")
        path = m_path;

    QFile file(path);

    if (! file.open(QFile::WriteOnly | QFile::Text))
    {
        m_error = QString("The file '%1' cannot be written: %2").arg(path, file.errorString());
        return;
    }

    QXmlStreamWriter writer(&file);

    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("DolmenFile");
    writeXmlHeader(&writer, CLASSNAME(this), "", "");


    writer.writeStartElement("Table");

    foreach(TableColumn *column, m_model->columns())
    {
        column->writeToXml(&writer);
    }

    writer.writeEndElement(); // </Table>

    writer.writeEndDocument();
    file.close();

    if (file.error())
        m_error = QString("The file '%1' cannot be written: %2").arg(path, file.errorString());
}

void DataTable::readFromDmt()
{
    QFile file(m_path);

    if (! file.open(QFile::ReadOnly | QFile::Text))
    {
        QString msg = QString("Cannot read file %1").arg(m_path);
        emit file_error(msg); return;
    }

    QXmlStreamReader reader(&file);
    reader.readNext();

    while (! reader.atEnd())
    {
        if (reader.isStartElement())
        {
            if (reader.name() == "Header")
            {
                readXmlHeader(&reader, "");
            }

            else if (reader.name() == "Table")
                readXmlTable(&reader);

            else
                reader.readNext();
        }
        else
            reader.readNext();
    }

    file.close();

    if (reader.hasError())
    {
        QString msg = QString("Problem parsing %1").arg(m_path);
        emit file_error(msg);
    }
    else if (file.error() != QFile::NoError)
    {
        QString msg = QString ("Cannot read %1").arg(m_path);
        emit file_error(msg);
    }

}

void DataTable::readXmlTable(QXmlStreamReader *reader)
{
    m_model = new TableModel;

    while (! (reader->isEndElement() && reader->name() == "Table"))
    {
        if (reader->isStartElement() && reader->name() == "Column")
            readXmlColumn(reader);

        else
            reader->readNext();
    }
}

void DataTable::readXmlColumn(QXmlStreamReader *reader)
{
   TableColumn *column;

    if (reader->attributes().hasAttribute("type") && reader->attributes().hasAttribute("name"))
    {
        QString type = reader->attributes().value("type").toString();
        QString name = reader->attributes().value("name").toString();

        if (type == "boolean")
            column = new ColumnOfBool(name);
        else if (type == "integer")
            column = new ColumnOfInt(name);
        else if (type == "real")
            column = new ColumnOfReal(name);
        else
            column = new ColumnOfString(name);
    }
    else
        column = new ColumnOfString;

    if (reader->attributes().hasAttribute("size"))
    {
        int size = QVariant(reader->attributes().value("size").toString()).toInt();
        column->reserve(size);
    }

    while (! (reader->isEndElement() && reader->name() == "Column"))
    {
        if (reader->isStartElement() && reader->name() == "Cell")
        {
            QString value = reader->readElementText();
            column->append(value);
        }
        else
            reader->readNext();
    }

    m_model->append(column);
}

void DataTable::open()
{
    readFromDmt();
    m_open = true;
}

QString DataTable::metadataAsHtml(bool with_description) const
{
    Q_UNUSED(with_description)
    return "";
}

void DataTable::writeToExcel(const QString &path)
{
    QFile file(path);

    if (! file.open(QFile::WriteOnly | QFile::Text))
    {
        m_error = QString("The file '%1' cannot be written: %2").arg(path, file.errorString());
        return;
    }

    QXmlStreamWriter writer(&file);

    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("Workbook");

    writer.writeAttribute("xmlns", "urn:schemas-microsoft-com:office:spreadsheet");
    writer.writeAttribute("xmlns:o", "urn:schemas-microsoft-com:office:office");
    writer.writeAttribute("xmlns:x", "urn:schemas-microsoft-com:office:excel");
    writer.writeAttribute("xmlns:ss", "urn:schemas-microsoft-com:office:spreadsheet");
    writer.writeAttribute("xmlns:html", "http://www.w3.org/TR/REC-html40");

    QFileInfo info(m_path);
    writer.writeStartElement("Worksheet");
    writer.writeAttribute("ss:Name", info.baseName());
    m_model->writeToExcel(&writer);
    writer.writeEndElement(); // </Worksheet>

    writer.writeEndDocument();
    file.close();

    if (file.error())
        m_error = QString("The file '%1' cannot be written: %2").arg(path, file.errorString());
}

void DataTable::writeToCsv(const QString &path)
{
    QFile file(path);

    if (! file.open(QFile::WriteOnly | QFile::Text))
    {
        m_error = QString("The file '%1' cannot be written: %2").arg(path, file.errorString());
        return;
    }

    QTextStream outfile(&file);
    outfile.setCodec("UTF-8");

    QList<QStringList> rows = m_model->asPrintableTable();
    foreach (QStringList row, rows)
    {
        // values are surrounded by quotes
        QString line = "\"" + row.join("\";\"") + "\"\n";
        outfile << line;
    }

    file.close();

    if (file.error())
        m_error = QString("The file '%1' cannot be written: %2").arg(path, file.errorString());
}
