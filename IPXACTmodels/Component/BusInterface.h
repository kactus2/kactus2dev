//-----------------------------------------------------------------------------
// File: businterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date:
//
// Description:
// Describes the ipxact:busInterface element.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACE_H
#define BUSINTERFACE_H

#include "AbstractionType.h"

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/BooleanValue.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/Component/InitiatorInterface.h>
#include <IPXACTmodels/Component/TargetInterface.h>
#include <IPXACTmodels/Component/MirroredTargetInterface.h>

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QMap>
#include <QStringList>
#include <QPointF>

class MonitorInterface;

//-----------------------------------------------------------------------------
//! Describes the ipxact:busInterface element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BusInterface : public NameGroup, public Extendable
{

public:

    //! The endianness of a bus interface.
    enum Endianness
    {
        LITTLE,
        BIG,
        ENDIANNESS_UNSPECIFIED
    };

	//! Implementation of the ipxact:monitor element.
	struct MonitorInterface
	{
		//! Defines the interface mode for which this monitor interface can be connected to.
		General::InterfaceMode interfaceMode_ = General::MONITOR;

		//! Defines the name of the system group for this monitor interface.
		QString group_;

		//! The default constructor.
		IPXACTMODELS_EXPORT MonitorInterface();
	};

	/*!
     *  Default constructor.
	 */
	BusInterface();

	//! Copy constructor.
	BusInterface(const BusInterface &other);

	//! Assignment operator.
	BusInterface &operator=(const BusInterface &other);

	/*!
     *  The destructor.
	 */
	~BusInterface();

    /*!
     *  Gets the presence.
     *
     *      @return The presence value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new presence value.
     */
    void setIsPresent(QString const& newIsPresent);

	/*!
     *  Get the list of the bitSteering attributes.
	 *
	 *      @return A QMap containing the attributes for bitSteering element.
	 */
	QMap<QString, QString> getBitSteeringAttributes();

	/*!
     *  Is the bitSteering on or off.
	 *
	 *      @return The bit of the bus interface.
	 */
	QString getBitSteering() const;

	/*!
     *  Get bits in lau value.
	 *
	 *      @return The bits in lau value.
	 */
    QString getBitsInLau() const;

	/*!
     *  Get the vlnv to the bus definition.
	 *
	 *      @return The vlnv tag of the bus definition that this bus interface references.
	 */
    ConfigurableVLNVReference getBusType() const;

	/*!
     *  Can this interface be left unconnected.
	 *
     *      @return Qstring of true, false or empty. Empty means an unspecified requirement.
	 */
    QString getConnectionRequired() const;

	/*!
     *  Get the endianness of the bus interface.
	 *
	 *      @return BIG, LITTLE or UNSPECIFIED.
	 */
	Endianness getEndianness() const;

	/*!
     *  Get the mode of this interface.
	 *
	 *      @return The interface mode.
	 */
	General::InterfaceMode getInterfaceMode() const;

	/*!
     *  Get the parameters of the bus interface.
	 *
	 *      @return Pointer to a list holding the parameters for this interface.
	 */
	QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

	/*!
     *  Get the abstraction types for this interface.
	 *
	 *      @return Pointer to a list containing the abstraction types for this interface.
	 */
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > getAbstractionTypes() const;

	/*!
     *  Set the bit steering attributes for this interface.
	 *
	 *      @param [in] bitSteeringAttributes   A QMap holding the new attributes.
	 */
	void setBitSteeringAttributes(QMap<QString, QString> const& bitSteeringAttributes);

	/*!
     *  Set the bit steering for this interface.
	 *
	 *      @param [in] bitSteering     The new bitSteering value.
	 */
	void setBitSteering(QString const& bitSteering);

	/*!
     *  Set the bits in lau value.
	 *
	 *      @param [in] newBitsInLau    The new bits for the lau value.
	 */
    void setBitsInLau(QString const& newBitsInLau);

	/*!
     *  Set the bus definition for this interface.
	 *
	 *      @param busType The vlnv of the bus definition.
	 */
    void setBusType(ConfigurableVLNVReference const& newBusType);

	/*!
     *  Set the connectionRequired value.
	 *
	 *      @param [in] connectionRequired  True or false.
	 */
	void setConnectionRequired(bool connectionRequired);

    /*!
     *  Set the connection requirement to unspecified.
     */
    void clearConnectionRequired();

	/*!
     *  Set the endianness for this interface.
	 *
     *      @param [in] endianness  BIG, LITTLE or UNSPECIFIED.
	 */
	void setEndianness(Endianness endianness);

	/*!
     *  Set the interface mode for this interface.
	 *
	 *      @param [in] interfaceMode   The new interface mode.
	 */
	void setInterfaceMode(General::InterfaceMode interfaceMode);

	/*!
     *  Set the port maps for this interface.
	 *
	 *      @param [in] portMaps    Pointer to a list containing the port maps for this interface.
	 */
    void setPortMaps(QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps);

	/*!
     *  Set the port maps for this interface.
	 *
	 *      @param [in] abstractionTypes    Pointer to a list containing the abstraction types for this interface.
	 */
    void setAbstractionTypes(QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionTypes);

	/*!
     *  Get the pointer to the master-element.
	 *
	 *      @return A pointer to the master-instance of the interface.
	 */
	QSharedPointer<InitiatorInterface> getMaster() const;

	/*!
     *  Set the master element for this interface.
     *
	 *      @param [in] master  A pointer to the new master interface.
	 */
	void setMaster(QSharedPointer<InitiatorInterface> master);

	/*!
	 *  Get the pointer to the initiator-element.
	 *
	 *      @return A pointer to the initiator-instance of the interface.
	 */
	QSharedPointer<InitiatorInterface> getInitiator() const;

	void setInitiator(QSharedPointer<InitiatorInterface> initiator);

	/*!
     *  Get the pointer to the monitor interface.
	 *
	 *      @return A pointer to the monitor interface.
	 */
	QSharedPointer<MonitorInterface> getMonitor() const;

	/*!
     *  Set the monitor element for this interface.
	 *
	 *      @param [in] monitor     A pointer to the new monitor instance.
	 */
	void setMonitor(QSharedPointer<MonitorInterface> monitor);

	/*!
     *  Get the pointer to the slave instance of the interface.
	 *
	 *      @return A pointer to the slave instance of the interface.
	 */
	QSharedPointer<TargetInterface> getSlave() const;

	/*!
     *  Set the slave element for this interface.
	 *
	 *      @param [in] slave   A pointer to the new slave interface.
	 */
	void setSlave(QSharedPointer<TargetInterface> slave);

	QSharedPointer<TargetInterface> getTarget() const;
	void setTarget(QSharedPointer<TargetInterface> target);
	/*!
     *  Get the system group name.
	 *
	 *      @return The system group name.
	 */
	QString getSystem() const;

	/*!
     *  Set the system element for this interface.
     *
	 *      @param [in] systemGroupName  The name of the system group to set.
	 */
	void setSystem(QString const& systemGroupName);

	/*!
     *  Get the pointer to the mirroredSlace element
	 *
	 *      @return A pointer to the mirroredSlave element of this interface.
	 */
	QSharedPointer<MirroredTargetInterface> getMirroredSlave() const;

	/*!
     *  Set the mirroredSlave element for this interface.
     *
	 *      @param [in] mirroredSlave   A pointer to the new mirroredSlave instance.
	 */
	void setMirroredSlave(QSharedPointer<MirroredTargetInterface> mirroredSlave);

	/*!
	 *  Get the pointer to the mirroredTarget element
	 *
	 *      @return A pointer to the mirroredTarget element of this interface.
	 */
	QSharedPointer<MirroredTargetInterface> getMirroredTarget() const;

	/*!
	 *  Set the mirroredTarget element for this interface.
	 *
	 *      @param [in] mirroredTarget   A pointer to the new mirroredTarget instance.
	 */
	void setMirroredTarget(QSharedPointer<MirroredTargetInterface> mirroredTarget);

	/*!
     *  Does this bus interface have a bridge element defined.
	 *
	 *      @return True if a bridge is found, otherwise false.
	 */
	bool hasBridge() const;

	/*!
     *  Get the attributes of the bus interface.
	 *
	 *      @return QMap containing the attributes.
 	 */
	QMap<QString, QString> getAttributes() const;

	/*!
     *  Set the attributes for the bus interface.
	 * 
	 *      @param [in] attributes  QMap containing the attributes.
	 */
	void setAttributes(QMap<QString, QString> const& attributes);

    /*!
     *  Check if the selected physical port has been mapped in the bus interface.
     *
     *      @param [in] portName    Name of the selected physical port.
     *
     *      @return True, if the physical port has been mapped, false otherwise.
     */
    bool containsMappedPhysicalPort(QString const& portName) const;

    /*!
     *  Check if the selected logical port has been mapped in the bus interface.
     *
     *      @param [in] portName    Name of the selected logical port.
     *
     *      @return True, if the selected logical port has been mapped in the bus interface.
     */
    bool containsMappedlogicalPort(QString const& portName) const;

    /*!
     *  Get the names of all the mapped physical ports.
     *
     *      @return A list of the names of all the mapped physical ports.
     */
    QStringList getAllMappedPhysicalPorts() const;

    /*!
     *  Get the port maps.
     *
     *      @return A list containing all the port maps.
     */
    QSharedPointer<QList<QSharedPointer<PortMap> > > getAllPortMaps() const;

    /*!
     *  Get all the port maps contained within the abstraction type with the selected view reference.
     *
     *      @param [in] view    Name of the view referenced by the selected abstraction type.
     *
     *      @return Port maps contained within the abstraction type referencing the selected view.
     */
    QList<QSharedPointer<PortMap> > getPortMapsForView(QString const& view) const;

    /*!
     *  Get the abstraction type referencing the selected view.
     *
     *      @param [in] view    Name of the selected view referenced by the abstraction type.
     *
     *      @return The abstraction type referencing the selected view.
     */
    QSharedPointer<AbstractionType> getAbstractionContainingView(QString const& view) const;

    /*!
     *  Remove all of the port maps contained within all of the abstraction types.
     */
    void clearAllPortMaps();

    /*!
     *  Sets the MCAPI port ID to the bus interface's parameters.
     */
    void setMCAPIPortID(int portID);

    /*!
     *  Returns the MCAPI port ID, or -1 if not set.
     */
    int getMCAPIPortID() const;

    /*!
     *  Sets the default position in the parent component's graphical representation.
     *
     *      @param [in] pos The position to set.
     */
    void setDefaultPos(QPointF const& pos);

    /*!
     *  Returns the default position in the parent component's graphical representation.
     *
     *      @return The default position.
     */
    QPointF getDefaultPos();

	/*!
     *  Get the memory map reference of a slave interface.
	 *
	 *      @return The name of the memory map referenced by slave interface.
	 */
	QString getMemoryMapRef() const;

	/*!
     *  Get the address space reference of a master or mirrored master interface.
	 *
	 *      @return The name of the address space referenced by interface.
	 */
	QString getAddressSpaceRef() const;

private:

    /*!
     *  Copy the abstraction types.
     *
     *      @param [in] other   The bus interface being copied.
     */
    void copyAbstractionTypes(const BusInterface& other);

    /*!
     *  Copy the parameters.
     *
     *      @param [in] other   The bus interface being copied.
     */
    void copyParameters(const BusInterface& other);

    /*!
     *  Copy the interface modes.
     *
     *      @param [in] other   The bus interface being copied.
     */
    void copyInterfaceModes(const BusInterface& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Presence of the businterface.
	QString isPresent_;

	//! Contains the attributes for the bus interface.
	QMap<QString, QString> attributes_;
	
	//! The vlnv of the bus definition this bus interface is referenced.
    ConfigurableVLNVReference busType_;

    //! List of abstraction types existing within this object.
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionTypes_ =
		QSharedPointer<QList<QSharedPointer<AbstractionType> > >(new QList<QSharedPointer<AbstractionType> >);

	//! Describes further information on the mode for this interface.
	General::InterfaceMode interfaceMode_ = General::INTERFACE_MODE_COUNT;

	//! Specifies if the busInterface can be unconnected.
    BooleanValue connectionRequired_;

	//! Number of data bits addressable in the bus interface.
    QString bitsInLau_;

	//! Is bitSteering used.
	QString bitSteering_;

	//! The parameters set as attributes for the bit steering.
	QMap<QString, QString> bitSteeringAttributes_;

	//! Indicates the endianness of the bus interface.
	Endianness endianness_ = BusInterface::ENDIANNESS_UNSPECIFIED;

	//! Specifies any parameter data values for this bus interface.
	QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_ =
		QSharedPointer<QList<QSharedPointer<Parameter> > >(new QList<QSharedPointer<Parameter> >);

	//! A pointer to the master / mirrored master instance
	QSharedPointer<InitiatorInterface> initiator_ = nullptr;

	//! A pointer to the slave instance.
	QSharedPointer<TargetInterface> target_ = nullptr;

	//! The system or mirrored system group name.
	QString systemGroup_;

	//! A pointer to the monitor instance.
	QSharedPointer<MonitorInterface> monitor_ = nullptr;

	//! A pointer to the mirroredTarget instance.
	QSharedPointer<MirroredTargetInterface> mirroredTarget_ = nullptr;


};

Q_DECLARE_METATYPE(QSharedPointer<BusInterface>);

#endif // BUSINTERFACE_H
