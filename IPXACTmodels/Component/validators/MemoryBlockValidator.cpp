//-----------------------------------------------------------------------------
// File: MemoryBlockValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2022
//
// Description:
// Validator for memory block.
//-----------------------------------------------------------------------------

#include "MemoryBlockValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/common/validators/CommonItemsValidator.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: MemoryBlockValidator::MemoryBlockValidator()
//-----------------------------------------------------------------------------
MemoryBlockValidator::MemoryBlockValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterValidator> parameterValidator,
    Document::Revision docRevision) :
HierarchicalValidator(),
docRevision_(docRevision),
expressionParser_(expressionParser),
parameterValidator_(parameterValidator)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryBlockValidator::getExpressionParser()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionParser> MemoryBlockValidator::getExpressionParser() const
{
    return expressionParser_;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockValidator::validate()
//-----------------------------------------------------------------------------
bool MemoryBlockValidator::validate(QSharedPointer<MemoryBlockBase> memoryBlock) const
{
    return validComparedToSiblings(memoryBlock) && 
        hasValidName(memoryBlock) &&
        hasValidIsPresent(memoryBlock) &&
        hasValidBaseAddress(memoryBlock) &&
        hasValidParameters(memoryBlock);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockValidator::hasValidName()
//-----------------------------------------------------------------------------
bool MemoryBlockValidator::hasValidName(QSharedPointer<MemoryBlockBase> memoryBlock) const
{
    return CommonItemsValidator::hasValidName(memoryBlock->name());
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool MemoryBlockValidator::hasValidIsPresent(QSharedPointer<MemoryBlockBase> memoryBlock) const
{
    if (docRevision_ != Document::Revision::Std22 && !memoryBlock->getIsPresent().isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(memoryBlock->getIsPresent());

        bool toIntOk = true;
        int intValue = solvedValue.toInt(&toIntOk);

        if (!toIntOk || intValue < 0 || intValue > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockValidator::hasValidBaseAddress()
//-----------------------------------------------------------------------------
bool MemoryBlockValidator::hasValidBaseAddress(QSharedPointer<MemoryBlockBase> memoryBlock) const
{
    bool toIntOk = true;

    QString parsedBaseAddress = memoryBlock->getBaseAddress();

    expressionParser_->parseExpression(memoryBlock->getBaseAddress()).toULongLong(&toIntOk);

    return toIntOk;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool MemoryBlockValidator::hasValidParameters(QSharedPointer<MemoryBlockBase> memoryBlock) const
{
    if (!memoryBlock->getParameters()->isEmpty())
    {
        QStringList parameterNames;
        for (QSharedPointer<Parameter> const& parameter : *memoryBlock->getParameters())
        {
            if (parameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter))
            {
                return false;
            }
            else
            {
                parameterNames.append(parameter->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void MemoryBlockValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<MemoryBlockBase> memoryBlock,
    QString const& context) const
{
    if (!hasValidName(memoryBlock))
    {
        errors.append(
            QObject::tr("Invalid name specified for %1 %2 within %3").
            arg(getBlockType(), memoryBlock->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void MemoryBlockValidator::findErrorsInIsPresent(QVector<QString>& errors,
    QSharedPointer<MemoryBlockBase> memoryBlock, QString const& context) const
{
    if (!hasValidIsPresent(memoryBlock))
    {
        errors.append(QObject::tr("Invalid isPresent set for %1 %2 within %3").
            arg(getBlockType(), memoryBlock->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockValidator::findErrorsInBaseAddress()
//-----------------------------------------------------------------------------
void MemoryBlockValidator::findErrorsInBaseAddress(QVector<QString>& errors,
    QSharedPointer<MemoryBlockBase> memoryBlock, QString const& context) const
{
    if (!hasValidBaseAddress(memoryBlock))
    {
        errors.append(QObject::tr("Invalid baseAddress set for %1 %2 within %3").
            arg(getBlockType(), memoryBlock->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void MemoryBlockValidator::findErrorsInParameters(QVector<QString>& errors,
    QSharedPointer<MemoryBlockBase> memoryBlock, QString const& context) const
{
    if (!memoryBlock->getParameters()->isEmpty())
    {
        QStringList parameterNames;
        for (QSharedPointer<Parameter> const& parameter : *memoryBlock->getParameters())
        {
            if (parameterNames.contains(parameter->name()))
            {
                errors.append(QObject::tr("Name %1 of parameters in %2 is not unique.").arg(parameter->name()).
                    arg(context));
            }
            else
            {
                parameterNames.append(parameter->name());
            }

            parameterValidator_->findErrorsIn(errors, parameter, context);
        }
    }
}
