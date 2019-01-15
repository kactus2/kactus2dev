//-----------------------------------------------------------------------------
// File: ResetsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.01.2019
//
// Description:
// The delegate to provide editors to add/remove/edit the resets of a field.
//-----------------------------------------------------------------------------

#include "ResetsDelegate.h"

#include <IPXACTmodels/Component/ResetType.h>

#include <editors/ComponentEditor/memoryMaps/ResetColumns.h>

#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: ResetsDelegate::ResetsDelegate()
//-----------------------------------------------------------------------------
ResetsDelegate::ResetsDelegate(QCompleter* parameterNameCompleter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<QList<QSharedPointer<ResetType>>> resetTypes, QObject *parent):
ExpressionDelegate(parameterNameCompleter, parameterFinder, parent),
resetTypes_(resetTypes)
{

}

//-----------------------------------------------------------------------------
// Function: ResetsDelegate::~ResetsDelegate()
//-----------------------------------------------------------------------------
ResetsDelegate::~ResetsDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ResetsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ResetsDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index ) const 
{
    if (index.column() == ResetColumns::RESETTYPEREFERENCE_COLUMN)
    {
        return createResetTypeCombo(parent);
    }
    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ResetsDelegate::createResetTypeCombo()
//-----------------------------------------------------------------------------
QComboBox* ResetsDelegate::createResetTypeCombo(QWidget* parent) const
{
    QComboBox* resetTypeCombo = new QComboBox(parent);
    resetTypeCombo->addItem("");

    for (auto resetType : *resetTypes_)
    {
        resetTypeCombo->addItem(resetType->name());
    }

    return resetTypeCombo;
}

//-----------------------------------------------------------------------------
// Function: ResetsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ResetsDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
    if (index.column() == ResetColumns::RESETTYPEREFERENCE_COLUMN)
    {
        QComboBox* resetTypeCombo = qobject_cast<QComboBox*>(editor);

        QString value = index.model()->data(index, Qt::DisplayRole).toString();
        resetTypeCombo->setCurrentText(value);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ResetsDelegate::setModelData()
//-----------------------------------------------------------------------------
void ResetsDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const 
{
    if (index.column() == ResetColumns::RESETTYPEREFERENCE_COLUMN)
    {
        QComboBox* resetTypeCombo = qobject_cast<QComboBox*>(editor);

        QString newResetType = resetTypeCombo->currentText();
        model->setData(index, newResetType, Qt::EditRole);
    }
    else
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ResetsDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool ResetsDelegate::columnAcceptsExpression(int column) const
{
    return column == ResetColumns::RESETVALUE_COLUMN || column == ResetColumns::RESETMASK_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: ResetsDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int ResetsDelegate::descriptionColumn() const
{
    return ResetColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ResetsDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void ResetsDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
