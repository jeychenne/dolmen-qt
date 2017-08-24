#include "SqlDatabase.h"

SqlDatabase::SqlDatabase(QObject *parent, QString path) :
    QObject(parent)
{
    setObjectName("database");
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    m_status = m_db.open();
}

void SqlDatabase::close()
{
    m_db.close();
}

bool SqlDatabase::status() const
{
    return m_status;
}

void SqlDatabase::commit()
{
    m_db.commit();
}

QString SqlDatabase::error() const
{
    QSqlError e = m_db.lastError();

    if (e.isValid())
        return e.text();
    else
        return "";
}

QString getSoundRef(DFile *file)
{
    QString soundref;

    if (isInstance(file, Annotation))
    {
        Annotation *annot = qobject_cast<Annotation*>(file);
        if (annot->soundFile())
            soundref = annot->soundFile()->path();
    }

    return soundref;
}


///////////////////////////////////////////////////////////////////
MainDatabase::MainDatabase(QObject *parent, QString path) :
    SqlDatabase(parent, path)
{

}

MainDatabase::~MainDatabase()
{
    m_db.commit();
    m_db.close();
}

void MainDatabase::createMainTable()
{
    // Internal fields start with an underscore
    m_db.exec("CREATE TABLE files (_file text, _path text, _soundref text, _type text, _description text)");
    m_db.commit();
}

void MainDatabase::addFile(DFile *file)
{
    QString type = file->metaObject()->className();
    QFileInfo info(file->path());
    QString path = file->path();
    QString soundref = getSoundRef(file);

    // if the file is in the database, tag it
    if (this->hasFile(path))
    {
        setFileMetadata(file);
    }
    // otherwise add it to the database
    else
    {
        QStringList columns, values;

        columns << "_file" << "_path" << "_soundref" << "_type" << "_description";
        values << "\"" + info.fileName() + "\"" << "\"" + file->path() + "\""
               << "\"" + soundref + "\"" << "\"" + type + "\""
               << "\"" + file->description() + "\"";

        foreach (Property *p, file->properties())
        {
            QString cat = p->category();

            this->addCategory(cat);
            columns << "\""+ cat + "\"";
            values << "\"" + p->value() + "\"";
        }

        QString col_stmt = columns.join(", ");
        QString val_stmt = values.join(", ");

        QString query = QString("INSERT INTO files (%1) VALUES (%2)").arg(col_stmt, val_stmt);
        //qDebug() << "QUERY" << query;
        m_db.exec(query);
        //m_db.commit();
    }
}

void MainDatabase::saveFileMetadata(DFile *file)
{
    QString path = file->path();
//    QFileInfo info(path);

    if (this->hasFile(path))
    {
        QStringList columns, values;

        columns << "_description" << "_soundref";
        values  << file->description() << getSoundRef(file);

        foreach (Property *p, file->properties())
        {
            QString cat = p->category();
            this->addCategory(cat);

            columns << cat;
            values << p->value();
        }

        QString col, val;
        QStringList stmts;
        int i;
        for (i = 0; i < values.size(); i++)
        {
            col = columns.at(i);
            val = values.at(i);
            stmts << QString("\"%1\" = \"%2\"").arg(col, val);
        }

        QString updates = stmts.join(", ");
        QString query = QString("UPDATE files SET %1 where _path = \"%2\";").arg(updates, path);
        //qDebug() << "QUERY" << query;

        m_db.exec(query);
        m_db.commit();
    }

    else
        this->addFile(file);
}

void MainDatabase::setFileMetadata(DFile *file)
{
    QString s = QString("select * from files where _path = \"%1\";").arg(file->path());
    //qDebug() << "QUERY" << s;
    QSqlQuery query(s, m_db);
    query.exec();
    QSqlRecord record = query.record();

    // set properties and description
    if (query.first())
    {
        int count = record.count();
        for (int i = 0; i < count; ++i)
        {
            QString cat = record.fieldName(i);
            QString val = query.value(i).toString();

            if (cat == "_description")
            {
                file->setDescriptionSilently(val);
            }
            else if (cat == "_soundref" && (!val.isEmpty()) && isInstance(file, Annotation))
            {
                Annotation *annot = qobject_cast<Annotation*>(file);
                annot->setSoundRef(val);
            }
            else if (! cat.startsWith('_') && ! val.isEmpty())
            {
                file->addPropertySilently(new Property(val, cat));
            }
        }
    }
}


void MainDatabase::addCategory(QString cat)
{
    // silently ignore error if the column already exists
    QString s = QString("ALTER TABLE files ADD \"%1\" TEXT").arg(cat);
    QSqlQuery query(s, m_db);
    query.exec();
}

bool MainDatabase::hasCategory(QString cat) const
{
    // If a table contains properties but no files (it is empty),
    // this method will return false. This shouldn't be a problem in practice
    if (cat.startsWith('_'))
        return false;

    QString s = QString("select \"%1\" from files where rowid = 1").arg(cat);
    QSqlQuery query(s, m_db);
    query.exec();

    return query.next();
}

QString MainDatabase::getValue(QString path, QString cat) const
{
    QString s = QString("select \"%1\" from files where path = \"%2\"").arg(cat, path);
    QSqlQuery query(s, m_db);
    query.exec();

    if (query.next())
        return query.value(0).toString();
    else
        return NULL;
}


bool MainDatabase::hasFile(QString path) const
{
    QString s = QString("select rowid from files where _path = \"%1\"").arg(path);
    QSqlQuery query(s, m_db);
    query.exec();

    return query.next();
}


/*
QSqlQuery query;
query.prepare("INSERT INTO qtsql (id, name, description) VALUES (?, ?, ?)");

query.bindValue(0, 1);
query.bindValue(1, "Bart");
query.bindValue(2, "Fils");
query.exec();
*/
