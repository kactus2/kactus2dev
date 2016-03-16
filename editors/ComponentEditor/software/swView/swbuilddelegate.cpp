//-----------------------------------------------------------------------------
// File: swbuilddelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 02.04.2013
//
// Description:
// The delegate class to edit SW build commands.
//-----------------------------------------------------------------------------

#include "swbuilddelegate.h"

#include <common/widgets/fileTypeSelector/filetypeselector.h>

#include <editors/ComponentEditor/fileBuilders/FileBuilderColumns.h>

#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QEvent>

//-----------------------------------------------------------------------------
// Function: SWBuildDelegate::SWBuildDelegate()
//-----------------------------------------------------------------------------
SWBuildDelegate::SWBuildDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: SWBuildDelegate::~SWBuildDelegate()
//-----------------------------------------------------------------------------
SWBuildDelegate::~SWBuildDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: SWBuildDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* SWBuildDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index ) const
{
	if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
    {
        FileTypeSelector* typeEditor = new FileTypeSelector(parent);
        typeEditor->refresh();
        typeEditor->setMaxVisibleItems(25);
        typeEditor->setMinimumContentsLength(30);

        return typeEditor;
    }
    else if (index.column() == FileBuilderColumns::COMMAND_COLUMN ||
        index.column() == FileBuilderColumns::FLAGS_COLUMN)
    {
        QLineEdit* lineEditor = new QLineEdit(parent);
        connect(lineEditor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return lineEditor;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: SWBuildDelegate::setEditorData()
//-----------------------------------------------------------------------------
void SWBuildDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
	if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
    {
		QString text = index.model()->data(index, Qt::DisplayRole).toString();
        FileTypeSelector* combo = qobject_cast<FileTypeSelector*>(editor);

        combo->selectFileType(text);
    }
    else if (index.column() == FileBuilderColumns::COMMAND_COLUMN ||
        index.column() == FileBuilderColumns::FLAGS_COLUMN)
    {
        QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
        QString value = index.model()->data(index, Qt::DisplayRole).toString();
        defaultEdit->setText(value);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: SWBuildDelegate::setModelData()
//-----------------------------------------------------------------------------
void SWBuildDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
	if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
    {
		FileTypeSelector* combo = qobject_cast<FileTypeSelector*>(editor);
		QString text = combo->currentText();
		model->setData(index, text, Qt::EditRole);
    }
    else if (index.column() == FileBuilderColumns::COMMAND_COLUMN ||
        index.column() == FileBuilderColumns::FLAGS_COLUMN)
    {
        QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
        QString value = defaultEdit->text();
        model->setData(index, value, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: SWBuildDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void SWBuildDelegate::commitAndCloseEditor()
{
	QWidget* editor = qobject_cast<QWidget*>(sender());

	if (editor)
    {
		emit commitData(editor);
		emit closeEditor(editor);
	}
}

//-----------------------------------------------------------------------------
// Function: SWBuildDelegate::paint()
//-----------------------------------------------------------------------------
void SWBuildDelegate::paint(QPainter *painter, QStyleOptionViewItem const& option, QModelIndex const& index) const
{
	QStyleOptionViewItemV4 viewItemOption(option);
	
	if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
    {
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

//-----------------------------------------------------------------------------
// Function: SWBuildDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool SWBuildDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option, 
    QModelIndex const& index)
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
	else
    {
		return false;
	}

	return model->setData(index, newState, Qt::CheckStateRole);
}
