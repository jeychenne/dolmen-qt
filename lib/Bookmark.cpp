#include <QMessageBox>
#include "Bookmark.h"


Bookmark::Bookmark()
{

}

void Bookmark::openInPraat()
{
    QMessageBox msg;
    msg.setText("This bookmark cannot be opened in Praat");
    msg.setIcon(QMessageBox::Warning);
    msg.exec();
}

QString Bookmark::notes() const
{
	return m_notes;
}

void Bookmark::setNotes(QString s)
{
    //TODO: this can be updated from the metadata panel, so we need to let the project know about it.
	m_notes = s;
}

QString Bookmark::title() const
{
	return m_title;
}

void Bookmark::setTitle(QString s)
{
	m_title = s;
}

