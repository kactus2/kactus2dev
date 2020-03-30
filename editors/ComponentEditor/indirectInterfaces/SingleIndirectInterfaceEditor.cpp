//-----------------------------------------------------------------------------
// File: SingleIndirectInterfaceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2017
//
// Description:
// Editor for a single indirect interface within a component.
//-----------------------------------------------------------------------------

#include "SingleIndirectInterfaceEditor.h"

#include <common/KactusColors.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>
#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/busInterfaces/general/bridgeseditor.h>

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
#include <IPXACTmodels/Component/IndirectInterface.h>

#include <IPXACTmodels/Component/validators/IndirectInterfaceValidator.h>

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::SingleIndirectInterfaceEditor()
//-----------------------------------------------------------------------------
SingleIndirectInterfaceEditor::SingleIndirectInterfaceEditor(QSharedPointer<IndirectInterface> indirectInterface,
    QSharedPointer<IndirectInterfaceValidator> validator, QSharedPointer<Component> component,
    LibraryInterface* library, QSharedPointer<ParameterFinder> finder,
    QSharedPointer<ExpressionFormatter> formatter, QWidget* parent):
ParameterItemEditor(component, library, parent),
indirectInterface_(indirectInterface),
validator_(validator),
component_(component),
nameEditor_(new NameGroupEditor(indirectInterface, this, tr("Indirect interface name and description"))),   
addressSelector_(new ReferenceSelector(this)),
dataSelector_(new ReferenceSelector(this)),
bitsInLauEditor_(new QLineEdit(this)),
endiannessSelector_(new QComboBox(this)),
memoryMapBox_(new QGroupBox(tr("Indirect memory map"), this)),
memoryMapSelector_(new ReferenceSelector(this)),
transparentBridgesEditor_(new BridgesEditor(indirectInterface->getTransparentBridges(), component, this)),
parametersEditor_(new ParameterGroupBox(indirectInterface_->getParameters(), component->getChoices(), finder,
    formatter, this))
{
    addressSelector_->setProperty("mandatoryField", true);
    dataSelector_->setProperty("mandatoryField", true);

    endiannessSelector_->addItem(QString("little"));
    endiannessSelector_->addItem(QString("big"));
    endiannessSelector_->addItem(QString(""));

    memoryMapBox_->setCheckable(true);
    transparentBridgesEditor_->setCheckable(true);

    setupLayout();

    connect(addressSelector_, SIGNAL(itemSelected(QString const&)), this, SLOT(onIndirectAddressChanged()));
    connect(dataSelector_, SIGNAL(itemSelected(QString const&)), this, SLOT(onIndirectDataChanged()));
    connect(bitsInLauEditor_, SIGNAL(editingFinished()), this, SLOT(onBitsInLauChanged()));
    connect(endiannessSelector_, SIGNAL(activated(QString const&)), this, SLOT(onEndiannessChanged()));
    connect(memoryMapSelector_, SIGNAL(itemSelected(QString const&)), this, SLOT(onMemoryMapChanged()));
 
    connect(memoryMapBox_, SIGNAL(clicked(bool)), this, SLOT(onMemoryMapSelected(bool)));
    connect(transparentBridgesEditor_, SIGNAL(clicked(bool)), this, SLOT(onTransparentBridgeSelected(bool)));

    connect(nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));   
    connect(transparentBridgesEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
    connect(parametersEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    connect(parametersEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(parametersEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(parametersEditor_, SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    connect(parametersEditor_,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, QSharedPointer<ParametersInterface>)),
        this,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, QSharedPointer<ParametersInterface>)),
        Qt::UniqueConnection);
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
    setAddressReferenceColorByValidity();

    dataSelector_->refresh(referenceList);
    dataSelector_->selectItem(indirectInterface_->getIndirectDataRef());
    setDataReferenceColorByValidity();

    bitsInLauEditor_->setText(indirectInterface_->getBitsInLau());
    
    endiannessSelector_->setCurrentIndex(endiannessSelector_->findText(indirectInterface_->getEndianness()));

    memoryMapBox_->setChecked(!indirectInterface_->getMemoryMapRef().isEmpty());
    memoryMapSelector_->refresh(component_->getMemoryMapNames());
    memoryMapSelector_->selectItem(indirectInterface_->getMemoryMapRef());

    transparentBridgesEditor_->setChecked(!indirectInterface_->getTransparentBridges()->isEmpty());
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
    setAddressReferenceColorByValidity();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::onIndirectDataChanged()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::onIndirectDataChanged()
{
    indirectInterface_->setIndirectDataRef(dataSelector_->currentText());
    setDataReferenceColorByValidity();

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
    setMemoryMapColorByValidity();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::onMemorySelectionChanged()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::onMemoryMapSelected(bool checked)
{
    bool canChange = checked && indirectInterface_->getTransparentBridges()->isEmpty();
    if (canChange)
    {
        memoryMapBox_->setChecked(true);
        transparentBridgesEditor_->setChecked(false);
        
    }
    else
    {
        memoryMapBox_->setChecked(!checked);
    }
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::onTransparentBridgeSelected()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::onTransparentBridgeSelected(bool checked)
{
    bool canChange = checked && indirectInterface_->getMemoryMapRef().isEmpty();
    if (canChange)
    {
        transparentBridgesEditor_->setChecked(true);
        memoryMapBox_->setChecked(false);
    }
    else
    {
        transparentBridgesEditor_->setChecked(!checked);
    }
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::setAddressReferenceColorByValidity()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::setAddressReferenceColorByValidity()
{
    if (validator_->hasValidAddressReference(indirectInterface_))
    {
        addressSelector_->setTextColor(KactusColors::REGULAR_TEXT);
    }
    else
    {
        addressSelector_->setTextColor(KactusColors::ERROR);
    }
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::setDataReferenceColorByValidity()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::setDataReferenceColorByValidity()
{
    if (validator_->hasValidDataReference(indirectInterface_))
    {
        dataSelector_->setTextColor(KactusColors::REGULAR_TEXT);
    }
    else
    {
        dataSelector_->setTextColor(KactusColors::ERROR);
    }
}

//-----------------------------------------------------------------------------
// Function: SingleIndirectInterfaceEditor::setMemoryMapColorByValidity()
//-----------------------------------------------------------------------------
void SingleIndirectInterfaceEditor::setMemoryMapColorByValidity()
{
    if (validator_->hasValidMemoryMapReference(indirectInterface_))
    {
        memoryMapSelector_->setTextColor(KactusColors::REGULAR_TEXT);
    }
    else
    {
        memoryMapSelector_->setTextColor(KactusColors::ERROR);
    }
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
    detailsLayout->addRow(tr("Indirect address field:"), addressSelector_);
    detailsLayout->addRow(tr("Indirect data field:"), dataSelector_);
    detailsLayout->addRow(tr("Bits in Lau:"), bitsInLauEditor_);
    detailsLayout->addRow(tr("Endianness:"), endiannessSelector_);

    QGroupBox* memoryBox = new QGroupBox(tr("Indirect access target"), this);

    QHBoxLayout* mapLayout = new QHBoxLayout(memoryMapBox_);
    mapLayout->addWidget(memoryMapSelector_);

    QGridLayout* memoryLayout = new QGridLayout(memoryBox);
    memoryLayout->addWidget(memoryMapBox_, 0, 0, 1, 2);
    memoryLayout->addWidget(transparentBridgesEditor_, 1, 0, 1, 2);
    memoryLayout->setColumnStretch(1, 1);

    QGridLayout* topLayout = new QGridLayout(topWidget);    
    topLayout->addWidget(nameEditor_, 0, 0, 1, 1);
    topLayout->addWidget(detailsGroup, 1, 0, 1, 1);
    topLayout->addWidget(memoryBox, 0, 1, 2, 1);
    topLayout->addWidget(parametersEditor_, 2, 0, 1, 2);
    topLayout->setRowStretch(2, 10);
    scrollArea->setWidget(topWidget);
}
