//-----------------------------------------------------------------------------
// File: ChoicesDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// Delegate class for a view and a ChoicesModel.
//-----------------------------------------------------------------------------

#include "ChoicesDelegate.h"

#include "EnumerationModel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <IPXACTmodels/choice.h>

#include <QScrollArea>

namespace
{
    const int ENUMERATION_COLUMN = 1;
}

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
    if (index.column() == ENUMERATION_COLUMN)
    {
        EditableTableView* editor = new EditableTableView(parent);
        editor->setAlternatingRowColors(false);

        QScrollArea* scrollingWidget = new QScrollArea(parent);
        scrollingWidget->setWidgetResizable(true);
        scrollingWidget->setWidget(editor);
        
        return scrollingWidget;
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
    if (index.column() == ENUMERATION_COLUMN)
    {
        EditableTableView* view = dynamic_cast<EditableTableView*>(dynamic_cast<QScrollArea*>(editor)->widget());

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
    if (index.column() == ENUMERATION_COLUMN)
    {
        return;
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}

//-----------------------------------------------------------------------------
// Function: ChoicesDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void ChoicesDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, 
    const QModelIndex & index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);

    if (index.column() == ENUMERATION_COLUMN)
    {
        repositionAndResizeEditor(editor, option);
    }
}

//-----------------------------------------------------------------------------
// Function: ChoicesDelegate::repositionEditor()
//-----------------------------------------------------------------------------
void ChoicesDelegate::repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option) const
{
    const int PARENT_HEIGHT = editor->parentWidget()->height();

    const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();
    const int AVAILABLE_HEIGHT_ABOVE = option.rect.bottom();

    const int MINIMUM_HEIGHT = 200;

    if (AVAILABLE_HEIGHT_BELOW > MINIMUM_HEIGHT)
    {
        editor->move(option.rect.topLeft());
        editor->setFixedHeight(AVAILABLE_HEIGHT_BELOW);
    }
    else if (AVAILABLE_HEIGHT_ABOVE > MINIMUM_HEIGHT)
    {
        editor->move(option.rect.left(), 0);
        editor->setFixedHeight(AVAILABLE_HEIGHT_ABOVE);
    }
    else
    {
        editor->move(option.rect.left(), 0);
        editor->setFixedHeight(PARENT_HEIGHT);
    }
}
