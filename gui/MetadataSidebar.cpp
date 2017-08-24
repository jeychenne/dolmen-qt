/*
 * MetadataSideBar.cpp
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
 * Purpose: Right-hand side metadata widget
 * Created: 24/03/2012
 */

#include "MetadataSidebar.h"

#define META_SIDEBAR_SIZE 230

MetadataSideBar::MetadataSideBar(QWidget *parent) :
    QWidget(parent)
{
    file = nullptr;
    bookmark = nullptr;
    description_is_editable = false;

    this->setupUi();
}

void MetadataSideBar::setupUi()
{
    this->setNothingToDisplay();
}

void MetadataSideBar::displaySearchMatchInfo(const QString &fileInfo, const QString &propertiesInfo, const QString &startInfo, const QString &endInfo)
{
    setWidgets(true);
    soundRef_label->hide();

    file_label->setText(fileInfo);
    if (propertiesInfo.isEmpty())
        properties_label->hide();
    else
        properties_label->setText(propertiesInfo);
    start_label->setText(startInfo);
    end_label->setText(endInfo);
    properties_btn->hide();
}

void MetadataSideBar::displaySearchMatchInfo(const QString &fileInfo, const QString &propertiesInfo)
{
    setWidgets(false);
    soundRef_label->hide();

    file_label->setText(fileInfo);
    if (propertiesInfo.isEmpty())
        properties_label->hide();
    else
        properties_label->setText(propertiesInfo);
}

void MetadataSideBar::displayFileInfo(DFile *df)
{
    setWidgets(false);
    file = df;

    QFileInfo info(df->path());
    file_label->setText(tr("<b>File:</b><br/>") + info.fileName());
    file_label->setToolTip(df->path());

    if (isInstance(df, Annotation))
    {
        Annotation *annot = qobject_cast<Annotation*>(df);
        QString sndfile = tr("<b>Sound file:</b><br/>");
        if (annot->soundFile())
        {
            QFileInfo soundPath(annot->soundFile()->path());
            sndfile += soundPath.fileName() ;
            soundRef_label->setText(sndfile);
            soundRef_label->setToolTip(annot->soundFile()->path());
        }
        else
            soundRef_label->setText(sndfile + tr("None"));
    }
    else
        soundRef_label->hide();

    if (df->hasProperties())
    {
        QString properties = tr("<b>Properties:</b><br/>") + df->propertiesAsStrings().join("<br/>");
        properties_label->setText(properties);
    }

    desc_edit->setText(df->description());
    description_is_editable = true;
}

void MetadataSideBar::displayBookmarkInfo(IBrowserElement *elem)
{
    auto bm = dynamic_cast<Bookmark*>(elem);
    bookmark = bm;
    // for now, we simply hack it using already existing labels. TODO: add proper labels for bookmarks
    setWidgets(false);

    if (bm->type() == "SearchMatch")
    {
        SearchMatch *match = static_cast<SearchMatch*>(bm);

        QFileInfo info(match->file()->path());
        QString label = tr("<b>Bookmark:</b><br/>%1<br/><b>File:</b><br/>%2<br/>").arg(bm->title(), info.fileName());
        file_label->setText(label);
        QString matchText = tr("<b>Match:</b><br/>%3<br/>").arg(match->toString());
        properties_label->setText(matchText);
//        soundRef_label->hide();
//        properties_label->hide();
        properties_btn->hide();
        desc_edit->setText(bm->notes());
        //description_is_editable = false;
    }
}

void MetadataSideBar::setWidgets(bool showTimes)
{
    clearWidgets();

    QVBoxLayout *vl = new QVBoxLayout;

    file_label  = new QLabel;
    properties_label  = new QLabel;
    soundRef_label = new QLabel;
    properties_btn = new QPushButton(tr("Edit properties..."));
    desc_edit = new QTextEdit;
    description_is_editable = false;
    connect(desc_edit, SIGNAL(textChanged()), this, SLOT(setFileDescription()));
    vl->addWidget(file_label);
    vl->addWidget(soundRef_label);

    vl->addWidget(properties_label);
    properties_label->setWordWrap(true);
    connect(properties_btn, SIGNAL(clicked()), this, SLOT(editProperties()));

    if (showTimes)
    {
        start_label = new QLabel;
        end_label   = new QLabel;

        vl->addWidget(start_label);
        vl->addWidget(end_label);
    }
    else
    {
        start_label = NULL;
        end_label = NULL;
    }

    QHBoxLayout *hl2 = new QHBoxLayout;
    hl2->addWidget(properties_btn);
    hl2->addStretch();
        hl2->setSpacing(0);
    vl->addLayout(hl2);


    vl->addWidget(new QLabel(tr("<b>Description:<b>")));
    vl->addWidget(desc_edit);

    vl->addStretch();
    this->setLayout(vl);
}

void MetadataSideBar::setNothingToDisplay()
{

    QVBoxLayout *vl = new QVBoxLayout;

    QHBoxLayout *headerLayout = new QHBoxLayout;
    main_label = new QLabel(tr("<font color=\"grey\">No metadata to display</font>"));

    headerLayout->addStretch();
    headerLayout->addWidget(main_label);
    headerLayout->addStretch();

    vl->addStretch();
    vl->addLayout(headerLayout);
    vl->addStretch();

    //HERE!!!
    tabs = new QTabWidget(this);
    tabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    info_tab = new QWidget;
    auto main_layout = new QVBoxLayout;
    main_layout->addWidget(tabs);
    main_layout->setContentsMargins(0, 0, 5, 0);
    info_tab->setLayout(vl);
    tabs->addTab(info_tab, tr("Info"));
    this->setLayout(main_layout);
}

void MetadataSideBar::editProperties()
{
    MetadataEditor *editor = new MetadataEditor(this, file);
    editor->show();
}


void MetadataSideBar::setFileDescription()
{
    if (description_is_editable)
    {
        QString desc = desc_edit->toPlainText();

        if (file)
        {
            if (desc != file->description())
                file->setDescription(desc);
        }
        else if (bookmark)
        {
            if (desc != bookmark->notes()) {
                bookmark->setNotes(desc);
            }
        }
    }
}


void MetadataSideBar::clearWidgets()
{
    //FIXME: this crashes on Windows but is fine on mac
    // Maybe not needed...
#ifdef Q_OS_MAC
    if (file_label)
        file_label->hide();
#endif
    delete this->layout();
    foreach(QObject *o, this->children())
        delete o;

    main_label = file_label = soundRef_label = properties_label = start_label = end_label = NULL;
    file = NULL;
}






