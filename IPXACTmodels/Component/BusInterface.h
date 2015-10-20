//-----------------------------------------------------------------------------
// File: AddressSpace.cpp
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

#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/PortMap.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <IPXACTmodels/common/BooleanValue.h>

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QMap>
#include <QStringList>
#include <QPointF>

class MasterInterface;
class SlaveInterface;
class MonitorInterface;
class MirroredSlaveInterface;

//-----------------------------------------------------------------------------
//! Describes the ipxact:busInterface element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BusInterface : public NameGroup, public Extendable
{

public:

	//! Equals the spirit:monitor element in IP-Xact specification.
	struct MonitorInterface
	{

		//! Defines the interface mode for which this monitor interface can be connected to.
		General::InterfaceMode interfaceMode_;

		//! Defines the name of the system group for this monitor interface.
		QString group_;

		//! The default constructor.
		IPXACTMODELS_EXPORT MonitorInterface();
	};
	
	/*!
	 * Class used to store port maps and references associated with an abstraction type.
	 */
	class AbstractionType
	{
		public:

			//! Specifies views where the abstraction type applies.
			QString viewRef_;

			//! Reference to an abstraction description for this abstractor instance.
			QSharedPointer<ConfigurableVLNVReference> abstractionRef_;

			//! Describes the mapping between the abstraction definition's logical ports and components physical ports.
			QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps_;

			//! Default constructor.
			AbstractionType();

			//! Copy constructor.
			AbstractionType(const AbstractionType &other);

			//! Assignment operator.
			AbstractionType &operator=(const AbstractionType &other);
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

	/*! \brief Check if the bus interface is in a valid state.
	 *
	 * \param physicalPorts     List of the physical ports of the component and their bounds.
	 * \param memoryMaps        List of the memory map names in the component.
     * \param addressSpaces     List of the address space names in the component.
     * \param componentChoices  Choices in the containing component.
	 * \param errorList         The list to add the possible error messages to.
	 * \param parentIdentifier  String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
// 	bool isValid(const QList<General::PortBounds>& physicalPorts, QStringList const& memoryMaps,
//         QStringList const& addressSpaces, QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
// 		QStringList& errorList, const QString& parentIdentifier) const;

	/*! \brief Check if the bus interface is in a valid state.
	 * 
	 * \param physicalPorts List of the physical ports of the component and their bounds.
     * \param memoryMaps List of the memory map names in the component.
     * \param addressSpaces     List of the address space names in the component.
     * \param componentChoices  Choices in the containing component.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
// 	bool isValid(const QList<General::PortBounds>& physicalPorts, QStringList const& memoryMaps,
//         QStringList const& addressSpaces, QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

	/*!
     *  Get the list of the bitSteering attributes.
	 *
	 *      @return A QMap containing the attributes for bitSteering element.
	 */
	const QMap<QString, QString>& getBitSteeringAttributes();

	/*!
     *  Is the bitSteering on or off.
	 *
	 *      @return General::BitSteering enum which is true/false/unspecified.
	 */
	General::BitSteering getBitSteering() const;

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
	VLNV getBusType() const;

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
	General::Endianness getEndianness() const;

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
     *  Get the port maps of this interface.
	 *
	 *      @return Pointer to a list holding the port maps for this interface.
	 */
    QSharedPointer<QList<QSharedPointer<PortMap> > > getPortMaps() const;

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
	void setBitSteeringAttributes(QMap<QString, QString>& bitSteeringAttributes);

	/*!
     *  Set the bit steering for this interface.
	 *
	 *      @param [in] bitSteering     The new bitSteering value.
	 */
	void setBitSteering(General::BitSteering bitSteering);

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
	void setBusType(const VLNV& busType);

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
	void setEndianness(General::Endianness endianness);

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
	QSharedPointer<MasterInterface> getMaster();

	/*!
     *  Set the master element for this interface.
     *
	 *      @param [in] master  A pointer to the new master interface.
	 */
	void setMaster(QSharedPointer<MasterInterface> master);

	/*!
     *  Get the pointer to the monitor interface.
	 *
	 *      @return A pointer to the monitor interface.
	 */
	QSharedPointer<MonitorInterface> getMonitor();

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
	QSharedPointer<SlaveInterface> getSlave();

	/*!
     *  Set the slave element for this interface.
	 *
	 *      @param [in] slave   A pointer to the new slave interface.
	 */
	void setSlave(QSharedPointer<SlaveInterface> slave);

	/*!
     *  Get the system group.
	 *
	 *      @return QString containing the system's group element.
	 */
	QString getSystem() const;

	/*!
     *  Set the system element for this interface.
     *
	 *      @param [in] system  A pointer to the new system instance.
	 */
	void setSystem(const QString& system);

	/*!
     *  Get the pointer to the mirroredSlace element
	 *
	 *      @return A pointer to the mirroredSlave element of this interface.
	 */
	QSharedPointer<MirroredSlaveInterface> getMirroredSlave();

	/*!
     *  Set the mirroredSlave element for this interface.
     *
	 *      @param [in] mirroredSlave   A pointer to the new mirroredSlave instance.
	 */
	void setMirroredSlave(QSharedPointer<MirroredSlaveInterface> mirroredSlave);

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
	const QMap<QString, QString>& getAttributes() const;

	/*!
     *  Set the attributes for the bus interface.
	 * 
	 *      @param [in] attributes  QMap containing the attributes.
	 *
	 */
	void setAttributes(const QMap<QString, QString>& attributes);

	/*!
     *  Get list of the physical port names contained in this interface.
	 *
	 *      @return QStringList Contains the physical port names.
	 */
	QStringList getPhysicalPortNames() const;

	/*!
     *  Get list of the logical port names contained in this interface.
	 *
	 *      @return QStringList Contains the logical port names.
	 */
	QStringList getLogicalPortNames() const;

	/*!
     *  Check if the bus interface contains the specified logical port.
	 *
	 *      @param [in] logicalPortName     Contains the name of the logical port to search for.
	 *
	 *      @return True if the logical port is found, false otherwise.
	 */
	bool hasLogicalPort(const QString& logicalPortName) const;

	/*!
     *  Get the name of the logical port mapped to specified physical port.
	 *
	 *      @param [in] physicalPortName    The name of the physical port that's mapping is searched.
	 *
	 *      @return The name of the logical port mapped with the physical.
	 */
	QString getLogicalPortName(const QString& physicalPortName) const;

	/*!
     *  Check if the bus interface contains the specified physical port.
	 *
	 *      @param [in] physicalPortName    Contains the name of the physical port to search for.
	 *
	 *      @return True if the physical port is found in the port maps, false otherwise.
	 */
	bool hasPhysicalPort(const QString& physicalPortName) const;

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
    QPointF const& getDefaultPos();

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
	VLNV busType_;

	//! Describes further information on the mode for this interface.
	General::InterfaceMode interfaceMode_;

	//! Specifies if the busInterface can be unconnected.
    BooleanValue connectionRequired_;

	//! Number of data bits addressable in the bus interface.
    QString bitsInLau_;

	//! Is bitSteering used.
	General::BitSteering bitSteering_;

	//! The parameters set as attributes for the bit steering.
	QMap<QString, QString> bitSteeringAttributes_;

	//! Indicates the endianness of the bus interface.
	General::Endianness endianness_;

	//! Specifies any parameter data values for this bus interface.
	QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;

	//! A pointer to the master / mirrored master instance
	QSharedPointer<MasterInterface> master_;

	//! A pointer to the slave instance.
	QSharedPointer<SlaveInterface> slave_;

	//! The system or mirrored system instance.
	QString system_;

	//! A pointer to the monitor instance.
	QSharedPointer<MonitorInterface> monitor_;

	//! A pointer to the mirroredSlave instance.
	QSharedPointer<MirroredSlaveInterface> mirroredSlave_;

	//! List of abstraction types existing within this object.
	QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionTypes_;
};

#endif // BUSINTERFACE_H
