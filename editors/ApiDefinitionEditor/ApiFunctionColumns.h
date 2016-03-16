//-----------------------------------------------------------------------------
// File: ApiFunctionColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.10.2015
//
// Description:
// Common declarations for editing API functions.
//-----------------------------------------------------------------------------

#ifndef API_FUNCTION_COLUMNS_H
#define API_FUNCTION_COLUMNS_H

namespace ApiFunctionColumns
{
    //-----------------------------------------------------------------------------
    // Constants defining which column represents what kind of information.
    //-----------------------------------------------------------------------------
    enum ApiFunctionParamColumn
    {
        API_FUNC_PARAM_COL_NAME = 0,      //!< Column for the parameter name.
        API_FUNC_PARAM_COL_TYPE,          //!< Column for setting the value type for the parameter.
        API_FUNC_PARAM_CONTENT_SOURCE,    //!< Column for setting COM content source.
        API_FUNC_PARAM_COM_TRANSFER_TYPE, //!< Column for setting allowed COM transfer type.
        API_FUNC_PARAM_COM_DIRECTION,     //!< Column for setting allowed COM direction.
        API_FUNC_PARAM_DEPENDENT_PARAM,   //!< Column for setting the index of the dependent parameter.
        API_FUNC_PARAM_COL_DESC,          //!< Column for adding a description for the parameter.
        API_FUNC_PARAM_COL_COUNT
    };
}

#endif // API_FUNCTION_COLUMNS_H