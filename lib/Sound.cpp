/*
 * Sound.cpp
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

#include "Sound.h"




Sound::Sound(const QString &path) : DFile()
{
	m_path = path;
	m_sound_file = NULL; // the sound file is only loaded if needed
	m_player = NULL;

	QString ext = QFileInfo(path).suffix().toLower();
	if (ext == DM_METADATA_EXT)
		readFromDmm();
}

void Sound::setFileHandle()
{
	if (m_sound_file == NULL)
	{
		QFileInfo info(m_path);

		if (info.exists())
#ifdef Q_OS_WIN
            m_sound_file = new SndfileHandle(m_path.toStdString().c_str());
#else // FIXME: On Mac and Linux, the above crashes if there's a non-ASCII character in the path.
            m_sound_file = new SndfileHandle(m_path.toUtf8().data());
#endif
        // TODO: add code to check that the file handle is in a consistent state...
		else
			emit file_error(tr("Sound file \"%1\" does not exist").arg(m_path));
	}
}

void Sound::readFromDmm()
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
				readXmlHeader(&reader);

			else if (reader.name() == "SoundStream")
			{
				m_metadata_path = m_path;
				m_path = interpolatePath(reader.readElementText());
			}
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

void Sound::save()
{
	if (hasUnsavedChanges())
	{
		qDebug() << "Writing sound metadata";

		if (m_metadata_path == "")
			m_metadata_path = getExternalMetadataPath();

		writeToDmf(m_metadata_path);
        unsaved_changes = false;
	}
}

void Sound::open()
{

}

void Sound::writeToTxt(QString path)
{
    Q_UNUSED(path);
}

void Sound::writeMetadata(QString path)
{
	qDebug() << "Writing metadata to sound:" << path;
	if (path.isEmpty())
		path = m_path;

	SndfileHandle *fh = new SndfileHandle(path.toStdString().c_str(), SFM_RDWR);
	fh->setString(SF_STR_COMMENT, "<Dolmen>this is a test</Dolmen>");
	delete fh;
}

void Sound::writeToDmf(QString path)
{
	if (path == "")
		path = m_path;

	QFile file(path);

	if (! file.open(QFile::WriteOnly | QFile::Text))
	{
		m_error = tr("The file '%1' cannot be written: %2").arg(path, file.errorString());
		return;
	}

	QXmlStreamWriter writer(&file);

	writer.setAutoFormatting(true);
	writer.writeStartDocument();
	writer.writeStartElement("DolmenFile");

	writer.writeStartElement("Header");
	writer.writeStartElement("Class");
	writer.writeCharacters(QString(CLASSNAME(this)));
	writer.writeEndElement();
    writer.writeStartElement("Properties");
    foreach(Property *tag, m_properties)
	{
		tag->writeToXml(&writer);
	}
    writer.writeEndElement(); // </Properties>
	writer.writeEndElement(); // </Header>

	// link to the physical sound file
	writer.writeStartElement("SoundStream");
	writer.writeCharacters(compressPath(m_path));
	writer.writeEndElement(); // </SoundStream>

	writer.writeEndDocument();
	file.close();

	if (file.error())
		m_error = tr("The file '%1' cannot be written: %2").arg(path, file.errorString());

	//this->writeMetadata(m_path);
}

QString Sound::metadataAsHtml(bool withDescription) const
{
	QString p = m_path;
	if (p.startsWith(dm_home()))
		p.replace(dm_home(), "~");

	QString metadata = "<strong>File:</strong><br/>" + p + "<br/>";
    // process properties
	QStringList properties = QStringList();

    foreach (Property *property, this->properties())
        properties << "\"" + property->toString() + "\"";

    if (properties.size() != 0)
        metadata += tr("<strong>Properties</strong>:<br/> %1").arg(properties.join(" | ")) + "<br/>";

	if (withDescription)
	{
		QString desc = this->description();
		if (desc != "")
			metadata += "<br/><strong>Description:</strong><br/> " + desc;
	}

	return metadata;
}

void Sound::play(double start, double end)
{
#ifdef Q_OS_WIN
//    qDebug() << "Playing: " << start << " to " << end ;
#endif
	this->setFileHandle();
	this->stop();
	this->initializeCallbackData(start, end);

	m_player = new SoundPlayer(this, m_callback_data);
	// notify callers
	connect(m_player, SIGNAL(finished()), this, SIGNAL(finished()));
	connect(m_player, SIGNAL(currentTime(double)), this, SIGNAL(currentTime(double)));
	// make sure thread is deleted if it ends without being interrupted
	connect(this, SIGNAL(finished()), this, SLOT(cleanupThread()));

    m_player->start();
}

void Sound::pause()
{
	if (m_player)
	{
		if (m_player->isPaused())
			m_player->resume();
		else
			m_player->pause();
	}
}

bool Sound::isPlaying() const
{
	if (m_player && m_player->isRunning())
		return true;

	return false;
}

bool Sound::isPaused() const
{
	if (m_player && m_player->isRunning() && m_player->isPaused())
		return true;

	return false;
}

int Sound::nchannels()
{
	this->setFileHandle();
	return m_sound_file->channels();
}

double Sound::length()
{
	this->setFileHandle();
	return (double) m_sound_file->frames() / (double) m_sound_file->samplerate();
}

void Sound::stop()
{
//	qDebug("Sound::stop()");
	if (m_player && m_player->isRunning())
		m_player->interrupt();

	this->cleanupThread();
}

void Sound::initializeCallbackData(double start, double end)
{
	int startf = time2frame(start);
	int endf = time2frame(end);
	m_callback_data = new CallbackData(m_sound_file, startf, endf);
}

double Sound::frame2time(size_t f) const
{
	//this->setFileHandle();

	return (double)f / (double)(m_sound_file->samplerate());
}

int Sound::samplerate() const
{
	return m_sound_file->samplerate();
}

size_t Sound::nframes() const
{
    return m_sound_file->frames();
}


size_t Sound::time2frame(double t) const
{
	//this->setFileHandle();

	size_t frame = (size_t) floor(t * m_sound_file->samplerate());

	return MIN(frame, (size_t)(m_sound_file->frames()));
}


FrameVector Sound::rawFramesMono()
{	
	this->setFileHandle();
	const int nbuffers = m_sound_file->frames() / BUFFER_SIZE + 1;
	emit startLoadingData(nbuffers);

	FrameVector samples;
	samples.resize(m_sound_file->frames() * m_sound_file->channels());
	dm_sample_t *ptr = samples.data();

	m_sound_file->seek(0, SEEK_SET);

	int progress, len;
	for (progress = 1; progress <= nbuffers; ++progress)
	{
		len = m_sound_file->readf(ptr, BUFFER_SIZE);
		ptr += len * m_sound_file->channels();
		emit bufferLoaded(progress);
	}
	m_sound_file->seek(0, SEEK_SET);

	return samples;
}

QPair<FrameVector,FrameVector> Sound::rawFramesStereo()
{
	FrameVector left, right, mono;
	QPair<FrameVector,FrameVector> channels;
	int i;

	mono = this->rawFramesMono();
	left.resize(mono.size() / 2);
	right.resize(mono.size() / 2);

	dm_sample_t *left_ptr = left.data();
	dm_sample_t *right_ptr = right.data();

	for (i = 0; i < mono.size(); ++i)
	{
		if (i % 2 == 0)
			*left_ptr++ = mono.at(i);
		else
			*right_ptr++ = mono.at(i);
	}

	mono.clear();
	channels.first = left;
	channels.second = right;

	return channels;
}

bool Sound::isMono()
{
	this->setFileHandle();
	return m_sound_file->channels() == 1;
}

bool Sound::isStereo()
{
	this->setFileHandle();
	return m_sound_file->channels() == 2;
}

void Sound::cleanupThread()
{
	if (m_player)
	{
		while (m_player->isRunning())
			m_player->wait();

		m_player->quit();
		delete m_player;
		m_player = NULL;

		if (m_callback_data)
		{
            delete m_callback_data;
			m_callback_data = NULL;
		}
	}
}


Sound::~Sound()
{
	if (m_sound_file)
	{
		delete m_sound_file;
		m_sound_file = NULL;
	}
}
