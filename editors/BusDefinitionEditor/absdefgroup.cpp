//-----------------------------------------------------------------------------
// File: absdefgroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 15.8.2011
//
// Description:
// Editor for the logical ports of an abstraction definition.
//-----------------------------------------------------------------------------

#include "absdefgroup.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>
#include <KactusAPI/include/PortAbstractionInterface.h>

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <editors/BusDefinitionEditor/AbstractionDefinitionPortsSortFilter.h>
#include <editors/BusDefinitionEditor/AbstractionPortsModel.h>
#include <editors/BusDefinitionEditor/LogicalPortColumns.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::AbsDefGroup()
//-----------------------------------------------------------------------------
AbsDefGroup::AbsDefGroup(LibraryInterface* libraryHandler, PortAbstractionInterface* portInterface,
    PortAbstractionInterface* extendInterface,
    QWidget *parent):
QWidget(parent),
vlnvDisplay_(new VLNVDisplayer(this, VLNV())),
extendEditor_(new VLNVEditor(VLNV::ABSTRACTIONDEFINITION, libraryHandler, this, this)),
descriptionEditor_(new QPlainTextEdit(this)),
portTabs_(this),
portInterface_(portInterface),
extendInterface_(extendInterface),
portModel_(new AbstractionPortsModel(libraryHandler, portInterface, extendInterface, this)),
wirePortsEditor_(new AbstractionPortsEditor(libraryHandler, portInterface, portModel_, LogicalPortColumns::AbstractionType::WIRE, &portTabs_)),
transactionalPortsEditor_(new AbstractionPortsEditor(libraryHandler, portInterface, portModel_, LogicalPortColumns::AbstractionType::TRANSACTIONAL, &portTabs_)),
abstraction_(),
libraryHandler_(libraryHandler)
{
    vlnvDisplay_->setTitle(QStringLiteral("Abstraction definition"));
    extendEditor_->setTitle(tr("Extended abstraction definition"));
    extendEditor_->setMandatory(false);

    portTabs_.addTab(wirePortsEditor_, QStringLiteral("Wire ports"));
    portTabs_.addTab(transactionalPortsEditor_, QStringLiteral("Transactional ports"));

    connect(wirePortsEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(wirePortsEditor_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(wirePortsEditor_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(wirePortsEditor_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)),
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);

    connect(transactionalPortsEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(transactionalPortsEditor_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(transactionalPortsEditor_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(transactionalPortsEditor_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)),
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);

    connect(descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);

    connect(extendEditor_, SIGNAL(vlnvEdited()), this, SLOT(onExtendChanged()), Qt::UniqueConnection);

	setupLayout();
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::save()
//-----------------------------------------------------------------------------
void AbsDefGroup::save()
{
    portInterface_->save();
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::setAbsDef()
//-----------------------------------------------------------------------------
void AbsDefGroup::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
    abstraction_ = absDef;

    portInterface_->setAbsDef(abstraction_);

    wirePortsEditor_->resetPortModel();
    transactionalPortsEditor_->resetPortModel();

    vlnvDisplay_->setVLNV(absDef->getVlnv());

    extendEditor_->setVLNV(absDef->getExtends());

    if (abstractionContainsTransactionalPorts())
    {
        portTabs_.setCurrentWidget(transactionalPortsEditor_);
    }

    if (absDef->getExtends().isValid())
    {
        setupExtendedAbstraction();
    }

    if (!absDef->getDescription().isEmpty())
    {
        descriptionEditor_->setPlainText(absDef->getDescription());
    }
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::abstractionContainsTransactionalPorts()
//-----------------------------------------------------------------------------
bool AbsDefGroup::abstractionContainsTransactionalPorts() const
{
    for (QSharedPointer<PortAbstraction> logicalPort : *abstraction_->getLogicalPorts())
    {
        if (logicalPort->hasTransactional())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::setBusDef()
//-----------------------------------------------------------------------------
void AbsDefGroup::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    wirePortsEditor_->setBusDef(busDefinition);
    transactionalPortsEditor_->setBusDef(busDefinition);
}

//-----------------------------------------------------------------------------
// Function: AbsDefGroup::setupLayout()
//-----------------------------------------------------------------------------
void AbsDefGroup::setupLayout()
{
    QGroupBox* descriptionGroup = new QGroupBox(tr("Description"), this);

    QVBoxLayout* descriptionLayout = new QVBoxLayout(descriptionGroup);
    descriptionLayout->addWidget(descriptionEditor_);

    QGridLayout* topLayout = new QGridLayout(this);
    topLayout->addWidget(vlnvDisplay_, 0, 0, 1, 1);
    topLayout->addWidget(extendEditor_, 0, 1, 1, 1);
    topLayout->addWidget(descriptionGroup, 0, 2, 1, 1);
    topLayout->addWidget(&portTabs_, 1, 0, 1, 3);

    topLayout->setColumnStretch(0, 25);
    topLayout->setColumnStretch(1, 25);
    topLayout->setColumnStretch(2, 50);

    topLayout->setRowStretch(0, 1);
    topLayout->setRowStretch(1, 10);
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::onDescriptionChanged()
//-----------------------------------------------------------------------------
void AbsDefGroup::onDescriptionChanged()
{
    abstraction_->setDescription(descriptionEditor_->toPlainText());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::onExtendChanged()
//-----------------------------------------------------------------------------
void AbsDefGroup::onExtendChanged()
{
    removeSignalsFromExtendedDefinition();

    abstraction_->setExtends(extendEditor_->getVLNV());
    setupExtendedAbstraction();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::setupExtendedAbstraction()
//-----------------------------------------------------------------------------
void AbsDefGroup::setupExtendedAbstraction()
{
    QSharedPointer<const AbstractionDefinition> extendedAbstraction = getExtendedAbstraction();
    extendInterface_->setAbsDef(extendedAbstraction);
    QString extendDescription = "";
    if (extendedAbstraction)
    {
        extendDescription = extendedAbstraction->getDescription();

        if (extendedAbstraction->getLogicalPorts() && !extendedAbstraction->getLogicalPorts()->isEmpty())
        {
            extendSignals(extendedAbstraction);
        }
    }

#if QT_VERSION > QT_VERSION_CHECK(5,3,0)
    descriptionEditor_->setPlaceholderText(extendDescription);
# endif
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::getExtendedAbstraction()
//-----------------------------------------------------------------------------
QSharedPointer<const AbstractionDefinition> AbsDefGroup::getExtendedAbstraction() const
{
    VLNV extendedVLNV = abstraction_->getExtends();
    if (extendedVLNV.isValid() && extendedVLNV.getType() == VLNV::ABSTRACTIONDEFINITION)
    {
        QSharedPointer<const Document> extendedDocument = libraryHandler_->getModelReadOnly(extendedVLNV);
        if (extendedDocument)
        {
            QSharedPointer<const AbstractionDefinition> extendAbstraction =
                extendedDocument.dynamicCast<const AbstractionDefinition>();
            if (extendAbstraction)
            {
                return extendAbstraction;
            }
        }
    }

    return QSharedPointer<const AbstractionDefinition>();
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::extendSignals()
//-----------------------------------------------------------------------------
void AbsDefGroup::extendSignals(QSharedPointer<const AbstractionDefinition> /*extendAbstraction*/)
{
    portModel_->setExtendedPorts();
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::removeSignalsFromExtendedDefinition()
//-----------------------------------------------------------------------------
void AbsDefGroup::removeSignalsFromExtendedDefinition()
{
    if (abstraction_->getExtends() != extendEditor_->getVLNV() || extendEditor_->getVLNV().isEmpty())
    {
        QSharedPointer<const AbstractionDefinition> extendedAbstraction = getExtendedAbstraction();
        if (extendedAbstraction || extendEditor_->getVLNV().isEmpty())
        {
            portModel_->removeExtendedPorts();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: absdefgroup::portIsWithinPorts()
//-----------------------------------------------------------------------------
bool AbsDefGroup::portIsWithinPorts(QSharedPointer<PortAbstraction> logicalPort,
    QList<QSharedPointer<PortAbstraction> > const& currentPorts)
{
    for (auto comparisonPort : currentPorts)
    {
        if (comparisonPort->name() == logicalPort->name())
        {
            return true;
        }
    }

    return false;
}
