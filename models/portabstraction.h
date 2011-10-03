/* 
 *
 *  Created on: 18.8.2010
 *      Author: Antti Kamppi
 */

#ifndef PORTABSTRACTION_H_
#define PORTABSTRACTION_H_

#include <models/wireabstraction.h>

#include <QDomNode>
#include <QSharedPointer>
#include <QString>

class TransactionalAbstraction;

/*! \brief Equals the spirit:port element in IP-Xact specification
 *
 * Defines the logical port information for the containing abstraction
 * definition.
 */
class PortAbstraction {

public:

	/*! \brief The constructor
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when mandatory element is missing
	 *
	 * \param portNode A reference to a QDomNode to parse the information from.
	 */
	PortAbstraction(QDomNode& portNode);

	/*! \brief Default constructor
	 *
	*/
	PortAbstraction();

	//! \brief Copy constructor
	PortAbstraction(const PortAbstraction& other);

	//! \brief Assignment operator
	PortAbstraction& operator=(const PortAbstraction& other);

	/*! \brief The destructor
	 *
	 */
	~PortAbstraction();

	/*! \brief Get the description of the port
	 *
	 * \return QString containing the description.
	 */
	QString getDescription() const;

	/*! \brief Get the display name of this port
	 *
	 * \return QString containing the displayName
	 */
	QString getDisplayName() const;

	/*! \brief Get the logical name of this port
	 *
	 * \return QString containing the logical name.
	 */
	QString getLogicalName() const;

	/*! \brief Get the transactional instance
	 *
	 * \return Pointer to the transactional instance. If the port is wire then
	 * null pointer is returned.
	 */
	TransactionalAbstraction* getTransactional() const;

	/*! \brief Get the wire instance
	 *
	 * \return Pointer to the wire instance. If the port is transactional
	 * then null pointer is returned.
	 */
	WireAbstraction* getWire() const;

	/*! \brief Set the description for this port
	 *
	 * \param description QString containing the description.
	 */
	void setDescription(const QString& description);

	/*! \brief Set the display name for this port
	 *
	 * \param displayName QString containing the display name.
	 */
	void setDisplayName(const QString& displayName);

	/*! \brief Set the logical name for this port
	 *
	 * \param logicalName QString containing the logical name.
	 */
	void setLogicalName(const QString& logicalName);

	/*! \brief Set the transactional element for this port
	 *
	 * Calling this function will delete old transactional and wire elements
	 * and change the port type into transactional.
	 *
	 * \param transactional A pointer to the new transactional element.
	 */
	void setTransactional(TransactionalAbstraction* transactional);

	/*! \brief Set the wire element for this port
	 *
	 * Calling this function will delete the old transactional and wire
	 * elements and change the port type into wire.
	 *
	 * \param wire A pointer to the new wire element.
	 */
	void setWire(WireAbstraction* wire);

	/*! \brief Get the type of the port
	 *
	 * \return PortType
	 */
	General::PortType getPortType() const;

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

	/*! \brief Checks if the port has master constraints defined.
	 *
	 * \return True is onMaster element is found.
	*/
	bool hasMaster() const;

	/*! \brief Checks if the port has slave constraints defined.
	 *
	 * \return True if onSlave element is found.
	*/
	bool hasSlave() const;

	/*! \brief Checks if the port has system constraints defined.
	 *
	 * \return True if onSystem element is found.
	*/
	bool hasSystem() const;

	/*! \brief Get pointer to the onMaster element.
	 *
	 * \return Pointer to the onMaster element, null pointer if one is not found.
	*/
	WireAbstraction::WirePort* getMaster() const;

	/*! \brief Get pointer to the onSlave element.
	 *
	 * \return Pointer to the onSlave element, null pointer if one is not found.
	*/
	WireAbstraction::WirePort* getSlave() const;

	/*! \brief Get pointer to the onSystem element.
	 *
	 * \return Pointer to the onSystem element, null pointer if one is not found.
	*/
	WireAbstraction::WirePort* getSystem() const;

	/*! \brief Is the qualifier set as address for the port.
	 *
	 * \return True if qualifier contains field isAddress.
	*/
	bool isAddress() const;

	/*! \brief Sets the qualifier as address and clears others.
	 *
	*/
	void setAsAddress();

	/*! \brief Is the qualifier set as data for the port.
	 *
	 * \return True if qualifier contains field isData.
	*/
	bool isData() const;

	/*! \brief Sets the qualifier as data and clears others.
	 *
	*/
	void setAsData();

	/*! \brief Is the qualifier set as clock for the port.
	 *
	 * \return True if qualifier contains field isClock.
	*/
	bool isClock() const;

	/*! \brief Sets the qualifier as clock and clears others.
	 *
	*/
	void setAsClock();

	/*! \brief Is the qualifier set as reset for the port.
	 *
	 * \return True if qualifier contains field isReset.
	*/
	bool isReset() const;

	/*! \brief Sets the qualifier as reset and clears others.
	 *
	*/
	void setAsReset();

	/*! \brief Clear the qualifier element so no qualifier is set.
	 *
	*/
	void clearQualifier();

	/*! \brief Get the default value of the port
	 *
	 * \return The default value set for the port.
	*/
	int getDefaultValue() const;

	/*! \brief Set the default value for the port.
	 *
	 * \param defaultValue The default value to be set.
	 *
	*/
	void setDefaultValue(const int defaultValue);

	/*! \brief Does the port require a driver
	 *
	 * \return True if driver is needed.
	*/
	bool getRequiresDriver() const;

	/*! \brief Get the driver type for the port (port must require a driver)
	 *
	 * \return General::DriverType defines the type of driver.
	*/
	General::DriverType getDriverType() const;

	/*! \brief Set the width for the onMaster element.
	 *
	 * \param width The number of bits on the signal.
	 *
	*/
	void setMasterWidth(const int width);

	/*! \brief Set the width for the onSlave element.
	 *
	 * \param width The number of bits on the signal.
	 *
	*/
	void setSlaveWidth(const int width);

	/*! \brief Set the width for the onSystem element.
	 *
	 * \param width The number of bits on the signal.
	 *
	*/
	void setSystemWidth(const int width);

	/*! \brief Set the width for the onMaster, onSlave and onSystem elements.
	 *
	 * \param width The number of bits on the signal.
	 *
	*/
	void setAllWidths(const int width);

	/*! \brief Set the direction for the onMaster element.
	 *
	 * \param direction Specifies the direction
	 *
	*/
	void setMasterDirection(const General::Direction direction);

	/*! \brief Set the direction for the onSlave element.
	 *
	 * \param direction Specifies the direction
	 *
	*/
	void setSlaveDirection(const General::Direction direction);

	/*! \brief Set the direction for the onSystem element.
	 *
	 * \param direction Specifies the direction
	 *
	*/
	void setSystemDirection(const General::Direction direction);

	/*! \brief Set the direction for the onMaster, onSlave and onSystem elements.
	 *
	 * \param direction Specifies the direction
	 *
	*/
	void setAllDirections(const General::Direction direction);

	/*! \brief Set the presence for the onMaster element.
	 *
	 * \param presence Specifies the presence.
	 *
	*/
	void setMasterPresence(const General::Presence presence);

	/*! \brief Set the presence for the onSlave element.
	 *
	 * \param presence Specifies the presence.
	 *
	*/
	void setSlavePresence(const General::Presence presence);

	/*! \brief Set the presence for the onSystem element.
	 *
	 * \param presence Specifies the presence.
	 *
	*/
	void setSystemPresence(const General::Presence presence);

	/*! \brief Set the presence for the onMaster, onSlave and onSystem elements.
	 *
	 * \param presence Specifies the presence.
	 *
	*/
	void setAllPresences(const General::Presence presence);

	/*! \brief Set the driver type for the port
	 *
	 * \param type Specifies the driver type required for the port.
	 *
	*/
	void setDriverType(const General::DriverType type);

	/*! \brief Set the requires driver element for the port.
	 *
	 * \param requiresDriver The value to set.
	 *
	*/
	void setRequiresDriver(bool requiresDriver);

	/*! \brief Get the direction of the port in given interface mode.
	 *
	 * \param mode Specifies the mode to which the direction is requested.
	 *
	 * \return General::Direction Specifies the direction of the port.
	*/
	General::Direction getDirection(General::InterfaceMode mode) const;

	/*! \brief Check if the port is required on given interface mode.
	 *
	 * \param mode Specifies the interface mode to check.
	 *
	 * \return bool True if port is required in given interface mode.
	*/
	bool isRequired(General::InterfaceMode mode) const;

	/*! \brief Check if the port is optional on given interface mode.
	 *
	 * \param mode Specifies the interface mode to check.
	 *
	 * \return bool True if port is optional in given interface mode.
	*/
	bool isOptional(General::InterfaceMode mode) const;

	/*! \brief Check if the port is illegal on given interface mode.
	 *
	 * \param mode Specifies the interface mode to check.
	 *
	 * \return bool True if port is illegal in given interface mode.
	*/
	bool isIllegal(General::InterfaceMode mode) const;

	/*! \brief Get the width of the port in specified interface mode.
	 *
	 * \param mode Specifies the interface mode for the port.
	 *
	 * \return int The number of bits the signal has. If unspecified then -1
	 * is returned.
	*/
	int getWidth(General::InterfaceMode mode) const;

private:

	/*!
	 * MANDATORY spirit:logicalName
	 * Gives a name to the logical port that can be used later in component
	 * description.
	 */
	QString logicalName_;

	/*!
	 * OPTIONAL spirit:displayName
	 * Allows a short descriptive text to be associated with the port.
	 */
	QString displayName_;

	/*!
	 * OPTIONAL spirit:description
	 * Allows a short description of the port.
	 */
	QString description_;

	/*!
	 * MANDATORY spirit:transactional (either wire or transactional is mandatory)
	 * Pointer to the Transactional instance. If the port is wire the pointer
	 * is a null pointer.
	 */
	QSharedPointer<TransactionalAbstraction> transactional_;

	/*!
	 * MANDATORY spirit:wire (either wire or transactional is mandatory)
	 * Pointer to the Wire instance. If the port is transactional the pointer
	 * is a null pointer.
	 */
	QSharedPointer<WireAbstraction> wire_;

	/*!
	 * Defines the type of the port.
	 */
	General::PortType portType_;
};

#endif /* PORTABSTRACTION_H_ */
