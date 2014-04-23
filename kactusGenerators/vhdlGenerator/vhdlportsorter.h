/* 
 *  	Created on: 28.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlportsorter.h
 *		Project: Kactus 2
 */

#ifndef VHDLPORTSORTER_H
#define VHDLPORTSORTER_H

#include <models/generaldeclarations.h>

#include <QString>

/*! \brief VhdlPortSorter can be used as a key in associative containers to sort VhdlPorts
 * 
 */
class VhdlPortSorter {

public:

	/*! \brief The constructor
	 *
	 * \param interface The name of the interface the port belongs to.
	 * \param name The name of the port.
	 * \param direction The direction of the port.
	 *
	*/
	VhdlPortSorter(const QString& interface,
		const QString& name, 
		General::Direction direction);
	
	//! \brief Copy constructor
	VhdlPortSorter(const VhdlPortSorter& other);

	//! \brief The destructor
	virtual ~VhdlPortSorter();

	//! \brief Assignment operator
	VhdlPortSorter& operator=(const VhdlPortSorter& other);

	/*! \brief The operator ==
	 *
	 * The ports are considered equal if they have the same name.
	 * 
	 * \param other Reference to the sorter to compare to.
	 *
	 * \return True if ports have same name.
	*/
	bool operator==(const VhdlPortSorter& other) const;

	/*! \brief The operator !=
	 * 
	 * The ports are considered unequal if they have different names.
	 * 
	 * \param other Reference to the sorter to compare to.
	 *
	 * \return True if ports have different names.
	*/
	bool operator!=(const VhdlPortSorter& other) const;

	/*! \brief The operator <
	 * 
	 * The ports are sorted according to the direction of the port.
	 * 
	 * \param other Reference to the sorter to compare to.
	 *
	 * \return bool VhdlPortSorter&
	*/
	bool operator<(const VhdlPortSorter& other) const;

	/*! \brief The operator >
	 *
	 * The ports are sorted according to the direction of the port.
	 *
	 * \param other Reference to the sorter to compare to.
	 *
	 * \return bool VhdlPortSorter&
	*/
	bool operator>(const VhdlPortSorter& other) const;

	/*! \brief Get the name of the interface the port belongs to.
	 *
	 * \return QString The name fo the interface.
	*/
	QString interface() const;

private:
	
	//! \brief The name of the interface the port belongs to.
	QString interface_;

	//! \brief The name of the port.
	QString name_;

	//! \brief The direction of the port.
	General::Direction direction_;
};

#endif // VHDLPORTSORTER_H
