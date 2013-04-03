/* 
 *	Created on:	2.4.2013
 *	Author:		Antti Kamppi
 *	File name:	swbuilddelegate.cpp
 *	Project:		Kactus 2
*/

#include "swbuilddelegate.h"
#include <common/widgets/fileTypeSelector/filetypeselector.h>

#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QEvent>

SWBuildDelegate::SWBuildDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

SWBuildDelegate::~SWBuildDelegate() {
}

QWidget* SWBuildDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
	case SWBuildDelegate::FILETYPE_COLUMN: {
		
		FileTypeSelector* typeEditor = new FileTypeSelector(parent);
		typeEditor->refresh();
		typeEditor->setMaxVisibleItems(25);
		typeEditor->setMinimumContentsLength(30);

		return typeEditor;
														}
	case SWBuildDelegate::COMMAND_COLUMN:
	case SWBuildDelegate::FLAGS_COLUMN: {
		QLineEdit* lineEditor = new QLineEdit(parent);
		connect(lineEditor, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return lineEditor;
													}
	case SWBuildDelegate::REPLACE_DEF_COLUMN:
	default: {
		return QStyledItemDelegate::createEditor(parent, option, index);
				}
	}
}

void SWBuildDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
	case SWBuildDelegate::FILETYPE_COLUMN: {
		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		FileTypeSelector* combo = qobject_cast<FileTypeSelector*>(editor);

		combo->selectFileType(text);
		break;
														}
	case SWBuildDelegate::COMMAND_COLUMN:
	case SWBuildDelegate::FLAGS_COLUMN: {
		QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
		QString value = index.model()->data(index, Qt::DisplayRole).toString();
		defaultEdit->setText(value);
		break;
													}
	case SWBuildDelegate::REPLACE_DEF_COLUMN: 
	default: {
		QStyledItemDelegate::setEditorData(editor, index);
		break;
				}
	}
}

void SWBuildDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
	case SWBuildDelegate::FILETYPE_COLUMN: {
		FileTypeSelector* combo = qobject_cast<FileTypeSelector*>(editor);
		QString text = combo->currentText();
		model->setData(index, text, Qt::EditRole);
		break;
														}
	case SWBuildDelegate::COMMAND_COLUMN:
	case SWBuildDelegate::FLAGS_COLUMN: {
		QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
		QString value = defaultEdit->text();
		model->setData(index, value, Qt::EditRole);
		break;
													}
	case SWBuildDelegate::REPLACE_DEF_COLUMN:
	default: {
		QStyledItemDelegate::setModelData(editor, model, index);
		break;
				}
	}
}

void SWBuildDelegate::commitAndCloseEditor() {
	QWidget* editor = qobject_cast<QWidget*>(sender());

	if (editor) {
		emit commitData(editor);
		emit closeEditor(editor);
	}
}

void SWBuildDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {
	QStyleOptionViewItemV4 viewItemOption(option);
	
	if (index.column() == SWBuildDelegate::REPLACE_DEF_COLUMN) {
		painter->fillRect(option.rect, Qt::white);

		const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

		QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
			QSize(option.decorationSize.width() + 5, option.decorationSize.height()),
			QRect(option.rect.x() + textMargin, option.rect.y(),
			option.rect.width() - (2 * textMargin), option.rect.height()));
		viewItemOption.rect = newRect;
	}

	QStyledItemDelegate::paint(painter, viewItemOption, index);
}

bool SWBuildDelegate::editorEvent( QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index ) {
	Q_ASSERT(event);
	Q_ASSERT(model);

	// Make sure that the item is checkable.
	Qt::ItemFlags flags = model->flags(index);

	if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
	{
		return false;
	}

	// Make sure that we have a check state.
	QVariant value = index.data(Qt::CheckStateRole);

	if (!value.isValid())
	{
		return false;
	}

	Qt::CheckState newState;

	// Handle the mouse button events.
	if (event->type() == QEvent::MouseButtonPress)
	{
		const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

		QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
			option.decorationSize,
			QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
			option.rect.width() - (2 * textMargin),
			option.rect.height()));

		if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
		{
			return false;
		}

		newState = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
	}
	else {
		return false;
	}

	return model->setData(index, newState, Qt::CheckStateRole);
}
