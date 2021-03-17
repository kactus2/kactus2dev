//-----------------------------------------------------------------------------
// File: busIfInterfaceMSlave.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to edit mirrored slave details of a bus interface.
//-----------------------------------------------------------------------------

#include "busifinterfacemslave.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/BusInterfaceInterface.h>

#include <QLabel>
#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::BusIfInterfaceMSlave()
//-----------------------------------------------------------------------------
BusIfInterfaceMSlave::BusIfInterfaceMSlave(BusInterfaceInterface* busInterface, std::string const& busName,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
    QWidget *parent):
BusIfInterfaceModeEditor(busInterface, busName, tr("Mirrored slave"), parent),
remapEditor_(new ExpressionEditor(parameterFinder, this)),
rangeEditor_(new ExpressionEditor(parameterFinder, this)),
expressionParser_(expressionParser),
parameterFinder_(parameterFinder)
{
    QString functionSymbol(0x0192);
	QLabel* remapLabel = new QLabel(tr("Remap address") + ", " + functionSymbol + "(x):", this);
	QLabel* rangeLabel = new QLabel(tr("Range") + ", " + functionSymbol + "(x):", this);

    remapEditor_->setFixedHeight(20);
    rangeEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParameterModel = new ComponentParameterModel(parameterFinder, this);
    componentParameterModel->setExpressionParser(expressionParser);

    ParameterCompleter* remapCompleter = new ParameterCompleter(this);
    remapCompleter->setModel(componentParameterModel);
    remapEditor_->setAppendingCompleter(remapCompleter);

    ParameterCompleter* rangeCompleter = new ParameterCompleter(this);
    rangeCompleter->setModel(componentParameterModel);
    rangeEditor_->setAppendingCompleter(rangeCompleter);

	QGridLayout* topLayout = new QGridLayout(this);
	topLayout->addWidget(remapLabel, 0, 0, 1, 1);
    topLayout->addWidget(remapEditor_, 0, 1, 1, 1);
	topLayout->addWidget(rangeLabel, 1, 0, 1, 1);
    topLayout->addWidget(rangeEditor_, 1, 1, 1, 1);

	topLayout->setColumnStretch(1, 1);
	topLayout->setRowStretch(2, 1);

    connect(remapEditor_, SIGNAL(editingFinished()), this, SLOT(onRemapChange()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SLOT(onRangeChange()), Qt::UniqueConnection);

    connect(remapEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(remapEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::isValid()
//-----------------------------------------------------------------------------
bool BusIfInterfaceMSlave::isValid() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::refresh()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::refresh()
{
    rangeEditor_->blockSignals(true);

    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    rangeEditor_->setExpression(QString::fromStdString(busInterface->getRangeExpression(busName)));
    rangeEditor_->setToolTip(QString::fromStdString(busInterface->getRangeValue(busName)));

    rangeEditor_->blockSignals(false);

    remapEditor_->blockSignals(true);

    remapEditor_->setExpression(QString::fromStdString(busInterface->getRemapAddressExpression(busName)));
    remapEditor_->setToolTip(QString::fromStdString(busInterface->getRemapAddressValue(busName)));

    remapEditor_->blockSignals(false);
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusIfInterfaceMSlave::getInterfaceMode() const
{
	return General::MIRROREDSLAVE;
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::saveModeSpecific()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::saveModeSpecific()
{
    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    busInterface->setRange(busName, rangeEditor_->getExpression().toStdString());
    busInterface->setRemapAddress(busName, remapEditor_->getExpression().toStdString());
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::onRemapChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::onRemapChange()
{
    remapEditor_->finishEditingCurrentWord();

    QString newRemapAddress = remapEditor_->getExpression();

    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    busInterface->setRemapAddress(busName, newRemapAddress.toStdString());

    remapEditor_->setToolTip(QString::fromStdString(busInterface->getRemapAddressValue(busName)));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::onRangeChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::onRangeChange()
{
    rangeEditor_->finishEditingCurrentWord();

    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    busInterface->setRange(busName, rangeEditor_->getExpression().toStdString());
    rangeEditor_->setToolTip(QString::fromStdString(busInterface->getRangeValue(busName)));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::removeReferencesFromExpressions()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::removeReferencesFromExpressions()
{
    QStringList expressionList;
    expressionList.append(rangeEditor_->getExpression());
    expressionList.append(remapEditor_->getExpression());

    ReferenceCalculator referenceCalculator(parameterFinder_);
    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(expressionList);

    foreach (QString referencedId, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedId); ++i)
        {
            emit decreaseReferences(referencedId);
        }
    }

    remapEditor_->clear();

    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    busInterface->setRemapAddress(busName, "");

    rangeEditor_->clear();
    busInterface->setRange(busName, "");
}
