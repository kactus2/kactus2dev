//-----------------------------------------------------------------------------
// File: ChoicesDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "ChoicesDelegate.h"

#include "EnumerationModel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <IPXACTmodels/choice.h>


//-----------------------------------------------------------------------------
// Function: ChoicesDelegate::ChoicesDelegate()
//-----------------------------------------------------------------------------
ChoicesDelegate::ChoicesDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, QObject* parent): 
QStyledItemDelegate(parent), choices_(choices)
{

}

//-----------------------------------------------------------------------------
// Function: ChoicesDelegate::~ChoicesDelegate()
//-----------------------------------------------------------------------------
ChoicesDelegate::~ChoicesDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ChoicesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ChoicesDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.column() == 1)
    {
        EditableTableView* editor = new EditableTableView(parent);
        editor->setAlternatingRowColors(false);

        editor->move(option.rect.topLeft());
        editor->setFixedHeight(parent->height() - option.rect.topLeft().y());
        return editor;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ChoicesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ChoicesDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == 1)
    {
        EditableTableView* view = dynamic_cast<EditableTableView*>(editor);

        EnumerationModel* model = new EnumerationModel(choices_->at(index.row())->enumerations(), view);


        view->setModel(model);

        connect(model, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
        connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);

        connect(view, SIGNAL(addItem(const QModelIndex&)),
            model, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
        connect(view, SIGNAL(removeItem(const QModelIndex&)),
            model, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: ChoicesDelegate::setModelData()
//-----------------------------------------------------------------------------
void ChoicesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() != 1)
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}