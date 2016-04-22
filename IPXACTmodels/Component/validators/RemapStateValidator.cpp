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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>
#include <IPXACTmodels/Component/Port.h>

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
// Function: RemapStateValidator::~RemapStateValidator()
//-----------------------------------------------------------------------------
RemapStateValidator::~RemapStateValidator()
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
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(remapState->name());

    if (remapState->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::hasValidRemapPorts()
//-----------------------------------------------------------------------------
bool RemapStateValidator::hasValidRemapPorts(QSharedPointer<RemapState> remapState) const
{
    foreach (QSharedPointer<RemapPort> remapPort, *remapState->getRemapPorts())
    {
        if (!remapPortHasValidPortReference(remapPort) ||
            !remapPortHasValidValue(remapPort))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::hasValidPortReference()
//-----------------------------------------------------------------------------
bool RemapStateValidator::remapPortHasValidPortReference(QSharedPointer<RemapPort> remapPort) const
{
    foreach (QSharedPointer<Port> port, *availablePorts_)
    {
        if (remapPort->getPortNameRef() == port->name())
        {
            return remapPortHasValidIndex(remapPort, port);
        }
    }

    return false;
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
    bool valueIsOk = true;
    quint64 value = expressionParser_->parseExpression(remapPort->getValue()).toULongLong(&valueIsOk);

    return valueIsOk && value >= 0;
}

//-----------------------------------------------------------------------------
// Function: RemapStateValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void RemapStateValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<RemapState> remapState,
    QString const& context) const
{
    QString const& remapStateContext = "remap state " + remapState->name();

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
    foreach (QSharedPointer<RemapPort> remapPort, *remapState->getRemapPorts())
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
    foreach (QSharedPointer<Port> port, *availablePorts_)
    {
        if (remapPort->getPortNameRef() == port->name())
        {
            if (!remapPortHasValidIndex(remapPort, port))
            {
                errors.append(QObject::tr("Invalid port index set for remap port %1 within %2")
                    .arg(remapPort->getPortNameRef()).arg(context));
            }

            return;
        }
    }

    errors.append(QObject::tr("Invalid port %1 set for %2").arg(remapPort->getPortNameRef()).arg(context));
}