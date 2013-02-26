//-----------------------------------------------------------------------------
// File: AddressValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.9.2012
//
// Description:
// Implements the address validator class.
//-----------------------------------------------------------------------------

#include "AddressValidator.h"

#include <common/utils.h>

#include <QRegExp>

//-----------------------------------------------------------------------------
// Function: AddressValidator::AddressValidator()
//-----------------------------------------------------------------------------
AddressValidator::AddressValidator(QObject* parent)
    : QValidator(parent),
      minAddress_(0),
      maxAddress_(0xFFFFFFFF)
{

}

//-----------------------------------------------------------------------------
// Function: AddressValidator::~AddressValidator()
//-----------------------------------------------------------------------------
AddressValidator::~AddressValidator()
{

}

//-----------------------------------------------------------------------------
// Function: AddressValidator::fixup()
//-----------------------------------------------------------------------------
void AddressValidator::fixup(QString& input) const
{
    // If the input is 100% valid, no need to do anything.
    QRegExp exactRegex("0x[0-9A-Fa-f]{8}");

    if (exactRegex.exactMatch(input))
    {
        return;
    }

    // If the input has too few characters, pad it with zero.
    QRegExp shortRegex("0x[0-9A-Fa-f]{,7}");

    if (shortRegex.exactMatch(input))
    {
        while (input.length() < 10)
        {
            input.append('0');
        }
    }

    // If the input has too many characters, remove the LSBs.
    if (input.length() > 10)
    {
        input = input.left(10);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressValidator::validate()
//-----------------------------------------------------------------------------
AddressValidator::State AddressValidator::validate(QString& input, int& pos) const
{
    // Check if the input is 100% valid.
    QRegExp exactRegex("0x[0-9A-Fa-f]{8}");

    if (exactRegex.exactMatch(input) && validateAddressBounds(input))
    {
        return Acceptable;
    }

    // Otherwise check if the input is an intermediate.
    QRegExp intermediateRegex("0|0x[0-9A-Fa-f]{,7}");

    if (input.isEmpty() || intermediateRegex.exactMatch(input))
    {
        return Intermediate;
    }

    return Invalid;
}

//-----------------------------------------------------------------------------
// Function: AddressValidator::setMinAddress()
//-----------------------------------------------------------------------------
void AddressValidator::setMinAddress(unsigned int address)
{
    minAddress_ = address;
}

//-----------------------------------------------------------------------------
// Function: AddressValidator::setMaxAddress()
//-----------------------------------------------------------------------------
void AddressValidator::setMaxAddress(unsigned int address)
{
    maxAddress_ = address;
}

//-----------------------------------------------------------------------------
// Function: AddressValidator::validateAddressBounds()
//-----------------------------------------------------------------------------
bool AddressValidator::validateAddressBounds(QString const& input) const
{
    unsigned int address = Utils::str2Uint(input);
    return (address >= minAddress_ && address <= maxAddress_);
}
