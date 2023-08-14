//-----------------------------------------------------------------------------
// File: FieldAccessPolicyColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Definitions for field access policies table.
//-----------------------------------------------------------------------------

#ifndef FIELDACCESSPOLICYCOLUMNS_H
#define FIELDACCESSPOLICYCOLUMNS_H

namespace FieldAccessPolicyColumns
{
    //! Enumeration for columns in field access policies table.
    enum Columns
    {
        MODE = 0,
        TYPE_DEFINITION,
        ACCESS,
        READ_ACTION,
        READ_RESPONSE,
        TESTABLE,
        TEST_CONSTRAINT,
        RESERVED,
        MODIFIED_WRITE,
        WRITE_VALUE_CONSTRAINT,
        WRITE_CONSTRAINT_MINIMUM,
        WRITE_CONSTRAINT_MAXIMUM,
        COUNT
    };
}

#endif // FIELDACCESSPOLICYCOLUMNS_H



