//-----------------------------------------------------------------------------
// File: AbstractionTypesDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.01.2018
//
// Description:
// The delegate that provides editors to edit bus interface abstraction types.
//-----------------------------------------------------------------------------

#include "AbstractionTypesDelegate.h"

#include <editors/ComponentEditor/busInterfaces/AbstractionTypesConstants.h>

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>

#include <QComboBox>
#include <QPen>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::AbstractionTypesDelegate()
//-----------------------------------------------------------------------------
AbstractionTypesDelegate::AbstractionTypesDelegate(QSharedPointer<Component> component, LibraryInterface* library,
    QWidget* parentWindow, QObject* parent):
QStyledItemDelegate(parent),
availableViews_(component->getViews()),
library_(library),
parentWindow_(parentWindow)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::~AbstractionTypesDelegate()
//-----------------------------------------------------------------------------
AbstractionTypesDelegate::~AbstractionTypesDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AbstractionTypesDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.isValid())
    {
        if (index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
        {
            return createVLNVEditor(parent);
        }
        else if (index.column() == AbstractionTypesConstants::VIEWREFERENCES)
        {
            return new EnumCollectionEditor(parent);
        }
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AbstractionTypesDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.isValid() && !index.parent().isValid())
    {
        if (index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
        {
            VLNVEditor* abstractionEditor = dynamic_cast<VLNVEditor*>(editor);
            QVariant vlnvVariant = index.data(AbstractionTypesConstants::ABSTRACTIONVLNVROLE);

            if (abstractionEditor && vlnvVariant.canConvert<VLNV> ())
            {
                VLNV vlnvReference = vlnvVariant.value<VLNV>();
                abstractionEditor->setVLNV(vlnvReference);
            }
        }
        else if (index.column() == AbstractionTypesConstants::VIEWREFERENCES)
        {
            EnumCollectionEditor* viewEditor = dynamic_cast<EnumCollectionEditor*>(editor);
            if (viewEditor)
            {
                QModelIndex viewIndex = index.sibling(index.row(), AbstractionTypesConstants::VIEWREFERENCES);
                QStringList selectedViews =
                    viewIndex.data(Qt::DisplayRole).toString().split(AbstractionTypesConstants::VIEW_SEPARATOR);

                foreach (QSharedPointer<View> view, *availableViews_)
                {
                    QString viewName = view->name();
                    viewEditor->addItem(viewName, selectedViews.contains(viewName));
                }
            }
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::setModelData()
//-----------------------------------------------------------------------------
void AbstractionTypesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index )
    const
{
    if (index.isValid() && !index.parent().isValid())
    {
        if (index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
        {
            VLNVEditor* abstractionEditor = dynamic_cast<VLNVEditor*>(editor);
            if (abstractionEditor)
            {
                VLNV abstractionReference = abstractionEditor->getVLNV();
                QVariant abstractionVariant;
                abstractionVariant.setValue(abstractionReference);

                model->setData(index, abstractionVariant, Qt::EditRole);
            }
        }
        else if (index.column() == AbstractionTypesConstants::VIEWREFERENCES)
        {
            EnumCollectionEditor* viewEditor = dynamic_cast<EnumCollectionEditor*>(editor);
            if (viewEditor)
            {
                QStringList views = viewEditor->getSelectedItems();
                model->setData(index, views, Qt::EditRole);
            }
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::createVLNVEditor()
//-----------------------------------------------------------------------------
QWidget* AbstractionTypesDelegate::createVLNVEditor(QWidget* parent) const
{
    VLNVEditor* abstractionEditor(new VLNVEditor(
        VLNV::ABSTRACTIONDEFINITION, library_, parentWindow_, parent));

    abstractionEditor->setAutoFillBackground(true);
    abstractionEditor->setFocusPolicy(Qt::StrongFocus);
    abstractionEditor->setAttribute(Qt::WA_NoMousePropagation);

    return abstractionEditor;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::setComponent()
//-----------------------------------------------------------------------------
void AbstractionTypesDelegate::setComponent(QSharedPointer<Component> newComponent)
{
    availableViews_ = newComponent->getViews();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void AbstractionTypesDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);

    if (index.isValid() && !index.parent().isValid() &&
        (index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION ||
        index.column() == AbstractionTypesConstants::VIEWREFERENCES))
    {
        repositionAndResizeEditor(editor, option);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesConstants::repositionAndResizeEditor()
//-----------------------------------------------------------------------------
void AbstractionTypesDelegate::repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option) const
{
    int editorMinimumSize = 120;

    const int PARENT_HEIGHT = editor->parentWidget()->height();
    const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();
    
    if (AVAILABLE_HEIGHT_BELOW > editorMinimumSize)
    {
        editor->move(option.rect.topLeft());
    }
    else
    {
        int editorNewY = PARENT_HEIGHT-editorMinimumSize;
        if (editorNewY <= 0)
        {
            editorNewY = 0;
        }
        
        editor->move(option.rect.left(), editorNewY);
    }
    if (editorMinimumSize > PARENT_HEIGHT)
    {
        editor->setFixedHeight(PARENT_HEIGHT);
    }
    else
    {
        editor->setFixedHeight(editorMinimumSize);
    }
}
