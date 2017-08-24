#ifndef BOOKMARKEDITOR_H
#define BOOKMARKEDITOR_H

#ifdef __APPLE__
#include <boost/tr1/memory.hpp>
#else
#include <memory>
#endif

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

#include "lib/Bookmark.h"



#ifdef __APPLE__
typedef boost::shared_ptr<Bookmark> BookmarkPtr;
#else
typedef std::shared_ptr<Bookmark> BookmarkPtr;
#endif


class BookmarkEditor : public QDialog
{
    Q_OBJECT

public:
    BookmarkEditor(const BookmarkPtr &bm, QWidget *parent = 0);
    ~BookmarkEditor();

signals:
	void bookmarkAvailable(Bookmark *);

public slots:
	void accept();

private:
    QLineEdit   *titleLine;
    QTextEdit   *bookmarkNotes;
    QPushButton *okButton, *cancelButton;

    BookmarkPtr    m_bookmark;
};

#endif // BOOKMARKEDITOR_H
