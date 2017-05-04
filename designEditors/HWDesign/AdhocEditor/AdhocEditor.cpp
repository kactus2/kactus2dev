//-----------------------------------------------------------------------------
// File: AdhocEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2016
//
// Description:
// Editor to edit the details of an Ad hoc port in designAd-hoc editor.
//-----------------------------------------------------------------------------

#include "AdhocEditor.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/GenericEditProvider.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <designEditors/HWDesign/AdHocItem.h>
#include <designEditors/HWDesign/AdHocPortItem.h>
#include <designEditors/HWDesign/AdHocInterfaceItem.h>
#include <designEditors/HWDesign/HWDesignDiagram.h>
#include <designEditors/HWDesign/undoCommands/AdHocTieOffChangeCommand.h>
#include <designEditors/HWDesign/undoCommands/AdHocTieOffBoundsChangeCommand.h>

#include <IPXACTmodels/common/validators/ValueFormatter.h>
#include <IPXACTmodels/Design/Design.h>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QGroupBox>

//-----------------------------------------------------------------------------
// Function: AdhocEditor::AdHocEditor()
//-----------------------------------------------------------------------------
AdHocEditor::AdHocEditor(QWidget* parent):
QWidget(parent),
componentFinder_(new ComponentParameterFinder(QSharedPointer<Component>(0))),
expressionParser_(new IPXactSystemVerilogParser(componentFinder_)),
expressionFormatter_(new ExpressionFormatter(componentFinder_)),
portName_(new QLabel(this)),
portDirection_(new QLabel(this)),
leftBoundValue_(new QLabel(this)),
rightBoundValue_(new QLabel(this)),
tiedValueEditor_(new ExpressionEditor(componentFinder_, this)),
containedPortItem_(),
editProvider_(),
designDiagram_(),
tiedValueLeftBoundEditor_(new ExpressionEditor(componentFinder_, this)),
tiedValueRightBoundEditor_(new ExpressionEditor(componentFinder_, this))
{
    tiedValueEditor_->setFixedHeight(20);

    tiedValueLeftBoundEditor_->setFixedHeight(20);
    tiedValueRightBoundEditor_->setFixedHeight(20);

    ComponentParameterModel* parameterModel = new ComponentParameterModel(componentFinder_, this);
    parameterModel->setExpressionParser(expressionParser_);

    ParameterCompleter* tiedValueCompleter = new ParameterCompleter(this);
    tiedValueCompleter->setModel(parameterModel);

    ParameterCompleter* tiedValueLeftBoundCompleter = new ParameterCompleter(this);
    tiedValueLeftBoundCompleter->setModel(parameterModel);

    ParameterCompleter* tiedValueRightBoundCompleter = new ParameterCompleter(this);
    tiedValueRightBoundCompleter->setModel(parameterModel);

    tiedValueEditor_->setAppendingCompleter(tiedValueCompleter);

    tiedValueLeftBoundEditor_->setAppendingCompleter(tiedValueLeftBoundCompleter);
    tiedValueRightBoundEditor_->setAppendingCompleter(tiedValueRightBoundCompleter);

    QStringList reservedWordsForTiedValue;
    reservedWordsForTiedValue << "open" << "default";
    tiedValueEditor_->setReservedWords(reservedWordsForTiedValue);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    setupLayout();

    connect(tiedValueEditor_, SIGNAL(editingFinished()), this, SLOT(onTiedValueChanged()), Qt::UniqueConnection);
    connect(tiedValueLeftBoundEditor_, SIGNAL(editingFinished()),
        this, SLOT(onTiedValueLeftBoundChanged()), Qt::UniqueConnection);
    connect(tiedValueRightBoundEditor_, SIGNAL(editingFinished()),
        this, SLOT(onTiedValueRightBoundChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AdHocEditor::~AdHocEditor()
//-----------------------------------------------------------------------------
AdHocEditor::~AdHocEditor()
{

}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::setupLayout()
//-----------------------------------------------------------------------------
void AdHocEditor::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout();
    QFormLayout* overallLayout = new QFormLayout();

    overallLayout->addRow(tr("Name:"), portName_);
    overallLayout->addRow(tr("Direction:"), portDirection_);
    overallLayout->addRow(tr("Left bound:"), leftBoundValue_);
    overallLayout->addRow(tr("Right bound:"), rightBoundValue_);

    topLayout->addLayout(overallLayout);

    QFormLayout* tiedValueLayout = new QFormLayout();
    tiedValueLayout->addRow(tr("Tied value:"), tiedValueEditor_);
    tiedValueLayout->addRow(tr("Tied value left bound:"), tiedValueLeftBoundEditor_);
    tiedValueLayout->addRow(tr("Tied value right bound:"), tiedValueRightBoundEditor_);

    QGroupBox* tiedValueGroup = new QGroupBox(QStringLiteral("Tied value"));
    tiedValueGroup->setLayout(tiedValueLayout);

    topLayout->addWidget(tiedValueGroup);

    setLayout(topLayout);
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::setAdhocPort()
//-----------------------------------------------------------------------------
void AdHocEditor::setAdhocPort(AdHocItem* endPoint, HWDesignDiagram* containingDiagram,
    QSharedPointer<IEditProvider> editProvider)
{
    if(!endPoint->isAdHoc())
    {
        return;
    }

    designDiagram_ = containingDiagram;
    editProvider_ = editProvider;
    containedPortItem_ = endPoint;

    portName_->show();
    portDirection_->show();
    leftBoundValue_->show();
    rightBoundValue_->show();
    tiedValueEditor_->show();
    tiedValueLeftBoundEditor_->show();
    tiedValueRightBoundEditor_->show();

    AdHocPortItem* adhocPortItem = dynamic_cast<AdHocPortItem*>(containedPortItem_);
    AdHocInterfaceItem* adhocInterfaceItem = dynamic_cast<AdHocInterfaceItem*>(containedPortItem_);
    
    QString containingItemName = "";
    QSharedPointer<Port> referencedPort = containedPortItem_->getPort();

    if (adhocPortItem)
    {
        ComponentItem* instanceItem = adhocPortItem->encompassingComp();
        if (instanceItem)
        {
            containingItemName = instanceItem->name();
        }
    }

    if (referencedPort)
    {
        componentFinder_->setComponent(containedPortItem_->getOwnerComponent());

        DirectionTypes::Direction direction = referencedPort->getDirection();

        portName_->setText(referencedPort->name());
        portDirection_->setText(DirectionTypes::direction2Str(direction));
        leftBoundValue_->setText(formattedValueFor(referencedPort->getLeftBound()));
        rightBoundValue_->setText(formattedValueFor(referencedPort->getRightBound()));

        QString tiedValue = "";

        QSharedPointer<PartSelect> portPartSelect;

        if ((adhocPortItem && direction == DirectionTypes::IN) ||
            (adhocInterfaceItem && direction == DirectionTypes::OUT) ||
            direction == DirectionTypes::INOUT)
        {
            tiedValueEditor_->setEnabled(!designDiagram_->isProtected());
            tiedValueLeftBoundEditor_->setEnabled(!designDiagram_->isProtected());
            tiedValueRightBoundEditor_->setEnabled(!designDiagram_->isProtected());

            QSharedPointer<AdHocConnection> connectionItem = getTiedConnection(containingItemName);
            tiedValue = getTiedValue(connectionItem);
            portPartSelect = getEndPointPartSelect(connectionItem);
        }
        else
        {
            tiedValueEditor_->setEnabled(false);
            tiedValueLeftBoundEditor_->setEnabled(false);
            tiedValueRightBoundEditor_->setEnabled(false);
        }

        tiedValueEditor_->blockSignals(true);
        tiedValueEditor_->setExpression(tiedValue);
        setTiedValueEditorToolTip(tiedValue);
        tiedValueEditor_->blockSignals(false);

        updateTiedValueBounds(portPartSelect);

        parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::getTiedValue()
//-----------------------------------------------------------------------------
QString AdHocEditor::getTiedValue(QSharedPointer<AdHocConnection> connectionItem) const
{
    if (connectionItem)
    {
        return connectionItem->getTiedValue();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::getTiedConnection()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnection> AdHocEditor::getTiedConnection(QString const& instanceName) const
{
    QSharedPointer<Design> containingDesign = designDiagram_->getDesign();

    foreach (QSharedPointer<AdHocConnection> connection, *containingDesign->getAdHocConnections())
    {
        if (!instanceName.isEmpty())
        {
            foreach (QSharedPointer<PortReference> internalReference, *connection->getInternalPortReferences())
            {
                if (internalReference->getPortRef() == containedPortItem_->name() &&
                    internalReference->getComponentRef() == instanceName &&
                    (!connection->getTiedValue().isEmpty() || internalReference->getPartSelect()))
                {
                    return connection;
                }
            }
        }
        else
        {
            foreach (QSharedPointer<PortReference> externalReference, *connection->getExternalPortReferences())
            {
                if (externalReference->getPortRef() == containedPortItem_->name() &&
                    (!connection->getTiedValue().isEmpty() || externalReference->getPartSelect()))
                {
                    return connection;
                }
            }
        }
    }

    return QSharedPointer<AdHocConnection>();
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::setTiedValueEditorToolTip()
//-----------------------------------------------------------------------------
void AdHocEditor::setTiedValueEditorToolTip(QString const& tiedValue)
{
    QString newToolTip = "";

    if (QString::compare(tiedValue, "open", Qt::CaseInsensitive) == 0)
    {
        newToolTip = "Open";
    }
    else
    {
        QString parsedTieOff = getParsedTieOffValue(tiedValue);

        if (parsedTieOff.isEmpty() && QString::compare(tiedValue, "default", Qt::CaseInsensitive) == 0)
        {
            newToolTip = QObject::tr("No default value defined for port ") + containedPortItem_->name();
        }
        else
        {
            newToolTip = formattedValueFor(getParsedTieOffValue(tiedValue));
        }
    }

    tiedValueEditor_->setToolTip(newToolTip);
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString AdHocEditor::formattedValueFor(QString const& expression) const
{
    if (expression.isEmpty() || expressionParser_->isPlainValue(expression))
    {
        return expression;
    }
    else if (expressionParser_->isValidExpression(expression))
    {
        ValueFormatter formatter;
        return formatter.format(
            expressionParser_->parseExpression(expression), expressionParser_->baseForExpression(expression));
    }
    else
    {
        return tr("n/a");
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::onTiedValueChanged()
//-----------------------------------------------------------------------------
void AdHocEditor::onTiedValueChanged()
{
    tiedValueEditor_->finishEditingCurrentWord();

    QString newTiedValue = tiedValueEditor_->getExpression();

    setTiedValueEditorToolTip(newTiedValue);

    createTieOffChangeCommand(newTiedValue);

    QString instanceName = "";
    ComponentItem* containingInstance = containedPortItem_->encompassingComp();
    if (containingInstance)
    {
        instanceName = containingInstance->name();
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::onTiedValueLeftBoundChanged()
//-----------------------------------------------------------------------------
void AdHocEditor::onTiedValueLeftBoundChanged()
{
    tiedValueLeftBoundEditor_->finishEditingCurrentWord();

    QString newLeftBoundValue = tiedValueLeftBoundEditor_->getExpression();

    QString leftToolTip = formattedValueFor(expressionParser_->parseExpression(newLeftBoundValue));
    tiedValueLeftBoundEditor_->setToolTip(leftToolTip);

    createTieOffBoundsChangeCommand(newLeftBoundValue, tiedValueRightBoundEditor_->getExpression());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::onTiedValueRightBoundChanged()
//-----------------------------------------------------------------------------
void AdHocEditor::onTiedValueRightBoundChanged()
{
    tiedValueRightBoundEditor_->finishEditingCurrentWord();

    QString newRightBoundValue = tiedValueRightBoundEditor_->getExpression();

    QString rightToolTip = formattedValueFor(expressionParser_->parseExpression(newRightBoundValue));
    tiedValueRightBoundEditor_->setToolTip(rightToolTip);

    createTieOffBoundsChangeCommand(tiedValueLeftBoundEditor_->getExpression(), newRightBoundValue);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::createTieOffBoundsChangeCommand()
//-----------------------------------------------------------------------------
void AdHocEditor::createTieOffBoundsChangeCommand(QString const& newLeftBound, QString const& newRightBound)
{
    QString instanceName = "";

    ComponentItem* containingInstance = containedPortItem_->encompassingComp();
    if (containingInstance)
    {
        instanceName = containingInstance->name();
    }

    QSharedPointer<AdHocConnection> connection = getTiedConnection(instanceName);

    QSharedPointer<QUndoCommand> tieOffBoundsCommand(new AdHocTieOffBoundsChangeCommand(
        containedPortItem_, newLeftBound, newRightBound, connection, designDiagram_));

    editProvider_->addCommand(tieOffBoundsCommand);
    tieOffBoundsCommand->redo();
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::updateTiedValueBounds()
//-----------------------------------------------------------------------------
void AdHocEditor::updateTiedValueBounds(QSharedPointer<PartSelect> portPartSelect)
{
    if (portPartSelect)
    {
        QString tiedValueLeftValue = portPartSelect->getLeftRange();
        QString leftToolTip = formattedValueFor(expressionParser_->parseExpression(tiedValueLeftValue));

        QString tiedValueRightValue = portPartSelect->getRightRange();
        QString rightToolTip = formattedValueFor(expressionParser_->parseExpression(tiedValueRightValue));

        tiedValueLeftBoundEditor_->blockSignals(true);
        tiedValueLeftBoundEditor_->setExpression(tiedValueLeftValue);
        tiedValueLeftBoundEditor_->setToolTip(leftToolTip);
        tiedValueLeftBoundEditor_->blockSignals(false);

        tiedValueRightBoundEditor_->blockSignals(true);
        tiedValueRightBoundEditor_->setExpression(tiedValueRightValue);
        tiedValueRightBoundEditor_->setToolTip(rightToolTip);
        tiedValueRightBoundEditor_->blockSignals(false);
    }
    else
    {
        QString emptyText = QStringLiteral("");

        tiedValueLeftBoundEditor_->blockSignals(true);
        tiedValueLeftBoundEditor_->setExpression(emptyText);
        tiedValueLeftBoundEditor_->setToolTip(emptyText);
        tiedValueLeftBoundEditor_->blockSignals(false);

        tiedValueRightBoundEditor_->blockSignals(true);
        tiedValueRightBoundEditor_->setExpression(emptyText);
        tiedValueRightBoundEditor_->setToolTip(emptyText);
        tiedValueRightBoundEditor_->blockSignals(false);
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::getEndPointPartSelect()
//-----------------------------------------------------------------------------
QSharedPointer<PartSelect> AdHocEditor::getEndPointPartSelect(QSharedPointer<AdHocConnection> adHocConnection)
    const
{
    if (adHocConnection)
    {
        QString adhocPortName = containedPortItem_->name();

        foreach (QSharedPointer<PortReference> internalPort, *adHocConnection->getInternalPortReferences())
        {
            if (internalPort->getPortRef().compare(adhocPortName) == 0)
            {
                return internalPort->getPartSelect();
            }
        }

        foreach (QSharedPointer<PortReference> externalPort, *adHocConnection->getExternalPortReferences())
        {
            if (externalPort->getPortRef().compare(adhocPortName) == 0)
            {
                return externalPort->getPartSelect();
            }
        }
    }

    return QSharedPointer<PartSelect>();
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::createTieOffChangeCommand()
//-----------------------------------------------------------------------------
void AdHocEditor::createTieOffChangeCommand(QString const& newTiedValue)
{
    QString instanceName = "";

    ComponentItem* containingInstance = containedPortItem_->encompassingComp();
    if (containingInstance)
    {
        instanceName = containingInstance->name();
    }

    QString parsedNewTieOff = getParsedTieOffValue(newTiedValue);

    QSharedPointer<AdHocConnection> connection = getTiedConnection(instanceName);
    
    QString oldTieOffValue = "";
    QString parsedOldTieOff = "";

    if (connection)
    {
        oldTieOffValue = connection->getTiedValue();
        parsedOldTieOff = getParsedTieOffValue(oldTieOffValue);
    }

    if (newTiedValue != oldTieOffValue)
    {
        QString formattedOldTieOff = expressionFormatter_->formatReferringExpression(oldTieOffValue);
        QString formattedNewTieOff = expressionFormatter_->formatReferringExpression(newTiedValue);

        int oldBase = expressionParser_->baseForExpression(oldTieOffValue);
        int newBase = expressionParser_->baseForExpression(newTiedValue);

        QSharedPointer<QUndoCommand> tieOffUndoCommand(new AdHocTieOffChangeCommand(containedPortItem_, connection,
            newTiedValue, parsedNewTieOff, formattedNewTieOff, newBase, oldTieOffValue, parsedOldTieOff,
            formattedOldTieOff, oldBase, designDiagram_));

        editProvider_->addCommand(tieOffUndoCommand);
        tieOffUndoCommand->redo();

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::getParsedTieOffValue()
//-----------------------------------------------------------------------------
QString AdHocEditor::getParsedTieOffValue(QString const& tieOffValue) const
{
    QString parsedTieOff;

    if (QString::compare(tieOffValue, "default", Qt::CaseInsensitive) == 0)
    {
        QString portDefaultValue = containedPortItem_->getPort()->getDefaultValue();
        parsedTieOff = expressionParser_->parseExpression(portDefaultValue);
    }
    else if (QString::compare(tieOffValue, "open", Qt::CaseInsensitive) == 0)
    {
        parsedTieOff = "";
    }
    else
    {
        parsedTieOff = expressionParser_->parseExpression(tieOffValue);
    }

    return parsedTieOff;
}

//-----------------------------------------------------------------------------
// Function: AdHocEditor::clear()
//-----------------------------------------------------------------------------
void AdHocEditor::clear()
{
    portName_->hide();
    portDirection_->hide();
    leftBoundValue_->hide();
    rightBoundValue_->hide();
    tiedValueEditor_->hide();
    tiedValueLeftBoundEditor_->hide();
    tiedValueRightBoundEditor_->hide();

    parentWidget()->setMaximumHeight(20);
}
