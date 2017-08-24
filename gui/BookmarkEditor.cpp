#include "BookmarkEditor.h"

BookmarkEditor::BookmarkEditor(const BookmarkPtr &bm, QWidget *parent)
    : QDialog(parent), m_bookmark(bm)
{
    QLabel *nameLabel = new QLabel(tr("Title:"));
    titleLine = new QLineEdit;

    QLabel *contentLabel = new QLabel(tr("Notes:"));
    bookmarkNotes = new QTextEdit;

    okButton = new QPushButton(tr("&OK"));
    cancelButton = new QPushButton(tr("&Cancel"));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(titleLine);
    mainLayout->addWidget(contentLabel);
    mainLayout->addWidget(bookmarkNotes);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setWindowTitle(tr("Bookmark Editor"));

	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void BookmarkEditor::accept()
{
	QString title = titleLine->text();

	if (!title.isEmpty())
	{
		m_bookmark->setTitle(title);
		m_bookmark->setNotes(bookmarkNotes->toPlainText());
        emit bookmarkAvailable(m_bookmark.get());
		QDialog::accept();
	}
}

BookmarkEditor::~BookmarkEditor()
{

}
