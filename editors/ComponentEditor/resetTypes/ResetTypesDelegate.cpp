//-----------------------------------------------------------------------------
// File: ResetTypesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.01.2019
//
// Description:
// The delegate to provide editors to add/remove/edit the reset types of a component.
//-----------------------------------------------------------------------------

#include "ResetTypesDelegate.h"

#include <editors/ComponentEditor/resetTypes/ResetTypeColumns.h>

#include <QLineEdit>
#include <QStringList>
#include <QString>

//-----------------------------------------------------------------------------
// Function: ResetTypesDelegate::ResetTypesDelegate()
//-----------------------------------------------------------------------------
ResetTypesDelegate::ResetTypesDelegate(QObject* parent):
QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ResetTypesDelegate::~ResetTypesDelegate()
//-----------------------------------------------------------------------------
ResetTypesDelegate::~ResetTypesDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ResetTypesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ResetTypesDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == ResetTypeColumns::NAME_COLUMN ||
        index.column() == ResetTypeColumns::DISPLAY_NAME_COLUMN ||
        index.column() == ResetTypeColumns::DESCRIPTION_COLUMN)
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return lineEdit;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ResetTypesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ResetTypesDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == ResetTypeColumns::NAME_COLUMN ||
        index.column() == ResetTypeColumns::DISPLAY_NAME_COLUMN ||
        index.column() == ResetTypeColumns::DESCRIPTION_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        const QString text = index.data(Qt::DisplayRole).toString();
        edit->setText(text);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ResetTypesDelegate::setModelData()
//-----------------------------------------------------------------------------
void ResetTypesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == ResetTypeColumns::NAME_COLUMN ||
        index.column() == ResetTypeColumns::DISPLAY_NAME_COLUMN ||
        index.column() == ResetTypeColumns::DESCRIPTION_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        QString text = edit->text();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ResetTypesDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void ResetTypesDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
