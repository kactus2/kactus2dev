/* 
 *
 *  Created on: 27.7.2010
 *      Author: Antti Kamppi
 */

#ifndef GENERALDECLARATIONS_H_
#define GENERALDECLARATIONS_H_

#include <common/Global.h>

#include <LibraryManager/vlnv.h>
#include "parameter.h"
#include "vector.h"
#include <common/Global.h>

#include <QDomNode>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QMap>
#include <QXmlStreamWriter>
#include <QSettings>
#include <QStringList>
#include <QFileInfo>

class Port;

/*!
 * The VLNV tags that name the elements containing information
 */
const QString SPIRIT_VENDOR("spirit:vendor");
const QString SPIRIT_LIBRARY("spirit:library");
const QString SPIRIT_NAME("spirit:name");
const QString SPIRIT_VERSION("spirit:version");

/*! \brief Namespace General is used to store the commonly used structs and enums
 *
 * This module holds the information structures that are used by many modules
 * in IP-Xact parsing that are not otherwise connected to each other.
 */
namespace General {

	//! \brief The file types specified in the IP-Xact
	const QString FILE_TYPES[] = {
		"asmSource",
		"cSource",
		"cppSource",
		"eSource",
		"OVASource",
		"perlSource",
		"pslSource",
		"SVASource",
		"tclSource",
		"veraSource",
		"systemCSource",
		"systemCSource-2.0",
		"systemCSource-2.0.1",
		"systemCSource-2.1",
		"systemCSource-2.2",
		"systemVerilogSource",
		"systemVerilogSource-3.0",
		"systemVerilogSource-3.1",
		"systemVerilogSource-3.1a",
		"verilogSource",
		"verilogSource-95",
		"verilogSource-2001",
		"vhdlSource",
		"vhdlSource-87",
		"vhdlSource-93",
		"swObject",
		"swObjectLibrary",
		"vhdlBinaryLibrary",
		"verilogBinaryLibrary",
		"executableHdl",
		"unelaboratedHdl",
		"SDC",
		"unknown"
	};

	//! \brief The number of supported file type definitions
	const unsigned int FILE_TYPE_COUNT = 33;

    /*! \brief Check if the file type is one of the specified ones.
	 *
	 * \param fileType The file type to check.
	 *
	 * \return True if the file type is one of the specified ones.
	*/
	KACTUS2_API bool isIpXactFileType(const QString& fileType);

	/*! \brief Get the file types which are associated with the given file suffix.
	 *
	 * \param settings The settings instance used to manage the Kactus2 settings.
	 * \param fileSuffix The file suffix to identify the file type.
	 *
	 * \return QStringList containing the file types.
	*/
	KACTUS2_API QStringList getFileTypes(QSettings& settings, const QString& fileSuffix);

	/*! \brief Get the file types which are associated with the given file suffix.
	 *
	 * \param settings The settings instance used to manage the Kactus2 settings.
	 * \param file Represents the file which's file types are searched.
	 *
	 * \return QStringList containing the file types.
	*/
	KACTUS2_API QStringList getFileTypes(QSettings& settings, const QFileInfo& file);

    /*!
     *  Returns true if the given file is IP-XACT.
     *
     *      @param [in] filename The name of the file to check.
     */
    bool isFileIPXact(QString const& filename);

/*!
 * Provides the capability to require or forbid a port to appear in a
 * bus interface.
 */
enum Presence {
	ILLEGAL,
	REQUIRED,
	OPTIONAL
};

/*! \brief Convert the value of the QString into enum Presence
 *
 * if The str does not match any of the enum values the default value
 * specifies as parameter will be returned
 *
 * \param str QString containing the string to be converted
 * \param defaultValue The value that will be returned if no match is found
 * \return General::Presence that matches the str or default value.
 */
General::Presence str2Presence(QString str, General::Presence defaultValue);

/*! \brief Convert the enum presence to QString.
 *
 * \param presence The enum value to be converted
 *
 * \return QString matching the enum value.
 */
QString presence2Str(General::Presence presence);

enum BitSteering {
	ON,
	OFF,
	BITSTEERING_UNSPECIFIED
};

/*! \brief Converts the QString to enum BitSteering
 *
 * If string doesn't match any enum value then BITSTEERING_UNSPECIFIED
 * is returned.
 *
 * \param str The QString that is converted to BitSteering.
 *
 * \return BitSteering value that matches string or BITSTEERING_UNSPECIFIED
 */
General::BitSteering str2BitSteering(const QString& str);

/*! \brief Converts the enum BitSteering into QString
 *
 * \param bitSteering the enum value that is converted to QString
 *
 * \return QString that matches the bitSteering value
 */
QString bitSteering2Str(const General::BitSteering bitSteering);

/*! \brief Convert the value of the QString into enum Presence
 *
 * if The str does not match any of the enum values the default value
 * specifies as parameter will be returned
 *
 * \param str QString containing the string to be converted
 * \param defaultValue The value that will be returned if no match is found
 * \return General::Presence that matches the str or default value.
 */
General::Presence str2Presence(QString str, General::Presence defaultValue);

//! \brief Describes the mode for an interface.
enum InterfaceMode {
	MASTER = 0,
	SLAVE,
	SYSTEM,
	MIRROREDMASTER,
	MIRROREDSLAVE,
	MIRROREDSYSTEM,
	MONITOR,
	INTERFACE_MODE_COUNT
};

//! \brief The names to display that match the enum InterfaceMode
const QString INTERFACE_MODE_NAMES[] = {
	"master",
	"slave",
	"system",
	"mirroredMaster",
	"mirroredSlave",
	"mirroredSystem",
	"monitor",
	"undefined"
};

/*! \brief Convert QString into boolean value
 *
 * \param str QString containing the string to be converted
 * \param defaultValue InterfaceMode value that is returned if str is none
 * of the defined values
 *
 * \return InterfaceMode matching the str or default value
 */
InterfaceMode str2Interfacemode(const QString& str, InterfaceMode defaultValue);

/*! \brief Convert InterfaceMode to QString
 *
 * \param mode InterfaceMode value that is converted to QString
 *
 * \return QString that matches the enum value.
 */
QString interfaceMode2Str(const General::InterfaceMode mode);

/*!
 *  Returns an interface mode that is compatible with the given interface mode.
 */
InterfaceMode getCompatibleInterfaceMode(General::InterfaceMode mode);

/*!
 * Specifies if the selection applies when one of the generator
 * group names matches or all the generator group names match.
 */
enum GroupSelectorOperator {
	OR,
	AND
};

/*! \brief Convert the value of the QString into enum GroupSelectorOperator
 *
 * if The str does not match any of the enum values the default value
 * specifies as parameter will be returned
 *
 * \param str QString containing the string to be converted
 * \param defaultValue The value that will be returned if no match is found
 * \return General::GroupSelectorOperator that matches the str or default value.
 */
General::GroupSelectorOperator str2GroupSelector(QString str,
		General::GroupSelectorOperator defaultValue);

/*!
 * Is the port Wire or Transactional.
 */
enum PortType {
	WIRE,
	TRANSACTIONAL
};

/*!
 * Endianness is used to indicate the endianness of a bus interface.
 */
enum Endianness {
	BIG,
	LITTLE
};

/*! \brief Convert the value of the QString into enum Endianness
 *
 * If the str does not contain strings "big" or "little" then the
 * default value specifies in the parameter will be returned
 *
 * \param str QString containing the string to be converted
 * \param defaultValue The default value that will be returned if the
 * conversion was not successful
 * \return General:Endianness The value found in the string or defaultValue
 */
General::Endianness str2Endianness(QString str,
		General::Endianness defaultValue);

/*! \brief Convert the Endianness to QString
 *
 * \param endianness Endianness value that is converted to QString
 *
 * \return QString that matches the enum value.
 */
QString endianness2Str(const General::Endianness endianness);

/*! \brief Specifies the time unit of a time value
 *
 */
enum TimeUnit {
	NS, //! nanosecond
	PS  //! picosecond
};

/*! \brief Convert the value of the QString into enum TimeUnit
 *
 * if The str does not match any of the enum values the default value
 * specifies as parameter will be returned
 *
 * \param str QString containing the string to be converted
 * \param defaultValue The value that will be returned if no match is found
 * \return General::TimeUnit that matches the str or default value.
 */
General::TimeUnit str2TimeUnit(QString str, General::TimeUnit defaultValue);

/*! \brief Convert the value of the time unit to QString
 *
 * \param timeUnit A reference to a General::TimeUnit enum.
 *
 * \return QString, "ns" if timeUnit is 'NS'. "ps" if timeUnit is 'PS'.
 */
QString timeUnit2Str(General::TimeUnit& timeUnit);

/*! \brief  Defines the type of access.
 *
 */
enum Initiative {
	REQUIRES,
	PROVIDES,
	BOTH,
	INITIATIVE_PHANTOM,
	NONE
};

/*! \brief Convert the value of the QString into enum Initiative
 *
 * If the str does not match any of the enum values then the default value
 * specified as parameter will be returned
 *
 * \param str QString containing the string to be converted
 * \param defaultValue The value that will be returned if no match is found
 * \return General::Initiative that matches the str or defaultValue.
 */
General::Initiative str2Initiative(QString str,
		General::Initiative defaultValue);

/*! \brief Convert the Initiative to QString
 *
 * \param initiative The Initiative value to be converted to QString.
 *
 * \return QString that matches the enum value.
 */
QString Initiative2Str(const General::Initiative initiative);

/*!
 * Specifies the required driver type
 */
enum DriverType {
	ANY, 		// any logic signal or value
	CLOCK, 		// repeating type waveform
	SINGLESHOT	// non-repeating type waveform
};

/*! \brief Convert the value of the QString into enum DriverType
 *
 * if The str does not match any of the enum values the default value
 * specifies as parameter will be returned
 *
 * \param str QString containing the string to be converted
 * \param defaultValue The value that will be returned if no match is found
 * \return General::DriverType that matches the str or default value.
 */
General::DriverType str2DriverType(QString str,
                General::DriverType defaultValue);

/*! \brief Convert the value of the DriverType into QString.
 *
 * \param type The DriverType to be converted.
 *
 * \return QString matching the driver type.
 */
QString driverType2Str(General::DriverType type);

/*!
 * Specifies the direction of the port
 */
enum Direction {
	IN,
	OUT,
	INOUT,
	DIRECTION_PHANTOM,
	DIRECTION_INVALID
};

/*! \brief Convert the given direction to mirrored.
 *
 * \param originalDirection The direction to convert.
 *
 * \return General::Direction The mirrored version of the direction.
*/
General::Direction convert2Mirrored(const General::Direction originalDirection);

/*! \brief Convert the value of the QString into enum Direction
 *
 * if The str does not match any of the enum values the default value
 * specifies as parameter will be returned
 *
 * \param str QString containing the string to be converted
 * \param defaultValue The value that will be returned if no match is found
 * \return General::Direction that matches the str or default value.
 */
KACTUS2_API General::Direction str2Direction(QString str, General::Direction defaultValue);

/*! \brief Convert the Direction value to QString.
 *
 * \param direction Direction value to be converted to QString.
 *
 * \return QString that matches the enum value.
 */
KACTUS2_API QString direction2Str(const General::Direction direction);

/*! \brief Equals to spirit:usage element within spirit:memoryBlockData
 *
 * Usage specifies the type of usage for the address block to which it
 * belongs
 */
enum Usage {
	MEMORY = 0,
	REGISTER,
	RESERVED,
	USAGE_COUNT
};

/*! \brief Convert the value of the QString into enum Usage
 *
 * if The str does not match any of the enum values the default value
 * specifies as parameter will be returned
 *
 * \param str QString containing the string to be converted
 * \param defaultValue The value that will be returned if no match is found
 * \return General::Usage that matches the str or default value.
 */
General::Usage str2Usage(QString str, General::Usage defaultValue);

/*! \brief Convert the Usage value to QString
 *
 * \param usage Usage value to be converted to QString.
 *
 * \return QString that matches the enum value.
 */
QString usage2Str(const General::Usage usage);

/*! \brief Equals to spirit:access element within spirit:memoryBlockData
 *
 * Access specifies the accessibility of the data in the address block.
 */
enum Access {
	READ_WRITE = 0,
	READ_ONLY,
	WRITE_ONLY,
	READ_WRITEONCE,
	WRITEONCE,
	ACCESS_COUNT
};

/*! \brief Convert the value of the QString into enum Access
 *
 * if The str does not match any of the enum values the default value
 * specifies as parameter will be returned
 *
 * \param str QString containing the string to be converted
 * \param defaultValue The value that will be returned if no match is found
 * \return General::Access that matches the str or default value.
 */
General::Access str2Access(QString str, General::Access defaultValue);

/*! \brief Convert the Access value to QString.
 *
 * \param access The Access value to be converted into QString.
 *
 * \return QString that matches the enum value.
 */
QString access2Str(const General::Access access);

/*! \brief Equals to spirit:modifiedWriteValue within spirit:fieldData.
 * 
 * ModifiedWrite describes the manipulation of data written to a field.
 */
enum ModifiedWrite {
	ONE_TO_CLEAR = 0,
	ONE_TO_SET,
	ONE_TO_TOGGLE,
	ZERO_TO_CLEAR,
	ZERO_TO_SET,
	ZERO_TO_TOGGLE,
	CLEAR, 
	SET,
	MODIFY,
	MODIFIED_WRITE_COUNT
};

//! \brief Contains the strings that match the enum ModifiedWrite values.
const QString MODIFIED_WRITE_STRINGS[] = {
	"oneToClear",
	"oneToSet",
	"oneToToggle",
	"zeroToClear",
	"zeroToSet",
	"zeroToToggle",
	"clear",
	"set",
	"modify",
	""
};

/*! \brief Convert the value of the QString into enum ModifiedWrite.
 *
 * If the string does not match any of the enum values then General::MODIFIED_WRITE_COUNT
 * is returned.
 * 
 * \param str contains the string to be converted.
 *
 * \return The enum value that matches the string.
*/
General::ModifiedWrite str2ModifiedWrite(const QString& str);

/*! \brief Convert the enum value to a matching string.
 *
 * \param modWrite The enum value to be converted to a string.
 *
 * \return QString that matches the enum value.
*/
QString modifiedWrite2Str(const General::ModifiedWrite modWrite);

/*! \brief Equals the spirit:readAction within spirit:fieldData.
 * 
 * ReadAction describes an action that happens to a field after a read operation.
 */
enum ReadAction {
	READ_CLEAR = 0,
	READ_SET,
	READ_MODIFY,
	READ_ACTION_COUNT
};

//! \brief Contains the strings that match the enum ReadAction values.
const QString READ_ACTION_STRINGS[] = {
	"clear",
	"set",
	"modify",
	""
};

/*! \brief Convert the enum value to a matching string.
 *
 * \param readAction The enum value to be converted to a string.
 *
 * \return QString that matches the enum value.
*/
QString readAction2Str(const General::ReadAction readAction);

/*! \brief Convert the string into matching enum ReadAction value.
 * 
 * If the string does not match any of the defined enum values then
 * General::READ_ACTION_COUNT is returned.
 * 
 * \param str Contains the string to be converted.
 *
 * \return The enum value that matches the string.
*/
General::ReadAction str2ReadAction(const QString& str);

/*! \brief Equals the spirit:testConstraint attribute within spirit:fieldData.
 * 
 * TestConstraint defines the constraint for the field during automated testing.
 */
enum TestConstraint {
	TEST_UNCONSTRAINED = 0,
	TEST_RESTORE,
	TEST_WRITE_AS_READ,
	TEST_READ_ONLY,
	TESTCONSTRAINT_COUNT
};

//! \brief Contains the strings that match the enum TestConstraint values.
const QString TEST_CONSTRAINT_STRINGS[] = {
	"unConstrained",
	"restore",
	"writeAsRead",
	"readOnly",
	""
};

/*! \brief Convert the enum value to a matching string.
 *
 * \param testConstraint The enum value to be converted to a string.
 *
 * \return QString that matches the enum value.
*/
QString testConstraint2Str(const General::TestConstraint testConstraint);

/*! \brief Convert the string into matching enum TestConstraint value.
 * 
 * If the string does not match any of the defined enum values then
 * General::TESTCONSTRAINT_COUNT is returned.
 *
 * \param str Contains the string to be converted.
 *
 * \return The enum value that matches the string.
*/
General::TestConstraint str2TestConstraint(const QString& str);

/*! \brief Used by the struct Choice to hold information
 *
 */
struct Enumeration {
	/*!
	 * MANDATORY spirit:enumeration
	 * Value of the enumeration
	 */
	QString value_;

	/*!
	 * OPTIONAL spirit:text
	 * The text-attribute
	 */
	QString attribute_text_;

	/*!
	 * OPTIONAL spirit:help
	 * The help-attribute
	 */
	QString attribute_help_;

	/*! \brief The constructor
	 *
	 * \param value The value of the enumeration
	 * \param attText The text-attribute
	 * \param attHelp The help-attribute
	 */
	Enumeration(QString &value, QString attText = QString(),
			QString attHelp = QString());

	/*! \brief Copy constructor
	 *
	 * \param other Reference to the Enumeration to copy
	 *
	*/
	Enumeration(const Enumeration& other);

	/*! \brief The assignment operator
	 *
	 * \param other Reference to the enumeration to assign.
	 *
	 * \return Reference to this enumeration.
	*/
	Enumeration& operator=(const Enumeration& other);
};

/*! \brief Used in spirit:clockDriver element in IP-Xact specification.
 *
 * Used to describe elements:
 * spirit:clockPeriod
 * spirit:clockPulseOffset
 * spirit:clockPulseDuration
 */
struct ClockStruct {

	/*!
	 * MANDATORY
	 * Specifies the value of the element defined in this struct
	 */
	double value_;

	/*!
	 * OPTIONAL
	 * default = NS
	 */
	General::TimeUnit timeUnit_;

	/*!
	 * MANDATORY
	 * Contains the attributes set for the containing element
	 */
	QMap<QString, QString> attributes_;

	/*! \brief The constructor
	 *
	 * \param clockNode A reference to a QDomNode to parse the information
	 * from.
	 */
	ClockStruct(QDomNode &clockNode);

	/*! \brief The constructor
	 *
	 * \param value The value to set for the struct
	 *
	*/
	ClockStruct(double value);

	/*! \brief The copy constructor
	 *
	 * \param other Reference to the ClockStruct to copy
	 *
	*/
	ClockStruct(const ClockStruct& other);

	/*! \brief The assignment operator
	 *
	 * \param other Reference to the ClockStruct to assign.
	 *
	 * \return Reference to this clock struct.
	*/
	ClockStruct& operator=(const ClockStruct& other);
};

/*! \brief Equals the spirit:clockPulseValue element in IP-Xact specification
 *
 * Specifies the logic value to which the port transitions. The value of the
 * port after the first clock edge.
 */
struct ClockPulseValue {

	/*!
	 * MANDATORY
	 * Specifies the value of the spirit:clockPulseValue element
	 * The value shall be 0 or 1.
	 */
	unsigned int value_;

	/*!
	 * MANDATORY
	 * Contains the attributes set for the containing element.
	 */
	QMap<QString, QString> attributes_;

	/*! \brief The constructor
	 *
	 * \param clockNode A reference to a QDomNode to parse the information
	 * from.
	 */
	ClockPulseValue(QDomNode &clockNode);

	/*! \brief The constructor
	 *
	 * \param value The value for the clock pulse
	 *
	*/
	ClockPulseValue(unsigned int value);

	/*! \brief The copy constructor
	 *
	 * \param other Reference to the ClockPulseValue to copy
	 *
	*/
	ClockPulseValue(const ClockPulseValue& other);

	/*! \brief The assignment operator.
	 *
	 * \param other Reference to the ClockPulseValue to assign.
	 *
	 * \return Reference to this ClockPulseValue.
	*/
	ClockPulseValue& operator=(const ClockPulseValue& other);
};

//! \brief Specifies a port name and it's bounds.
struct PortBounds {

	//! \brief The name of the port.
	QString portName_;

	//! \brief The left bound of the port.
	int left_;

	//! \brief The right bound of the port.
	int right_;

	//! \brief Default constructor
	PortBounds();

	/*! \brief The constructor
	* 
	* Constructs a port with given name and both bounds set to 0.
	* 
	* \param portName The name of the port.
	*
	*/
	PortBounds(const QString& portName);

	/*! \brief The constructor
	*
	* \param portName The name of the port.
	* \param left The left bound of the port.
	* \param right The right bound of the port.
	*
	*/
	PortBounds(const QString& portName, const int left, const int right);

	//! \brief Copy constructor
	PortBounds(const PortBounds& other);

	//! \brief Assignment operator
	PortBounds& operator=(const PortBounds& other);

	//! \brief Operator <
	bool operator<(const PortBounds& other) const;

	//! \brief Operator ==
	bool operator==(const PortBounds& other) const;

	//! \brief Operator !=
	bool operator!=(const PortBounds& other) const;
};

/*! \brief Describes the mapping between the logical and physical ports
 *
 * This struct is used to store the spirit:portMap element info. It can be used
 * to map logical ports, defined in abstraction definition to physical ports,
 * defined in component description.
 *
 */
struct PortMap {
	//! MANDATORY Name of the logical port
	QString logicalPort_;

	/*! \brief The optional vector element for the logical Port
	 *
	 * If this element exists then the port is vectored and it's type is
	 * std_logic_vector. This element specifies the left and right bound of the
	 * vectored port.
	 */
	QSharedPointer<Vector> logicalVector_;

	//!MANDATORY Name of the physical port
	QString physicalPort_;

	/*! \brief The optional vector element for the physical port.
	 *
	 * If this element exists then the port is vectored and it's type is
	 * std_logic_vector. This element specifies the left and right bound of the
	 * vectored port.
	 */
	QSharedPointer<Vector> physicalVector_;

	/*! \brief The constructor
	 *
	 * \param portMap a reference to the QDomNode that can used to parse the
	 * information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs if a mandatory element is missing in
	 * this portMap or one of it's members.
	 */
	PortMap(QDomNode &portMap);

	/*! \brief Default constructor
	 *
	 */
	PortMap();

	//! \brief The copy constructor
	PortMap(const PortMap& other);

	/*! \brief The assignment operator
	 *
	 * \param other Reference to the PortMap to assign.
	 *
	 * \return Reference to this PortMap
	*/
	PortMap& operator=(const PortMap& other);

	/*! \brief Check if the port map is in a valid state.
	 *
	 * \param physicalPorts List of the physical ports of the component and their bounds.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QList<General::PortBounds>& physicalPorts, 
		QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the port map is in a valid state.
	 * 
	 * \param physicalPorts List of the physical ports of the component and their bounds.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QList<General::PortBounds>& physicalPorts) const;

    /*!
     *  Gets the physical range in the port map.
     *
     *      @param [in] referencedPhysicalPort   The physical port referenced in the port map 
     *                                           in the component containing the port map.
     *
     *      @return The physical range in the port map.
     */
    General::PortBounds getPhysicalRange(QSharedPointer<Port> referencedPhysicalPort) const;

    /*!
     *  Gets the logical range in the port map.
     *
     *      @param [in] referencedPhysicalPort   The physical port referenced in the port map 
     *                                           in the component containing the port map.
     *
     *      @return The logical range in the port map.
     */
    General::PortBounds getLogicalRange(QSharedPointer<Port> referencedPhysicalPort) const;
};

/*! \brief Convert the physical port into string.
 * 
 * \param portMap reference to the port map that's physical port is converted to string.
 *
 * \return QString Contains the physical port in string.
*/
QString toPhysString(const PortMap& portMap);

/*! \brief Convert the logical port into string.
 *
 * \param portMap reference to the port map that's logical port is converted to string.
 *
 * \return QString Contains the logical port in string.
*/
QString toLogicalString(const PortMap& portMap);

/*! \brief Convert the info into string.
 *
 * \param portName The name of the port.
 * \param leftBound The left bound of the port.
 * \param rightBound The right bound of the port.
 *
 * \return QString String that matches the info ( <portName>[<left>..<right>] )
*/
QString port2String(const QString& portName, int leftBound, int rightBound);

//! \brief Port alignment is used to contain the physical bounds of two ports.
struct PortAlignment {
	
	//! \brief The calculated left bound for port 1.
	int port1Left_;

	//! \brief The calculated right bound for port 1.
	int port1Right_;

	//! \brief The calculated left bound for port 2.
	int port2Left_;

	//! \brief The calculated right bound for port 2.
	int port2Right_;

	//! \brief Tells if the calculated alignment is valid or not.
	bool invalidAlignment_;

	/*! \brief The default constructor
	 *
	 * Constructs the struct with all bounds valued at -1 and invalid as true.
	*/
	PortAlignment();

	//! \brief Copy constructor
	PortAlignment(const PortAlignment& other);

	//! \brief Assignment operator
	PortAlignment& operator=(const PortAlignment& other);
};

/*! \brief Calculate the alignment for the ports from the port maps.
 *
 * \param portMap1 Pointer to the port map of port 1.
 * \param phys1LeftBound The left bound for the physical port 1.
 * \param phys1RightBound The right bound for the physical port1.
 * \param portMap2 Pointer to the port map of port 2.
 * \param phys2LeftBound The left bound for the physical port 2.
 * \param phys2RightBound The right bound for the physical port 2.
 *
 * \return General::PortAlignment Contains the alignments for the ports.
*/
PortAlignment calculatePortAlignment(const PortMap* portMap1, 
									 int phys1LeftBound,
									 int phys1RightBound,
									 const PortMap* portMap2,
									 int phys2LeftBound,
									 int phys2RightBound);

/*! \brief The  NameGroup matches the spirit:nameGroup element in IP-Xact.
 * 
 */
struct NameGroup {

	//! \brief The name matches the spirit:name field within nameGroup.
	QString name_;

	//! \brief The displayName matches the spirit:displayName field within nameGroup.
	QString displayName_;

	//! \brief description matches the spirit:description field within nameGroup.
	QString description_;

	/*! \brief The default constructor
	*
	* Constructs NameGroup with all elements null.
	*/
	NameGroup();

	/*! \brief The constructor
	*
	* Constructs a nameGroup and parses the info from the nameNode
	* 
	* \param nameNode Reference to the QDomNode to parse the information from.
	*/
	NameGroup(QDomNode& node);

	/*! \brief The constructor
	 *
	 * \param name The name element
	 *
	*/
	NameGroup(const QString name);

	/*! \brief The copy constructor
	 *
	 * \param other Reference to the NameGroup to copy.
	 *
	*/
	NameGroup(const NameGroup& other);

	/*! \brief The assignment operator
	 *
	 * \param other Reference to the NameGroup to assign.
	 *
	 * \return Reference to this NameGroup.
	*/
	NameGroup& operator=(const NameGroup& other);
};

/*! \brief Convert a boolean value into QString
 *
 * \param value The varable to convert into QString
 * \return if value = true, returns "true"
 * 			if value = false, returns "false"
 */
QString bool2Str(bool value);

/*! \brief Convert QString into boolean value
 *
 * \param str QString containing the string to be converted
 * \param defaultValue boolean value that is returned if str is neither
 * "true" or "false"
 *
 * \return if str = "true" returns true
 * 			if str = "false" returns false
 * 			if str is something else returns defaultValue
 */
bool str2Bool(const QString str, bool defaultValue);

/*! \brief BooleanValue is used to store bool types that can also be unspecifed
 *
 * In some IP-Xact elements there are boolean-type values that may be left
 * unspecified. This enum is used to define undefined boolean values.
 */
enum BooleanValue {
	BOOL_TRUE,
	BOOL_FALSE,
	BOOL_UNSPECIFIED
};

/*! \brief Convert BooleanValue to bool.
 *
 * \param value BooleanValule to be converted to bool
 * \param defaultValue The value that is used if BooleanValue was unspecified.
 *
 * \return bool that matched the BooleanValue.
 */
bool BooleanValue2Bool(const General::BooleanValue value,
		const bool defaultValue);

/*! \brief Convert bool to BooleanValue
 *
 * \param value bool value to be converted to BooleanValue.
 *
 * \return BooleanValue that matches the bool.
 */
General::BooleanValue bool2BooleanValue(const bool value);

/*! \brief Convert QString to booleanValue.
 *
 * \param str QString that is converted to BooleanValue.
 *
 * \return BooleanValue that matches the QString. If no match is found then
 * BooleanValue is BOOL_UNSPECIFIED.
 */
General::BooleanValue str2BooleanValue(const QString& str);

/*! \brief Convert BooleanValue to QString
 *
 * \param value BooleanValue to be converted into QString.
 *
 * \return QString that matches the value. If value was BOOL_UNSPECIFIED then
 * empty QString is returned.
 */
QString booleanValue2Str(const General::BooleanValue value);

/*! \brief Parse attributes to QMap
 *
 * Parses the attributes from the root of the given node to the QMap given
 * as a parameter.
 *
 * \param node A reference to a QDomNode to parse the attributes from
 * \param map A reference to a QMap to store the parsen attributes to.
 */
void parseAttributes(const QDomNode &node, QMap<QString, QString> &map);

/*! \brief Write the attributes from the QMap to the writer
 *
 * Reads the attributes from the QMap and uses writer to write them. The
 * element to write the attributes to must be opened by the writer before
 * calling this function.
 *
 * \param writer A reference to a QXmlStreamWriter instance that has been
 * set up to write a document.
 * \param attributes A reference to a QMap that contains the attributes to
 * be written.
 */
void writeAttributes(QXmlStreamWriter& writer, const QMap<QString, QString>&
		attributes);

/*! \brief Parse a vlnv tag from the attributes in the node
 *
 * The node parameter must have attributes matching a vlnv tag
 *
 * \param node A reference to a QDomNode to parse the vlnv from.
 * \return A pointer to the new vlnv instance.
 */
VLNV createVLNV(const QDomNode& node, VLNV::IPXactType type);

/*! \brief removes white spaces from the given QString
 *
 *	This function removes white spaces from the beginning and end of the string
 *	and also replaces any embedded whitespaces with characted '_'. If you only
 *	want to remove the white spaces from the start and end use
 *	QString::trimmed() instead of this.
 *
 *	IP-Xact types used for this function:
 *	-Name
 *	-NMTOKEN
 *	-PortName
 *	-ID
 *	-IDREF
 *
 * \param str A reference to the QString where white spaces are wanted to
 * be removed from.
 *
 * \return QString containing the string that has been stripped from
 * whitespaces and embedded whitespaces have been replaces with '_'
 */
QString removeWhiteSpace(QString str);

/*! \brief Equals the spirit:qualifier element in IP-Xact specification
 *
 * Qualifier indicates which type of information a port carries.
 */
struct Qualifier {
	/*!
	 * OPTIONAL spirit:isAddress
	 * When true specifies the port contains address information.
	 */
	bool isAddress_;

	/*!
	 * OPTIONAL spirit:isData
	 * When true specifies the port contains data information
	 */
	bool isData_;

	/*!
	 * OPTIONAL spirit:isClock
	 * When true specifies the port is a clock for this bus interface
	 */
	bool isClock_;

	/*!
	 * OPTIONAL spirit:isReset
	 * When true specifies this port is a reset for this bus interface
	 */
	bool isReset_;

	/*! \brief The constructor
	 *
	 * \param qualifierNode A reference to a QDomNode to parse the information
	 * from.
	 */
	Qualifier(QDomNode& qualifierNode);

	/*! \brief The default constructor
	 *
	*/
	Qualifier();

	/*! \brief The copy constructor
	 *
	 * \param other Reference to the Qualifier to copy.
	 *
	*/
	Qualifier(const Qualifier& other);

	/*! \brief The assignment operator
	 *
	 * \param other Reference to the Qualifier to assign.
	 *
	 * \return Reference to this Qualifier.
	*/
	Qualifier& operator=(const Qualifier& other);
};

/*! \brief Contains the file path for a file and the library for the file.
 *
 * This is used to get a file name from fileSet and the logical name for it.
 */
struct LibraryFilePair {

	//! \brief Contains the file path and file name.
	QString filePath_;

	//! \brief Contains the library name for the file.
	QString libraryName_;

	/*! \brief The constructor for the struct
	 *
	 * \param filePath the path of the file
	 * \param libraryName Name of the library
	 */
	LibraryFilePair(const QString filePath, const QString libraryName);

	/*! \brief The == operator
	 *
	 * \param other The other LibaryFilePair to check
	 *
	 * \return bool True if they have the same file path and library name.
	*/
	bool operator==(const LibraryFilePair& other) const;

	/*! \brief The copy constructor
	 *
	 * \param other Reference to the LibraryFilePair to copy
	 *
	*/
	LibraryFilePair(const LibraryFilePair& other);

	/*! \brief The assignment operator
	 *
	 * \param other Reference to the LibraryFilePair to assign
	 *
	 * \return Reference to this LibraryFilePair.
	*/
	LibraryFilePair& operator=(const LibraryFilePair& other);
};

/*! \brief Write the attributes using the writer.
 *
 * \param writer A reference to a QXmlStreamwriter instance that is used to
 * write the attributes.
 * \param vlnv A pointer to the VLNV instance that is written as attributes.
 */
void writeVLNVAttributes(QXmlStreamWriter& writer, const VLNV* vlnv);

/*! \brief Get a relative file path from one location to another.
 *
 * This function can be used to create a QString that contains the relative
 * file path from a directory to a single file. If either of the parameters
 * doesn't exist in the file system a null string is returned.
 *
 * \param from Path to the directory that is the base where the path is
 * generated to.
 * \param to The path to a file which is the target of the generated relative
 * path.
 *
 * \return QString containing the relative path.
 */
KACTUS2_API QString getRelativePath(const QString from, const QString to);

/*! \brief Get a relative file path from one location to another.
 *
 * This function can be used to create a QString that contains the relative
 * file path from a directory to a single file. This function can be used to get
 * relative path to a file that does not exist. The source file must exist.
 *
 * \param from Path to the directory that is the base where the path is
 * generated to.
 * \param to The path to a file which is the target of the generated relative
 * path.
 *
 * \return QString containing the relative path.
 */
KACTUS2_API QString getRelativeSavePath(const QString& from, const QString& to);

/*! \brief Get an absolute path of a file.
 *
 * This function takes original path and uses it as base. Then the relative
 * path is used from the original path to find the correct location of a file.
 * The absolute path to the file is returned. If the relative path contains a
 * file but it is not found null string is returned. Original path may contain
 * a file path or only a directory path but only directory path is used.
 *
 * \param originalPath The absolute path that is used as base for relative path
 * \param relativePath The relative path that is used to find the file or
 * directory.
 *
 * \return The absolute path of the target directory/file
 */
KACTUS2_API QString getAbsolutePath(const QString& originalPath, const QString& relativePath);

}

#endif /* GENERALDECLARATIONS_H_ */
