//-----------------------------------------------------------------------------
// File: PowerDomainLinkDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.10.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "PowerDomainLinkDelegate.h"
#include "PowerDomainLinkColumns.h"

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <IPXACTmodels/Component/Component.h>

#include <QComboBox>
#include <QString>

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkDelegate::PowerDomainLinkDelegate()
//-----------------------------------------------------------------------------
PowerDomainLinkDelegate::PowerDomainLinkDelegate(QObject* parent):
EnumerationEditorConstructorDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PowerDomainLinkDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.column() == PowerDomainLinkColumns::EXTERNAL_LINK)
    {
        auto externalSelector = new QComboBox(parent);
        externalSelector->setEditable(true);
        
        if (externalComponent_.isNull() == false)
        {
            externalSelector->addItems(externalComponent_->getPowerDomainNames());
        }
        
        connect(externalSelector, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);

        return externalSelector;
    }
    else if (index.column() == PowerDomainLinkColumns::INTERNAL_LINK )
    {
        return EnumerationEditorConstructorDelegate::createEditor(parent, option, index);
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PowerDomainLinkDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == PowerDomainLinkColumns::EXTERNAL_LINK)
    {
        auto edit = qobject_cast<QComboBox*>(editor);
        Q_ASSERT(edit);

        edit->setCurrentText(index.data(Qt::DisplayRole).toString());
    }
    else
    {
        EnumerationEditorConstructorDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkDelegate::setModelData()
//-----------------------------------------------------------------------------
void PowerDomainLinkDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == PowerDomainLinkColumns::EXTERNAL_LINK)
    {
        auto edit = qobject_cast<QComboBox*>(editor);
        Q_ASSERT(edit);

        model->setData(index, edit->currentText(), Qt::EditRole);
    }
    else
    {
        EnumerationEditorConstructorDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkDelegate::setComponents()
//-----------------------------------------------------------------------------
void PowerDomainLinkDelegate::setComponents(QSharedPointer<Component> externalComponent, 
    QSharedPointer<Component> internalComponent) noexcept
{
    externalComponent_ = externalComponent;
    internalComponent_ = internalComponent;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkDelegate::isEnumerationEditorColumn()
//-----------------------------------------------------------------------------
bool PowerDomainLinkDelegate::isEnumerationEditorColumn(QModelIndex const& index) const
{
    return index.column() == PowerDomainLinkColumns::INTERNAL_LINK;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkDelegate::getCurrentSelection()
//-----------------------------------------------------------------------------
QStringList PowerDomainLinkDelegate::getCurrentSelection(QModelIndex const& index) const
{
    return index.data(Qt::EditRole).toStringList();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkDelegate::getAvailableItems()
//-----------------------------------------------------------------------------
QStringList PowerDomainLinkDelegate::getAvailableItems() const
{
    if (internalComponent_.isNull())
    {
        return QStringList();
    }

    return internalComponent_->getPowerDomainNames();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkDelegate::setEnumerationDataToModel()
//-----------------------------------------------------------------------------
void PowerDomainLinkDelegate::setEnumerationDataToModel(QModelIndex const& index, 
    QAbstractItemModel* model, QStringList const& selectedItems) const
{
    model->setData(index, selectedItems, Qt::EditRole);
}
