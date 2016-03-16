//-----------------------------------------------------------------------------
// File: Transactional.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// Describes the ipxact:transactional element within component ports.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONAL_H
#define TRANSACTIONAL_H

#include "WireTypeDef.h"

#include <IPXACTmodels/common/Protocol.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QList>
#include <QString>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Describes the ipxact:transactional element within component ports.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Transactional
{

public:

    /*!
     *  The constructor.
     */
    Transactional();

	/*!
	 *  Copy constructor.
	 */
	Transactional(const Transactional &other);

	/*!
	 *  Assignment operator.
	 */
	Transactional &operator=(const Transactional &other);

	/*!
     *  The destructor.
	 */
	~Transactional();

	/*!
	 *  Get the all logical initiatives allowed settings.
	 *
     *      @return True if the logical initiatives are allowed, false otherwise.
	 */
	bool getAllLogicalInitiativesAllowed() const;

	/*!
	 *  Set the all logical initiatives to allowed or disallowed.
	 *
	 *      @param [in] allLogicalInitiativesAllowed    The new value to be set.
	 */
	void setAllLogicalInitiativesAllowed(bool allLogicalInitiativesAllowed);

    /*!
     *  Get the initiative.
     *
     *      @return The initiative.
     */
    QString getInitiative() const;

    /*!
     *  Set the initiative.
     *
     *      @param [in] newInitiative   The new initiative.
     */
    void setInitiative(QString const& newInitiative);

    /*!
     *  Get the kind.
     *
     *      @return The kind.
     */
    QString getKind() const;

    /*!
     *  Set the kind.
     *
     *      @param [in] newKind     The kind.
     */
    void setKind(QString const& newKind);

    /*!
     *  Get the bus width.
     *
     *      @return The bus width.
     */
    QString getBusWidth() const;

    /*!
     *  Set the bus width.
     *
     *      @param [in] newBusWidth     The new bus width.
     */
    void setBusWidth(QString const& newBusWidth);

    /*!
     *  Get the protocol.
     *
     *      @return Pointer to a protocol used in this port.
     */
    QSharedPointer<Protocol> getProtocol() const;

    /*!
     *  Set a protocol for this port.
     *
     *      @param [in] newProtocol     The new protocol.
     */
    void setProtocol(QSharedPointer<Protocol> newProtocol);

    /*!
     *  Get the transactional type definitions.
     *
     *      @return Pointer to a list containing transactional type definitions.
     */
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > getTransTypeDef() const;

    /*!
     *  Set the transactional type definitions.
     *
     *      @param [in] newTransTypeDefs    The new transactional type definitions.
     */
    void setTransTypeDefs(QSharedPointer<QList<QSharedPointer<WireTypeDef> > > newTransTypeDefs);

    /*!
     *  Get the maximum number of possible connections.
     *
     *      @return Expression containing the maximum number of connections.
     */
    QString getMaxConnections() const;

    /*!
     *  Set the maximum number of connections.
     *
     *      @param [in] newMaxConnectionsExpression     Expression containing the maximum number of connections.
     */
    void setMaxConnections(QString const& newMaxConnectionsExpression);

    /*!
     *  Get the minimum number of connections.
     *
     *      @return The minimum number of connections.
     */
    QString getMinConnections() const;

    /*!
     *  Set the minimum number of connections.
     *
     *      @param [in] newMinConnectionsExpression     The new expression for minimum connections.
     */
    void setMinConnections(QString const& newMinConnectionsExpression);

    /*!
     *  Get a type name associated with a view name.
     *
     *      @param [in] viewName    The associated view name.
     *
     *      @return The type name.
     */
    QString getTypeName(QString const& viewName) const;

    /*!
     *  Set a type name.
     *
     *      @param [in] typeName    The new type name.
     *      @param [in] viewName    The view name associated with the new type name.
     */
    void setTypeName(QString const& typeName, QString const& viewName);

    /*!
     *  Check if a type is contained.
     *
     *      @param [in] viewName    View name associated with the type.
     *
     *      @return True, if a type is contained, false otherwise.
     */
    bool hasType(QString const& viewName) const;

    /*!
     *  Get a type definition.
     *
     *      @param [in] typeName    The name of the type.
     *
     *      @return Type definition associated with the given type name.
     */
    QString getTypeDefinition(QString const& typeName) const;

    /*!
     *  Get the type definitions.
     *
     *      @return A list of type definitions.
     */
    QStringList getTypeDefinitions() const;

    /*!
     *  Set a type definition.
     *
     *      @param [in] typeName        The type name associated with the definition.
     *      @param [in] typeDefinition  The new type definition.
     */
    void setTypeDefinition(const QString& typeName, const QString& typeDefinition);

    /*!
     *  Check if a type definition exists.
     *
     *      @return True, if a type definition exists, false otherwise.
     */
    bool hasTypeDefinitions() const;

private:

    /*!
     *  Copy the transactional type definitions.
     *
     *      @param [in] other   The target of the copying.
     */
    void copyTransactionalTypeDefinitions(const Transactional& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Defines the allowed logical initiatives.
	bool allLogicalInitiativesAllowed_;

	//! Defines the type of access.
    QString initiative_;

    //! Defines the kind.
    QString kind_;

    //! Defines the bus width.
    QString busWidth_;

    //! Defines the used protocol
    QSharedPointer<Protocol> protocol_;

	//! Defines the port type expressed in the default language for this port.
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > transTypeDefs_;

    //! Defines the maximum number of connections.
    QString maxConnections_;

    //! Defines the minimum number of connections.
    QString minConnections_;
};

#endif // TRANSACTIONAL_H
