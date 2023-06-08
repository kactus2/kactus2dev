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

#include <QMap>
#include <QString>
#include <QStringList>

//-----------------------------------------------------------------------------
//! NameSpace FileTypes is used to store functions related to file types.
//-----------------------------------------------------------------------------
namespace TransactionalTypes
{ 
    //! Initiative types.
    enum class Initiative {
        REQUIRES,
        PROVIDES,
        BOTH,
        PHANTOM,
        INITIATIVE_INVALID
    };

    //! Strings for the initiative types.
    QString const INITIATIVE_REQUIRES = QStringLiteral("requires");
    QString const INITIATIVE_PROVIDES = QStringLiteral("provides");
    QString const INITIATIVE_BOTH = QStringLiteral("both");
    QString const INITIATIVE_PHANTOM = QStringLiteral("phantom");
    QString const INITIATIVE_REQUIRES_PROVIDES = QStringLiteral("requires/provides");

    const QMap<Initiative, QString> INITIATIVE_TYPES =
    {
        {Initiative::REQUIRES, INITIATIVE_REQUIRES},
        {Initiative::PROVIDES, INITIATIVE_PROVIDES},
        {Initiative::BOTH, INITIATIVE_BOTH},
        {Initiative::PHANTOM, INITIATIVE_PHANTOM}
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

    //! The kind types specified in the IP-XACT.
    const QStringList KIND_TYPES = {
        QStringLiteral("tlm_port"),
        QStringLiteral("tlm_socket"),
        QStringLiteral("simple_socket"),
        QStringLiteral("multi_socket"),
        QStringLiteral("custom")
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
