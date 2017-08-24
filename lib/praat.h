/*
 * praat.h
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


#ifndef PRAAT_H_
#define PRAAT_H_

#include <iostream>
#include <QProcess>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QTextStream>
#include <QRegularExpression>
#include "extra/sendpraat.h"

#include <QDebug>


// class to communicate/run Praat
class PraatInstance : public QObject
{
	Q_OBJECT

public:
    PraatInstance();

public slots:
    void openInterval(int tier, double xmin, const QString &textgrid, const QString &sound);
	void openTextGrid(const QString &textgrid, const QString &sound = QString());
	void openSound(const QString &sound);
	void playPart(const QString &sound, double start, double end);
	bool runTemporaryScript(const QString &content);

signals:
	void praat_error(QString);

private slots:
	void checkProcessExitStatus(int);

private:
	QProcess *process;
};

namespace praat
{
	bool readTierHeader(QTextStream *stream, QString *line, QString *name);
	bool readInterval(QTextStream *stream, QString *line, double *start, double *end, QString *text);
	bool readPoint(QTextStream *stream, QString *line, double *time, QString *text);
}





#endif /* PRAAT_H_ */
