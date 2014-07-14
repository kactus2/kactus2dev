/* 
 *  	Created on: 28.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlport.h
 *		Project: Kactus 2
 */

#ifndef VHDLPORT_H
#define VHDLPORT_H

#include <kactusGenerators/HDLGenerator/HDLPort.h>

class Port;

/*! VhdlPort represents one port declaration in vhdl.
 * 
 * VhdlPort is used within top component entity and component declarations
 * to print the declaration for one port.
 */
class VhdlPort : public HDLPort
{
	Q_OBJECT

public:

	/*! The constructor
	 *	 
	 * @param [in]  port    Pointer to the port that contains the details for this port.
     * @param [in]  parent  Pointer to the owner of this port.
	 *
	*/
	VhdlPort(Port* port, QObject* parent = 0);
	
	//! The destructor
	virtual ~VhdlPort();

	/*! Write the contents of the port to the text stream.
	 *
	 * @param [in] stream   The text stream to write the port into.
	 *
	*/
	virtual void write(QTextStream& stream) const;

private:
	//! No copying
	VhdlPort(const VhdlPort& other);

	//! No assignment
	VhdlPort& operator=(const VhdlPort& other);

};

#endif // VHDLPORT_H
