/*
 * SearchMatch.cpp
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
 * Purpose: implement query matches in queries
 * Created: 21/09/2010
 */

#include <cassert>
#include "SearchMatch.h"

uint qHash(const SearchMatchPtr &p)
{
    return qHash(p.get());
}

// TODO: this constructor is currently not used
SearchMatch::SearchMatch(DFile *file, Item *item, int tier, double match) :
	Bookmark()
{
    m_dfile     = file;
    matched_item = item;
    m_match     = QString::number(match);
    m_left      = m_right = "";
    m_nth_match = 1;
    m_tier      = tier;
    m_siblings  = QList<SearchMatchPtr>();
}

SearchMatch::SearchMatch(DFile *file, Item *item, int tier, QString match, QString left, QString right)
{
    m_dfile     = file;
    matched_item = item;
    m_match     = match;
    m_left      = left;
    m_right     = right;
    m_tier      = tier;
    m_nth_match = 1;
    m_siblings  = QList<SearchMatchPtr>();
}

SearchMatch::SearchMatch(DFile *file, QString match, QString left, QString right)
{
    m_dfile     = file;
    m_match     = match;
    m_left      = left;
    m_right     = right;
    matched_item = NULL;
    m_tier      = -1;
    m_nth_match = 1;
    m_siblings  = QList<SearchMatchPtr>();
}

// sort by file, and then by tier, then by time stamp
bool SearchMatch::operator <(const SearchMatch &other) const
{
    int cmp;

    // Compare file names
    cmp = this->file()->path().compare(other.file()->path());

    if (cmp < 0) {
        return true;
    }
    else if (cmp > 0) {
        return false;
    }
    else
    {
        // Compare tier in the file
        cmp = (this->tier() - other.tier());

        if (cmp < 0) {
            return true;
        }
        else if (cmp > 0) {
            return false;
        }
        else
        {
            // Compare temporal position
            double cmp = (this->itemStart() - other.itemStart());

            if (cmp < 0) {
                return true;
            }
            else if (cmp > 0) {
                return false;
            }
            else
            {
                return this->nth() < other.nth();
            }
        }
    }
}

QString SearchMatch::toHtml() const
{
	if (m_left.isEmpty() && m_right.isEmpty())
		return "<font face = \"" DM_MONO_FONT "\">" + dm_escapeHtml(m_match) + "</font>";

	else
		return "<font face = \"" DM_MONO_FONT "\">" + dm_escapeHtml(m_left)
				+ "<font color = \"red\"><strong>" + dm_escapeHtml(m_match) + "</strong></font>"
				+ dm_escapeHtml(m_right) + "</font>"; //</center>

}


QString SearchMatch::toHtmlWithError() const
{
    if (m_left.isEmpty() && m_right.isEmpty())
        return "<strong><font color = \"red\" face = \"" DM_MONO_FONT "\">" + dm_escapeHtml(m_match) + "</font></strong>";

    else
        return "<strong><font color = \"red\" face = \"" DM_MONO_FONT "\">" + dm_escapeHtml(m_left)
                + "<font color = \"blue\">" + dm_escapeHtml(m_match) + "</font>"
                + dm_escapeHtml(m_right) + "</font></strong>"; //</center>

}

QString SearchMatch::toString() const
{
	return m_left + m_match + m_right;
}


QString SearchMatch::match() const
{
	return m_match;
}

bool SearchMatch::hasSiblings() const
{
    return ! m_siblings.isEmpty();
}

void SearchMatch::addSibling(const SearchMatchPtr &m)
{
    m_siblings.append(m);
}
//  CHANGE HERE WHEN COMPARING FIELDS
bool SearchMatch::hasDivergingAnnotations(Grammar *grammar) const
{
    QStringList fields;
    if (grammar)
        fields << grammar->matchActiveFields(m_match).join("");

    foreach (auto &sibling, m_siblings)
    {
        if (grammar)
        {
            QString sibling_field = grammar->matchActiveFields(sibling->match()).join("");

            // compare all fields one to one, in case annotators 2 and 3 disagree but 1 and 2 agree, for instance
            foreach (QString field, fields)
            {
                if (field != sibling_field)
                    return true;
                else
                    fields << sibling_field;
            }
        }
        else if (sibling->match() != m_match)
            return true;
    }

    // Also returns false if m_siblings is empty
    return false;
}

QList<SearchMatchPtr> SearchMatch::siblings() const
{
    return m_siblings;
}

QString SearchMatch::left() const
{
	return m_left;
}

QString SearchMatch::right() const
{
	return m_right;
}

double SearchMatch::itemStart() const
{
    if (matched_item)
        return matched_item->left();
    else
        return -1.0;
}

double SearchMatch::itemEnd() const
{
    if (matched_item)
        return matched_item->right();
    else
        return -1.0;
}

int SearchMatch::nth() const
{
    return m_nth_match;
}

void SearchMatch::setNth(int n)
{
    m_nth_match = n;
}

DFile* SearchMatch::file() const
{
	return m_dfile;
}

Item* SearchMatch::item() const
{
    return matched_item;
}

int SearchMatch::tier() const
{
	return m_tier + 1;
}

int SearchMatch::tier0() const
{
	return m_tier;
}

void SearchMatch::setTier(int tier)
{
    m_tier = tier - 1;
}

void SearchMatch::openInPraat()
{
     if (isInstance(m_dfile, Annotation))
     {
         Annotation *annot = qobject_cast<Annotation*>(m_dfile);
         Sound *snd = annot->soundFile();
         QString sound_path = snd ? snd->path() : QString();

         Global::Praat->openInterval(tier(), matched_item->left() + 0.00001, annot->path(), sound_path);
     }
     else
     {
         Bookmark::openInPraat();
     }
}

void SearchMatch::setTier0(int tier)
{
	m_tier = tier;
}

bool SearchMatch::readFromXml(QXmlStreamReader *reader)
{
	QString path;
	double start = INVALID_TIME;
	double end   = INVALID_TIME;

	while (! (reader->isEndElement() && reader->name() == "Bookmark"))
	{
		if (reader->isStartElement())
		{
			if (reader->name() == "Title")
				m_title = reader->readElementText();
			else if (reader->name() == "Notes")
				m_notes = reader->readElementText();
			else if (reader->name() == "LeftContext")
				m_left = reader->readElementText();
			else if (reader->name() == "Match")
				m_match = reader->readElementText();
			else if (reader->name() == "RightContext")
				m_right = reader->readElementText();
			else if (reader->name() == "File")
				path = interpolatePath(reader->readElementText());
			else if (reader->name() == "Start")
				start = reader->readElementText().toDouble();
			else if (reader->name() == "End")
				end = reader->readElementText().toDouble();
			else if (reader->name() == "Tier")
				m_tier = reader->readElementText().toInt() - 1;
		}

		reader->readNext();
	}

	m_dfile = Global::FileHash.value(path);
	if (m_dfile)
	{
		if (isInstance(m_dfile, Annotation))
		{
			Annotation *a = qobject_cast<Annotation*>(m_dfile);
            a->open();

			matched_item = a->tier(m_tier)->item(start, end);
			if (! matched_item)
			{
				qDebug() << "Matched Item not found in " + path + QString("at %1 to %2").arg(start).arg(end);
				return false;
			}
		}
	}
	else
	{
		qDebug() << "empty DFile in Bookmark " + path;
		return false;
	}

	return true;
}

void SearchMatch::writeToXml(QXmlStreamWriter *writer)
{
	writer->writeStartElement("Bookmark");
	writer->writeAttribute("type", this->type());

	writer->writeStartElement("Title");
	writer->writeCharacters(m_title);
	writer->writeEndElement(); // </Title>

	writer->writeStartElement("Notes");
	writer->writeCharacters(m_notes);
	writer->writeEndElement(); // </Notes>

	writer->writeStartElement("LeftContext");
	writer->writeCharacters(m_left);
	writer->writeEndElement(); // </LeftContext>

	writer->writeStartElement("Match");
	writer->writeCharacters(m_match);
	writer->writeEndElement(); // </Match>

	writer->writeStartElement("RightContext");
	writer->writeCharacters(m_right);
	writer->writeEndElement(); // </RightContext>

	writer->writeStartElement("File");
	writer->writeCharacters(compressPath(m_dfile->path()));
	writer->writeEndElement(); // </File>

	writer->writeStartElement("Tier");
	writer->writeCharacters(QString::number(this->tier()));
	writer->writeEndElement(); // </Tier>

	writer->writeStartElement("Start");
	writer->writeCharacters(QString::number(matched_item->left(), 'f', 15));
	writer->writeEndElement(); // </Start>

	writer->writeStartElement("End");
	writer->writeCharacters(QString::number(matched_item->right(), 'f', 15));
	writer->writeEndElement(); // </End>

	writer->writeEndElement(); // </Bookmark>
}

QString SearchMatch::type() const
{
	return QString(this->metaObject()->className());
}

void SearchMatch::open()
{
	if (isInstance(m_dfile, Annotation))
	{
		double start = matched_item->left();
		double end = matched_item->right();

		if (start == end && start >= 0.5)
		{
			start -= 0.5;
			end += 0.5;
		}

		emit openDFile(m_dfile, start, end);
	}
}

QString SearchMatch::tooltip() const
{
	QString tip = "<b>File:</b><br/>" + m_dfile->path()
			+ "<br/><b>Match:</b><br/>" + this->toHtml();

	if (! m_notes.isEmpty())
			tip += "<br/><b>Notes:</b><br/>" + m_notes;

	return tip;
}

