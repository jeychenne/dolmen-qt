/*
 * Annotation.cpp
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
 * Created: 01/09/10
 */

#include <algorithm>
#include <QRegularExpression>
#include "Annotation.h"


Annotation::Annotation(const QString &path, bool bogus) : DFile()
{
	m_path			= path;
    m_sound_ref		= NULL;
    path_is_bogus   = bogus; // Only native annotations have a bogus path
    unsaved_changes = false;

    QFileInfo info  = QFileInfo(m_path);
    QString ext     = info.suffix().toLower();

    // We are slowly getting rid of .dmm files. For now, this is
    // is still working. When a project stores a dmm file, we parse
    // it as we used to. When the project is saved, only the textgrid
    // is written do the project and the metadata is now stored in the db.
    // When we find a plain textgrid in the project, we send the annotation
    // to the database so that it gets tagged.
    if (bogus || ext == DM_FILE_EXT || ext == DM_METADATA_EXT)
    {
        m_type = DM_ANNOTATION_NATIVE;
    }
    else if (ext == "textgrid")
    {
        m_type = DM_ANNOTATION_TEXTGRID;
    }
    else if (ext == "lab")
    {
        m_type = DM_ANNOTATION_WAVESURFER;
    }
}

void Annotation::open()
{
	if (m_open) { return; }
	load();
}

void Annotation::load()
{
	QFileInfo info = QFileInfo(m_path);

	if (! info.exists())
	{
		m_error = tr("File %1 doesn't exist").arg(m_path);
		return;
	}

    switch(m_type)
    {
    case DM_ANNOTATION_TEXTGRID:
    {
        this->parseFromTextgrid();
        break;
    }
    case DM_ANNOTATION_WAVESURFER:
    {
        this->parseFromLab();
        break;
    }
    default:
        this->readFromDmf();
    }

    m_open = true;
}

Tier* Annotation::tier(int no)
{
	if (no >= m_tiers.size())
	{
		m_error = tr("Trying to access tier %1 in %2").arg(no+1).arg(m_path);
		return NULL;
	}

    return m_tiers[no];
}

struct ItemLessThan
{
    bool operator()(Item* const &i1, Item* const &i2) const
    {
        return i1->left() < i2->left();
    }
};

Item *Annotation::nextItem(int tier_no, Item *item)
{
    auto tier = m_tiers.at(tier_no);
    auto items = tier->items();

    for (int i = 0; i < items.size()-1; ++i)
    {
        if (items.at(i) == item) {
            return items.at(i+1);
        }
    }

    return nullptr;

    //FIXME: doesn't seem to work...
//    auto it = std::lower_bound(items.cbegin(), items.cend(), item, ItemLessThan());

//    if (it == items.end()) {
//        return nullptr;
//    }
//    else {
//        return *it;
//    }
}

void Annotation::addNewTier(const QString &label, tier_type_t t, int pos)
{
    Tier *tier = new Tier;
    tier->setLabel(label);
    tier->setType(t);

    if (pos < 0 || pos >= m_tiers.size())
    {
        m_tiers.append(tier);
    }
    else
    {
        m_tiers.insert(pos, tier);
    }

    content_modified = unsaved_changes = true;
}

QList<Tier*> Annotation::tiers()
{
	open();
	return m_tiers;
}

int Annotation::countTiers()
{
	open();
	return m_tiers.size();
}

dm_annotation_type_t Annotation::type() const
{
    return m_type;
}

QString Annotation::leftCotext(int tier, int item, int pos, const QString &separator)
{
	QString cotext = m_tiers[tier]->item(item)->text().left(pos);
	cotext = cotext.right(Global::MatchCotextLength);

	while (cotext.size() != Global::MatchCotextLength)
	{
		item--;

		if (item == -1)
			cotext = cotext.rightJustified(Global::MatchCotextLength);
		else
		{
            cotext = m_tiers[tier]->item(item)->text() + separator + cotext;
			cotext = cotext.right(Global::MatchCotextLength);
			cotext.replace("  ", " ");
		}
	}

	return cotext;
}

QString Annotation::rightCotext(int tier, int item, int pos, const QString &separator)
{
	QString txt = m_tiers[tier]->item(item)->text();
	QString cotext = txt.right(txt.size()-pos);
	cotext = cotext.left(Global::MatchCotextLength);

	while (cotext.size() != Global::MatchCotextLength)
	{
		item++;

		if (item == m_tiers[tier]->countItems())
			cotext = cotext.leftJustified(Global::MatchCotextLength);
		else
		{
            cotext += separator + m_tiers[tier]->item(item)->text();
			cotext = cotext.left(Global::MatchCotextLength);
			cotext.replace("  ", " ");
		}
	}

	return cotext;
}


std::unique_ptr<GraphNode> Annotation::graphNode(Item *root, int targetTier) const
{
	int i;
	Q_ASSERT(targetTier < m_tiers.size());
    auto node = std::make_unique<GraphNode>(root);
	Tier *tier = m_tiers.at(targetTier);
	if (!tier) return NULL;
	// get inner items
	Item *firstItem = tier->rightmostItemAtTime(root->left());
	Item *lastItem  = tier->leftmostItemAtTime(root->right());

	if (! firstItem || ! lastItem)
		return NULL;

	if (firstItem->right() > root->right() || lastItem->left() < root->left())
	{
        return NULL;
	}

	long lindex = tier->indexOf(firstItem);
    long rindex = tier->indexOf(lastItem);

    for (i = lindex; i <= rindex; ++i) {
        node->addDaughter(tier->item(i));
    }

	return node;
}

// Find tier by name
std::unique_ptr<GraphNode> Annotation::graphNode(Item *root, QString tierName) const
{
    for (int i = 0; i < m_tiers.size(); ++i)
    {
        if (m_tiers.at(i)->label() == tierName)
            return this->graphNode(root, i);
    }

    return NULL;
}

QString Annotation::getTextSpan(int tier_no, double start, double end, const QString &separator) const
{
    QString text;
    Tier *tier = m_tiers.at(tier_no);
    Item *firstItem = tier->rightmostItemAtTime(start);
    Item *lastItem  = tier->leftmostItemAtTime(end);
    long lindex = tier->indexOf(firstItem);
    long rindex = tier->indexOf(lastItem);

    for (long i = lindex; i <= rindex; ++i)
    {
        text.append(tier->item(i)->text());
        if (i < rindex) {
            text.append(separator);
        }
    }

    return text;
}


void Annotation::parseFromTextgrid()
{
	using namespace std;
	QString line, tierName, text;
	QFile data(m_path);
	double start, end, time;
	int tier_no = -1;
	Tier *tier = NULL;

	if (!data.open(QIODevice::ReadOnly))
	{
		QString msg = QString("Cannot read TextGrid %1").arg(m_path);
		emit file_error(msg); return;
	}
	else
	{
		QTextStream stream(&data);
        // Default to UTF-8...
        stream.setCodec("UTF-8");
        // ... unless another encoding is found.
		stream.setAutoDetectUnicode(true);


		do
		{
			line = stream.readLine();
			start = end = time = -1.0; // reset values
			text = "";

			if (praat::readTierHeader(&stream, &line, &tierName))
			{
				tier = new Tier();
				tier->setLabel(tierName);
				m_tiers.append(tier);
				tier_no++;
			}

			else if (praat::readInterval(&stream, &line, &start, &end, &text))
				this->m_tiers[tier_no]->addItem(new DSpan(start, end, text));

			else if (praat::readPoint(&stream, &line, &time, &text))
				this->m_tiers[tier_no]->addItem(new DPoint(time, text));
		}
		while (! stream.atEnd());
	}

	data.close();

    emit file_setMetadata(this);
}

void Annotation::parseFromLab()
{
	using namespace std;

	QStringList line;
	QFile data(m_path);
	Tier *tier = new Tier();

	if (!data.open(QIODevice::ReadOnly))
	{
		QString msg = tr("Cannot read WaveSurfer transcription %1").arg(m_path);
		emit file_error(msg); return;
	}
	else
	{
		QTextStream stream(&data);

		while (! stream.atEnd())
		{
			line = stream.readLine().split(" ");

			if (line.size() != 3)
			{
				QString msg = tr("Error parsing Wavesurfer transcription %1").arg(m_path);
				emit file_error(msg);
				return;
			}

			tier->addItem(new DSpan(line.at(0).toDouble(), line.at(1).toDouble(), line.at(2)));
		}
	}
	data.close();

	if (tier->countItems() != 0)
	{
		m_tiers << tier;
	}
}

Item* Annotation::getItem(int tier_no, int item_no)
{
	return m_tiers[tier_no]->item(item_no);
}


void Annotation::readFromDmf()
{
    QString ref = ""; // sound ref
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
				ref = interpolatePath(readXmlHeader(&reader, "SoundReference"));
				//qDebug() << "SOUND: " + ref;
			}

			else if (reader.name() == "Tiers")
				readXmlTiers(&reader);

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

    if (ref != "")
        buffer_ref = ref;
}


void Annotation::readXmlTiers(QXmlStreamReader *reader)
{
	if (reader->attributes().hasAttribute("source") && reader->attributes().hasAttribute("format"))
	{
		QString attr = reader->attributes().value("format").toString();

		if (attr == "TextGrid" || attr == "WaveSurfer")
		{
            m_metadata_path = m_path;
			m_path = interpolatePath(reader->attributes().value("source").toString());
			this->load();
		}

		reader->readNext();
		return;
	}

	while (! (reader->isEndElement() && reader->name() == "Tiers"))
	{
		if (reader->isStartElement() && reader->name() == "Tier")
			readXmlTier(reader);

		else
			reader->readNext();
	}
}

void Annotation::readXmlTier(QXmlStreamReader *reader)
{
	Tier *tier = new Tier();

	while (! (reader->isEndElement() && reader->name() == "Tier"))
	{
		if (reader->isStartElement() && reader->name() == "Item")
			readXmlItem(reader, tier);

		else
			reader->readNext();
	}

	m_tiers.append(tier);
	//qDebug() << QString("Parsed a tier: %1 items").arg(tier->countItems());
}

void Annotation::readXmlItem(QXmlStreamReader *reader, Tier *tier)
{
	bool itemIsSpan = reader->attributes().value("class") == "Span"? true: false;
	QString start, end, time, text;

	while (! (reader->isEndElement() && reader->name() == "Item"))
	{
		if (reader->isStartElement())
		{
			if (reader->name() == "Start")
				start = reader->readElementText();

			else if (reader->name() == "End")
				end = reader->readElementText();

			else if (reader->name() == "Time")
				time = reader->readElementText();

			else if (reader->name() == "Text")
				text = reader->readElementText();

			else
				reader->readNext();
		}
		else
			reader->readNext();
	}

	if (itemIsSpan)
		tier->addItem(new DSpan(start.toDouble(), end.toDouble(), text));
	else
		tier->addItem(new DPoint(time.toDouble(), text));
}

void Annotation::save()
{
	if (hasUnsavedChanges())
	{
		if (isNative())
			writeToDmf(m_path);
		else
		{
            emit file_saveMetadata(this);

            if (m_type == DM_ANNOTATION_TEXTGRID && content_modified)
				writeToTextGrid(m_path);
		}

        unsaved_changes = content_modified = false;
	}

    path_is_bogus = false;
}

void Annotation::writeToDmf(QString path)
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

    QString ref = (m_sound_ref == NULL)? "": compressPath(m_sound_ref->path());

    writeXmlHeader(&writer, CLASSNAME(this), "SoundReference", ref);

	if (isNative())
	{
		writer.writeStartElement("Tiers");

		foreach(Tier *tier, m_tiers)
			tier->writetoXml(&writer);

		writer.writeEndElement(); // </Tiers>
	}
	else
	{
		// An external file can be used to store tiers, as follows:
		// <Tiers source="/path/to/textgrid" format="TextGrid" />
		writer.writeStartElement("Tiers");
		writer.writeAttribute("source", compressPath(m_path)); // the source is the external file, not the DMM stored in 'path'
		if (m_path.toLower().endsWith(".textgrid"))
			writer.writeAttribute("format", "TextGrid");
		else if (m_path.toLower().endsWith(".lab"))
			writer.writeAttribute("format", "WaveSurfer");
		writer.writeEndElement();
	}

	writer.writeEndDocument();
	file.close();

	if (file.error())
		m_error = QString("The file '%1' cannot be written: %2").arg(path, file.errorString());
}



void Annotation::writeToTxt(QString path)
{
	QFile file(path);

	if (! file.open(QFile::WriteOnly | QFile::Text))
	{
		m_error = tr("The file '%1' cannot be written: %2").arg(path, file.errorString());
		return;
	}

	QTextStream outfile(&file);

	foreach (Tier *tier, this->tiers())
	{
		outfile << "\n\nTier " << tier->label() << ":\n";
		foreach (Item *item, tier->items())
		{
			QString text = item->text();
			if (! text.endsWith(' '))
				text += " ";

			outfile << text;
		}
	}

	file.close();
}

void Annotation::writeToTextGrid(QString path)
{
	if (! this->canbeWrittenToTextGrid())
	{
		m_error = tr("The annotation '%1' cannot be written as a TextGrid").arg(path);
		return;
	}

	Tier *tier;
	Item *item;
	DSpan *sp;
	DPoint *pt;
	QFile file(path);
    QString s;

	if (! file.open(QFile::WriteOnly | QFile::Text))
	{
		m_error = tr("The TextGrid '%1' cannot be written: %2").arg(path, file.errorString());
		return;
	}

	QTextStream out(&file);
	out.setCodec("UTF-16");
	out.setGenerateByteOrderMark(true);

	// write TextGrid header
	out << "File type = \"ooTextFile\"\nObject class = \"TextGrid\"\n\n";
    s.sprintf("%.16f", this->endTime());
    out << QString("xmin = 0\nxmax = %1\n\n").arg(s);
	out << QString("tiers? <exists>\nsize = %1\nitem []:\n").arg(m_tiers.size());

	int n = 0;
	foreach (tier, m_tiers)
	{
		if (tier->countItems() == 0)
			continue;

		QString cls = isInstance(tier->item(0), DSpan)? "IntervalTier": "TextTier";

		// write tier header
		out << QString("\titem [%1]:\n").arg(++n);
		out << QString("\t\tclass = \"%1\"\n").arg(cls);
		out << QString("\t\tname = \"%1\"\n").arg(tier->label());
        s.sprintf("%.16f", tier->endTime());
        out << QString("\t\txmin = 0\n\t\txmax = %1\n").arg(s);

		bool spanTier = isInstance(tier->item(0), DSpan);
		if (spanTier)
			out << QString("\t\tintervals: size = %1\n").arg(tier->countItems());
		else
			out << QString("\t\tpoints: size = %1\n").arg(tier->countItems());

		int i = 0;
		foreach (item, tier->items())
		{
			if (spanTier)
			{
				sp = qobject_cast<DSpan*>(item);

				out << QString("\t\tintervals [%1]:\n").arg(++i);
                s.sprintf("%.16f", sp->start());
                out << QString("\t\t\txmin = %1\n").arg(s);
                s.sprintf("%.16f", sp->end());
                out << QString("\t\t\txmax = %1\n").arg(s);
				out << QString("\t\t\ttext = \"%1\"\n").arg(sp->text());
			}
			else
			{
				pt = qobject_cast<DPoint*>(item);

				out << QString("\t\tpoints [%1]:\n").arg(++i);
                QString s;
                s.sprintf("%.16f", pt->time());
                out << QString("\t\t\tnumber = %1\n").arg(s); // Praat changed from time to number
				out << QString("\t\t\tmark = \"%1\"\n").arg(pt->text());
			}
		}
	}

	file.close();
}

bool Annotation::canbeWrittenToTextGrid()
{
	Tier *tier;
	Item *item;
	bool spanTier;

	foreach(tier, m_tiers)
	{
		if (tier->countItems())
		{
			spanTier = isInstance(tier->item(0), DSpan);

			foreach (item, tier->items())
			{
				if (spanTier)
				{
					if (isInstance(item, DPoint))
						return false;
				}
				else
				{
					if (isInstance(item, DSpan))
						return false;
				}
			}
		}
	}

	return true;
}

double Annotation::endTime() const
{
	double tmp, end;
	tmp = end = 0.0;
	Tier *tier;

	foreach (tier, m_tiers)
	{
		if (tier->countItems())
			tmp = tier->endTime();

		if (tmp > end)
			end = tmp;
	}

	return end;
}


bool Annotation::isNative()
{
    return m_type == DM_ANNOTATION_NATIVE;
}

void Annotation::nativizePath()
{
	QFileInfo info(m_path);
	setPath(info.path() + QDir::separator() + info.baseName() + DM_DOT_FILE_EXT);
	// mark the file as native so that the tiers are written to the DMF file
    m_type = DM_ANNOTATION_NATIVE;
}


Sound* Annotation::soundFile() const
{
    return m_sound_ref;
}

void Annotation::setSoundFile(Sound *sound)
{
    m_sound_ref = sound;
    unsaved_changes = true;
    emit file_modified();
}


void Annotation::setSoundFile()
{
    if (buffer_ref != "" && m_sound_ref == NULL)
    {
		this->setSoundRef();
    }
	// try to find a sound file which matches the annotation but is not in the project
    else if (buffer_ref.isEmpty() && ! m_sound_ref)
	{
		// build base path of the sound file
		QFileInfo info = QFileInfo(m_path);
		QString basename = info.path();
		if (! basename.endsWith(QDir::separator()))
			basename += QDir::separator();
		basename = QDir::toNativeSeparators(basename);
		basename += info.completeBaseName();

		this->searchSoundFile(basename);

		if (!buffer_ref.isEmpty())
			this->setSoundRef();
	}
}


void Annotation::setSoundRef(QString ref)
{
    buffer_ref = ref;
}

void Annotation::setSoundRef()
{
	if (Global::FileHash.contains(buffer_ref))
        m_sound_ref = qobject_cast<Sound*>(Global::FileHash[buffer_ref]);
	else
        m_sound_ref = new Sound(buffer_ref);

	buffer_ref.clear();
}

void Annotation::copyMetadata(Annotation *a)
{
	this->setSoundFile(a->soundFile());
	this->setDescription(a->description());

	foreach (Property *tag, a->properties())
		this->addProperty(tag);
}

void Annotation::parsePfcPath()
{
	QFileInfo info(m_path);
	QString base = info.baseName();
    QRegularExpression regex("(.+)(...)([lgmt])(gx?)");
    auto match = regex.match(base);
	QString soundFile;

    if (match.hasMatch())
	{
		QString survey, speaker, task, supplement;

        survey = match.captured(1);
        speaker = survey + match.captured(2);
        task = match.captured(3);
        supplement = match.captured(4);

		// build base path of the sound file
		soundFile = info.path();
		if (! soundFile.endsWith(QDir::separator()))
			soundFile += QDir::separator();
		soundFile = QDir::toNativeSeparators(soundFile);

		soundFile += speaker + task + "w";

		if (task == "l")
			task = "Informal interview";
		else if (task == "g")
			task = "Formal interview";
		else if (task == "m")
			task = "Word list";
		else if (task == "t")
			task = "Read text";

		if (supplement == "gx")
		{
			task += " (supplementary)";
			soundFile += "x";
		}

		Property *t1 = new Property(survey.toUpper(), "Survey");
        this->addPropertySilently(t1);
		Property *t2 = new Property(speaker.toUpper(), "Speaker");
        this->addPropertySilently(t2);
		Property *t3 = new Property(task, "Task");
        this->addPropertySilently(t3);

		this->searchSoundFile(soundFile);
	}
}

void Annotation::parsePacPath()
{
	QFileInfo info(m_path);
	QString base = info.baseName();
    QRegularExpression regex("(.+)(...)([vcfit])(gx?)");
    auto match = regex.match(base);
	QString soundFile;

    if (match.hasMatch())
	{
		QString survey, speaker, task, supplement;

        survey = match.captured(1);
        speaker = survey + match.captured(2);
        task = match.captured(3);
        supplement = match.captured(4);

		// build base path of the sound file
		soundFile = info.path();
		if (! soundFile.endsWith(QDir::separator()))
			soundFile += QDir::separator();
		soundFile = QDir::toNativeSeparators(soundFile);

		soundFile += speaker + task + "w";

		if (task == "i")
			task = "Informal interview";
		else if (task == "f")
			task = "Formal interview";
		else if (task == "v")
			task = "Word list 1";
		else if (task == "c")
			task = "Word list 2";
		else if (task == "t")
			task = "Read text";

		if (supplement == "gx")
		{
			task += " (supplementary)";
			soundFile += "x";
		}

		Property *t1 = new Property(survey.toUpper(), "Survey");
        this->addPropertySilently(t1);
		Property *t2 = new Property(speaker.toUpper(), "Speaker");
        this->addPropertySilently(t2);
		Property *t3 = new Property(task, "Task");
        this->addPropertySilently(t3);

		this->searchSoundFile(soundFile);
	}
}

void Annotation::searchSoundFile(const QString &basename)
{
	QString ext;
	foreach(ext, Global::CommonAudioFormats)
	{
		QFileInfo snd(basename + "." + ext);
		if (snd.exists())
		{
			this->buffer_ref = nat(snd.absoluteFilePath());
            break;
		}
	}
}


QString Annotation::metadataAsHtml(bool withDescription) const
{
	QString p = m_path;
	if (p.startsWith(dm_home()))
		p.replace(dm_home(), "~");

	QString metadata = "<strong>File:</strong><br/>" + p + "<br/>";
	// process properties
	QStringList properties = QStringList();

	foreach (Property *tag, this->properties())
		properties << "\"" + tag->toString() + "\"";

	if (properties.size() != 0)
		metadata += tr("<strong>Properties</strong>:<br/> %1").arg(properties.join(" | ")) + "<br/>";

	if (this->soundFile() != NULL)
	{
		QString p = this->soundFile()->path();
		if (p.startsWith(dm_home()))
			p.replace(dm_home(), "~");

		metadata += "<strong> Sound file:</strong><br/>" + p;
	}

	if (withDescription)
	{
		QString desc = this->description();
		if (desc != "")
			metadata += "<br/><strong>Description:</strong><br/> " + desc;
	}

	return metadata;
}


Annotation::~Annotation()
{
	// TODO Auto-generated destructor stub
}

