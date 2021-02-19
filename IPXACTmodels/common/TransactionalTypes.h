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
    //! Strings for the initiative types.
    QString const INITIATIVE_REQUIRES = QLatin1String("requires");
    QString const INITIATIVE_PROVIDES = QLatin1String("provides");
    QString const INITIATIVE_BOTH = QLatin1String("both");
    QString const INITIATIVE_PHANTOM = QLatin1String("phantom");
    QString const INITIATIVE_REQUIRES_PROVIDES = QLatin1String("requires/provides");

    //! The number of supported file type definitions.
    const unsigned int INITIATIVE_TYPE_COUNT = 4;

	//! The file types specified in the IP-Xact.
	const QString INITIATIVE_TYPES[INITIATIVE_TYPE_COUNT] = {
        INITIATIVE_REQUIRES,
        INITIATIVE_PROVIDES,
        INITIATIVE_BOTH,
        INITIATIVE_PHANTOM
    };

    //! Initiative types.
    enum Initiative {
        REQUIRES,
        PROVIDES,
        BOTH,
        PHANTOM,
        INITIATIVE_INVALID
    };

    /*!
     *  Change the selected initiative value to string.
     *
     *      @param [in] initiative  The selected initiative value.
     *
     *      @return The selected initiative in string format.
     */
    IPXACTMODELS_EXPORT QString initiativeToString(TransactionalTypes::Initiative const& initiative);

    /*!
     *  Change the selected string to initiative value.
     *
     *      @param [in] str     The selected initiative string.
     *
     *      @return The selected initiative in initiative format.
     */
    IPXACTMODELS_EXPORT TransactionalTypes::Initiative strToInitiative(QString const& str);

	/*!
	 *  Checks if the file type belongs to the specified file types.
	 *
	 *      @param [in] fileType    The file type to check.
     *
     *      @return True if the file type is one of the specified ones, false otherwise.
	 */
    IPXACTMODELS_EXPORT bool isIpXactInitiativeType(QString const& initiativeType);

    /*!
     *  Convert the selected initiative to mirrored initiative.
     *
     *      @param [in] initiative  The selected initiative.
     *
     *      @return The mirrored initiative.
     */
    IPXACTMODELS_EXPORT TransactionalTypes::Initiative convertToMirrored(QString const& initiative);

    //! The number of supported kind type definitions.
    const unsigned int KIND_TYPE_COUNT = 5;

    //! The kind types specified in the IP-XACT.
    const QString KIND_TYPES[KIND_TYPE_COUNT] = {
        QLatin1String("tlm_port"),
        QLatin1String("tlm_socket"),
        QLatin1String("simple_socket"),
        QLatin1String("multi_socket"),
        QLatin1String("custom")
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
