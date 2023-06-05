//-----------------------------------------------------------------------------
// File: WireAbstraction.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.08.2015
//
// Description:
// Implementation for ipxact:wire within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef WIREABSTRACTION_H
#define WIREABSTRACTION_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/Qualifier.h>

#include <QSharedPointer>

class WirePort;

//-----------------------------------------------------------------------------
//! Implementation for ipxact:wire within abstraction definition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT WireAbstraction
{
public:

	//! The default constructor.
	WireAbstraction();

	//! Copy constructor.
	WireAbstraction(const WireAbstraction& other);

	//! Assignment operator.
	WireAbstraction& operator=(const WireAbstraction& other);

	//! The destructor.
	~WireAbstraction();

    /*! Sets the default value for this wire.
	 *
	 *      @param [in] defaultValue    The new default value.
	 */
	void setDefaultValue(QString const& defaultValue);

	/*! Gets the default value of this wire.
	 *
	 *      @return The default value
	 */
	QString getDefaultValue() const;

    /*!
     *  Sets a qualifier for the information the wire carries.
     *
     *      @param [in] qualifier   The qualifier to set.
     */
    void setQualifier(QSharedPointer<Qualifier> qualifier);
    
    /*!
     *  Add a qualifier type for the information the wire carries.
     *
     *      @param [in] qualifierType   The qualifier type to add.
     */
    void addQualifier(Qualifier::Type qualifierType);
    
    /*!
     *  Gets the qualifier for the information the wire carries.
     *
     *      @return The wire qualifier.
     */
	QSharedPointer<Qualifier> getQualifier();

    /*!
     *  Adds a system port definition for the wire.
     *
     *      @param [in] systemPort   The system wire port to add.
     */
    void addSystemPort(QSharedPointer<WirePort> systemPort);

    /*!
     *  Gets the system port definitions for the wire.
     *
     *      @return The system ports for the wire.
     */
    QSharedPointer<QList<QSharedPointer<WirePort> > > getSystemPorts() const;
    
    /*!
     *  Finds system port corresponding the name.
     *
     *      @return The wire port corresponding the name if it exists, else null.
     */
    QSharedPointer<WirePort> findSystemPort(QString const& groupName) const;

    /*!
     *  Checks if the wire has a port definition for initiator mode.
     *
     *      @return True, if initiator mode is defined, otherwise false.
     */
    bool hasInitiatorPort() const;

    /*!
     *  Sets the initiator mode port definitions for the wire.
     *
     *      @param [in] initiatorPort The initiator port definition for the wire.
     */
    void setInitiatorPort(QSharedPointer<WirePort> initiatorPort);

    /*!
     *  Gets the initiator mode port definitions for the wire.
     *
     *      @return The initiator port definition for the wire.
     */
    QSharedPointer<WirePort> getInitiatorPort() const;

    /*!
     *  Checks if the wire has a port definition for target mode.
     *
     *      @return True, if target mode is defined, otherwise false.
     */
    bool hasTargetPort() const;

    /*!
     *  Sets the target mode port definitions for the wire.
     *
     *      @param [in] slavePort The target port definition for the wire.
     */
    void setTargetPort(QSharedPointer<WirePort> targetPort);

    /*!
     *  Gets the target mode port definitions for the wire.
     *
     *      @return The target port definition for the wire.
     */
    QSharedPointer<WirePort> getTargetPort() const;

    /*!
     *  Checks if the wire has a port definition for master mode.
     *
     *      @return True, if master mode is defined, otherwise false.
     */
    bool hasMasterPort() const;
    
    /*!
     *  Sets the master mode port definitions for the wire.
     *
     *      @param [in] masterPort The master port definition for the wire.
     */
    void setMasterPort(QSharedPointer<WirePort> masterPort);
    
    /*!
     *  Gets the master mode port definitions for the wire.
     *
     *      @return The master port definition for the wire.
     */
    QSharedPointer<WirePort> getMasterPort() const;

    /*!
     *  Checks if the wire has a port definition for slave mode.
     *
     *      @return True, if slave mode is defined, otherwise false.
     */
    bool hasSlavePort() const;
  
    /*!
     *  Sets the slave mode port definitions for the wire.
     *
     *      @param [in] slavePort The slave port definition for the wire.
     */
    void setSlavePort(QSharedPointer<WirePort> slavePort);
        
    /*!
     *  Gets the slave mode port definitions for the wire.
     *
     *      @return The slave port definition for the wire.
     */
    QSharedPointer<WirePort> getSlavePort() const;
    
    /*!
     *  Sets the wire to require a driver or not.
     *
     *      @param [in] requiresDriver   The r.
     *
     *      @return <Description>.
     */
    void setRequiresDriver(bool requiresDriver);

    /*!
     *  Checks if the wire requires a driver.
     *
     *      @return True, if the wire requires a driver, otherwise false.
     */
    bool requiresDriver() const;

    /*! Sets the driver type for this wire.
	 *	 
	 *      @param [in] driverType The driver type to set.
	 */
	void setDriverType(General::DriverType driverType);
    
    /*! Gets the driver type for this wire.
	 *	 
	 *      @return The driver type for this wire.
	 */
	General::DriverType getDriverType() const;

    /*!
     *  Gets the wire direction for a given interface mode.
     *
     *      @param [in] mode            The mode to get the direction for.
     *      @param [in] systemGroup     The system group to get direction for.
     *
     *      @return The direction of the wire for the given interface mode.
     */
    DirectionTypes::Direction getDirection(General::InterfaceMode mode,
        QString const& systemGroup) const;

    /*!
     *  Gets the wire width for a given interface mode.
     *
     *      @param [in] mode            The mode to get the width for.
     *      @param [in] systemGroup     The system group to get widthn for.
     *
     *      @return The width of the wire for the given interface mode.
     */
    QString getWidth(General::InterfaceMode mode, QString const& systemGroup) const;

    /*!
     *  Check if the wire has the selected interface mode.
     *
     *      @param [in] mode            The selected interface mode.
     *      @param [in] systemGroup     The selected interface system group.
     *
     *      @return True, if the wire has the selected mode, otherwise false.
     */
    bool hasMode(General::InterfaceMode mode, QString const& systemGroup) const;

private:

    //! The qualifier describing the information the wire carries.
    QSharedPointer<Qualifier> qualifier_ = QSharedPointer<Qualifier>(new Qualifier());

    //! The default value for the wire.
    QString defaultValue_;
    
    //! Specifies whether the port requires a driver.
    bool requiresDriver_;

    //! Specifies the required driver type.
    General::DriverType driverType_;

    //! The port definition for initiator mode.
    QSharedPointer<WirePort> onInitiator_;

    //! The port definition for target mode.
    QSharedPointer<WirePort> onTarget_;

    //! The port definitions for system mode.
    QSharedPointer<QList<QSharedPointer<WirePort> > > onSystem_;

};

#endif /* WIREABSTRACTION_H_ */
