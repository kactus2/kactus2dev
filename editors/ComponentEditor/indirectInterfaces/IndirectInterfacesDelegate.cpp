//-----------------------------------------------------------------------------
// File: IndirectInterfacesDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2017
//
// Description:
// Delegate class for Indirect interfaces table of a component.
//-----------------------------------------------------------------------------

#include "IndirectInterfacesDelegate.h"

#include "IndirectInterfaceColumns.h"

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <QComboBox>
#include <QLineEdit>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Field.h>

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesDelegate::IndirectInterfacesDelegate()
//-----------------------------------------------------------------------------
IndirectInterfacesDelegate::IndirectInterfacesDelegate(QSharedPointer<Component> component,
    QObject *parent):
MultilineDescriptionDelegate(parent),
    component_(component)
{
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesDelegate::~IndirectInterfacesDelegate()
//-----------------------------------------------------------------------------
IndirectInterfacesDelegate::~IndirectInterfacesDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* IndirectInterfacesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, 
    QModelIndex const& index) const
{
    if (index.column() == IndirectInterfaceColumns::NAME ||
        index.column() == IndirectInterfaceColumns::DISPLAY_NAME)
    {
        QLineEdit* lineEditor = new QLineEdit(parent);
        connect(lineEditor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);

        return lineEditor;
    }
    else if (index.column() == IndirectInterfaceColumns::INDIRECT_ADDRESS_REF ||
        index.column() == IndirectInterfaceColumns::INDIRECT_DATA_REF)
    {
        ReferenceSelector* referenceSelector = new ReferenceSelector(parent);

        referenceSelector->refresh(findAvailableReferences());

        connect(referenceSelector, SIGNAL(currentIndexChanged(int)),
            this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);

        return referenceSelector;
    }
    else if (index.column() == IndirectInterfaceColumns::MEMORY_MAP_REF)
    {
        ReferenceSelector* memoryMapSelector = new ReferenceSelector(parent);

        memoryMapSelector->refresh(component_->getMemoryMapNames());

        connect(memoryMapSelector, SIGNAL(currentIndexChanged(int)),
            this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);

        return memoryMapSelector;
    }
    else
    {
        return MultilineDescriptionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void IndirectInterfacesDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == IndirectInterfaceColumns::NAME ||
        index.column() == IndirectInterfaceColumns::DISPLAY_NAME)
    {
        QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
        lineEditor->setText(index.data(Qt::DisplayRole).toString());
    }
    else if (index.column() == IndirectInterfaceColumns::INDIRECT_ADDRESS_REF ||
        index.column() == IndirectInterfaceColumns::INDIRECT_DATA_REF ||
        index.column() == IndirectInterfaceColumns::MEMORY_MAP_REF)
    {
          ReferenceSelector* selector = qobject_cast<ReferenceSelector*>(editor);
          selector->selectItem(index.data(Qt::DisplayRole).toString());
    }
    else
    {
        MultilineDescriptionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesDelegate::setModelData()
//-----------------------------------------------------------------------------
void IndirectInterfacesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, 
    QModelIndex const& index) const
{
    if (index.column() == IndirectInterfaceColumns::NAME ||
        index.column() == IndirectInterfaceColumns::DISPLAY_NAME)
    {
        QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
        model->setData(index, lineEditor->text(), Qt::EditRole);
    }
    else if (index.column() == IndirectInterfaceColumns::INDIRECT_ADDRESS_REF ||
        index.column() == IndirectInterfaceColumns::INDIRECT_DATA_REF ||
        index.column() == IndirectInterfaceColumns::MEMORY_MAP_REF)
    {
        QComboBox* selector = qobject_cast<QComboBox*>(editor);
        model->setData(index, selector->currentText(), Qt::EditRole);
    }
    else
    {
        MultilineDescriptionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int IndirectInterfacesDelegate::descriptionColumn() const
{
    return IndirectInterfaceColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void IndirectInterfacesDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	QComboBox* combo = qobject_cast<QComboBox*>(edit);

	if (combo)
    {
		emit commitData(combo);
	}
	else
    {
		emit commitData(edit);
		emit closeEditor(edit);
	}
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesDelegate::findAvailableReferences()
//-----------------------------------------------------------------------------
QStringList IndirectInterfacesDelegate::findAvailableReferences() const
{
    QStringList fields;
    foreach (QSharedPointer<MemoryMap> map, *component_->getMemoryMaps())
    {
        foreach (QSharedPointer<MemoryBlockBase> block, *map->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> addressBlock = block.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                foreach (QSharedPointer<RegisterBase> registerBase, *addressBlock->getRegisterData())
                {
                    QSharedPointer<Register> reg = registerBase.dynamicCast<Register>();
                    if (reg)
                    {
                        foreach (QSharedPointer<Field> field, *reg->getFields())
                        {
                            if (!field->getId().isEmpty())
                            {
                                fields.append(field->getId());
                            }
                        }
                    }
                }
            }
        }
    }

    return fields;
}

