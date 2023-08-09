//-----------------------------------------------------------------------------
// File: FieldAccessPolicyColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 8.8.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef FIELDACCESSPOLICYCOLUMNS_H
#define FIELDACCESSPOLICYCOLUMNS_H

namespace FieldAccessPolicyColumns
{
    enum Columns
    {
        MODE = 0,
        TYPE_DEFINITION,
        ACCESS,
        MODIFIED_WRITE,
        WRITE_VALUE_CONSTRAINT,
        READ_ACTION,
        READ_RESPONSE,
        TESTABLE,
        RESERVED,
        COUNT
    };
}

#endif // FIELDACCESSPOLICYCOLUMNS_H



