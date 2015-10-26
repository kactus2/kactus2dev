//-----------------------------------------------------------------------------
// File: envidentifiersdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 18.04.2011
//
// Description:
// The delegate that provides editors to be used to edit environment identifiers.
//-----------------------------------------------------------------------------

#include "envidentifiersdelegate.h"

#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: envidentifiersdelegate::EnvIdentifiersDelegate()
//-----------------------------------------------------------------------------
EnvIdentifiersDelegate::EnvIdentifiersDelegate(QObject *parent): 
QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: envidentifiersdelegate::~EnvIdentifiersDelegate()
//-----------------------------------------------------------------------------
EnvIdentifiersDelegate::~EnvIdentifiersDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: envidentifiersdelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* EnvIdentifiersDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& )
    const
{
	QLineEdit* editor = new QLineEdit(parent);
	connect(editor, SIGNAL(editingFinished()),
	  this, SLOT(commitAndCloseEditor()));
	return editor;
}

//-----------------------------------------------------------------------------
// Function: envidentifiersdelegate::setEditorData()
//-----------------------------------------------------------------------------
void EnvIdentifiersDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
	QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
	QString text = index.model()->data(index, Qt::DisplayRole).toString();
	lineEditor->setText(text);
}

//-----------------------------------------------------------------------------
// Function: envidentifiersdelegate::setModelData()
//-----------------------------------------------------------------------------
void EnvIdentifiersDelegate::setModelData( QWidget* editor, QAbstractItemModel* model,  const QModelIndex& index )
    const
{
	QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
	QString text = lineEditor->text();
	model->setData(index, text, Qt::EditRole);
}

//-----------------------------------------------------------------------------
// Function: envidentifiersdelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void EnvIdentifiersDelegate::commitAndCloseEditor()
{
	QLineEdit* editor = qobject_cast<QLineEdit*>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}
