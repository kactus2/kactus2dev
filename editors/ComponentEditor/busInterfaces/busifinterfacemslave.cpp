/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemslave.cpp
 */

#include "busifinterfacemslave.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ValueFormatter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <QLabel>
#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::BusIfInterfaceMSlave()
//-----------------------------------------------------------------------------
BusIfInterfaceMSlave::BusIfInterfaceMSlave(QSharedPointer<BusInterface> busif, 
										   QSharedPointer<Component> component,
                                           QSharedPointer<ParameterFinder> parameterFinder,
                                           QSharedPointer<ExpressionParser> expressionParser,
										   QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("Mirrored slave"), parent),
mirroredSlave_(QSharedPointer<MirroredSlaveInterface>(new MirroredSlaveInterface())),
remapEditor_(new ExpressionEditor(parameterFinder, this)),
rangeEditor_(new ExpressionEditor(parameterFinder, this)),
expressionParser_(expressionParser),
parameterFinder_(parameterFinder)
{
    QString functionSymbol(0x0192);
	QLabel* remapLabel = new QLabel(tr("Remap address") + ", " + functionSymbol + "(x)", this);
	QLabel* rangeLabel = new QLabel(tr("Range") + ", " + functionSymbol + "(x)", this);

    remapEditor_->setFixedHeight(20);
    rangeEditor_->setFixedHeight(20);

    remapEditor_->setFrameShadow(QFrame::Sunken);
    rangeEditor_->setFrameShadow(QFrame::Sunken);

    ComponentParameterModel* componentParameterModel = new ComponentParameterModel(parameterFinder, this);
    componentParameterModel->setExpressionParser(expressionParser);

    ParameterCompleter* remapCompleter = new ParameterCompleter(this);
    remapCompleter->setModel(componentParameterModel);
    remapEditor_->setAppendingCompleter(remapCompleter);

    ParameterCompleter* rangeCompleter = new ParameterCompleter(this);
    rangeCompleter->setModel(componentParameterModel);
    rangeEditor_->setAppendingCompleter(rangeCompleter);

	QGridLayout* topLayout = new QGridLayout(this);
	topLayout->addWidget(remapLabel, 0, 0, Qt::AlignLeft);
    topLayout->addWidget(remapEditor_, 0, 1, Qt::AlignLeft);
	topLayout->addWidget(rangeLabel, 1, 0, Qt::AlignLeft);
    topLayout->addWidget(rangeEditor_, 1, 1, Qt::AlignLeft);

	topLayout->setColumnStretch(2, 1);
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
// Function: busifinterfacemslave::~BusIfInterfaceMSlave()
//-----------------------------------------------------------------------------
BusIfInterfaceMSlave::~BusIfInterfaceMSlave()
{

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
	// if the model contains master-element
	if (busif_->getMirroredSlave())
    {
		mirroredSlave_ = busif_->getMirroredSlave();
	}
	else
    {
		mirroredSlave_.clear();
		mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(new MirroredSlaveInterface());
	}

    rangeEditor_->blockSignals(true);

    rangeEditor_->setExpression(mirroredSlave_->getRange());
    rangeEditor_->setToolTip(formattedValueFor(mirroredSlave_->getRange()));

    rangeEditor_->blockSignals(false);

    remapEditor_->blockSignals(true);

    remapEditor_->setExpression(mirroredSlave_->getRemapAddress());
    remapEditor_->setToolTip(formattedValueFor(mirroredSlave_->getRemapAddress()));

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
	busif_->setMirroredSlave(mirroredSlave_);
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::onRemapChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::onRemapChange()
{
    remapEditor_->finishEditingCurrentWord();
    mirroredSlave_->setRemapAddress(remapEditor_->getExpression());
    remapEditor_->setToolTip(formattedValueFor(remapEditor_->getExpression()));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::onRangeChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceMSlave::onRangeChange()
{
    rangeEditor_->finishEditingCurrentWord();
    mirroredSlave_->setRange(rangeEditor_->getExpression());
    rangeEditor_->setToolTip(formattedValueFor(rangeEditor_->getExpression()));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemslave::formattedValueFor()
//-----------------------------------------------------------------------------
QString BusIfInterfaceMSlave::formattedValueFor(QString const& expression) const
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
    mirroredSlave_->setRemapAddress("");

    rangeEditor_->clear();
    mirroredSlave_->setRange("");
}
