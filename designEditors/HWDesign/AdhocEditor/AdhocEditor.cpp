//-----------------------------------------------------------------------------
// File: AdhocEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <designEditors/HWDesign/AdHocItem.h>
#include <designEditors/HWDesign/AdHocPortItem.h>
#include <designEditors/HWDesign/AdHocInterfaceItem.h>
#include <designEditors/HWDesign/HWDesignDiagram.h>
#include <designEditors/HWDesign/undoCommands/AdHocTieOffChangeCommand.h>

#include <IPXACTmodels/common/validators/ValueFormatter.h>
#include <IPXACTmodels/Design/Design.h>

#include <QFormLayout>

//-----------------------------------------------------------------------------
// Function: AdhocEditor::AdHocEditor()
//-----------------------------------------------------------------------------
AdHocEditor::AdHocEditor(QWidget* parent):
QWidget(parent),
componentFinder_(new ComponentParameterFinder(QSharedPointer<Component>(0))),
expressionParser_(new IPXactSystemVerilogParser(componentFinder_)),
portName_(new QLabel(this)),
portDirection_(new QLabel(this)),
leftBoundValue_(new QLabel(this)),
rightBoundValue_(new QLabel(this)),
tiedValueEditor_(new ExpressionEditor(componentFinder_, this)),
containedPortItem_(),
editProvider_(),
designDiagram_()
{
    tiedValueEditor_->setFixedHeight(20);

    ComponentParameterModel* parameterModel = new ComponentParameterModel(componentFinder_, this);
    parameterModel->setExpressionParser(expressionParser_);

    ParameterCompleter* tiedValueCompleter = new ParameterCompleter(this);
    tiedValueCompleter->setModel(parameterModel);

    tiedValueEditor_->setAppendingCompleter(tiedValueCompleter);

    QStringList reservedWordsForTiedValue;
    reservedWordsForTiedValue << "open" << "default";
    tiedValueEditor_->setReservedWords(reservedWordsForTiedValue);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    setupLayout();

    connect(tiedValueEditor_, SIGNAL(editingFinished()), this, SLOT(onTiedValueChanged()), Qt::UniqueConnection);
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
    QFormLayout* overallLayout = new QFormLayout();

    overallLayout->addRow(tr("Name:"), portName_);
    overallLayout->addRow(tr("Direction:"), portDirection_);
    overallLayout->addRow(tr("Left bound:"), leftBoundValue_);
    overallLayout->addRow(tr("Right bound:"), rightBoundValue_);
    overallLayout->addRow(tr("Tied value:"), tiedValueEditor_);

    setLayout(overallLayout);
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::setAdhocPort()
//-----------------------------------------------------------------------------
void AdHocEditor::setAdhocPort(AdHocItem* endPoint, HWDesignDiagram* containingDiagram,
    QSharedPointer<IEditProvider> editProvider)
{
    if(endPoint->isAdHoc())
    {
        designDiagram_ = containingDiagram;
        editProvider_ = editProvider;
        containedPortItem_ = endPoint;

        portName_->show();
        portDirection_->show();
        leftBoundValue_->show();
        rightBoundValue_->show();
        tiedValueEditor_->show();

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
            leftBoundValue_->setText(expressionParser_->parseExpression(referencedPort->getLeftBound()));
            rightBoundValue_->setText(expressionParser_->parseExpression(referencedPort->getRightBound()));

            QString tiedValue = "";

            if ((adhocPortItem && direction == DirectionTypes::IN) ||
                (adhocInterfaceItem && direction == DirectionTypes::OUT) ||
                direction == DirectionTypes::INOUT)
            {
                bool locked = designDiagram_->isProtected();

                tiedValue = getTiedValue(containingItemName);

                tiedValueEditor_->setEnabled(!locked);
            }
            else
            {
                tiedValueEditor_->setEnabled(false);
            }

            tiedValueEditor_->blockSignals(true);
            tiedValueEditor_->setExpression(tiedValue);
            setTiedValueEditorToolTip(tiedValue);
            tiedValueEditor_->blockSignals(false);

            parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::getTiedValue()
//-----------------------------------------------------------------------------
QString AdHocEditor::getTiedValue(QString const& instanceName) const
{
    QSharedPointer<AdHocConnection> connection = getTiedConnection(instanceName);
    if (connection)
    {
        return connection->getTiedValue();
    }
    else
    {
        return QString("");
    }
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
                    !connection->getTiedValue().isEmpty())
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
                    !connection->getTiedValue().isEmpty())
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
        QSharedPointer<QUndoCommand> tieOffUndoCommand(new AdHocTieOffChangeCommand(containedPortItem_, connection,
            newTiedValue, parsedNewTieOff, oldTieOffValue, parsedOldTieOff, designDiagram_));
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

    parentWidget()->setMaximumHeight(20);
}
