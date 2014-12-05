/* 
 *  	Created on: 28.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlport.h
 *		Project: Kactus 2
 */

#ifndef VHDLPORT_H
#define VHDLPORT_H

#include "VhdlTypedObject.h"
#include "vhdlportsorter.h"

class Port;

/*! \brief VhdlPort represents one port declaration in vhdl.
 * 
 * VhdlPort is used within top component entity and component declarations
 * to print the declaration for one port.
 */
class VhdlPort : public VhdlTypedObject
{

public:

	/*! \brief The constructor
	 *	 
	 * \param port Pointer to the port that contains the details for this port.
     * \param parent Pointer to the owner of this port.
	 *
	*/
	VhdlPort(Port* port);
	
	//! \brief The destructor
	virtual ~VhdlPort();

	/*! \brief Write the contents of the port to the text stream.
	 *
	 * \param stream The text stream to write the port into.
	 *
	*/
	virtual void write(QTextStream& stream) const;

	/*! \brief Get the size of the port.
	 * 
	 * Size is calculated: left - right + 1
	 * 
	 * \return int The size of the port.
	*/
	virtual int size() const;

	/*! \brief Get the left bound of the port.
	 *
	 * \return int The left bound.
	*/
	virtual int left() const;

	/*! \brief Get the right bound of the port.
	 *
	 * \return int The right bound.
	*/
	virtual int right() const;

	/*! \brief Set the port to be commented out or not.
	 *
	 * \param commentOut If true then the port is commented out when written.
	 *
	*/
	virtual void setCommented(bool commentOut);

	/*! \brief Check if the port is commented out or not.
	 *
	 * \return bool True if the port is commented out.
	*/
	virtual bool isCommented() const;

	/*! \brief Checks the map for uncommented ports that are used in synthesis.
	 *
	 * \param QMap contains the ports that are checked.
	 *
	 * \return bool True if at least one uncommented port is found.
	*/
	static bool hasRealPorts(const QMap<VhdlPortSorter, QSharedPointer<VhdlPort> >& ports);

private:
	//! No copying
	VhdlPort(const VhdlPort& other);

	//! No assignment
	VhdlPort& operator=(const VhdlPort& other);

	//! The direction of the port.
	General::Direction direction_;

	//! The left bound of the port.
	int left_;

	//! The right bound of the port.
	int right_;

	//! If true then the port is commented out when printed to text stream
	bool commentOut_;
};

#endif // VHDLPORT_H
