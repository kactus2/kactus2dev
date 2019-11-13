/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlportmap.h
 *		Project: Kactus 2
 */

#ifndef VHDLPORTMAP_H
#define VHDLPORTMAP_H

#include "VhdlTypedObject.h"

#include <QString>
#include <QTextStream>

/*! VhdlPortMap represents a port and it's optional bounds in vhdl.
 * 
 * This class is used in VhdlComponentInstance to store a mapped port and the 
 * boundaries of the mapping.
 */
class VhdlPortMap  : public VhdlTypedObject
{

public:

	/*! The default constructor.
	 *
	*/
	VhdlPortMap();

	/*! The constructor
	 *
	 * @param [in] name The name of the port
	 * @param [in] leftBound The left bound of the port.
	 * @param [in] rightBound The right bound of the port.
	 * @param [in] type The type of the mapped item.
	 *
	*/
	VhdlPortMap(const QString& name, 
		const QString& leftBound, 
		const QString& rightBound,
		const QString& type = QString());

	//! Copy constructor
	VhdlPortMap(const VhdlPortMap& other) = default;

	//! Assignment operator
	VhdlPortMap& operator=(const VhdlPortMap& other) = default;

	//! The operator ==
	bool operator==(const VhdlPortMap& other) const;

	//! The operator !=
	bool operator!=(const VhdlPortMap& other) const;

	//! The operator <
	bool operator<(const VhdlPortMap& other) const;

	//! The operator >
	bool operator>(const VhdlPortMap& other) const;

	//! The destructor
	virtual ~VhdlPortMap() = default;

	/*! Write the port map into the text stream.
	 *
	 * @param [in] stream The stream to write into.
	 *
	*/
	virtual void write(QTextStream& stream) const override final;

    /*! Create a mapping statement for this map with the given map.
     *
     * The string is format: <port name> => <port name>(<left> downto <right>)
     *
     * @return The mapping with the given map.
    */

    QString mappingWith(VhdlPortMap const& value) const;

private:

	//! The left bound of the port for the port map
	QString left_;

	//! The right bound of the port for the port map
	QString right_;

};

#endif // VHDLPORTMAP_H
