#ifndef METADATAEDITOR_H
#define METADATAEDITOR_H

#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>

//#include "gui/PropertyEditor.h"
#include "lib/metadata.h"
#include "gui/PropertyTable.h"
#include "lib/DFile.h"
#include "lib/Annotation.h"
#include "lib/Sound.h"

using namespace metadata;

class MetadataEditor : public QDialog
{
	Q_OBJECT

public:
    MetadataEditor(QWidget *parent, DFile *file);
    MetadataEditor(QWidget *parent, QList<DFile*> files);

private slots:
	void accept();
	void addProperty();
    void removeProperty();
    void updateValues();
    void updateSelectedValue();
    void tagContentEdited();
    void selectRow(int);
    void checkPropertyIsRemovable();

private:
	QVBoxLayout			*layout;
    QPushButton			*addProperty_btn, *rmProperty_btn, *ok_btn;
    QList<DFile*>        m_files;
    PropertyTable       *property_table;
    QListWidget         *cat_list, *value_list;
    QLineEdit           *cat_line, *value_line;

	void setupUi();
    PropertySet getProperties();
};

#endif // METADATAEDITOR_H
