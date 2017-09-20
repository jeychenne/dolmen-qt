#include "QueryView.h"

QueryView::QueryView(QTabWidget *parent, QWidget *metaWidget, PraatInstance *praat, Query *query, QString title) :
		View(parent)
{
	this->query = query;
    this->matches = QList<SearchMatchPtr>();
	this->praat = praat;
    this->wgt_meta = metaWidget;
    current_sound = NULL;
    box_hide_consistent_codings = NULL;

	int index = parent->indexOf(this);
	if (title == "")
        id_string = tr("Query %1").arg(genGuiQueryId());
	else
        id_string = title;

    parent->setTabText(index, id_string);
	this->setupUi();

	if (Global::PraatIsInstalled)
	{
        action_praat = new QAction(tr("Open selection in Praat"), this);
#ifndef Q_OS_MAC
        action_praat->setIcon(QIcon(":/icons/praat.png"));
#endif
        connect(action_praat, SIGNAL(triggered()), this, SLOT(openCurrentMatchInPraat()));
	}

	//FOCUS to handle 'Esc'
	setFocusPolicy(Qt::StrongFocus);
	setFocus(Qt::OtherFocusReason);
}

void QueryView::setupUi()
{
    int count_mismatches = 0;
    QVBoxLayout *resLayout = new QVBoxLayout;

    list_results = new QListWidget();
    list_results->setItemDelegate(new HtmlItemDelegate);
    list_results->setFocusPolicy(Qt::NoFocus);
    list_results->setFocusProxy(this);

    QHBoxLayout *header_layout = new QHBoxLayout;
    QLabel *match_label = new QLabel;
    header_layout->addWidget(match_label);

    QStringList labels = getDisplayLines(&count_mismatches);
    list_results->addItems(labels);
    list_results->setContextMenuPolicy(Qt::CustomContextMenu);

    if (count_mismatches > 0)
    {
        QString label = tr("Hide consistent codings");
        label.prepend("<strong>"); label.append("</strong>");

        box_hide_consistent_codings = new QCheckBox;
        header_layout->addWidget(box_hide_consistent_codings);
        header_layout->addWidget(new QLabel(label));
        connect(box_hide_consistent_codings, SIGNAL(toggled(bool)), this, SLOT(hideConsistentCodings(bool)));
    }
    header_layout->addStretch();
    resLayout->addLayout(header_layout);
    resLayout->addWidget(list_results);
    resLayout->setContentsMargins(0,11,0,0);

    match_label->setText(getMatchLabel(count_mismatches));

    layout->addLayout(resLayout);

    connect(list_results, SIGNAL(customContextMenuRequested(const QPoint &)),
            SLOT(displayMatchContextMenu(const QPoint &)));
    connect(list_results, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(onItemClicked()));
    connect(list_results, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(playCurrentItem()));
}

void QueryView::hideConsistentCodings(bool discard)
{
    int count_mismatches = 0;

    // crashes if not disconnected because of connect(resultList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this, SLOT(onItemClicked()));
    disconnect(list_results, 0, 0, 0);
    list_results->clear();
    // reconnect
    connect(list_results, SIGNAL(customContextMenuRequested(const QPoint &)),
            SLOT(displayMatchContextMenu(const QPoint &)));
    connect(list_results, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(onItemClicked()));
    connect(list_results, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(playCurrentItem()));

    matches.clear();

    QStringList labels = getDisplayLines(&count_mismatches, discard);
    list_results->addItems(labels);
}

QStringList QueryView::getDisplayLines(int *count_mismatches, bool discardCorrect)
{
    QStringList labels = QStringList();
    Grammar *grammar = query->grammar();

    for (const SearchMatchPtr &match : query->results())
    {
        if (!discardCorrect || match->hasDivergingAnnotations(grammar))
        {
            this->matches << match;
            labels << match->toHtml();
        }
        // Annotator comparison
        if (match->hasDivergingAnnotations(grammar))
        {
            ++(*count_mismatches);

            for (const SearchMatchPtr &m : match->siblings())
            {
                labels << m->toHtmlWithError();
                this->matches << m;
            }
        }
    }

    return labels;
}

QString QueryView::getMatchLabel(int count_mismatches) const
{
    QString txt, percent;
    int size = query->resultCount();
    if (matches.size() < 2)
        txt = tr("  %1 match").arg(size);
    else
        txt = tr("  %1 matches").arg(size);

    if (count_mismatches != 0)
    {
		percent.sprintf("%.2f", (double)count_mismatches * 100 / size);
        txt += tr("&nbsp;&nbsp;&nbsp;&nbsp;Annotator disagreement: %1 codings (%2\%)").arg(count_mismatches).arg(percent);
    }
    txt.prepend("<strong>");
    txt.append("</strong>");

    return txt;
}

void QueryView::displayMatchContextMenu(const QPoint &pos)
{
	ContextMenu menu(this);
    SearchMatchPtr match = getSelectedMatch();
    bool fileIsAnnotation = isInstance(match->file(), Annotation);
    bool fileIsDocument = isInstance(match->file(), Document);

    if (fileIsAnnotation)
    {
        QAction *playAction = new QAction(tr("Play selection"), this);
    #ifndef Q_OS_MAC
        playAction->setIcon(QIcon(":/icons/play.png"));
    #endif
        menu.addAction(playAction);
        menu.addSeparator();

        QAction *openAction = new QAction(tr("Open in annotation"), this);
        connect(openAction, SIGNAL(triggered()), this, SLOT(openCurrentMatch()));
        menu.addAction(openAction);


        if (Global::PraatIsInstalled)
            menu.addAction(action_praat);
        menu.addSeparator();


        QAction *editAction = new QAction(tr("Edit item text..."), this);
        menu.addAction(editAction);
        menu.addSeparator();

        connect(playAction, SIGNAL(triggered()), this, SLOT(playCurrentItem()));
        connect(editAction, SIGNAL(triggered()), this, SLOT(editItem()));
    }
    else if (fileIsDocument)
    {

    }

    if (fileIsAnnotation || fileIsDocument)
    {

        QAction *tableAction = new QAction(tr("Create table view"), this);
        menu.addAction(tableAction);

        QAction *csvAction = new QAction(tr("Export results to tab-separated file (CSV)..."), this);
        menu.addAction(csvAction);

//        QAction *writeOutputAction = new QAction(tr("Copy all matches to output tab"), this);
//        menu.addAction(writeOutputAction);
        menu.addSeparator();

        QAction *bookmarkAction = new QAction(tr("Bookmark search result..."), this);
    #ifndef Q_OS_MAC
        bookmarkAction->setIcon(QIcon(":/icons/16x16/bookmark.png"));
    #endif
        menu.addAction(bookmarkAction);

        //connect(writeOutputAction, SIGNAL(triggered()), this, SLOT(writeMatchesToOutput()));
        connect(csvAction, SIGNAL(triggered()), this, SLOT(exportMatchesToCsv()));
        connect(tableAction, SIGNAL(triggered()), this, SLOT(createDataTable()));
        connect(bookmarkAction, SIGNAL(triggered()), this, SLOT(createBookmark()));
    }

	menu.display(pos);
}


//The user can press 'space' to play the current match and 'Esc' to stop it
void QueryView::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Space)
	{
        if (list_results->selectedItems().size() > 0)
			playCurrentItem();
		event->accept();
	}
	else if (event->key() == Qt::Key_Escape)
	{
        if (current_sound)
            current_sound->stop();
		event->accept();
	}

	else
		event->ignore();
}

// move up and down to select a match item
void QueryView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up)
        selectPreviousItem();

    else if (event->key() == Qt::Key_Down)
        selectNextItem();

    else
        event->ignore();
}

void QueryView::selectPreviousItem()
{
    int row = list_results->currentIndex().row() - 1;

    if (row >= 0 && row < list_results->count())
    {
        list_results->setCurrentRow(row);
        onItemClicked();
    }
}

void QueryView::selectNextItem()
{
    int row = list_results->currentIndex().row() + 1;

    if (row >= 0 && row < list_results->count())
    {
        list_results->setCurrentRow(row);
        onItemClicked();
    }
}

void QueryView::onItemClicked()
{
    QString file_txt, properties_txt, start_txt, end_txt;
    SearchMatchPtr match = this->getSelectedMatch();

    if (! match) return;

	QFileInfo info(match->file()->path());
    file_txt = "<strong>File:</strong><br/>" + info.fileName();

    if (match->file()->hasProperties())
    {
        QMultiHash<QString, QString> hash = hashProperties(match->file()->properties());
        properties_txt = "<strong>Properties:</strong><br/>";

        foreach (QString cat, hash.uniqueKeys())
        {
            foreach (QString label, hash.values(cat))
                properties_txt += QString("%1<br/>\n").arg(str2tag(cat, label));

        }
    }
    else properties_txt = "";

    if (match->lastItem() && isInstance(match->lastItem(), DSpan))
	{
		DSpan *span = qobject_cast<DSpan*>(match->lastItem());
        start_txt = "<strong>Start: </strong>" + dm_time2string(span->start());
        end_txt   = "<strong>End: </strong>" + dm_time2string(span->end());
	}
    else if (match->lastItem() && isInstance(match->lastItem(), DPoint))
	{
		DPoint *point = qobject_cast<DPoint*>(match->lastItem());

        start_txt = "<strong>Time: </strong>" + dm_time2string(point->time());
        end_txt   = "";
	}

    if (match->lastItem())
        emit itemMatchMetadata(file_txt, properties_txt, start_txt, end_txt);
    else
        emit textMatchMetadata(file_txt, properties_txt);
}

void QueryView::createBookmark()
{
	BookmarkEditor *editor = new BookmarkEditor(this->getSelectedMatch());
	connect(editor, SIGNAL(bookmarkAvailable(Bookmark*)), this, SIGNAL(forwardBookmark(Bookmark*)));
	editor->show();
}

void QueryView::editItem()
{
    SearchMatchPtr match = this->getSelectedMatch();
	bool ok;
	QString text = QInputDialog::getText(this, tr("Edit item"),
										  tr("New text"), QLineEdit::Normal,
										  match->lastItem()->text(), &ok);

	if (ok)
	{
		match->lastItem()->setText(text);
		match->file()->modifiedExternally();
	}
}

void QueryView::playCurrentItem()
{
    qDebug("\nQueryView::playCurrentItem()");
    SearchMatchPtr match = this->getSelectedMatch();

	if (match && isInstance(match->file(), Annotation))
    {
        playMatch(match.get());
    }
	else qDebug("Can't play match");
}

bool QueryView::isQuery(Query *query)
{
	return this->query == query;
}

void QueryView::playMatch(SearchMatch *match)
{
    if (current_sound && current_sound->isPlaying())
    {
        current_sound->stop();
        current_sound = NULL;
    }

    Annotation *annot = qobject_cast<Annotation*>(match->file());
    current_sound = annot->soundFile();

    if (current_sound)
	{
		Item *item =  match->lastItem();
		double start = item->left();
		double end = item->right();

		// Point
		if (start == end)
		{
            start -= 1.0;
            end += 1.0;
		}
        qDebug() << "playing sound with " << start << " " << end;
        current_sound->play(start, end);
	}
}

void QueryView::openCurrentMatchInPraat()
{
    SearchMatchPtr match = this->getSelectedMatch();
	Annotation *annot = qobject_cast<Annotation*>(match->file());
	QString soundPath = "";

	if (annot->soundFile() != NULL)
		soundPath = annot->soundFile()->path();

	if (praat)
        praat->openInterval(match->tier(), match->lastItem()->left(), annot->path(), soundPath);
    qDebug() << "ITEM" << match->lastItem()->left() << match->lastItem()->right() << match->lastItem()->text();
}

void QueryView::openCurrentMatch()
{
    SearchMatchPtr match = this->getSelectedMatch();
	Annotation *annot = qobject_cast<Annotation*>(match->file());

	if (annot->soundFile())
		emit openMatch(annot, match->tier0(), match->lastItem()->left(), match->lastItem()->right());

}

SearchMatchPtr QueryView::getSelectedMatch()
{
    return this->matches[list_results->currentIndex().row()];
}


void QueryView::writeMatchesToOutput()
{
	QStringList texts;

    for (int i = 0; i < list_results->count(); ++i)
        texts << list_results->item(i)->text();

	emit output(texts.join("<br />"));
}

QString QueryView::id() const
{
    return id_string;
}

void QueryView::leave()
{
    if (current_sound && current_sound->isPlaying())
        current_sound->stop();
}

QStringList QueryView::getCategories()
{
	QSet<QString> categories = QSet<QString>();

	foreach (Property *tag, Property::properties.values())
		categories.insert(tag->category());

	QStringList lst = QStringList::fromSet(categories);
	lst.sort();

	return lst;
}


void QueryView::exportMatchesToCsv() // [SLOT]
{
	QString path = nat(QFileDialog::getSaveFileName(this->parentWidget(), tr("Save project...")));

	if (path == "")
		return;

	if (! path.toLower().endsWith(".csv"))
		path += ".csv";

	this->writeMatchesToCsv(path);
}

void QueryView::createDataTable()
{
    DataTable  *table  = new DataTable(this->matchesToTable());
    QTabWidget *parent = tabWidget();
    TableView  *view   = new TableView(parent, table);
    parent->setCurrentWidget(view);

    // since the table is created here, we forward the signal from the table view to the application
    // when the table is saved to disk in the table view.
    connect(view, SIGNAL(dataTableCreated(DataTable*)), this, SIGNAL(forwardDataTable(DataTable*)));
}

QList<QStringList> QueryView::matchesToTable(bool toCsv)
{
    Grammar *grammar = query->grammar();
    SearchMatchPtr match = getSelectedMatch();
    bool fileIsAnnotation = isInstance(match->file(), Annotation);
    QString quote = toCsv? "\"": "";

    // if the table if for a DataTable, we pass types in the first row
    QStringList types;

	// Create an in-memory table representing the matches and their metadata
	QList<QStringList> table;
	QStringList row, values;
	QStringList tagList, categories = this->getCategories();
	QString currentFile = "";
	QString latestFile = "";

    row << quote + "File" + quote;
    types << "text";

    if (fileIsAnnotation)
    {
        row << quote + "Item Start" + quote << quote + "Item End" + quote;
        types << "real" << "real";
    }

    if (grammar)
    {
        row << quote + "Left Context" + quote;
        types << "text";

        foreach (SearchField field, grammar->fields())
        {
            row << quote + field.name + quote;
            types << "text";
        }
        row << quote + "Right Context" + quote;
        types << "text";
		row << quote + "Match" + quote;
		types << "text";
    }
    else
    {
        row << quote + "Left Context" + quote << quote + "Match" + quote << quote + "Right Context" + quote;
        types << "text" << "text" << "text";
    }

	// Add one column per category
	foreach (QString c, categories)
	{
		if (c.isEmpty())
            row << quote + DM_DEFAULT_CAT + quote;
		else
            row << quote + c + quote;

        types << "text";
	}
    if (!toCsv)
        table << types;
	table << row;
	row.clear();

    foreach (const SearchMatchPtr &m, matches)
	{
		currentFile = QFileInfo(m->file()->path()).fileName(); // assume there are no 2 identical files in different folders
        row << quote + currentFile + quote;

        if (fileIsAnnotation)
        {
            row	<< quote + QString::number(m->lastItem()->left()) + quote
                << quote + QString::number(m->lastItem()->right()) + quote;
        }

        // write match with its context
        row	<< quote + m->left() + quote;

        if (grammar)
        {
            QStringList fields = grammar->matchFields(m->match());

            foreach (QString field, fields)
			{
                row << quote + field + quote;
			}
        }
        else
            row << quote + m->match() + quote;

        row << quote + m->right() + quote;

		if (grammar) { row << quote + m->match() + quote; }

        // write tags
		if (currentFile == latestFile)
		{
			QString t;
			foreach (t, tagList)
				row << t;
		}
		else
		{
			tagList.clear(); values.clear();
			QString c;
			foreach (c, categories)
			{
				Property *tag;
				foreach (tag, m->file()->properties())
				{
					if (tag->category() == c)
						values << tag->value();
				}
                QString cell = quote + values.join(" | ") + quote;
				row << cell;
				tagList << cell;

                latestFile = currentFile;
				values.clear();
			}
		}
		table.append(row);
		row.clear();
	}

	return table;
}

// This is partly redundant with DataTable::writeToCsv() but we use matchesToTable()
// to create a table from a query view.
void QueryView::writeMatchesToCsv(const QString &path, const QString &sep)
{
	QFile file(path);

	if (! file.open(QFile::WriteOnly | QFile::Text))
	{

		QString msg = tr("The file '%1' cannot be written: %2").arg(path, file.errorString());
		emit error(msg); return;
	}

	QTextStream outfile(&file);
    outfile.setCodec("UTF-8");

    foreach (QStringList row, this->matchesToTable(true))
		outfile << row.join(sep) << "\n";

	file.close();
}

