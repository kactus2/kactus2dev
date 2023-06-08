//-----------------------------------------------------------------------------
// File: PacketValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 06.06.2023
//
// Description:
// Validator for ipxact:packet.
//-----------------------------------------------------------------------------

#include "PacketValidator.h"

#include <IPXACTmodels/common/validators/QualifierValidator.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: PacketValidator::PacketValidator()
//-----------------------------------------------------------------------------
PacketValidator::PacketValidator(QSharedPointer<ExpressionParser> expressionParser):
    expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: PacketValidator::validate()
//-----------------------------------------------------------------------------
bool PacketValidator::validate(QSharedPointer<Packet> packet) const
{
    if (!isValidName(packet->name()))
    {
        return false;
    }

    if (packet->getPacketFields()->isEmpty())
    {
        return false;
    }

    for (auto const& packetField : *packet->getPacketFields())
    {
        if (!isValidPacketField(packetField))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void PacketValidator::findErrorsIn(QStringList& errors, QSharedPointer<Packet> packet, QString const& context) const
{
    if (packet->name().isEmpty())
    {
        errors.append(QObject::tr("Unnamed packet in %1").arg(context));
    }

    if (packet->getPacketFields()->isEmpty())
    {
        errors.append(QObject::tr("No packet field defined for packet %1 in %2").arg(packet->name(), context));
    }
    
    for (auto const& packetField : *packet->getPacketFields())
    {
        findErrorsInPacketField(errors, context, packetField);
    }
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::isValidPacketField()
//-----------------------------------------------------------------------------
bool PacketValidator::isValidPacketField(QSharedPointer<PacketField> packetField) const
{
    if (!isValidName(packetField->name()))
    {
        return false;
    }

    // Check validity of field width.
    bool widthIsValid = false;
    auto resolvedWidth = parseFieldWidth(packetField->getWidth(), &widthIsValid);
    if (!widthIsValid)
    {
        return false;
    }

    if (!hasValidFieldValue(packetField, resolvedWidth))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::findErrorsInPacketField()
//-----------------------------------------------------------------------------
void PacketValidator::findErrorsInPacketField(QStringList& errors, QString const& context, 
    QSharedPointer<PacketField> packetField) const
{
    if (packetField->name().isEmpty())
    {
        errors.append(QObject::tr("No name defined for packet field in %1").arg(context));
    }

    bool validWidth = false;
    auto resolvedWidth = parseFieldWidth(packetField->getWidth(), &validWidth);
    if (!validWidth)
    {
        errors.append(QObject::tr("Missing or invalid width defined for packet field %1 in %2").arg(
            packetField->name(), context));
    }

    findErrorsInPacketFieldValue(errors, context, packetField, resolvedWidth);
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::isValidName()
//-----------------------------------------------------------------------------
bool PacketValidator::isValidName(QString const& name) const
{
    QRegularExpression whiteSpaceExpression(QStringLiteral("^\\s*$"));
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(name);

    if (name.isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::parseFieldWidth()
//-----------------------------------------------------------------------------
QString PacketValidator::parseFieldWidth(QString const& width, bool* widthIsValid) const
{
    bool validWidth = false;
    auto resolvedWidth = expressionParser_->parseExpression(width, &validWidth);
    if (width.isEmpty() || !validWidth)
    {
        *widthIsValid = false;
        return QStringLiteral("");
    }

    *widthIsValid = true;
    return resolvedWidth;
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::hasValidFieldValue()
//-----------------------------------------------------------------------------
bool PacketValidator::hasValidFieldValue(QSharedPointer<PacketField> packetField, 
    QString const& fieldWidth) const
{
    // Validate that field value bit width <= field width.

    auto value = packetField->getValue();
    if (value.isEmpty())
    {
        return true;
    }

    // Check if value is constant.
    bool valueIsConstant = expressionParser_->isPlainValue(value);
    QString resolvedValue;

    if (!valueIsConstant)
    {
        // Value must be fixed, if opcode qualifier is set.
        if (packetField->getQualifier()->hasType(Qualifier::Type::Opcode))
        {
            return false;
        }

        bool valueIsValid = false;
        resolvedValue = expressionParser_->parseExpression(value, &valueIsValid);

        if (!valueIsValid)
        {
            return false;
        }
        
        // Convert to binary
        resolvedValue = QString::number(resolvedValue.toInt(), 2);
    }
    else
    {
        resolvedValue = parseConstantFieldValue(value);
    }

    // Return false if parsing failed or if value width exceeds field width.
    if (resolvedValue == QStringLiteral("x") || resolvedValue.length() > fieldWidth.toInt())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::findErrorsInPacketFieldValue()
//-----------------------------------------------------------------------------
void PacketValidator::findErrorsInPacketFieldValue(QStringList& errors, QString const& context, 
    QSharedPointer<PacketField> packetField, QString const& resolvedFieldWidth) const
{
    if (!hasValidFieldValue(packetField, resolvedFieldWidth))
    {
        errors.append(QObject::tr("Invalid packet field value in packet field %1 within %2.").arg(
            packetField->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::parseConstantFieldValue()
//-----------------------------------------------------------------------------
QString PacketValidator::parseConstantFieldValue(QString token) const
{
    if (token.contains(QLatin1Char('.')))
    {
        return token;
    }

    int base = baseOf(token);

    // Remove formating of the number.
    static QRegularExpression prefix(QStringLiteral("^([1-9][0-9_]*)?'[sS]?[dDbBoOhH]?"));
    token.remove(0, prefix.match(token).capturedLength());
    token.remove(QLatin1Char('_'));

    bool valid = false;
    qlonglong value = token.toLongLong(&valid, base);
    if (valid == false)
    {
        return QStringLiteral("x");
    }

    // Convert to binary
    return QString::number(value, 2);
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::baseOf()
//-----------------------------------------------------------------------------
int PacketValidator::baseOf(QString const& constantNumber) const
{
    const static QMap<QString, int> base_formats =
    {
        { QString(), 10 },
        { QStringLiteral("d"), 10 },
        { QStringLiteral("D"), 10 },
        { QStringLiteral("h"), 16 },
        { QStringLiteral("H"), 16 },
        { QStringLiteral("o"), 8 },
        { QStringLiteral("O"), 8 },
        { QStringLiteral("b"), 2 },
        { QStringLiteral("B"), 2 },
    };

    static QRegularExpression baseFormat(QStringLiteral("'[sS]?([dDbBoOhH]?)"));

    QString format = baseFormat.match(constantNumber).captured(1);

    return base_formats.value(format, 0);
}
