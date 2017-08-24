#include "HtmlItemDelegate.h"


HtmlItemDelegate::HtmlItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

// Found on: http://www.qtforum.org/article/26942/suggestion-how-to-implement-a-calendar-weekly-view.html
void HtmlItemDelegate::paint (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	painter->save();
	if (option.state & QStyle::State_Selected)
		painter->fillRect(option.rect, option.palette.highlight());

	QTextDocument doc;
	doc.setUndoRedoEnabled(false);
	doc.setTextWidth(option.rect.width());
	doc.setHtml(index.data().toString());

	painter->translate(option.rect.topLeft());
	doc.drawContents(painter);
	painter->restore();
}

QSize HtmlItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QStyledItemDelegate::sizeHint(option, index);
	// FIXME: ad hoc padding that avoids overlapping of items.
	size.rheight() += 5;

	return size;
}
