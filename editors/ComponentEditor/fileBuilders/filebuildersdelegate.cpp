/* 
 *
 *  Created on: 18.4.2011
 *      Author: Antti Kamppi
 * 		filename: filebuildersdelegate.cpp
 */

#include "filebuildersdelegate.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/fileTypeSelector/filetypeselector.h>

#include <QComboBox>
#include <QLineEdit>
#include <QStringList>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QEvent>

FileBuildersDelegate::FileBuildersDelegate(QObject *parent): 
QStyledItemDelegate(parent) {
}

FileBuildersDelegate::~FileBuildersDelegate() {
}

QWidget* FileBuildersDelegate::createEditor( QWidget* parent, 
											const QStyleOptionViewItem& option, 
											const QModelIndex& index ) const {

	// if editor for file type or replace default flags
	if (index.column() == FILETYPE_COLUMN) {
		FileTypeSelector* typeEditor = new FileTypeSelector(parent);
		typeEditor->refresh();
		typeEditor->setMaxVisibleItems(25);
		typeEditor->setMinimumContentsLength(30);

		return typeEditor;
	}
	// if editor for command or flags
	else if (index.column() == COMMAND_COLUMN || index.column() == FLAGS_COLUMN) {
		QLineEdit* lineEditor = new QLineEdit(parent);
		connect(lineEditor, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return lineEditor;
	}
	// if editor for replace default flags
	else if (index.column() == REPLACE_DEFAULT_COLUMN) {
		BooleanComboBox* boolBox = new BooleanComboBox(parent);
		connect(boolBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return boolBox;
	}
	// use the default delegate
	else {
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

void FileBuildersDelegate::setEditorData( QWidget* editor, 
										 const QModelIndex& index ) const {

	// if editor for file type
	if (index.column() == FILETYPE_COLUMN) {

		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		FileTypeSelector* combo = qobject_cast<FileTypeSelector*>(editor);

		combo->selectFileType(text);
	}
	// if editor for command or flags
	else if (index.column() == COMMAND_COLUMN || index.column() == FLAGS_COLUMN) {

		QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
		QString value = index.model()->data(index, Qt::DisplayRole).toString();
		defaultEdit->setText(value);
	}
	// if editor for replace default flags
	else if (index.column() == REPLACE_DEFAULT_COLUMN) {

		BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
		bool value = index.model()->data(index, Qt::DisplayRole).toBool();
		boolBox->setCurrentValue(value);
	}
	// use the default delegate
	else {
	 QStyledItemDelegate::setEditorData(editor, index);
	}
}

void FileBuildersDelegate::setModelData( QWidget* editor, 
										QAbstractItemModel* model, 
										const QModelIndex& index ) const {

	// if editor for file type
	if (index.column() == FILETYPE_COLUMN) {

		FileTypeSelector* combo = qobject_cast<FileTypeSelector*>(editor);
		QString text = combo->currentText();
		model->setData(index, text, Qt::EditRole);
	}
	// if editor for command or flags
	else if (index.column() == COMMAND_COLUMN || index.column() == FLAGS_COLUMN) {

		QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
		QString value = defaultEdit->text();
		model->setData(index, value, Qt::EditRole);
	}
	// if editor for replace default flags
	else if (index.column() == REPLACE_DEFAULT_COLUMN) {

		BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
		bool value = boolBox->getCurrentValue();
		model->setData(index, value, Qt::EditRole);
	}
	// use the default delegate
	else {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

void FileBuildersDelegate::commitAndCloseEditor() {

	QComboBox* comboEditor = qobject_cast<QComboBox*>(sender());
	QLineEdit* lineEditor = qobject_cast<QLineEdit*>(sender());
	QWidget* editor = qobject_cast<QWidget*>(sender());

	if (comboEditor) {
		emit commitData(comboEditor);
		emit closeEditor(comboEditor);
	}
	else if (lineEditor) {
		emit commitData(lineEditor);
		emit closeEditor(lineEditor);
	}
	else {
		emit commitData(editor);
		emit closeEditor(editor);
	}
}

void FileBuildersDelegate::paint( QPainter *painter, 
	const QStyleOptionViewItem &option,
	const QModelIndex &index ) const {

	QStyleOptionViewItemV4 viewItemOption(option);

	if (index.column() == FileBuildersDelegate::REPLACE_DEFAULT_COLUMN) {
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

bool FileBuildersDelegate::editorEvent( QEvent *event,
	QAbstractItemModel *model, 
	const QStyleOptionViewItem &option, const QModelIndex &index ) {

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
