/* 
 *
 *  Created on: 3.11.2010
 *      Author: Antti Kamppi
 */

#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <models/generaldeclarations.h>

//#include "componenttype.h"

#include <QObject>
#include <QString>
#include <QTextStream>

class Vector;

/*! \brief Defines a connection.
 *
 * This class is used to define which bit of a vectored port are connected
 * and what is the type of the connection.
 */
class Connection: public QObject {

	Q_OBJECT

public:

	//! \brief Specifies a connection endpoint for a signal.
	struct EndPoint {
	
		//! \brief The name of the component instance.
		QString instanceName_;

		//! \brief The name of the port on the component instance.
		QString portName_;

		//! \brief Left bound for signal
		int signalLeft_;

		//! \brief Right bound for signal
		int signalRight_;

		//! \brief Left bound for port
		int portLeft_;

		//! \brief Right bound for port
		int portRight_;

		/*! \brief The default constructor.
		 *
		*/
		EndPoint();

		/*! \brief The constructor.
		 *
		 * \param instance Name of the component instance.
		 * \param port Name of the port on the component instance.
		 *
		*/
		EndPoint(const QString& instance, const QString& port);

		/*! \brief Copy constructor
		 *
		 * \param other Reference to the EndPoint to copy.
		 *
		*/
		EndPoint(const EndPoint& other);

		/*! \brief The assignment operator
		 *
		 * \param other Reference to the other EndPoint instance.
		 *
		*/
		EndPoint& operator=(const EndPoint& other);

		/*! \brief The == operator
		 * 
		 * If EndPoints have same instance name and port name they are considered
		 * equal.
		 *
		 * \param other Reference to the EndPoint to compare.
		 *
		 * \return bool True if EndPoints are equal.
		*/
		bool operator==(const EndPoint& other) const;

		/*! \brief The < comparison operator
		 * 
		 * This operator uses the QString's < operator.
		 *
		 * \param other Reference to the EndPoint to compare.
		 *
		 * \return bool 
		*/
		bool operator<(const EndPoint& other) const;

		/*! \brief The > comparison operator
		 *
		 * This operator uses the QString's > operator.
		 *
		 * \param other Reference to the EndPoint to compare.
		 *
		 * \return bool
		*/
		bool operator>(const EndPoint& other) const;
	};

	//! \brief Specifies the type of a signal or port.
	enum PortType {
		STD_LOGIC,
		STD_LOGIC_VECTOR
	};

	/*! \brief A constructor
	 *
	 * \param name The name of the signal.
	 * \param parent Pointer to the owner of this instance.
	 */
	Connection(const QString name = QString(), QObject* parent = 0);

	/*! \brief A constructor
	 *
	 * This constructor is used when defining a port declarations where the
	 * direction of the connection is needed.
	 */
	/*! \brief A constructor
	 *
	 * \param name The name for the port.
	 * \param direction The direction for the port.
	 * \param left Left bound for the port.
	 * \param right Right bound for the port.
	 * \param description Description for the port.
	 * \param parent Pointer to the owner of the port.
	 *
	*/
	Connection(const QString name, 
		General::Direction direction,
			int left, 
			int right, 
			const QString& description = QString(),
			QObject* parent = 0);

	/*! \brief A constructor
	 *
	 * \param name QString containing the name of the connection
	 * \param vector A pointer to the vector that is used.
	 */
	Connection(const QString& name, Vector* vector, QObject* parent = 0);

	//! \brief The destructor
	virtual ~Connection();

	/*! \brief Set the name for the signal.
	 *
	 * \param name The new name for the signal.
	 */
	//void setName(const QString name);

	/*! \brief Get the name of the Connection.
	 *
	 * \return QString containing the name
	 */
	QString getName() const;

	/*! \brief Get the direction of the connection.
	 *
	 * This function can be used when writing port declarations to specify
	 * port direction.
	 *
	 * \return Enum Direction
	 */
	General::Direction getDirection() const;

	/*! \brief Set the direction of a port.
	 *
	 * \param direction The direction of a port.
	 */
	void setDirection(const General::Direction direction);

	/*! \brief Set type for the signal.
	 *
	 * Supported types:
	 * - STD_LOGIC
	 * - STD_LOGIC_VECTOR
	 *
	 * \param type The new type.
	 */
	void setType(const PortType type);

	/*! \brief Set the bounds of the signal.
	 *
	 * Calling this function will also set the type of the signal to
	 * STD_LOGIC_VECTOR if the left != right. If left == right the signal type
	 * is set to STD_LOGIC.
	 *
	 * \param left The left bound.
	 * \param right The right bound.
	 */
	void setBounds(const int left, const int right);

	/*! \brief Used to sort the signals in a QMap.
	 *
	 * For signal to be used as key in QMap this operator must be implemented.
	 * This implementation only compares signals' names with QString <()
	 * operator.
	 *
	 * \param other A reference to another Signal to be compared.
	 */
	bool operator<(const Connection &other) const;

	/*! \brief The copy constructor
	 *
	 * \param other A reference to a Connection to copy.
	 */
	Connection(const Connection& other);

	/*! \brief The assignment operator
	 *
	 * \param other A reference to another Connection to assign the values from
	 */
	Connection& operator=(const Connection& other);

	/*! \brief Writes the connection mapping.
	 *
	 * Used when writing port maps. Can be also used to write signal mappings.
	 *
	 * This function can be used when writing the port map.
	 * The output is <name_>(<left_> downto <right_>) if type is std_logic the
	 * bounds are not written.
	 *
	 * \param stream A reference to QTextStream to write into.
	 */
	void writePortMap(QTextStream& stream);

	/*! \brief Writes the port declaration
	 *
	 * Used when writing port declaration.
	 *
	 * This function can be used when writing a port declaration. Then the
	 * direction of the port must be defined. The output is i.e.
	 * <name_> : <direction_> <type>
	 *
	 * \param stream A reference to QTextStream to write into.
	 *
	 * Exception guarantee: basic
	 * \exception Vhdl_error Occurs if port type is invalid.
	 */
	void writePortDeclaration(QTextStream& stream);

	/*! \brief Writes the signal information into the stream.
	 *
	 * Used when writing a signal declaration.
	 *
	 * Before calling this function the stream must be set up properly.
	 * This function can be used when writing a signal declaration. The output
	 * is i.e. <name_> : <type_>(<left_> downto <right_>);
	 *
	 * \param stream A reference to a text stream to be written in.
	 *
	 * Exception guarantee: basic
	 * \exception Vhdl_error Occurs if signal type is invalid.
	 */
	void writeSignalVhdl(QTextStream& stream);

	/*! \brief Write the type of the connection into stream
	 *
	 * \param stream A reference to QTextStream to write into.
	 *
	 * Exception guarantee: basic
	 * \exception Vhdl_error Occurs if a signal type is invalid.
	 */
	void writeType(QTextStream& stream);

	/*! \brief Get the left bound of the signal or port.
	 *
	 * \return The left bound.
	 */
	unsigned int getLeft() const;

	/*! \brief Get the right bound of the signal or port.
	 *
	 * \return The right bound.
	 */
	unsigned int getRight() const;

	/*! \brief Get the width of the connection.
	 *
	 * \return The number of bits in this connection.
	 */
	unsigned int getSize() const;

	/*! \brief Set the default value for a signal or port.
	 *
	 * This function can be used for a signal or port declaration to assign
	 * a default value for the port. The Default value is set as QString so
	 * if a type-conversion function is needed it can be set inside the string.
	 * i.e. "to_stdlogicvector(x'D1')"
	 *
	 * \param value The default value.
	 */
	void setDefaultValue(const QString value);

	/*! \brief Get the default value of a port or signal.
	 *
	 * \return QString Containing the default value and the possibly required
	 * conversion functions.
	 */
	QString getDefaultValue() const;

	/*! \brief Get the description set for the connection.
	 *
	 *
	 * \return QString contains the description.
	*/
	QString getDescription() const;

	/*! \brief Convert a QString to int-format.
	 *
	 * This function can be used to convert a QString containing a number to
	 * integer type. This function supports following digit-formats:
	 * - hexadecimal
	 * - decimal
	 * - binary
	 * - octal
	 *
	 * \param ok Boolean value that is used to tell the caller if the conversion
	 * was succesful or not.
	 * \param value The QString that is converted to integer.
	 *
	 * \return The value defined in QString converted to integer. If conversion
	 * was unsuccesful 0 is returned.
	 */
	static int str2Int(bool& ok, const QString& value);

signals:

	//! \brief Send error message to user.
	void errorMessage(const QString& other);

	//! \brief Send notification to user.
	void noticeMessage(const QString& other);

private:

	/*! \brief Write the default value of the connections into stream
	 *
	 * This function can be used to write a default value for a signal or port
	 * declaration.
	 *
	 * \param stream The text stream to write into.
	 */
	void writeDefault(QTextStream& stream);

	//! \brief Name of the connection.
	QString name_;

	/*! \brief Contains the direction of the connection.
	 *
	 * Used for port declarations in component types.
	 */
	General::Direction direction_;

	//! \brief Type of the signal. 
	PortType type_;

	//! \brief Left bound of the signal if signal is not scalar.
	unsigned int left_;

	//! \brief Right bound of the signal if signal is not scalar.
	unsigned int right_;

	//! \brief The default value of a port or signal.
	QString defaultValue_;

	//! \brief The description for the connection
	QString description_;
};
#endif /* SIGNAL_H_ */
