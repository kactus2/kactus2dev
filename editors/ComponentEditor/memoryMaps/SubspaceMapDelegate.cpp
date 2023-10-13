//-----------------------------------------------------------------------------
// File: SubspaceMapDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.02.2022
//
// Description:
// The delegate that provides editors to add/remove/edit details of subspace maps.
//-----------------------------------------------------------------------------

#include "SubspaceMapDelegate.h"

#include <editors/ComponentEditor/memoryMaps/SubspaceMapColumns.h>
#include <KactusAPI/include/SubspaceMapInterface.h>
#include <KactusAPI/include/BusInterfaceInterface.h>

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Segment.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QLineEdit>
#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: SubspaceMapDelegate::SubspaceMapDelegate()
//-----------------------------------------------------------------------------
SubspaceMapDelegate::SubspaceMapDelegate(QAbstractItemModel* completionModel,
    QSharedPointer<ParameterFinder> parameterFinder, SubspaceMapInterface* subspaceInterface,
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces, Document::Revision docRevision, QObject *parent) :
MemoryBlockDelegate(completionModel, parameterFinder, parent),
subspaceInterface_(subspaceInterface),
busInterface_(subspaceInterface_->getBusInterface()),
addressSpaces_(addressSpaces),
docRevision_(docRevision)
{

}

//-----------------------------------------------------------------------------
// Function: SubspaceMapDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* SubspaceMapDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const 
{
    if (index.column() == SubspaceMapColumns::INITIATORREFERENCE ||
        index.column() == SubspaceMapColumns::SEGMENTREFERENCE)
    {
        QComboBox* comboBox = new QComboBox(parent);
        comboBox->addItem("");

        if (index.column() == SubspaceMapColumns::INITIATORREFERENCE)
        {
            comboBox->addItems(getInitiatorInterfaceNames());
        }
        else if (index.column() == SubspaceMapColumns::SEGMENTREFERENCE)
        {
            comboBox->addItems(getSegmentNames(index));
        }
        
        return comboBox;
    }
    else
    {
        return MemoryBlockDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapDelegate::getInitiatorInterfaceNames()
//-----------------------------------------------------------------------------
QStringList SubspaceMapDelegate::getInitiatorInterfaceNames() const
{
    QStringList interfaceNames;

    auto initiatorMode = docRevision_ == Document::Revision::Std22 ? General::INITIATOR : General::MASTER;

    for (auto interfaceName : busInterface_->getItemNames())
    {
        if (busInterface_->getMode(interfaceName) == initiatorMode)
        {
            interfaceNames.append(QString::fromStdString(interfaceName));
        }
    }

    return interfaceNames;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapDelegate::getSegmentNames()
//-----------------------------------------------------------------------------
QStringList SubspaceMapDelegate::getSegmentNames(QModelIndex const& index) const
{
    QStringList segmentNames;

    QModelIndex masterIndex = index.sibling(index.row(), SubspaceMapColumns::INITIATORREFERENCE);
    QString initiatorReference = masterIndex.data().toString();
    if (!initiatorReference.isEmpty())
    {
        QSharedPointer<AddressSpace> space = getAddressSpace(initiatorReference);
        if (space)
        {
            for (auto segment : *space->getSegments())
            {
                segmentNames.append(segment->name());
            }
        }
    }

    return segmentNames;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapDelegate::getAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> SubspaceMapDelegate::getAddressSpace(QString const& busName) const
{
    QString spaceReference =
        QString::fromStdString(busInterface_->getAddressSpaceReference(busName.toStdString()));
    if (!spaceReference.isEmpty())
    {
        for (auto space : *addressSpaces_)
        {
            if (space->name() == spaceReference)
            {
                return space;
            }
        }
    }

    return QSharedPointer<AddressSpace>();
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapDelegate::setEditorData()
//-----------------------------------------------------------------------------
void SubspaceMapDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == SubspaceMapColumns::INITIATORREFERENCE ||
        index.column() == SubspaceMapColumns::SEGMENTREFERENCE)
    {
        QString currentValue = index.model()->data(index).toString();

        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        if (combo)
        {
            int valueIndex = combo->findText(currentValue);
            if (valueIndex < 0)
            {
                combo->addItem(currentValue);
                valueIndex = combo->findText(currentValue);
            }

            combo->setCurrentIndex(valueIndex);
        }
    }
    else
    {
        MemoryBlockDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapDelegate::setModelData()
//-----------------------------------------------------------------------------
void SubspaceMapDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    QComboBox* combo = qobject_cast<QComboBox*>(editor);
    if (combo)
    {
        QString text = combo->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else 
    {
        MemoryBlockDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int SubspaceMapDelegate::descriptionColumn() const
{
    return SubspaceMapColumns::DESCRIPTION_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapDelegate::nameColumn()
//-----------------------------------------------------------------------------
int SubspaceMapDelegate::nameColumn() const
{
    return SubspaceMapColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapDelegate::baseAddressColumn()
//-----------------------------------------------------------------------------
int SubspaceMapDelegate::baseAddressColumn() const
{
    return SubspaceMapColumns::BASE;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapDelegate::isPresentColumn()
//-----------------------------------------------------------------------------
int SubspaceMapDelegate::isPresentColumn() const
{
    return SubspaceMapColumns::IS_PRESENT;
}
