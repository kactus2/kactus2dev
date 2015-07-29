//-----------------------------------------------------------------------------
// File: lineeditdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 9.12.2011
//
// Description:
// The base class to provide QLineEditor for a widget.
//-----------------------------------------------------------------------------

#include "lineeditdelegate.h"

#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: LineEditDelegate::LineEditDelegate()
//-----------------------------------------------------------------------------
LineEditDelegate::LineEditDelegate(QObject *parent):
QStyledItemDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: LineEditDelegate::~LineEditDelegate()
//-----------------------------------------------------------------------------
LineEditDelegate::~LineEditDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: LineEditDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* LineEditDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const&, QModelIndex const&) const
{
	QLineEdit* editor = new QLineEdit(parent);
	connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
	editor->setMinimumHeight(LineEditDelegate::MINIMUM_EDITOR_HEIGHT);
	return editor;
}

//-----------------------------------------------------------------------------
// Function: LineEditDelegate::setEditorData()
//-----------------------------------------------------------------------------
void LineEditDelegate::setEditorData(QWidget* editor, QModelIndex const& index ) const {

	QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
	Q_ASSERT(edit);

	const QString text = index.data(Qt::DisplayRole).toString();
	edit->setText(text);
}

//-----------------------------------------------------------------------------
// Function: LineEditDelegate::setModelData()
//-----------------------------------------------------------------------------
void LineEditDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index ) const
{
	QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
	Q_ASSERT(edit);

	QString text = edit->text();
	model->setData(index, text, Qt::EditRole);
}

//-----------------------------------------------------------------------------
// Function: LineEditDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void LineEditDelegate::commitAndCloseEditor()
{
	QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
