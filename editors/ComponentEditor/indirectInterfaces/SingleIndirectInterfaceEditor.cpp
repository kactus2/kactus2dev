//-----------------------------------------------------------------------------
// File: SingleIndirectInterfaceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2017
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "SingleIndirectInterfaceEditor.h"

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>
#include <editors/ComponentEditor/busInterfaces/general/bridgeseditor.h>

#include <IPXACTmodels/Component/IndirectInterface.h>

#include <QGroupBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QLabel>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Field.h>

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::SingleIndirectInterfaceEditor()
//-----------------------------------------------------------------------------
SingleIndirectInterfaceEditor::SingleIndirectInterfaceEditor(
    QSharedPointer<IndirectInterface> indirectInterface, QSharedPointer<Component> component,
    LibraryInterface* library, QWidget* parent): ItemEditor(component, library, parent),
    indirectInterface_(indirectInterface),
    component_(component),
    nameEditor_(new NameGroupEditor(indirectInterface, this, tr("Indirect interface name and description"))),   
    addressSelector_(new ReferenceSelector(this)),
    dataSelector_(new ReferenceSelector(this)),
    bitsInLauEditor_(new QLineEdit(this)),
    endiannessSelector_(new QComboBox(this)),
    memoryMapSelector_(new ReferenceSelector(this)),
    transparentBridgesEditor_(new BridgesEditor(indirectInterface->getTransparentBridges(), component, this))
{    
    addressSelector_->setProperty("mandatoryField", true);
    dataSelector_->setProperty("mandatoryField", true);

    endiannessSelector_->addItem(QString("little"));
    endiannessSelector_->addItem(QString("big"));
    endiannessSelector_->addItem(QString(""));

    setupLayout();

    connect(addressSelector_, SIGNAL(itemSelected(QString const&)), this, SLOT(onIndirectAddressChanged()));
    connect(dataSelector_, SIGNAL(itemSelected(QString const&)), this, SLOT(onIndirectDataChanged()));
    connect(bitsInLauEditor_, SIGNAL(editingFinished()), this, SLOT(onBitsInLauChanged()));
    connect(endiannessSelector_, SIGNAL(activated(QString const&)), this, SLOT(onEndiannessChanged()));
    connect(memoryMapSelector_, SIGNAL(itemSelected(QString const&)), this, SLOT(onMemoryMapChanged()));
    
    connect(nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));   
    connect(transparentBridgesEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::~SingleIndirectInterfaceEditor()
//-----------------------------------------------------------------------------
SingleIndirectInterfaceEditor::~SingleIndirectInterfaceEditor()
{

}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::refresh()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::refresh()
{
    nameEditor_->refresh();

    QStringList referenceList = findAvailableReferences();

    addressSelector_->refresh(referenceList);
    addressSelector_->selectItem(indirectInterface_->getIndirectAddressRef());

    dataSelector_->refresh(referenceList);
    dataSelector_->selectItem(indirectInterface_->getIndirectDataRef());
    
    bitsInLauEditor_->setText(indirectInterface_->getBitsInLau());
    
    endiannessSelector_->setCurrentIndex(endiannessSelector_->findText(indirectInterface_->getEndianness()));

    memoryMapSelector_->refresh(component_->getMemoryMapNames());
    memoryMapSelector_->selectItem(indirectInterface_->getMemoryMapRef());
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/singleIndirectInterface.html");
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::onIndirectAddressChanged()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::onIndirectAddressChanged()
{
    indirectInterface_->setIndirectAddressRef(addressSelector_->currentText());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::onIndirectDataChanged()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::onIndirectDataChanged()
{
    indirectInterface_->setIndirectDataRef(dataSelector_->currentText());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::onBitsInLauChanged()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::onBitsInLauChanged()
{
    indirectInterface_->setBitsInLau(bitsInLauEditor_->text());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::onEndiannessChanged()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::onEndiannessChanged()
{
    indirectInterface_->setEndianness(endiannessSelector_->currentText());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::onMemoryMapChanged()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::onMemoryMapChanged()
{
    indirectInterface_->setMemoryMapRef(memoryMapSelector_->currentText());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::findAvailableReferences()
//-----------------------------------------------------------------------------
QStringList SingleIndirectInterfaceEditor::findAvailableReferences() const
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

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* topWidget = new QWidget(this);

    QGroupBox* detailsGroup = new QGroupBox(tr("General"), this);

    QFormLayout* detailsLayout = new QFormLayout(detailsGroup);
    detailsLayout->addRow(tr("Indirect address reference:"), addressSelector_);
    detailsLayout->addRow(tr("Indirect data reference:"), dataSelector_);
    detailsLayout->addRow(tr("Bits in Lau:"), bitsInLauEditor_);
    detailsLayout->addRow(tr("Endianness:"), endiannessSelector_);

    QGroupBox* memoryBox = new QGroupBox(tr("Indirect access"), this);

    QGridLayout* memoryLayout = new QGridLayout(memoryBox);
    memoryLayout->addWidget(new QLabel(tr("Memory map:")), 0, 0, 1, 1);
    memoryLayout->addWidget(memoryMapSelector_, 0, 1, 1, 1);
    memoryLayout->addWidget(transparentBridgesEditor_, 1, 0, 1, 2);
    memoryLayout->setColumnStretch(1, 1);

    QGridLayout* topLayout = new QGridLayout(topWidget);    
    topLayout->addWidget(nameEditor_, 0, 0, 1, 1);
    topLayout->addWidget(detailsGroup, 1, 0, 1, 1);
    topLayout->addWidget(memoryBox, 0, 1, 2, 1);
    topLayout->setRowStretch(2, 10);
    scrollArea->setWidget(topWidget);
}
