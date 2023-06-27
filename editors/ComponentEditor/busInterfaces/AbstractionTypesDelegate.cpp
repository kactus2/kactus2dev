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
EnumerationEditorConstructorDelegate(parent),
availableViews_(component->getViews()),
library_(library),
parentWindow_(parentWindow)
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
    }

    return EnumerationEditorConstructorDelegate::createEditor(parent, option, index);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AbstractionTypesDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.isValid() && !index.parent().isValid() && index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
    {
        VLNVEditor* abstractionEditor = dynamic_cast<VLNVEditor*>(editor);
        QVariant vlnvVariant = index.data(AbstractionTypesConstants::ABSTRACTIONVLNVROLE);

        if (abstractionEditor && vlnvVariant.canConvert<VLNV>())
        {
            VLNV vlnvReference = vlnvVariant.value<VLNV>();
            abstractionEditor->setVLNV(vlnvReference);
        }
    }

    else
    {
        EnumerationEditorConstructorDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::setModelData()
//-----------------------------------------------------------------------------
void AbstractionTypesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index )
    const
{
    if (index.isValid() && !index.parent().isValid() && index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
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
    else
    {
        EnumerationEditorConstructorDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::createVLNVEditor()
//-----------------------------------------------------------------------------
QWidget* AbstractionTypesDelegate::createVLNVEditor(QWidget* parent) const
{
    VLNVEditor* abstractionEditor(new VLNVEditor(
        VLNV::ABSTRACTIONDEFINITION, library_, parentWindow_, parent));

    abstractionEditor->setFixedHeight(abstractionEditor->sizeHint().height());
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
// Function: AbstractionTypesDelegate::isEnumerationEditorColumn()
//-----------------------------------------------------------------------------
bool AbstractionTypesDelegate::isEnumerationEditorColumn(QModelIndex const& index) const
{
    return index.column() == AbstractionTypesConstants::VIEWREFERENCES;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::getCurrentSelection()
//-----------------------------------------------------------------------------
QStringList AbstractionTypesDelegate::getCurrentSelection(QModelIndex const& index) const
{
    return index.data(Qt::DisplayRole).toString().split(AbstractionTypesConstants::VIEW_SEPARATOR);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::getAvailableItems()
//-----------------------------------------------------------------------------
QStringList AbstractionTypesDelegate::getAvailableItems() const
{
    QStringList viewNames;
    for (auto view : *availableViews_)
    {
        viewNames.append(view->name());
    }

    return viewNames;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesDelegate::setEnumerationDataToModel()
//-----------------------------------------------------------------------------
void AbstractionTypesDelegate::setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model, QStringList const& selectedItems) const
{
    model->setData(index, selectedItems, Qt::EditRole);
}

