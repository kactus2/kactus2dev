/* 
 *
 *  Created on: 27.7.2010
 *      Author: Antti Kamppi
 */

#ifndef BUSINTERFACE_H_
#define BUSINTERFACE_H_

#include <LibraryManager/vlnv.h>
#include "generaldeclarations.h"
#include "parameter.h"
 
#include <QDomNode>
#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QMap>
#include <QXmlStreamWriter>
#include <QStringList>  

class MasterInterface;
class SlaveInterface;
class MonitorInterface;
class MirroredSlaveInterface;

/*! \brief Equals the spirit:busInterface element in IP-Xact specification.
 *
 * BusInterface defines properties of the specific interface in a component.
 */
class BusInterface {

public:

	/*! \brief Equals the spirit:monitor element in IP-Xact specification.
	 *
	 * MonitorInterface holds info on what kind of interface types this
	 * monitor can be connected to.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	struct MonitorInterface {

		/*! \brief MANDATORY spirit:interfaceMode
		 * Defines the interface mode for which this monitor interface can be
		 * connected to.
		 */
		General::InterfaceMode interfaceMode_;

		/*! \brief OPTIONAL spirit:group
		 * Defines the name of the system group for this monitor interface.
		 */
		QString group_;

		/*! \brief The constructor
		 *
		 * \param monitorNode A reference to a QDomNode to parse the
		 * information from.
		 */
		MonitorInterface(QDomNode& monitorNode);

		/*! \brief The default contructor
		 *
		*/
		MonitorInterface();
	};

	/*! \brief The constructor
	 *
	 * \param busInterface A reference to a QDomNode to parse the information
	 * from.
	 * \param isBus A reference to a boolean variable in Component to specify
	 * if the component is a bus or normal component.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	BusInterface(QDomNode &busInterface);

	/*! \brief Default constructor
	 *
	 */
	BusInterface();

	//! \brief Copy constructor
	BusInterface(const BusInterface &other);

	//! \brief Assignment operator
	BusInterface &operator=(const BusInterface &other);

	/*! \brief The destructor
	 *
	 */
	~BusInterface();

	/*! \brief get the vlnv to the abstraction definition
	 *
	 * \return The vlnv tag of the abstraction definition where this bus interface
	 * is referenced.
	 */
	VLNV getAbstractionType() const;

	/*! \brief Get the list of the bitSteering attributes
	 *
	 * \return A QMap containing the attributes for bitSteering element
	 */
	const QMap<QString, QString>& getBitSteeringAttributes();

	/*! \brief Is the bitSteering on or off
	 *
	 * \return General::BitSteering enum which is true/false/unspecified.
	 */
	General::BitSteering getBitSteering() const;

	/*! \brief Get bits in lau value
	 *
	 * \return The spirit:bitsInLau value
	 */
	unsigned int getBitsInLau() const;

	/*! \brief Get the vlnv to the bus definition
	 *
	 * \return The vlnv tag of the bus definition that this bus interface is
	 * referenced.
	 */
	VLNV getBusType() const;

	/*! \brief Can this interface be left unconnected
	 *
	 * \return boolean, true if interface must be connected, else if interface
	 * can be left unconnected.
	 */
	bool getConnectionRequired() const;

	/*! \brief Get the endianness of the bus interface
	 *
	 * \return BIG or LITTLE
	 */
	General::Endianness getEndianness() const;

	/*! \brief Get the mode of this interface
	 *
	 * \return The interface mode
	 */
	General::InterfaceMode getInterfaceMode() const;

	/*! \brief get the name of the bus interface
	 *
	 * \return The name of the interface.
	 */
	QString getName() const;

	/*! \brief Get the display name of the bus interface
	 *
	 * \return QString containing the display name.
	*/
	QString getDisplayName() const;

	/*! \brief Get the description of the bus interface
	 *
	 * \return QString containing the description.
	*/
	QString getDescription() const;

	/*! \brief Get the parameters of the bus interface
	 *
	 * \return A QList holding the parameters for this interface.
	 */
	QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Get the port maps for this interface
	 *
	 * \return A QList holding the port maps for this interface.
	 */
	QList<QSharedPointer<General::PortMap> >& getPortMaps();

	/*! \brief Set the abstractionType for this interface
	 *
	 * Calling this function deletes the old abstraction type element.
	 *
	 * \param abstractionType The vlnv of the abstraction definition
	 */
	void setAbstractionType(const VLNV& abstractionType);

	/*! \brief set the bit steering attributes for this interface
	 *
	 * Calling this function removes old bit steering attributes.
	 *
	 * \param bitSteeringAttributes A QList holding the attributes
	 */
	void setBitSteeringAttributes(QMap<QString, QString>
	&bitSteeringAttributes);

	/*! \brief Set the bit steering for this interface
	 *
	 * \param bitSteering Enum General::BitSteering
	 */
	void setBitSteering(General::BitSteering bitSteering);

	/*! \brief Set the bits in lau value
	 *
	 * \param bitsInLau The wanted bits in lau value
	 */
	void setBitsInLau(unsigned int bitsInLau);

	/*! \brief Set the bus definition for this interface
	 *
	 * Calling this function deletes the old bus type element.
	 *
	 * \param busType The vlnv of the bus definition
	 */
	void setBusType(const VLNV& busType);

	/*! \brief Set the connectionRequired value
	 *
	 * \param connectionRequired, true or false
	 */
	void setConnectionRequired(bool connectionRequired);

	/*! \brief Set the endianness for this interface
	 *
	 * \param endianness, BIG or LITTLE.
	 */
	void setEndianness(General::Endianness endianness);

	/*! \brief Set the interface mode for this interface
	 *
	 * Note: Calling this function will erase the defined elemets that are
	 * associated with previous interface types. I.e if new interface mode is 
	 * master and previous was slave then the slave settings are lost when function
	 * is called.
	 * 
	 * \param interfaceMode The interface mode
	 */
	void setInterfaceMode(General::InterfaceMode interfaceMode);

	/*! \brief Set the name of this interface
	 *
	 * \param name The name wanted
	 */
	void setName(const QString &name);

	/*! \brief Set the display name for the bus interface
	 *
	 * \param displayName QString containing the display name.
	 *
	*/
	void setDisplayName(const QString& displayName);

	/*! \brief Set the description for the bus interface.
	 *
	 * \param description QString containing the description.
	 *
	*/
	void setDescription(const QString& description);

	/*! \brief Set the parameters of this interface
	 *
	 * Calling this function deletes the old parameters
	 *
	 * \param parameters A QList holding the parameters.
	 */
	void setParameters(QList<QSharedPointer<Parameter> > &parameters);

	/*! \brief Set the port maps for this interface
	 *
	 * Calling this function deletes the old port maps.
	 *
	 * \param portMaps A QList holding the port maps for this interface.
	 */
	void setPortMaps(QList<QSharedPointer<General::PortMap> > &portMaps);

	/*! \brief Get the pointer to the master-element
	 *
	 * \return A pointer to the master-instance of the interface.
	 */
	MasterInterface* getMaster() const;

	/*! \brief Get the pointer to the monitor instance
	 *
	 * \return A pointer to the monitor instance of the interface.
	 */
	MonitorInterface* getMonitor() const;

	/*! \brief Get the pointer to the slave instance of the interface.
	 *
	 * \return A pointer to the slave instance of the interface.
	 */
	SlaveInterface* getSlave() const;

	/*! \brief Get the system group
	 *
	 * \return QString containing the system's group element.
	 */
	QString getSystem() const;

	/*! \brief Set the master element for this interface
	 *
	 * Calling this function will delete old master, slave, mirroredSlave,
	 * system and monitor instances.
	 * setInterfaceMode must be called after this function to define the
	 * type of the businterface as master or mirroredMaster.
	 *
	 * \param master A pointer to the new master instance
	 */
	void setMaster(MasterInterface* master);

	/*! \brief Set the monitor element for this interface
	 *
	 * Calling this function will delete old master, slave, mirroredSlave,
	 * system and monitor instances.
	 * This function changes the interfaceMode as GENERAL::MONITOR
	 *
	 * \param monitor A pointer to the new monitor instance.
	 */
	void setMonitor(MonitorInterface* monitor);

	/*! \brief Set the slave element for this interface.
	 *
	 * Calling this function will delete old master, slave, mirroredSlave,
	 * system and monitor instances.
	 * This function changes the interfaceMode as GENERAL::SLAVE
	 *
	 * \param slave A pointer to the new slave instance.
	 */
	void setSlave(SlaveInterface* slave);

	/*! \brief Set the system element for this interface.
	 *
	 * Calling this function will delete old master, slave, mirroredSlave,
	 * system and monitor instances.
	 * setInterfaceMode must be called after this function to define the
	 * type of the businterface as system or mirroredSystem.
	 *
	 * \param system A pointer to the new system instance.
	 */
	void setSystem(const QString& system);

	/*! \brief Get the pointer to the mirroredSlace element
	 *
	 * \return A pointer to the mirroredSlave element of this interface.
	 */
	MirroredSlaveInterface* getMirroredSlave() const;

	/*! \brief Set the mirroredSlave element for this interface.
	 *
	 * Calling this function will delete old master, slave, mirroredSlave,
	 * system and monitor instances.
	 * This function changes the interfaceMode to GENERAL::MIRROREDSLAVE.
	 *
	 * \param mirroredSlave A pointer to the new mirroredSlave instance.
	 */
	void setMirroredSlave(MirroredSlaveInterface* mirroredSlave);

	/*! \brief Write the contents of the class using the writer.
	 *
	 * Uses the specified writer to write the class contents into file as valid
	 * IP-Xact.
	 *
	 * \param writer A reference to a QXmlStreamWrite instance that is used to
	 * write the document into file.
	 *
	 * Exception guarantee: basic
	 * \exception Write_error Occurs if class or one of it's member classes is
	 * not valid IP-Xact in the moment of writing.
	 */
	void write(QXmlStreamWriter& writer);

	/*! \brief Does this bus interface have a bridge element defined
	 *
	 * \return True if bridge is found, otherwise false.
	 */
	bool hasBridge() const;

	/*! \brief Get the attributes of the bus interface.
	 * 
	 * Key = name of the attribute
	 * Value = value of the attribute
	 *
	 * \return QMap containing the attributes.
	*/
	const QMap<QString, QString>& getAttributes() const;

	/*! \brief Set the attributes for the bus interface.
	 * 
	 * Key = name of the attribute
	 * Value = value of the attribute
	 * 
	 * \param attributes QMap containing the attributes.
	 *
	*/
	void setAttributes(const QMap<QString, QString>& attributes);

	/*! \brief Get list of the physical port names contained in this interface.
	 *
	 * \return QStringList Contains the physical port names.
	*/
	QStringList getPhysicalPortNames() const;

	/*! \brief Get list of the logical port names contained in this interface.
	 *
	 * \return QStringList Contains the logical port names.
	*/
	QStringList getLogicalPortNames() const;

	/*! \brief Check if the bus interface contains the specified logical port.
	 *
	 * \param logicalPortName Contains the name of the logical port to search for.
	 *
	 * \return bool True if the logical port is found.
	*/
	bool hasLogicalPort(const QString& logicalPortName) const;

    /*!
     *  Sets the MCAPI port ID to the bus interface's parameters.
     */
    void setMCAPIPortID(int portID);

    /*!
     *  Returns the MCAPI port ID, or -1 if not set.
     */
    int getMCAPIPortID() const;

private:

	//! \brief Contains the name, display name and description of bus interface.
	General::NameGroup nameGroup_;

	//! \brief Contains the attributes for the bus interface
	QMap<QString, QString> attributes_;
	
	/*! \brief The vlnv of the bus definition this bus interface is referenced.
	 * MANDATORY
	 */
	VLNV busType_;

	/*! \brief Specifies abstraction definition this bus interface references.
	 * OPTIONAL
	 */
	VLNV abstractionType_;

	/*! \brief Describes further information on the mode for this interface.
	 * OPTIONAL
	 */
	General::InterfaceMode interfaceMode_;

	/*! \brief Specifies if the busInterface can be unconnected.
	 * OPTIONAL
	 * If false, this interface may be left unconnected.
	 * (false by default)
	 */
	bool connectionRequired_;

	/*! \brief Contains the mapping of the ports.
	 * OPTIONAL
	 * Describes the mapping between the abstraction definition's
	 * logical ports and components physical ports.
	 */
	QList<QSharedPointer<General::PortMap> > portMaps_;

	/*!\brief Number of data bits addressable in the bus interface.
	 * OPTIONAL
	 * Describes the number of data bits that are addressable by the
	 * LSB in the bus interface
	 * (default value 8)
	 */
	unsigned int bitsInLau_;

	/*! \brief Is bitSteering used.
	 * OPTIONAL
	 * Does this interface have the ability to dynamically align data
	 * on different byte channels on data bus.
	 */
	General::BitSteering bitSteering_;

	/*! \brief The parameters set as attributes for the bit steering
	 */
	QMap<QString, QString> bitSteeringAttributes_;

	/*! \brief Indicates the endianness of the bus interface.
	 * OPTIONAL
	 * (default value little)
	 */
	General::Endianness endianness_;

	/*! \brief Specifies any parameter data values for this bus interface.
	 * OPTIONAL
	 */
	QList<QSharedPointer<Parameter> > parameters_;

	/*! \brief spirit:master or spirit:mirroredMaster
	 * (one of five is mandatory)
	 * A pointer to the master instance
	 */
	QSharedPointer<MasterInterface> master_;

	/*! \brief spirit:slave (one of the five is mandatory)
	 * A pointer to the slave instance.
	 */
	QSharedPointer<SlaveInterface> slave_;

	/*! \brief spirit:system or spirit:mirroredSystem
	 * (one of five is mandatory)
	 * name of the group.
	 */
	QString system_;

	/*! \brief spirit:monitor (one of five is mandatory)
	 * A pointer to the monitor instance.
	 */
	QSharedPointer<MonitorInterface> monitor_;

	/*! \brief spirit:mirroredSlave (one of five is mandatory)
	 * A pointer to the mirroredSlave instance.
	 */
	QSharedPointer<MirroredSlaveInterface> mirroredSlave_;
};

#endif /* BUSINTERFACE_H_ */
