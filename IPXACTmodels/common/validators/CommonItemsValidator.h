//-----------------------------------------------------------------------------
// File: CommonItemsValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 19.05.2023
//
// Description:
// Functions for validating common items in IPXACT models.
//-----------------------------------------------------------------------------

#ifndef COMMONITEMSVALIDATOR_H
#define COMMONITEMSVALIDATOR_H

#include <KactusAPI/include/ExpressionParser.h>

#include <QSharedPointer>

namespace CommonItemsValidator
{
    /*!
     * Validates the given name.
     *
     *     @param [in] name The name to validate.
     *
     *     @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name);

    /*!
      * Validates the given isPresent expression.
      *
      *     @param [in] isPresent   The expression to validate.
      *     @param [in] parser      The expression parser to use.
      *
      *     @return True, if the isPresent is valid, otherwise false.
      */
    bool hasValidIsPresent(QString const& isPresent, QSharedPointer<ExpressionParser> parser);

    bool isValidExpression(QString const& expression, QSharedPointer<ExpressionParser> parser);

};


#endif // COMMONITEMSVALIDATOR_H
