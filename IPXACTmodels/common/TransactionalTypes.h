//-----------------------------------------------------------------------------
// File: TransactionalTypes.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// NameSpace TransactionalTypes is used to store functions related to transactional ports.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONALTYPES_H
#define TRANSACTIONALTYPES_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QStringList>

//-----------------------------------------------------------------------------
//! NameSpace FileTypes is used to store functions related to file types.
//-----------------------------------------------------------------------------
namespace TransactionalTypes
{
    //! The number of supported file type definitions.
    const unsigned int INITIATIVE_TYPE_COUNT = 4;

	//! The file types specified in the IP-Xact.
	const QString INITIATIVE_TYPES[INITIATIVE_TYPE_COUNT] = {
		"requires",
        "provides",
        "both",
        "phantom"
	};

	/*!
	 *  Checks if the file type belongs to the specified file types.
	 *
	 *      @param [in] fileType    The file type to check.
     *
     *      @return True if the file type is one of the specified ones, false otherwise.
	 */
    IPXACTMODELS_EXPORT bool isIpXactInitiativeType(QString const& initiativeType);

    //! The number of supported kind type definitions.
    const unsigned int KIND_TYPE_COUNT = 5;

    //! The kind types specified in the IP-XACT.
    const QString KIND_TYPES[KIND_TYPE_COUNT] = {
        "tlm_port",
        "tlm_socket",
        "simple_socket",
        "multi_socket",
        "custom"
    };

    /*!
     *  Checks if the kind type belongs to the specified kind types.
     *
     *      @param [in] kindType    The kind type to check.
     *
     *      @return True if the kind type is one of the specified ones, false otherwise.
     */
    IPXACTMODELS_EXPORT bool isIpXactKindType(QString const& kindType);
}

#endif // TRANSACTIONALTYPES_H
