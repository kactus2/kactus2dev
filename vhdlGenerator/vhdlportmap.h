/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlportmap.h
 *		Project: Kactus 2
 */

#ifndef VHDLPORTMAP_H
#define VHDLPORTMAP_H

#include <QString>
#include <QTextStream>

class VhdlComponentInstance;

/*! \brief VhdlPortMap represents a port and it's optional bounds in vhdl.
 * 
 * This class is used in VhdlComponentInstance to store a mapped port and the 
 * boundaries of the mapping.
 */
class VhdlPortMap  {

public:

	/*! \brief The default constructor.
	 *
	*/
	VhdlPortMap();

	/*! \brief The constructor
	 *
	 * \param name The name of the port
	 * \param size The size of the port
	 * 
	 * If size is 1 or 0 then bounds are set to -1 and they are not used.
	 * If size is greater than 1 then the bounds are set:
	 *  left = size -1
	 *  right = 0
	 *
	*/
	VhdlPortMap(const QString& name, unsigned int size = 1);

	/*! \brief The constructor
	 *
	 * \param name The name of the port
	 * \param leftBound The left bound of the port.
	 * \param rightBound The right bound of the port.
	 *
	*/
	VhdlPortMap(const QString& name, int leftBound, int rightBound);

	//! \brief Copy constructor
	VhdlPortMap(const VhdlPortMap& other);

	//! \brief Assignment operator
	VhdlPortMap& operator=(const VhdlPortMap& other);

	//! \brief The operator ==
	bool operator==(const VhdlPortMap& other) const;

	//! \brief The operator !=
	bool operator!=(const VhdlPortMap& other) const;

	//! \brief The operator <
	bool operator<(const VhdlPortMap& other) const;

	//! \brief The operator >
	bool operator>(const VhdlPortMap& other) const;

	//! \brief The destructor
	virtual ~VhdlPortMap();

	/*! \brief Write the port map into the text stream.
	 *
	 * \param stream The stream to write into.
	 *
	*/
	void write(QTextStream& stream) const;

	/*! \brief Convert the port map into string.
	 * 
	 * The string is format: <port name>(<left> downto <right>)
	 *
	 * \return QString
	*/
	QString toString() const;

	/*! \brief Get the name of the port for the map.
	 *
	 * \return QString contains the 
	*/
	QString name() const;

private:
	
	//! \brief The name of the port for the port map
	QString name_;

	//! \brief The left bound of the port for the port map
	int left_;

	//! \brief The right bound of the port for the port map
	int right_;
};

#endif // VHDLPORTMAP_H
