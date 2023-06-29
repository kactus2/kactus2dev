//-----------------------------------------------------------------------------
// File: busifinterfacemaster.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to edit master or mirrored master settings of a bus interface.
//-----------------------------------------------------------------------------

#include "busifinterfacemaster.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <KactusAPI/include/ExpressionParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>
#include <KactusAPI/include/BusInterfaceInterface.h>

#include <QLabel>
#include <QGridLayout>
#include <QString>

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::BusIfInterfaceMaster()
//-----------------------------------------------------------------------------
BusIfInterfaceMaster::BusIfInterfaceMaster(General::InterfaceMode mode,
    BusInterfaceInterface* busInterface, std::string const& busName,
    QSharedPointer<Component> component, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser, QWidget *parent):
BusIfInterfaceModeEditor(busInterface, busName, tr("Master"), parent),
addressSpaceReferenceSelector_(this),
baseAddressEditor_(new ExpressionEditor(parameterFinder, this)),
parameterFinder_(parameterFinder),
component_(component)
{
    baseAddressEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParameterModel = new ComponentParameterModel(parameterFinder, this);
    componentParameterModel->setExpressionParser(expressionParser);
    ParameterCompleter* baseAddressCompleter = new ParameterCompleter(this);
    baseAddressCompleter->setModel(componentParameterModel);
    baseAddressEditor_->setAppendingCompleter(baseAddressCompleter);

    // set the title depending on the mode
    if (mode == General::MASTER)
    {
        setTitle(tr("Master"));
    }
    else if (mode == General::MIRRORED_MASTER)
    {
        setTitle(tr("Mirrored master"));
    }
    // this editor should only be used for masters and mirrored masters
    else
    {
        Q_ASSERT(false);
    }

	QLabel* addrSpaceLabel = new QLabel(tr("Address space:"), this);

    QString functionSymbol(QChar(0x0192));
	QLabel* baseAddrLabel = new QLabel(tr("Base address") + ", " + functionSymbol +"(x):", this);

	QGridLayout* topLayout = new QGridLayout(this);
	topLayout->addWidget(addrSpaceLabel, 0, 0, 1, 1);
	topLayout->addWidget(&addressSpaceReferenceSelector_, 0, 1, 1, 1);
	topLayout->addWidget(baseAddrLabel, 1, 0, 1, 1);
    topLayout->addWidget(baseAddressEditor_, 1, 1, 1, 1);
    topLayout->setColumnStretch(1, 1);
	topLayout->setRowStretch(2, 10);

	connect(&addressSpaceReferenceSelector_, SIGNAL(itemSelected(const QString&)),
		this, SLOT(onAddressSpaceChange(const QString&)), Qt::UniqueConnection);
    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SLOT(onBaseAddressChange()), Qt::UniqueConnection);

    connect(baseAddressEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(baseAddressEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::~BusIfInterfaceMaster()
//-----------------------------------------------------------------------------
BusIfInterfaceMaster::~BusIfInterfaceMaster()
{
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::isValid()
//-----------------------------------------------------------------------------
bool BusIfInterfaceMaster::isValid() const
{
	QString selectedAddrSpace = addressSpaceReferenceSelector_.currentText();

	// address space ref is not mandatory
	if (selectedAddrSpace.isEmpty())
    {
		return true;
	}
	
	// if the selected address space does not belong to component
    QStringList addrSpaceNames = component_->getAddressSpaceNames();
	if (!addrSpaceNames.contains(selectedAddrSpace))
    {
		return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::refresh()
//-----------------------------------------------------------------------------
void BusIfInterfaceMaster::refresh()
{
    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();


	// update the selectable items
    addressSpaceReferenceSelector_.refresh(component_->getAddressSpaceNames());
    QString addrSpaceRef = QString::fromStdString(busInterface->getAddressSpaceReference(busName));

	// if address space ref is empty then there can be no base address
	if (addrSpaceRef.isEmpty())
    {
        baseAddressEditor_->setDisabled(true);
	}
	else 
    {
        baseAddressEditor_->blockSignals(true);

        baseAddressEditor_->setEnabled(true);
        baseAddressEditor_->setExpression(QString::fromStdString(busInterface->getBaseAddressExpression(busName)));
        baseAddressEditor_->setToolTip(
            QString::fromStdString(busInterface->getBaseAddressValue(busName)));

        baseAddressEditor_->blockSignals(false);
	}

	// select the address space ref and base address
	addressSpaceReferenceSelector_.selectItem(addrSpaceRef);
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusIfInterfaceMaster::getInterfaceMode() const
{
    return General::MASTER;
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::onAddressSpaceChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceMaster::onAddressSpaceChange(const QString& addrSpaceName)
{
    getBusInterface()->setAddressSpaceReference(getBusName(), addrSpaceName.toStdString());

	// if address space reference is empty then there can be no base address
	if (addrSpaceName.isEmpty())
    {
        removeReferencesFromExpressions();

        baseAddressEditor_->setDisabled(true);
	}
	else
    {
        baseAddressEditor_->setEnabled(true);
	}

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::onBaseAddressChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceMaster::onBaseAddressChange()
{
    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    baseAddressEditor_->finishEditingCurrentWord();
    busInterface->setBaseAddress(busName, baseAddressEditor_->getExpression().toStdString());
    baseAddressEditor_->setToolTip(
        QString::fromStdString(busInterface->getBaseAddressValue(busName)));

    emit contentChanged();
}	

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::saveModeSpecific()
//-----------------------------------------------------------------------------
void BusIfInterfaceMaster::saveModeSpecific()
{
    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    busInterface->setAddressSpaceReference(busName, addressSpaceReferenceSelector_.currentText().toStdString());
    busInterface->setBaseAddress(busName, baseAddressEditor_->getExpression().toStdString());
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemaster::removeReferencesFromExpressions()
//-----------------------------------------------------------------------------
void BusIfInterfaceMaster::removeReferencesFromExpressions()
{
    QStringList baseAddressExpression;
    baseAddressExpression.append(baseAddressEditor_->getExpression());

    ReferenceCalculator referenceCalculator(parameterFinder_);
    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(baseAddressExpression);

    foreach (QString referencedId, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedId); ++i)
        {
            emit decreaseReferences(referencedId);
        }
    }

    baseAddressEditor_->clear();
    getBusInterface()->setBaseAddress(getBusName(), "");
}
