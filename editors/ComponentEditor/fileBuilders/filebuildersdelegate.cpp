//-----------------------------------------------------------------------------
// File: filebuildersdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 18.04.2011
//
// Description:
// Delegate to provide editors to edit file builders.
//-----------------------------------------------------------------------------

#include "filebuildersdelegate.h"
#include "FileBuilderColumns.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/fileTypeSelector/filetypeselector.h>

#include <QComboBox>
#include <QLineEdit>
#include <QStringList>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QEvent>

//-----------------------------------------------------------------------------
// Function: filebuildersdelegate::FileBuildersDelegate()
//-----------------------------------------------------------------------------
FileBuildersDelegate::FileBuildersDelegate(QCompleter* parameterNameCompleter,
                                           QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
ExpressionDelegate(parameterNameCompleter, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: filebuildersdelegate::~FileBuildersDelegate()
//-----------------------------------------------------------------------------
FileBuildersDelegate::~FileBuildersDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: filebuildersdelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* FileBuildersDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option,
                                             const QModelIndex& index ) const
{
	// if editor for file type or replace default flags
	if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
    {
		FileTypeSelector* typeEditor = new FileTypeSelector(parent);
		typeEditor->refresh();
		typeEditor->setMaxVisibleItems(25);
		typeEditor->setMinimumContentsLength(30);

		return typeEditor;
	}
	// if editor for command or flags
    else if (index.column() == FileBuilderColumns::COMMAND_COLUMN ||
        index.column() == FileBuilderColumns::FLAGS_COLUMN)
    {
		QLineEdit* lineEditor = new QLineEdit(parent);
		connect(lineEditor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return lineEditor;
	}
	else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
	}
}

//-----------------------------------------------------------------------------
// Function: filebuildersdelegate::setEditorData()
//-----------------------------------------------------------------------------
void FileBuildersDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
	// if editor for file type
	if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
    {
		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		FileTypeSelector* combo = qobject_cast<FileTypeSelector*>(editor);

		combo->selectFileType(text);
	}
	// if editor for command or flags
    else if (index.column() == FileBuilderColumns::COMMAND_COLUMN ||
        index.column() == FileBuilderColumns::FLAGS_COLUMN)
    {
		QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
		QString value = index.model()->data(index, Qt::DisplayRole).toString();
		defaultEdit->setText(value);
	}
	// use the default delegate
	else
    {
        ExpressionDelegate::setEditorData(editor, index);
	}
}

//-----------------------------------------------------------------------------
// Function: filebuildersdelegate::setModelData()
//-----------------------------------------------------------------------------
void FileBuildersDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index )
    const
{
	// if editor for file type
	if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
    {
		FileTypeSelector* combo = qobject_cast<FileTypeSelector*>(editor);
		QString text = combo->currentText();
		model->setData(index, text, Qt::EditRole);
	}
	// if editor for command or flags
    else if (index.column() == FileBuilderColumns::COMMAND_COLUMN ||
        index.column() == FileBuilderColumns::FLAGS_COLUMN)
    {
		QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
		QString value = defaultEdit->text();
		model->setData(index, value, Qt::EditRole);
	}
	// use the default delegate
	else
    {
        ExpressionDelegate::setModelData(editor, model, index);
	}
}

//-----------------------------------------------------------------------------
// Function: filebuildersdelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void FileBuildersDelegate::commitAndCloseEditor()
{
	QComboBox* comboEditor = qobject_cast<QComboBox*>(sender());
	QLineEdit* lineEditor = qobject_cast<QLineEdit*>(sender());
	QWidget* editor = qobject_cast<QWidget*>(sender());

	if (comboEditor)
    {
		emit commitData(comboEditor);
		emit closeEditor(comboEditor);
	}
	else if (lineEditor)
    {
		emit commitData(lineEditor);
		emit closeEditor(lineEditor);
	}
	else
    {
		emit commitData(editor);
		emit closeEditor(editor);
	}
}

//-----------------------------------------------------------------------------
// Function: filebuildersdelegate::paint()
//-----------------------------------------------------------------------------
void FileBuildersDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index )
    const
{
	QStyleOptionViewItemV4 viewItemOption(option);
	QStyledItemDelegate::paint(painter, viewItemOption, index);
}

//-----------------------------------------------------------------------------
// Function: filebuildersdelegate::editorEvent()
//-----------------------------------------------------------------------------
bool FileBuildersDelegate::editorEvent( QEvent *event, QAbstractItemModel *model,
                                        const QStyleOptionViewItem &option, const QModelIndex &index )
{
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

//-----------------------------------------------------------------------------
// Function: filebuildersdelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool FileBuildersDelegate::columnAcceptsExpression(int column) const
{
    return column == FileBuilderColumns::REPLACE_DEFAULT_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: filebuildersdelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int FileBuildersDelegate::descriptionColumn() const
{
    return FileBuilderColumns::COLUMN_COUNT;
}
