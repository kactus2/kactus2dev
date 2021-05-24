/* 
 *  	Created on: 28.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlportsorter.h
 *		Project: Kactus 2
 */

#ifndef VHDLPORTSORTER_H
#define VHDLPORTSORTER_H

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QString>

/*! VhdlPortSorter can be used as a key in associative containers to sort VhdlPorts
 * 
 */
class VhdlPortSorter {

public:

	/*! The constructor
	 *
	 * @param [in] interface The name of the interface the port belongs to.
	 * @param [in] name The name of the port.
	 * @param [in] direction The direction of the port.
	 *
	*/
	VhdlPortSorter(const QString& interface, const QString& name, DirectionTypes::Direction direction);
	
	//! Copy constructor
	VhdlPortSorter(const VhdlPortSorter& other) = default;

	//! The destructor
	~VhdlPortSorter() = default;

	//! Assignment operator
	VhdlPortSorter& operator=(const VhdlPortSorter& other) = default;

	/*! The operator ==
	 *
	 * The ports are considered equal if they have the same name.
	 * 
	 * @param [in] other Reference to the sorter to compare to.
	 *
	 * @return True if ports have same name.
	*/
	bool operator==(const VhdlPortSorter& other) const;

	/*! The operator !=
	 * 
	 * The ports are considered unequal if they have different names.
	 * 
	 * @param [in] other Reference to the sorter to compare to.
	 *
	 * @return True if ports have different names.
	*/
	bool operator!=(const VhdlPortSorter& other) const;

	/*! The operator <
	 * 
	 * The ports are sorted according to the direction of the port.
	 * 
	 * @param [in] other Reference to the sorter to compare to.
	 *
	 * @return bool VhdlPortSorter&
	*/
	bool operator<(const VhdlPortSorter& other) const;

	/*! The operator >
	 *
	 * The ports are sorted according to the direction of the port.
	 *
	 * @param [in] other Reference to the sorter to compare to.
	 *
	 * @return bool VhdlPortSorter&
	*/
	bool operator>(const VhdlPortSorter& other) const;

	/*! Get the name of the interface the port belongs to.
	 *
	 * @return QString The name for the interface.
	*/
	QString interface() const;

private:
	
	//! The name of the interface the port belongs to.
	QString interface_;

	//! The name of the port.
	QString name_;

	//! The direction of the port.
	DirectionTypes::Direction direction_;
};

#endif // VHDLPORTSORTER_H
