//-----------------------------------------------------------------------------
// File: RemapStateValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.11.2015
//
// Description:
// Validator for the ipxact:remapState.
//-----------------------------------------------------------------------------

#include "RemapStateValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>
#include <IPXACTmodels/Component/Port.h>

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>

#include <IPXACTmodels/utilities/Search.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::RemapStateValidator()
//-----------------------------------------------------------------------------
RemapStateValidator::RemapStateValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<Port> > > ports):
expressionParser_(expressionParser),
availablePorts_(ports)
{

}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::componentChange()
//-----------------------------------------------------------------------------
void RemapStateValidator::componentChange(QSharedPointer<QList<QSharedPointer<Port> > > newPorts)
{
    availablePorts_ = newPorts;
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::validate()
//-----------------------------------------------------------------------------
bool RemapStateValidator::validate(QSharedPointer<RemapState> remapState) const
{
    return hasValidName(remapState) && hasValidRemapPorts(remapState);
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::hasValidName()
//-----------------------------------------------------------------------------
bool RemapStateValidator::hasValidName(QSharedPointer<RemapState> remapState) const
{
    return CommonItemsValidator::hasValidName(remapState->name());
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::hasValidRemapPorts()
//-----------------------------------------------------------------------------
bool RemapStateValidator::hasValidRemapPorts(QSharedPointer<RemapState> remapState) const
{
    return std::all_of(remapState->getRemapPorts()->cbegin(), remapState->getRemapPorts()->cend(),
        [this](auto remapPort) 
        { 
            return remapPortHasValidPortReference(remapPort) && remapPortHasValidValue(remapPort);  
        });
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::hasValidPortReference()
//-----------------------------------------------------------------------------
bool RemapStateValidator::remapPortHasValidPortReference(QSharedPointer<RemapPort> remapPort) const
{
    auto port = Search::findByName(remapPort->getPortNameRef(), *availablePorts_);
    return port.isNull() == false && remapPortHasValidIndex(remapPort, port);
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::remapPortHasValidIndex()
//-----------------------------------------------------------------------------
bool RemapStateValidator::remapPortHasValidIndex(QSharedPointer<RemapPort> remapPort,
    QSharedPointer<Port> referencedPort) const
{
    if (!remapPort->getPortIndex().isEmpty())
    {
        bool leftOk = true;
        int leftBound = expressionParser_->parseExpression(referencedPort->getLeftBound()).toInt(&leftOk);

        bool rightOk = true;
        int rightBound = expressionParser_->parseExpression(referencedPort->getRightBound()).toInt(&rightOk);

        int maxBound = qMax(leftBound, rightBound);

        bool indexOk = true;
        int remapIndex = expressionParser_->parseExpression(remapPort->getPortIndex()).toInt(&indexOk);

        return leftOk && rightOk && indexOk && remapIndex >= 0 && remapIndex <= maxBound;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::remapPortHasValidValue()
//-----------------------------------------------------------------------------
bool RemapStateValidator::remapPortHasValidValue(QSharedPointer<RemapPort> remapPort) const
{
    bool valueValid = false;
    bool valueIsOk = true;
    expressionParser_->parseExpression(remapPort->getValue(), &valueValid).toULongLong(&valueIsOk);

    return valueValid && valueIsOk;
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void RemapStateValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<RemapState> remapState,
    QString const& context) const
{
    QString const& remapStateContext = QStringLiteral("remap state ") + remapState->name();

    findErrorsInName(errors, remapState, context);
    findErrorsInRemapPorts(errors, remapState, remapStateContext);
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void RemapStateValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<RemapState> remapState,
    QString const& context) const
{
    if (!hasValidName(remapState))
    {
        errors.append(QObject::tr("Invalid name specified for remap state %1 within %2")
            .arg(remapState->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::findErrorsInRemapPorts()
//-----------------------------------------------------------------------------
void RemapStateValidator::findErrorsInRemapPorts(QVector<QString>& errors, QSharedPointer<RemapState> remapState,
    QString const& context) const
{
    for (QSharedPointer<RemapPort> remapPort : *remapState->getRemapPorts())
    {
        findErrorsInRemapPortPortReference(errors, remapPort, context);

        if (!remapPortHasValidValue(remapPort))
        {
            errors.append(QObject::tr("Invalid value set for remap port %1 within %2")
                .arg(remapPort->getPortNameRef()).arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::findErrorsInPortReference()
//-----------------------------------------------------------------------------
void RemapStateValidator::findErrorsInRemapPortPortReference(QVector<QString>& errors,
    QSharedPointer<RemapPort> remapPort, QString const& context) const
{
    auto port = Search::findByName(remapPort->getPortNameRef(), *availablePorts_);
    if (port.isNull())
    {
        errors.append(QObject::tr("Invalid port %1 set for %2").arg(remapPort->getPortNameRef(), context));
    }
    else if (!remapPortHasValidIndex(remapPort, port))
    {
        errors.append(QObject::tr("Invalid port index set for remap port %1 within %2")
            .arg(remapPort->getPortNameRef(), context));
    }
}
