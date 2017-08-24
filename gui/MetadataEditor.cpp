#include "MetadataEditor.h"
#include "utils/routines.h"


MetadataEditor::MetadataEditor(QWidget *parent, DFile *file) :
	QDialog(parent)
{
    m_files << file;
    this->setupUi();
}


MetadataEditor::MetadataEditor(QWidget *parent, QList<DFile*> files) :
    QDialog(parent)
{
    m_files = files;
    this->setupUi();
}

void MetadataEditor::setupUi()
{
    setWindowTitle(tr("Property editor"));
    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("PropertyEditor"));
    resize(430, 500);

    layout = new QVBoxLayout;
    layout->setContentsMargins(15,20,15,10);


    ok_btn = new QPushButton("OK");
//	buttonBox = new QDialogButtonBox(this);
//	buttonBox->setOrientation(Qt::Horizontal);
//	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
	QHBoxLayout *btn_layout = new QHBoxLayout;
	btn_layout->addStretch();
    btn_layout->addWidget(ok_btn);

    addProperty_btn = new QPushButton(tr("Add"));
    addProperty_btn->setEnabled(false);
    rmProperty_btn  = new QPushButton(tr("Remove"));
    rmProperty_btn->setEnabled(false);

    property_table = new PropertyTable(this->getProperties());
    property_table->setMaximumHeight(200);
    property_table->horizontalHeader()->resizeSection(0, 150);

    QHBoxLayout *add_rm_layout = new QHBoxLayout;
    add_rm_layout->setContentsMargins(0,0,0,0);
    add_rm_layout->setSpacing(2);

    add_rm_layout->addWidget(addProperty_btn);
    add_rm_layout->addWidget(rmProperty_btn);
    add_rm_layout->addStretch();

    cat_line = new QLineEdit;
    value_line = new QLineEdit;

    cat_list = new QListWidget;
    int HEIGHT = 55;
    cat_list->setMinimumHeight(HEIGHT);
    cat_list->setMaximumHeight(HEIGHT);
    value_list = new QListWidget;
    value_list->setMinimumHeight(HEIGHT);
    value_list->setMaximumHeight(HEIGHT);
    QStringList categories = getAllCategories();
    cat_list->addItems(categories);

    int row = 0;
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(new QLabel(tr("Category:")), row, 0);
    grid->addWidget(cat_line, row++, 1);
    grid->addWidget(cat_list, row++, 1);
    grid->addWidget(new QLabel(tr("Value:")), row, 0);
    grid->addWidget(value_line, row++, 1);
    grid->addWidget(value_list, row++, 1);
    grid->addWidget(property_table, row++, 1);
    grid->addLayout(add_rm_layout, row, 1);

    layout->addLayout(grid);
    layout->addStretch();
    layout->addLayout(btn_layout);

    this->setLayout(layout);

    connect(cat_list, SIGNAL(clicked(QModelIndex)), this, SLOT(updateValues()));
    connect(value_list, SIGNAL(clicked(QModelIndex)), this, SLOT(updateSelectedValue()));
    connect(cat_line, SIGNAL(textChanged(QString)), this, SLOT(tagContentEdited()));
    //connect(cat_line, SIGNAL(textChanged(QString)), this, SLOT(checkPropertyIsRemovable()));
    connect(value_line, SIGNAL(textChanged(QString)), this, SLOT(tagContentEdited()));
    //connect(value_line, SIGNAL(textChanged(QString)), this, SLOT(checkPropertyIsRemovable()));
    connect(property_table, SIGNAL(cellClicked(int,int)), this, SLOT(selectRow(int)));
    connect(ok_btn, SIGNAL(clicked()), this, SLOT(accept()));
//	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(addProperty_btn, SIGNAL(clicked()), this, SLOT(addProperty()));
	connect(rmProperty_btn, SIGNAL(clicked()), this, SLOT(removeProperty()));
}

void MetadataEditor::updateValues()
{
    if (cat_list->count() > 0)
    {
        QString category = cat_list->currentItem()->text();
        cat_line->setText(category);
        cat_line->setFocus();
        cat_line->selectAll();

        QStringList values = getAllValues(category);
        value_list->clear();
        value_list->addItems(values);
        value_line->clear();
    }
}

void MetadataEditor::tagContentEdited()
{
    if (!cat_line->text().isEmpty() && !value_line->text().isEmpty())
    {
        addProperty_btn->setEnabled(true);
    }
}

void MetadataEditor::updateSelectedValue()
{
    if (value_list->count() > 0)
    {
        QString value = value_list->currentItem()->text();
        value_line->setText(value);
        value_line->setFocus();
        value_line->selectAll();
    }
}

void MetadataEditor::addProperty()
{
    QString category = cat_line->text();
    QString value = value_line->text();

    Property *tag = new Property(value, category);

    foreach (DFile *df, m_files)
        df->addProperty(tag);

    // refresh all properties: this avoids having duplicates if the value of a property has been
    // overwritten by another one.
    property_table->resetProperties(this->getProperties());

    cat_line->clear();
    value_line->clear();
    addProperty_btn->setEnabled(false);
    rmProperty_btn->setEnabled(false);
}

void MetadataEditor::removeProperty()
{
    int row = property_table->currentRow();

    QString cat = property_table->category(row);
    QString val = property_table->value(row);

    property_table->removeRow(row);
    Property tag(val, cat);

    foreach(DFile *df, m_files)
        df->removeProperty(&tag);

    rmProperty_btn->setEnabled(false);
}

PropertySet MetadataEditor::getProperties()
{
    PropertySet properties;

    foreach (DFile *df, m_files)
    {
        foreach (Property *p, df->properties())
        {
             bool found = false;

             foreach (Property *q, properties)
             {
                 if (p->equals(q))
                 {
                     found = true;
                     break;
                 }
             }

             if (! found)
                 properties << p;
        }
    }

    return properties;
}

void MetadataEditor::selectRow(int)
{
    rmProperty_btn->setEnabled(true);
}

void MetadataEditor::checkPropertyIsRemovable()
{
    if (!cat_line->text().isEmpty() && !value_line->text().isEmpty())
        rmProperty_btn->setEnabled(true);
}


void MetadataEditor::accept()
{
	QDialog::accept();
}
