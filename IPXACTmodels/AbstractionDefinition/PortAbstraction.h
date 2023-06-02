//-----------------------------------------------------------------------------
// File: PortAbstraction.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.08.2015
//
// Description:
// Implementation for ipxact:port element within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef PORTABSTRACTION_H
#define PORTABSTRACTION_H

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Qualifier.h>

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QSharedPointer>
#include <QString>

class TransactionalAbstraction;
class VendorExtension;
class WireAbstraction;
class Packet;

//-----------------------------------------------------------------------------
//! Implementation for ipxact:port element within abstraction definition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortAbstraction : public NameGroup, public Extendable
{

public:

	//! The default constructor.
	PortAbstraction();

	//! The copy constructor.
	PortAbstraction(const PortAbstraction& other);
  
    //! The destructor.
    ~PortAbstraction();

	//! Assignment operator.
	PortAbstraction& operator=(const PortAbstraction& other);

    /*!
     *  Sets the logical name of the port.
     *
     *      @param [in] logicalName   The name of the port to set.
     */
    void setLogicalName(QString const& logicalName);

    /*!
     *  Gets the logical name of the port.
     *
     *      @return The name of the port.
     */
    QString getLogicalName() const;

    /*!
     *  Sets the condition for the port existence.
     *
     *      @param [in] presentExpression   The condition when port is present.
     */
    void setIsPresent(QString const& presentExpression);

    /*!
     *  Gets the condition for the port existence.
     *
     *      @return The condition when the port is present in the document.
     */
    QString isPresent() const;

    /*!
     *  Sets the port match.
     *
     *      @param [in] match   The match flag.
     */
    void setMatch(bool match);

    /*!
     *  Gets the port match.
     *
     *      @return True, if port has to be present on both sides of a connection.
     */
    bool getMatch() const;

    /*!
     *  Checks if the port has wire definition.
     *
     *      @return True, if the port is a wire definition, otherwise false.
     */
    bool hasWire() const;
    
    /*!
     *  Sets the wire definition for the port.
     *
     *      @param [in] wire   The wire to set.
     */
    void setWire(QSharedPointer<WireAbstraction> wire);

	/*!
	 *  Gets the wire definition of the port.
	 *
	 *      @return The wire definition.
	 */
	QSharedPointer<WireAbstraction> getWire() const;

    /*!
     *  Checks if the port has transactional definition.
     *
     *      @return True, if the port is a transactional definition, otherwise false.
     */  
    bool hasTransactional() const;
    
    /*!
     *  Sets the transactional definition for the port.
     *
     *      @param [in] transactional   The transactional definition to set.
     */
    void setTransactional(QSharedPointer<TransactionalAbstraction> transactional);

    /*!
	 *  Gets the transactional definition of the port.
	 *
	 *      @return The transactional definition.
	 */
    QSharedPointer<TransactionalAbstraction> getTransactional() const;

    /*!
     *  Sets the packets of the port.
     *
     *      @param [in] packets     The packets to set.
     */
    void setPackets(QSharedPointer<QList<QSharedPointer<Packet> > > packets);

    /*!
     *  Gets the packets of the port.
     *
     *      @return The port packets.
     */
    QSharedPointer<QList<QSharedPointer<Packet> > > getPackets() const;

	/*!  Get the default value of the wire port.
	 *
	 *      @return The default value set for the wire port.
	*/
	QString getDefaultValue() const;

	/*! Set the default value for the wire port.
	 *
	 *      @param [in] defaultValue    The default value to set.
	*/
	void setDefaultValue(QString const& defaultValue);

    /*!
     *  Gets the wire qualifier.
     *
     *      @return The wire qualifier.
     */
    Qualifier getQualifier() const;

    /*!
     *  Sets the wire driver requirement.
     *
     *      @param [in] requiresDriver   Whether or not the driver is required on the wire.
     */
    void setRequiresDriver(bool requiresDriver);

    /*!
     *  Checks if the wire requires a driver.
     *
     *      @return True, if a driver is required, otherwise false.
     */
    bool requiresDriver() const;

    /*!
     *  Gets the presence requirement for the port in given interface mode.
     *
     *      @param [in] mode            The mode to get the presence for.
     *      @param [in] systemGroup     The system group, if applicable.
     *
     *      @return The required presence for the mode.
     */
    PresenceTypes::Presence getPresence(General::InterfaceMode mode, QString const& systemGroup) const;

    /*!
     *  Check if the port has the selected interface mode.
     *
     *      @param [in] mode            The selected interface mode.
     *      @param [in] systemGroup     The system group, if applicable.
     *
     *      @return True, if the port has the selected mode, otherwise false.
     */
    bool hasMode(General::InterfaceMode mode, QString const& systemGroup) const;

private:

    //! Condition for port existence.
    QString isPresent_;

    //! Determines if ports in a connection must be present on both sides of a connection.
    bool match_;

    //! Definitions for wire type port.
	QSharedPointer<WireAbstraction> wire_;

    //! Definitions for transactional type port.
    QSharedPointer<TransactionalAbstraction> transactional_;

    //! The packets element within port.
    QSharedPointer<QList<QSharedPointer<Packet> > > packets_ =
        QSharedPointer<QList<QSharedPointer<Packet> > >(new QList<QSharedPointer<Packet> >());
};

#endif /* PORTABSTRACTION_H_ */
