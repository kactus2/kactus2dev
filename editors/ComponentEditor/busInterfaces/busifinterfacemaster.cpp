//-----------------------------------------------------------------------------
// File: busifinterfacemaster.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to edit master or mirrored master settings of a bus interface.
//-----------------------------------------------------------------------------

#include "busifinterfacemaster.h"

#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/masterinterface.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ValueFormatter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <QLabel>
#include <QGridLayout>
#include <QString>

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::BusIfInterfaceMaster()
//-----------------------------------------------------------------------------
BusIfInterfaceMaster::BusIfInterfaceMaster(General::InterfaceMode mode, QSharedPointer<BusInterface> busif,
                                           QSharedPointer<Component> component,
                                           QSharedPointer<ParameterFinder> parameterFinder,
                                           QSharedPointer<ExpressionParser> expressionParser, QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("Master"), parent), 
master_(QSharedPointer<MasterInterface>(new MasterInterface())),
mode_(mode),
addressSpaceReferenceSelector_(this),
baseAddressEditor_(new ExpressionEditor(parameterFinder, this)),
expressionParser_(expressionParser),
parameterFinder_(parameterFinder)
{
    baseAddressEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParameterModel = new ComponentParameterModel(parameterFinder, this);
    componentParameterModel->setExpressionParser(expressionParser_);
    ParameterCompleter* baseAddressCompleter = new ParameterCompleter(this);
    baseAddressCompleter->setModel(componentParameterModel);
    baseAddressEditor_->setAppendingCompleter(baseAddressCompleter);

    baseAddressEditor_->setFrameShadow(QFrame::Sunken);

    // set the title depending on the mode
    if (mode == General::MASTER)
    {
        setTitle(tr("Master"));
    }
    else if (mode == General::MIRROREDMASTER)
    {
        setTitle(tr("Mirrored master"));
    }
    // this editor should only be used for masters and mirrored masters
    else
    {
        Q_ASSERT(false);
    }

	QLabel* addrSpaceLabel = new QLabel(tr("Address space"), this);

    QString functionSymbol(0x0192);
	QLabel* baseAddrLabel = new QLabel(tr("Base address") + ", " + functionSymbol +"(x)", this);

	QGridLayout* topLayout = new QGridLayout(this);
	topLayout->addWidget(addrSpaceLabel, 0, 0, Qt::AlignLeft);
	topLayout->addWidget(&addressSpaceReferenceSelector_, 0, 1, Qt::AlignLeft);
	topLayout->addWidget(baseAddrLabel, 1, 0, Qt::AlignLeft);
    topLayout->addWidget(baseAddressEditor_, 1, 1, Qt::AlignLeft);
    topLayout->setColumnStretch(2, 1);
	topLayout->setRowStretch(2, 1);

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
	// if the model contains master-element
	if (busif_->getMaster())
    {
		master_ = busif_->getMaster();
	}
	else
    {
		master_.clear();
		master_ = QSharedPointer<MasterInterface>(new MasterInterface());
	}

	// update the selectable items
	addressSpaceReferenceSelector_.refresh(component_->getAddressSpaceNames());

	QString addrSpaceRef = master_->getAddressSpaceRef();

	// if address space ref is empty then there can be no base address
	if (addrSpaceRef.isEmpty())
    {
        baseAddressEditor_->setDisabled(true);
	}
	else 
    {
        baseAddressEditor_->blockSignals(true);

        baseAddressEditor_->setEnabled(true);
        baseAddressEditor_->setExpression(master_->getBaseAddress());
        baseAddressEditor_->setToolTip(formattedValueFor(master_->getBaseAddress()));

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
	return mode_;
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::onAddressSpaceChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceMaster::onAddressSpaceChange(const QString& addrSpaceName)
{
	master_->setAddressSpaceRef(addrSpaceName);

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
    baseAddressEditor_->finishEditingCurrentWord();
    master_->setBaseAddress(baseAddressEditor_->getExpression());
    baseAddressEditor_->setToolTip(formattedValueFor(baseAddressEditor_->getExpression()));

    emit contentChanged();
}	

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::saveModeSpecific()
//-----------------------------------------------------------------------------
void BusIfInterfaceMaster::saveModeSpecific()
{
	busif_->setMaster(master_);
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemaster::formattedValueFor()
//-----------------------------------------------------------------------------
QString BusIfInterfaceMaster::formattedValueFor(QString const& expression) const
{
    if (expressionParser_->isValidExpression(expression))
    {
        ValueFormatter formatter;
        return formatter.format(expressionParser_->parseExpression(expression),
            expressionParser_->baseForExpression(expression));
    }
    else if (expressionParser_->isPlainValue(expression))
    {
        return expression;
    }
    else
    {
        return "n/a";
    }
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
    master_->setBaseAddress("");
}
