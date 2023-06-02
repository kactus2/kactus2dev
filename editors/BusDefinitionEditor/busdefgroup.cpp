//-----------------------------------------------------------------------------
// File: busdefgroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// BusDefGroup contains elements to set the properties of a Bus Definition.
//-----------------------------------------------------------------------------

#include "busdefgroup.h"

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <editors/common/DocumentNameGroupEditor.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QSizePolicy>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: BusDefGroup::BusDefGroup()
//-----------------------------------------------------------------------------
BusDefGroup::BusDefGroup(LibraryInterface* libraryHandler, QWidget *parent):
QWidget(parent),
library_(libraryHandler),
busDef_(),
directConnection_(tr("Allow direct connections"), this),
isBroadcast_(tr("Support broadcast"), this),
isAddressable_(tr("Addressable bus"), this),
maxInitiatorsEditor_(this),
maxTargetsEditor_(this),
systemGroupEditor_(libraryHandler, this),
documentNameGroupEditor_(this),
extendEditor_(new VLNVEditor(VLNV::BUSDEFINITION, libraryHandler, parent, this))
{
    extendEditor_->setTitle(QStringLiteral("Extended bus definition"));
    extendEditor_->setMandatory(false);

    QRegularExpression numberExpression(QString("[0-9]*"));
    QRegularExpressionValidator* numberValidator = new QRegularExpressionValidator(numberExpression, this);
    maxInitiatorsEditor_.setValidator(numberValidator);
    maxTargetsEditor_.setValidator(numberValidator);

#if QT_VERSION > QT_VERSION_CHECK(5,3,0)
    maxInitiatorsEditor_.setPlaceholderText(tr("unbound"));
    maxTargetsEditor_.setPlaceholderText(tr("unbound"));
#endif

    documentNameGroupEditor_.setTitle("Bus definition");

    setupLayout();

    connect(&maxInitiatorsEditor_, SIGNAL(editingFinished()), this, SLOT(onInitiatorsChanged()), Qt::UniqueConnection);
    connect(&maxTargetsEditor_, SIGNAL(editingFinished()),	this, SLOT(onTargetsChanged()), Qt::UniqueConnection);
     
    connect(&directConnection_, SIGNAL(toggled(bool)),
        this, SLOT(onDirectConnectionChanged(bool)), Qt::UniqueConnection);
    connect(&isBroadcast_, SIGNAL(toggled(bool)), this, SLOT(onIsBroadcastChanged(bool)), Qt::UniqueConnection);
    connect(&isAddressable_, SIGNAL(toggled(bool)),
        this, SLOT(onIsAddressableChanged(bool)), Qt::UniqueConnection);

    connect(&systemGroupEditor_, SIGNAL(contentChanged()),
        this, SLOT(onSystemNamesChanged()), Qt::UniqueConnection);

    connect(&documentNameGroupEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(extendEditor_, SIGNAL(vlnvEdited()), this, SLOT(onExtendChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::setBusDef()
//-----------------------------------------------------------------------------
void BusDefGroup::setBusDef( QSharedPointer<BusDefinition> busDef )
{
    busDef_ = busDef;
    documentNameGroupEditor_.setDocumentNameGroup(busDef, library_->getPath(busDef->getVlnv()));

    extendEditor_->setVLNV(busDef_->getExtends());

    if (busDef_->getExtends().isValid())
    {
        setupExtendedBus();
    }

    directConnection_.setChecked(busDef_->getDirectConnection());
    isBroadcast_.setChecked(busDef_->getBroadcast().toBool());
    isAddressable_.setChecked(busDef_->getIsAddressable());

    maxInitiatorsEditor_.setText(QString::fromStdString(busDef_->getMaxInitiators()));
    maxTargetsEditor_.setText(QString::fromStdString(busDef_->getMaxTargets()));

    systemGroupEditor_.setItems(busDef_);

}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onDirectConnectionChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onDirectConnectionChanged(bool checked)
{
    busDef_->setDirectConnection(checked);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onIsBroadcastChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onIsBroadcastChanged(bool checked)
{
    busDef_->setBroadcast(checked);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onIsAddressableChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onIsAddressableChanged(bool checked)
{
    busDef_->setIsAddressable(checked);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onInitiatorsChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onInitiatorsChanged()
{
    busDef_->setMaxInitiators(maxInitiatorsEditor_.text().toStdString());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onTargetsChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onTargetsChanged()
{
    busDef_->setMaxTargets(maxTargetsEditor_.text().toStdString());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onSystemNamesChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onSystemNamesChanged()
{
    QStringList systemGroupNames;
    for (int i = 0; i < systemGroupEditor_.count(); ++i)
    {
        QListWidgetItem* systemItem = systemGroupEditor_.item(i);
        QString systemName = systemItem->text();
        if (!systemGroupNames.contains(systemName))
        {
            systemGroupNames.append(systemName);
        }
    }

    busDef_->setSystemGroupNames(systemGroupNames);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onDescriptionChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onDescriptionChanged()
{
    //busDef_->setDescription(descriptionEditor_.toPlainText());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: busdefgroup::onExtendChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onExtendChanged()
{
    VLNV newExtendVLNV = extendEditor_->getVLNV();

    removeSystemGroupsFromExtendedDefinition();

    busDef_->setExtends(extendEditor_->getVLNV());
    setupExtendedBus();

    systemGroupEditor_.setItems(busDef_);
    onSystemNamesChanged();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: busdefgroup::removeSystemGroupsFromExtendedDefinition()
//-----------------------------------------------------------------------------
void BusDefGroup::removeSystemGroupsFromExtendedDefinition()
{
    QSharedPointer<const BusDefinition> extendedBus = getExtendedBus(busDef_);
    if (extendedBus)
    {
        QStringList currentSystemGroups = busDef_->getSystemGroupNames();
        QStringList extendSystemGroups = BusDefinitionUtils::getSystemGroups(extendedBus, library_);
        for (auto extendSystem : extendSystemGroups)
        {
            if (currentSystemGroups.contains(extendSystem))
            {
                currentSystemGroups.removeAll(extendSystem);
            }
        }

        busDef_->setSystemGroupNames(currentSystemGroups);
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::setupLayout()
//-----------------------------------------------------------------------------
void BusDefGroup::setupLayout()
{
    QFormLayout* initiatorTargetLayout = new QFormLayout();
    initiatorTargetLayout->addRow(tr("Max initiators on bus:"), &maxInitiatorsEditor_);
    initiatorTargetLayout->addRow(tr("Max targets on bus:"), &maxTargetsEditor_);

    QGroupBox* selectionGroup = new QGroupBox(tr("Constraints"), this);

    QVBoxLayout* selectionsLayout = new QVBoxLayout(selectionGroup);
    selectionsLayout->addWidget(&directConnection_);
    selectionsLayout->addWidget(&isBroadcast_);
    selectionsLayout->addWidget(&isAddressable_);
    selectionsLayout->addLayout(initiatorTargetLayout);
    selectionsLayout->addStretch();    

    QGroupBox* systemGroupBox = new QGroupBox(tr("System group names"), this);

    QVBoxLayout* systemGroupLayout = new QVBoxLayout(systemGroupBox);
    systemGroupLayout->addWidget(&systemGroupEditor_);

    QGridLayout* topLayout = new QGridLayout(this);

    QWidget* rightSideContainer = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout();
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->addWidget(extendEditor_);
    containerLayout->addWidget(selectionGroup);
    rightSideContainer->setLayout(containerLayout);

    topLayout->addWidget(&documentNameGroupEditor_, 0, 0, 1, 1);
    topLayout->addWidget(rightSideContainer, 0, 1, 1, 1);
    topLayout->addWidget(systemGroupBox, 1, 0, 1, 1);

    topLayout->setColumnStretch(0, 1);
    topLayout->setColumnStretch(1, 1);

    maxInitiatorsEditor_.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    maxTargetsEditor_.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//-----------------------------------------------------------------------------
// Function: busdefgroup::setupExtendedBus()
//-----------------------------------------------------------------------------
void BusDefGroup::setupExtendedBus()
{
    QSharedPointer<const BusDefinition> extendedBus = getExtendedBus(busDef_);
    if (extendedBus)
    {
        extendBusDefinition(extendedBus);
        //descriptionEditor_.setPlaceholderText(extendedBus->getDescription());

        return;
    }

    removeBusExtension();
    //descriptionEditor_.setPlaceholderText(QString());
}

//-----------------------------------------------------------------------------
// Function: busdefgroup::getExtendedBus()
//-----------------------------------------------------------------------------
QSharedPointer<const BusDefinition> BusDefGroup::getExtendedBus(QSharedPointer<const BusDefinition> busDefinition)
    const
{
    VLNV extendedVLNV = busDefinition->getExtends();
    if (extendedVLNV.isValid() && extendedVLNV.getType() == VLNV::BUSDEFINITION)
    {
        QSharedPointer<const Document> extendedDocument = library_->getModelReadOnly(extendedVLNV);
        if (extendedDocument)
        {
            QSharedPointer<const BusDefinition> extendedBus = extendedDocument.dynamicCast<const BusDefinition>();
            if (extendedBus)
            {
                return extendedBus;
            }
        }
    }

    return QSharedPointer<const BusDefinition>();
}

//-----------------------------------------------------------------------------
// Function: busdefgroup::extendBusDefinition()
//-----------------------------------------------------------------------------
void BusDefGroup::extendBusDefinition(QSharedPointer<const BusDefinition> extendedBus)
{
    directConnection_.setChecked(extendedBus->getDirectConnection());
    isBroadcast_.setChecked(extendedBus->getBroadcast().toBool());
    isAddressable_.setChecked(extendedBus->getIsAddressable());

    directConnection_.setDisabled(true);
    isBroadcast_.setDisabled(true);
    isAddressable_.setDisabled(true);
}

//-----------------------------------------------------------------------------
// Function: busdefgroup::removeBusExtension()
//-----------------------------------------------------------------------------
void BusDefGroup::removeBusExtension()
{
    directConnection_.setDisabled(false);
    isBroadcast_.setDisabled(false);
    isAddressable_.setDisabled(false);
}
