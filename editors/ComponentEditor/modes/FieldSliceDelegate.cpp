//-----------------------------------------------------------------------------
// File: FieldSliceDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 01.09.2023
//
// Description:
// Delegate that provides widgets for editing field slices in mode.
//-----------------------------------------------------------------------------

#include "FieldSliceDelegate.h"

#include "FieldSliceColumns.h"

#include <common/widgets/TreeItemSelector/FramedTreeItemEditor.h>

//-----------------------------------------------------------------------------
// Function: FieldSliceDelegate::FieldSliceDelegate()
//-----------------------------------------------------------------------------
FieldSliceDelegate::FieldSliceDelegate(QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces,
    QAbstractItemModel* completionModel,
    QSharedPointer<ParameterFinder> parameterFinder,
    QObject* parent):
    ExpressionDelegate(completionModel, parameterFinder, parent),
memoryMaps_(memoryMaps),
addressSpaces_(addressSpaces)
{

}

//-----------------------------------------------------------------------------
// Function: FieldSliceDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* FieldSliceDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == FieldSliceColumns::FIELD_REF)
    {
        auto factory =  MemoryTreeFactory(memoryMaps_, addressSpaces_);
        auto referenceSelector = new FramedTreeItemEditor(&factory, parent);


        connect(referenceSelector, SIGNAL(finishEditing()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        connect(referenceSelector, SIGNAL(cancelEditing()), this, SLOT(onEditorCancel()), Qt::UniqueConnection);

        return referenceSelector;
    }
    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldSliceDelegate::setEditorData()
//-----------------------------------------------------------------------------
void FieldSliceDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == FieldSliceColumns::FIELD_REF)
    {
        auto path = index.model()->data(index, Qt::EditRole).toStringList();
        auto referenceSelector = qobject_cast<FramedTreeItemEditor*>(editor);

        referenceSelector->selectPath(path);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldSliceDelegate::setModelData()
//-----------------------------------------------------------------------------
void FieldSliceDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index)
    const
{
    if (index.column() == FieldSliceColumns::FIELD_REF)
    {
        auto referenceSelector = qobject_cast<FramedTreeItemEditor*>(editor);
        auto path = referenceSelector->selectedPath();

        model->setData(index, path, Qt::EditRole);
    }
    else
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldSliceDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void FieldSliceDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == FieldSliceColumns::FIELD_REF)
    {
        int editorMinimumHeight = editor->sizeHint().height();

        const int PARENT_HEIGHT = editor->parentWidget()->height();
        const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();

        if (AVAILABLE_HEIGHT_BELOW > editorMinimumHeight)
        {
            editor->move(option.rect.topLeft());
        }
        else
        {
            int repositionY = PARENT_HEIGHT - editorMinimumHeight;
            if (repositionY <= 0)
            {
                repositionY = 0;
            }

            editor->move(option.rect.left(), repositionY);
        }

        editor->setFixedHeight(qMin(editorMinimumHeight, PARENT_HEIGHT));
        editor->setFixedWidth(qMax(editor->sizeHint().width() + 8, option.rect.width()));
    }
    else
    {
        ExpressionDelegate::updateEditorGeometry(editor, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldSliceDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int FieldSliceDelegate::descriptionColumn() const
{
    return FieldSliceColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool FieldSliceDelegate::columnAcceptsExpression(int column) const
{
    return column == FieldSliceColumns::RANGE_LEFT || column == FieldSliceColumns::RANGE_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void FieldSliceDelegate::commitAndCloseEditor()
{
    QWidget* edit = qobject_cast<QWidget*>(sender());
    Q_ASSERT(edit);

    emit commitData(edit);
    emit closeEditor(edit);
}

//-----------------------------------------------------------------------------
// Function: FieldSliceDelegate::onEditorCancel()
//-----------------------------------------------------------------------------
void FieldSliceDelegate::onEditorCancel()
{
    QWidget* edit = qobject_cast<QWidget*>(sender());
    if (edit)
    {
        emit closeEditor(edit);
    }
}