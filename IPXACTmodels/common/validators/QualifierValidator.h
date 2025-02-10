//-----------------------------------------------------------------------------
// File: QualifierValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 05.06.2023
//
// Description:
// Validator for ipxact:qualifier.
//-----------------------------------------------------------------------------

#ifndef QUALIFIERVALIDATOR_H
#define QUALIFIERVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/Qualifier.h>

#include <QString>
#include <QStringList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Validator for ipxact:qualifier.
//-----------------------------------------------------------------------------
namespace QualifierValidator
{
    /*!
     *  Validates the given Qualifier.
     *
     *      @param [in] AbstractionDefinition           The Qualifier to validate.
     *
     *      @return True, if the Qualifier is valid IP-XACT, otherwise false.
     */
    IPXACTMODELS_EXPORT bool validate(QSharedPointer<Qualifier> qualifier, Document::Revision revision);

    /*!
     *  Finds possible errors in a Qualifier and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] AbstractionDefinition   The AbstractionDefinition whose errors to find.
     */
    IPXACTMODELS_EXPORT void findErrorsIn(QStringList& errors, QSharedPointer<Qualifier> qualifier,
        QString const& context, Document::Revision revision);

    namespace Details
    {

        /*!
         *	Validates the attributes of a qualifier.
         *  
         *      @param [in] qualifier	The qualifier whose attributes are validated.
         *		
         * 		@return True, if attributes are valid, otherwise false.
         */
        bool hasValidAttributes(QSharedPointer<Qualifier> qualifier);

        /*!
         *	Finds possible errors in a Qualifier's attributes and creates a list of them.
         *  
         *      @param [in] errorList	List of found errors.
         *      @param [in] qualifier	The qualifier whose attributes are checked.
         *      @param [in] context	    The context to help locate the error.
         */
        void findErrorsInAttributes(QVector<QString>& errorList, QSharedPointer<Qualifier> qualifier,
            QString const& context);
    }
}

#endif // QUALIFIERVALIDATOR_H
