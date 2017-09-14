#include "Document.h"

Document::Document(const QString &path) : DFile()
{
    m_path = path;
}

void Document::readFromDisk()
{
    QFile file(m_path);

    if (! file.open(QFile::ReadOnly | QFile::Text))
    {
        QString msg = QString("Cannot read file %1").arg(m_path);
        emit file_error(msg); return;
    }

    QTextStream text(&file);
    m_text = text.readAll();

    file.close();
}

QString Document::text() const
{
    return m_text;
}

QString Document::leftCotext(int pos, const QString &separator) const
{
    QString cotext;

    if (pos < Global::MatchCotextLength - 1)
    {
        cotext = m_text.left(pos);
        cotext = cotext.rightJustified(Global::MatchCotextLength);
    }
    else
        cotext = m_text.mid(pos - Global::MatchCotextLength, Global::MatchCotextLength);

    cotext.replace('\n', separator);

    return cotext;
}

QString Document::rightCotext(int pos, const QString &separator) const
{
    QString cotext;

    if (pos + Global::MatchCotextLength > m_text.size() - 1)
    {
        int new_size = m_text.size() - 1 - pos;
        cotext = m_text.right(new_size);
        cotext = cotext.leftJustified(Global::MatchCotextLength);
    }
    else
        cotext = m_text.mid(pos, Global::MatchCotextLength);

    cotext.replace('\n', separator);

    return cotext;
}

void Document::save()
{

}

void Document::open()
{
    if (m_open) { return; }

    if (this->pathIsValid()) {
        readFromDisk();
    }
}

QString Document::metadataAsHtml(bool with_description) const
{
    Q_UNUSED(with_description)
    return "";
}
