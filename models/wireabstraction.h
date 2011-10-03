/* 
 *
 *  Created on: 19.8.2010
 *      Author: Antti Kamppi
 */

#ifndef WIREABSTRACTION_H_
#define WIREABSTRACTION_H_

#include "generaldeclarations.h"

#include <QSharedPointer>
#include <QDomNode>

struct Qualifier;

/*! \brief Equals the spirit:wire in IP-Xact specification under the
 * abstractionDefinition element.
 *
 * Represents a port that carries logic values or an array of logic values.
 */
class WireAbstraction {

public:

	/*! \brief Equals the spirit:wirePort element in IP-Xact specification
	 *
	 * Specifies what elements are used in a wire port.
	 */
	struct WirePort {

		/*!
		 * OPTIONAL spirit:presence
		 * Provides the capability to require or forbid a port from appearing
		 * in a busInterface.
		 *
		 * Default value : OPTIONAL
		 */
		General::Presence presence_;

		/*!
		 * OPTIONAL spirit:width
		 * How many bits are needed to represent this port.
		 */
		int width_;

		/*!
		 * OPTIONAL spirit:optional
		 * Restricts the direction of the port.
		 */
		General::Direction direction_;

		// TODO missing spirit:modeConstraints element
		// TODO missing spirit:mirroredModeConstraints element

		/*! \brief The constructor
		 *
		 * \param wireNode A reference to a QDomNode to parse the information
		 * from.
		 *
		 * Exception guarantee: basic
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this class or one of it's member classes.
		 */
		WirePort(QDomNode& wireNode);

		/*! \brief The default constructor
		 *
		*/
		WirePort();

		/*! \brief Copy constructor
		 *
		 * \param other reference to the WirePort to copy.
		 *
		*/
		WirePort(const WirePort& other);

		/*! \brief Assignment operator
		 *
		 * \param other Reference to the WirePort to assign.
		 *
		 * \return Reference to this WirePort.
		*/
		WirePort& operator=(const WirePort& other);
	};

	/*! \brief The constructor
	 *
	 * \param wireNode A reference to a QDomNode to parse the information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	WireAbstraction(QDomNode& wireNode);

	/*! \brief The default constructor
	 *
	*/
	WireAbstraction();

	//! \brief Copy constructor
	WireAbstraction(const WireAbstraction& other);

	//! \brief Assignment operator
	WireAbstraction& operator=(const WireAbstraction& other);

	/*! \brief The destructor
	 *
	 */
	~WireAbstraction();

	/*! \brief Get the default value of this port
	 *
	 * \return int containing the default value
	 */
	int getDefaultValue() const;

	/*! \brief Get the driver type of this port
	 *
	 * \return enum DriverType defining the driver type.
	 */
	General::DriverType getDriverType() const;

	/*! \brief Get the onMaster constraints
	 *
	 * \return A pointer to the onMaster WirePort instance
	 */
	WirePort* getOnMaster() const;

	/*! \brief Get the onSlave constraints
	 *
	 * \return A pointer to the onSlave WirePort instance.
	 */
	WirePort* getOnSlave() const;

	/*! \brief Get the onSystem constraints
	 *
	 * \return A pointer to the onSystem WirePort instance.
	*/
	WirePort* getOnSystem() const;

	/*! \brief Get the qualifier element for this port
	 *
	 * \return pointer to the qualifier instance.
	 */
	General::Qualifier* getQualifier() const;

	/*! \brief Get the requiresDriver setting
	 *
	 * \return boolean value
	 */
	bool getRequiresDriver() const;

	/*! \brief Set the default value for this port
	 *
	 * Calling this function will set the requiresDriver false and driverType
	 * as any.
	 *
	 * \param defaultValue The new default value.
	 */
	void setDefaultValue(int defaultValue);

	/*! \brief Set the driver type for this port
	 *
	 * Calling this function will set the requiresDriver true and defaultValue
	 * as -1
	 *
	 * \param driverType enum specifying the driver type
	 */
	void setDriverType(General::DriverType driverType);

	/*! \brief set The onMaste WirePort instance
	 *
	 * \param onMaster Pointer to the new onMaster WirePort instance.
	 */
	void setOnMaster(WirePort* onMaster);

	/*! \brief Set the onSlave WirePort instance
	 *
	 * \param onSlave Pointer to the new onSlave WirePort instance.
	 */
	void setOnSlave(WirePort* onSlave);

	/*! \brief Set the qualifier element for this port
	 *
	 * \param qualifier Pointer to the new qualifier element.
	 */
	void setQualifier(General::Qualifier* qualifier);

	/*! \brief Set the requiresDriver option
	 *
	 * Calling this function with parameter true will set the defaultValue
	 * as -1
	 *
	 * \param requiresDriver boolean value.
	 */
	void setRequiresDriver(bool requiresDriver);

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

	/*! \brief Checks if wire has master constraints defined
	 *
	 * \return True if onMaster element is found.
	*/
	bool hasMaster() const;

	/*! \brief Checks if wire has slave constraints defined
	 *
	 * \return True if onSlave element is found.
	*/
	bool hasSlave() const;

	/*! \brief Checks if wire has system constraints defined.
	 *
	 * \return True if onSystem element is found.
	*/
	bool hasSystem() const;

	/*! \brief Is the qualifier set as address for the wire port.
	 *
	 * \return True if the qualifier contains field isAddress.
	*/
	bool isAddress() const;

	/*! \brief Sets the qualifier to address and clears others.
	 *
	*/
	void setAsAddress();

	/*! \brief Is the qualifier set as data for the wire port.
	 *
	 * \return True if the qualifier contains field isData.
	*/
	bool isData() const;

	/*! \brief Sets the qualifier as data and clears others.
	 *
	*/
	void setAsData();

	/*! \brief Is the qualifier set as clock for the wire port.
	 *
	 * \return True if the qualifier contains field isClock.
	*/
	bool isClock() const;

	/*! \brief Sets the qualifier as clock and clears others.
	 *
	*/
	void setAsClock();

	/*! \brief Is the qualifier set as reset for the wire port.
	 *
	 * \return True if the qualifier contains field isReset.
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

	/*! \brief Write the contents of a wirePort using the writer.
	 *
	 * The pointer MUST NOT be null.
	 *
	 * \param writer Reference to the writer to be used.
	 * \param port Pointer to the wirePort to use.
	 */
	void writeWirePort(QXmlStreamWriter& writer, WirePort* port);

	/*!
	 * OPTIONAL spirit:qualifier
	 * Indicates which type of information this wire port carries
	 */
	QSharedPointer<General::Qualifier> qualifier_;

	/*!
	 * OPTIONAL spirit:defaultValue
	 * Contains the default logic value for this wire port.
	 */
	int defaultValue_;

	/*!
	 * OPTIONAL spirit:requiresDriver
	 * Specifies whether the port requires a driver.
	 */
	bool requiresDriver_;

	/*!
	 * OPTIONAL spirit:driverType
	 * Qualifies which driver type is required.
	 */
	General::DriverType driverType_;

	/*!
	 * OPTIONAL spirit:onMaster
	 * Defines constraints for this port when present in a master bus interface
	 */
	QSharedPointer<WirePort> onMaster_;

	/*!
	 * OPTIONAL spirit:onSlave
	 * Defines constraints for this wire port when present in a slave bus
	 * interface.
	 */
	QSharedPointer<WirePort> onSlave_;

	/*! 
	 * OPTIONAL spirit:onSystem
	 * Defines constraints for this wire port when present in system bus
	 * interface.
	 */
	QSharedPointer<WirePort> onSystem_;
};

#endif /* WIREABSTRACTION_H_ */
