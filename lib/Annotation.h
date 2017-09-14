/*
 * Annotation.h
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
 * Created: 01/09/10
 */


#ifndef ANNOTATION_H_
#define ANNOTATION_H_

#include <QtCore>
#include <QList>
#include <QTextStream>
#include <iostream>

#include "dolmen.h"
#include "lib/Graph.h"
#include "lib/DFile.h"
#include "lib/Sound.h"
#include "lib/Tier.h"
#include "lib/praat.h"
#include "utils/routines.h"
#include "utils/Global.h"
#include "search/SearchMatch.h"

// keep track of recognized file formats for annotations
typedef enum
{
    DM_ANNOTATION_NATIVE,
    DM_ANNOTATION_TEXTGRID,
    DM_ANNOTATION_WAVESURFER
} dm_annotation_type_t;



class Annotation : public DFile
{
	Q_OBJECT

public:
    Annotation(const QString &path, bool bogus = false);
	virtual ~Annotation();

    void open();

    Item* getItem(int tier_no, int item_no);
	Tier* tier(int no);
    Item* nextItem(int tier_no, Item *item);
    void addNewTier(const QString &label, tier_type_t t, int pos);
	QList<Tier*> tiers();
    dm_annotation_type_t type() const;
	int countTiers();
	// create a sub-tree
    std::unique_ptr<GraphNode> graphNode(Item *root, int targetTier) const;
    std::unique_ptr<GraphNode> graphNode(Item *root, QString tierName) const;

    // Get the concatenated text of a sequence of items.
    QString getTextSpan(int tier_no, double start, double end, const QString &separator) const;

	bool isNative();
	// ensure the whole file is written as DMF
	void nativizePath();	
	// native files
	void writeToDmf(QString path = "");
	void writeToTxt(QString path);
	void writeToTextGrid(QString path);
	void readFromDmf();

	Sound* soundFile() const;
	void setSoundFile(Sound *sound);
	void setSoundFile(); // overloaded method: called after all files have been loaded in the project
	void copyMetadata(Annotation *);
    void setSoundRef(QString ref);

	void save();
	QString metadataAsHtml(bool withDescription = true) const;

	// get match in context
    QString leftCotext(int tier, int item, int pos, const QString &separator); // pos == start of match
    QString rightCotext(int tier, int item, int pos, const QString &separator); // pos == end of match

	// Additional properties based on the file name for the PFC and PAC projects.
    void parsePfcPath();
	void parsePacPath();

private:
    QList<Tier*> m_tiers;
    Sound *m_sound_ref; // sound file which the annotation is bound to
    // when a project is opened, the annotation might be loaded before the sound
    // so binding is delayed to the end.
    QString buffer_ref;
    dm_annotation_type_t m_type;

    void parseFromTextgrid();
    void parseFromLab();

	// parse DMF file
    void readXmlTiers(QXmlStreamReader *);
    void readXmlTier(QXmlStreamReader *);
    void readXmlItem(QXmlStreamReader *, Tier *);

	// look in valid extensions if a file matching the annotation exists
    void searchSoundFile(const QString &basename);
    double endTime() const;
    bool canbeWrittenToTextGrid();

    void setSoundRef();
    void load(); // load content from physical file
};

#endif /* ANNOTATION_H_ */
