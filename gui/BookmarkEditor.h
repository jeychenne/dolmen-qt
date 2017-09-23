#ifndef BOOKMARKEDITOR_H
#define BOOKMARKEDITOR_H


#include <memory>
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

#include "lib/Bookmark.h"


typedef std::shared_ptr<Bookmark> BookmarkPtr;



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
