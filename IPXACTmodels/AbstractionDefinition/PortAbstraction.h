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
#include <IPXACTmodels/generaldeclarations.h>

#include <QSharedPointer>
#include <QString>

class TransactionalAbstraction;
class VendorExtension;
class WireAbstraction;

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
     *  Sets the wire qualifier.
     *
     *      @param [in] qualifierType   The qualifier to set.
     */
    void setQualifier(Qualifier::Type qualifierType);

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

    General::Presence getPresence(General::InterfaceMode mode) const;

    //TransactionalAbstraction* getTransactional() const;

private:

    //! Condition for port existence.
    QString isPresent_;

	//QSharedPointer<TransactionalAbstraction> transactional_;

    //! Definitions for wire type port.
	QSharedPointer<WireAbstraction> wire_;

};

#endif /* PORTABSTRACTION_H_ */
