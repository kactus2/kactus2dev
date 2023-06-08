//-----------------------------------------------------------------------------
// File: AdhocPortEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2016
//
// Description:
// Editor to edit the details of an Ad hoc port in designAd-hoc editor.
//-----------------------------------------------------------------------------

#include "AdhocPortEditor.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/GenericEditProvider.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <KactusAPI/include/ComponentParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <editors/HWDesign/AdHocItem.h>
#include <editors/HWDesign/ActivePortItem.h>
#include <editors/HWDesign/HierarchicalPortItem.h>
#include <editors/HWDesign/HWDesignDiagram.h>
#include <editors/HWDesign/undoCommands/AdHocTieOffChangeCommand.h>
#include <editors/HWDesign/undoCommands/AdHocTieOffBoundsChangeCommand.h>

#include <IPXACTmodels/common/validators/ValueFormatter.h>
#include <IPXACTmodels/Design/Design.h>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QGroupBox>

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::AdhocPortEditor()
//-----------------------------------------------------------------------------
AdhocPortEditor::AdhocPortEditor(QSharedPointer<ParameterFinder> designParameterFinder, QWidget* parent) :
    QWidget(parent),
    expressionParser_(new IPXactSystemVerilogParser(designParameterFinder)),
    expressionFormatter_(new ExpressionFormatter(designParameterFinder)),
    portName_(new QLabel(this)),
    portDirection_(new QLabel(this)),
    leftBoundValue_(new QLabel(this)),
    rightBoundValue_(new QLabel(this)),
    tiedValueEditor_(new ExpressionEditor(designParameterFinder, this)),
    containedPortItem_(),
    editProvider_(),
    designDiagram_(),
    tiedValueLeftBoundEditor_(new ExpressionEditor(designParameterFinder, this)),
    tiedValueRightBoundEditor_(new ExpressionEditor(designParameterFinder, this)),
    designParameterFinder_(designParameterFinder)
{
    tiedValueEditor_->setFixedHeight(20);

    tiedValueLeftBoundEditor_->setFixedHeight(20);
    tiedValueRightBoundEditor_->setFixedHeight(20);

    ComponentParameterModel* parameterModel = new ComponentParameterModel(designParameterFinder, this);
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

    tiedValueEditor_->setReservedWords(QStringList({ "open", "default" }));

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    setupLayout();

    connect(tiedValueEditor_, SIGNAL(editingFinished()), this, SLOT(onTiedValueChanged()), Qt::UniqueConnection);
    connect(tiedValueLeftBoundEditor_, SIGNAL(editingFinished()),
        this, SLOT(onTiedValueLeftBoundChanged()), Qt::UniqueConnection);
    connect(tiedValueRightBoundEditor_, SIGNAL(editingFinished()),
        this, SLOT(onTiedValueRightBoundChanged()), Qt::UniqueConnection);
}


//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::setAdhocPort()
//-----------------------------------------------------------------------------
void AdhocPortEditor::setAdhocPort(AdHocItem* endPoint, HWDesignDiagram* containingDiagram,
    QSharedPointer<IEditProvider> editProvider)
{
    if (!endPoint->isAdHoc())
    {
        return;
    }

    designDiagram_ = containingDiagram;
    editProvider_ = editProvider;
    containedPortItem_ = endPoint;

    portName_->parentWidget()->show();
    tiedValueEditor_->parentWidget()->show();

    ActivePortItem* adhocPortItem = dynamic_cast<ActivePortItem*>(containedPortItem_);
    HierarchicalPortItem* adhocInterfaceItem = dynamic_cast<HierarchicalPortItem*>(containedPortItem_);

    QSharedPointer<Port> referencedPort = containedPortItem_->getPort();

    if (referencedPort)
    {
        DirectionTypes::Direction direction = referencedPort->getDirection();

        portName_->setText(referencedPort->name());
        portDirection_->setText(DirectionTypes::direction2Str(direction));
        leftBoundValue_->setText(formattedValueFor(referencedPort->getLeftBound()));
        rightBoundValue_->setText(formattedValueFor(referencedPort->getRightBound()));

        QString tiedValue = QString();

        QSharedPointer<PartSelect> portPartSelect;

        if ((adhocPortItem && direction == DirectionTypes::IN) ||
            (adhocInterfaceItem && direction == DirectionTypes::OUT) ||
            direction == DirectionTypes::INOUT)
        {
            tiedValueEditor_->setEnabled(!designDiagram_->isProtected());
            tiedValueLeftBoundEditor_->setEnabled(!designDiagram_->isProtected());
            tiedValueRightBoundEditor_->setEnabled(!designDiagram_->isProtected());

            QSharedPointer<AdHocConnection> connectionItem = getTiedConnection();
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
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::clear()
//-----------------------------------------------------------------------------
void AdhocPortEditor::clear()
{
    portName_->parentWidget()->hide();
    tiedValueEditor_->parentWidget()->hide();
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::getTiedValue()
//-----------------------------------------------------------------------------
QString AdhocPortEditor::getTiedValue(QSharedPointer<AdHocConnection> connectionItem) const
{
    if (connectionItem)
    {
        return connectionItem->getTiedValue();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::getTiedConnection()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnection> AdhocPortEditor::getTiedConnection() const
{
    QString instanceName = getInstanceName();

    QSharedPointer<Design> containingDesign = designDiagram_->getDesign();
    for (QSharedPointer<AdHocConnection> connection : *containingDesign->getAdHocConnections())
    {
        if (!instanceName.isEmpty())
        {
            for (QSharedPointer<PortReference> internalReference : *connection->getInternalPortReferences())
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
            for (QSharedPointer<PortReference> externalReference : *connection->getExternalPortReferences())
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
// Function: AdhocPortEditor::getInstanceName()
//-----------------------------------------------------------------------------
QString AdhocPortEditor::getInstanceName() const
{
    QString instanceName;
    ComponentItem* containingInstance = containedPortItem_->encompassingComp();
    if (containingInstance)
    {
        instanceName = containingInstance->name();
    }

    return instanceName;
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::setTiedValueEditorToolTip()
//-----------------------------------------------------------------------------
void AdhocPortEditor::setTiedValueEditorToolTip(QString const& tiedValue)
{
    QString newToolTip;

    if (QString::compare(tiedValue, QLatin1String("open"), Qt::CaseInsensitive) != 0)
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
// Function: AdhocPortEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString AdhocPortEditor::formattedValueFor(QString const& expression) const
{
    bool isValidExpression = false;
    QString value = expressionParser_->parseExpression(expression, &isValidExpression);

    if (isValidExpression)
    {
        ValueFormatter formatter;
        return formatter.format(value, expressionParser_->baseForExpression(expression));
    }
    else
    {
        return tr("n/a");
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::onTiedValueChanged()
//-----------------------------------------------------------------------------
void AdhocPortEditor::onTiedValueChanged()
{
    tiedValueEditor_->finishEditingCurrentWord();

    QString newTiedValue = tiedValueEditor_->getExpression();

    setTiedValueEditorToolTip(newTiedValue);

    createTieOffChangeCommand(newTiedValue);
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::onTiedValueLeftBoundChanged()
//-----------------------------------------------------------------------------
void AdhocPortEditor::onTiedValueLeftBoundChanged()
{
    tiedValueLeftBoundEditor_->finishEditingCurrentWord();

    QString newLeftBoundValue = tiedValueLeftBoundEditor_->getExpression();

    QString leftToolTip = formattedValueFor(expressionParser_->parseExpression(newLeftBoundValue));
    tiedValueLeftBoundEditor_->setToolTip(leftToolTip);

    createTieOffBoundsChangeCommand(newLeftBoundValue, tiedValueRightBoundEditor_->getExpression());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::onTiedValueRightBoundChanged()
//-----------------------------------------------------------------------------
void AdhocPortEditor::onTiedValueRightBoundChanged()
{
    tiedValueRightBoundEditor_->finishEditingCurrentWord();

    QString newRightBoundValue = tiedValueRightBoundEditor_->getExpression();

    QString rightToolTip = formattedValueFor(expressionParser_->parseExpression(newRightBoundValue));
    tiedValueRightBoundEditor_->setToolTip(rightToolTip);

    createTieOffBoundsChangeCommand(tiedValueLeftBoundEditor_->getExpression(), newRightBoundValue);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::createTieOffBoundsChangeCommand()
//-----------------------------------------------------------------------------
void AdhocPortEditor::createTieOffBoundsChangeCommand(QString const& newLeftBound, QString const& newRightBound)
{
    QSharedPointer<AdHocConnection> connection = getTiedConnection();
    if (tieOffBoundsHaveChanged(newLeftBound, newRightBound, connection))
    {
        QSharedPointer<AdHocTieOffBoundsChangeCommand> tieOffBoundsCommand(new AdHocTieOffBoundsChangeCommand(
            containedPortItem_, newLeftBound, newRightBound, connection, designDiagram_));

        connect(tieOffBoundsCommand.data(), SIGNAL(increaseReferences(QString const&)),
            this, SLOT(onIncreaseReferencesInExpression(QString const&)), Qt::UniqueConnection);
        connect(tieOffBoundsCommand.data(), SIGNAL(decreaseReferences(QString const&)),
            this, SLOT(onDecreaseReferencesInExpression(QString const&)), Qt::UniqueConnection);
        connect(tieOffBoundsCommand.data(), SIGNAL(refreshEditors()),
            this, SLOT(refreshEditors()), Qt::UniqueConnection);

        editProvider_->addCommand(tieOffBoundsCommand);
        tieOffBoundsCommand->redo();
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::tieOffBoundsHaveChanged()
//-----------------------------------------------------------------------------
bool AdhocPortEditor::tieOffBoundsHaveChanged(QString const& newLeftBound, QString const& newRightBound,
    QSharedPointer<AdHocConnection> connection) const
{
    QString oldLeft;
    QString oldRight;

    QSharedPointer<PartSelect> partSelect = getEndPointPartSelect(connection);
    if (partSelect)
    {
        oldLeft = partSelect->getLeftRange();
        oldRight = partSelect->getRightRange();
    }

    return !(oldLeft == newLeftBound && oldRight == newRightBound);
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::updateTiedValueBounds()
//-----------------------------------------------------------------------------
void AdhocPortEditor::updateTiedValueBounds(QSharedPointer<PartSelect> portPartSelect)
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
        QString emptyText;

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
// Function: AdhocPortEditor::getEndPointPartSelect()
//-----------------------------------------------------------------------------
QSharedPointer<PartSelect> AdhocPortEditor::getEndPointPartSelect(QSharedPointer<AdHocConnection> adHocConnection)
const
{
    QString instanceName = getInstanceName();

    if (adHocConnection)
    {
        auto adhocPortName = containedPortItem_->name();

        for (QSharedPointer<PortReference> internalPort : *adHocConnection->getInternalPortReferences())
        {
            if (internalPort->getPortRef().compare(adhocPortName) == 0 &&
                internalPort->getComponentRef() == instanceName)
            {
                return internalPort->getPartSelect();
            }
        }

        for (QSharedPointer<PortReference> externalPort : *adHocConnection->getExternalPortReferences())
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
// Function: AdhocPortEditor::createTieOffChangeCommand()
//-----------------------------------------------------------------------------
void AdhocPortEditor::createTieOffChangeCommand(QString const& newTiedValue)
{
    QString parsedNewTieOff = getParsedTieOffValue(newTiedValue);

    QSharedPointer<AdHocConnection> connection = getTiedConnection();

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

        QSharedPointer<AdHocTieOffChangeCommand> tieOffUndoCommand(new AdHocTieOffChangeCommand(
            containedPortItem_, connection, newTiedValue, parsedNewTieOff, formattedNewTieOff, newBase,
            oldTieOffValue, parsedOldTieOff, formattedOldTieOff, oldBase, designDiagram_));

        connect(tieOffUndoCommand.data(), SIGNAL(increaseReferences(QString const&)),
            this, SLOT(onIncreaseReferencesInExpression(QString const&)), Qt::UniqueConnection);
        connect(tieOffUndoCommand.data(), SIGNAL(decreaseReferences(QString const&)),
            this, SLOT(onDecreaseReferencesInExpression(QString const&)), Qt::UniqueConnection);
        connect(tieOffUndoCommand.data(), SIGNAL(refreshEditors()),
            this, SLOT(refreshEditors()), Qt::UniqueConnection);

        editProvider_->addCommand(tieOffUndoCommand);
        tieOffUndoCommand->redo();

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::getParsedTieOffValue()
//-----------------------------------------------------------------------------
QString AdhocPortEditor::getParsedTieOffValue(QString const& tieOffValue) const
{
    QString parsedTieOff;

    if (QString::compare(tieOffValue, QLatin1String("default"), Qt::CaseInsensitive) == 0)
    {
        QString portDefaultValue = containedPortItem_->getPort()->getDefaultValue();
        parsedTieOff = expressionParser_->parseExpression(portDefaultValue);
    }
    else if (QString::compare(tieOffValue, QLatin1String("open"), Qt::CaseInsensitive) != 0)
    {
        parsedTieOff = expressionParser_->parseExpression(tieOffValue);
    }

    return parsedTieOff;
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::onIncreaseReferencesInExpression()
//-----------------------------------------------------------------------------
void AdhocPortEditor::onIncreaseReferencesInExpression(QString const& expression)
{
    if (!expression.isEmpty())
    {
        for (QString const& parameterID : designParameterFinder_->getAllParameterIds())
        {
            auto const referencesToID = expression.count(parameterID);
            for (int i = 0; i < referencesToID; ++i)
            {
                emit increaseReferences(parameterID);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::onDecreaseReferencesInExpression()
//-----------------------------------------------------------------------------
void AdhocPortEditor::onDecreaseReferencesInExpression(QString const& expression)
{
    if (!expression.isEmpty())
    {
        for (QString const& parameterID : designParameterFinder_->getAllParameterIds())
        {
            auto const referencesToID = expression.count(parameterID);
            for (int i = 0; i < referencesToID; ++i)
            {
                emit decreaseReferences(parameterID);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::refreshEditors()
//-----------------------------------------------------------------------------
void AdhocPortEditor::refreshEditors()
{
    blockExpressionEditorSignals(true);

    QString tiedValue;
    QString tieLeftBound;
    QString tieRightBound;

    QSharedPointer<AdHocConnection> connection = getTiedConnection();
    if (connection)
    {
        tiedValue = connection->getTiedValue();

        QSharedPointer<PartSelect> partSelect = getEndPointPartSelect(connection);
        if (partSelect)
        {
            tieLeftBound = partSelect->getLeftRange();
            tieRightBound = partSelect->getRightRange();
        }
    }

    tiedValueEditor_->setExpression(tiedValue);
    tiedValueLeftBoundEditor_->setExpression(tieLeftBound);
    tiedValueRightBoundEditor_->setExpression(tieRightBound);

    blockExpressionEditorSignals(false);
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::blockExpressionEditorSignals()
//-----------------------------------------------------------------------------
void AdhocPortEditor::blockExpressionEditorSignals(bool block)
{
    tiedValueEditor_->blockSignals(block);
    tiedValueLeftBoundEditor_->blockSignals(block);
    tiedValueRightBoundEditor_->blockSignals(block);
}

//-----------------------------------------------------------------------------
// Function: AdhocPortEditor::setupLayout()
//-----------------------------------------------------------------------------
void AdhocPortEditor::setupLayout()
{
    QGroupBox* detailBox = new QGroupBox(tr("Port information"), this);
    detailBox->setFlat(true);
    QFormLayout* overallLayout = new QFormLayout(detailBox);
    overallLayout->addRow(tr("Name:"), portName_);
    overallLayout->addRow(tr("Direction:"), portDirection_);
    overallLayout->addRow(tr("Left bound:"), leftBoundValue_);
    overallLayout->addRow(tr("Right bound:"), rightBoundValue_);

    QGroupBox* tiedValueGroup = new QGroupBox(tr("Tied value"));
    tiedValueGroup->setFlat(true);
    QFormLayout* tiedValueLayout = new QFormLayout(tiedValueGroup);
    tiedValueLayout->addRow(tr("Value:"), tiedValueEditor_);
    tiedValueLayout->addRow(tr("Left bound:"), tiedValueLeftBoundEditor_);
    tiedValueLayout->addRow(tr("Right bound:"), tiedValueRightBoundEditor_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(detailBox);
    topLayout->addWidget(tiedValueGroup);
}
