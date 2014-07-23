//-----------------------------------------------------------------------------
// File: HDLPort.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.7.2014
//
// Description:
// Base class for all HDL port declarations.
//-----------------------------------------------------------------------------

#ifndef HDLPort_H
#define HDLPort_H

#include "HDLObject.h"
#include "vhdlportsorter.h"

#include <IPXACTmodels/generaldeclarations.h>

class Port;

//-----------------------------------------------------------------------------
//! HDLPort is used within top component entity and component declarations to print the declaration for one port.
//-----------------------------------------------------------------------------
class HDLPort : public HDLObject
{
public:

	/*! The constructor
	 *	 
	 *      @param [in] port     The port that contains the details for this port.
     *      @param [in] parent   The parent of this port.
	 *
	*/
	HDLPort(Port* port, QObject* parent = 0);
	
	//! The destructor
	virtual ~HDLPort();

	/*! Write the contents of the port to the text stream.
	 *
	 *      @param [in] stream The text stream to write the port into.	 
	*/
	virtual void write(QTextStream& stream) const = 0;

	/*! Get the size of the port.	 	 
	 * 
	 *      @return The size of the port.
	 */
	virtual int size() const;

	/*! Get the left bound of the port.
	 *
	 *      @return int The left bound.
	 */
	virtual int left() const;

	/*! Get the right bound of the port.
	 *
	 *      @return int The right bound.
	 */
	virtual int right() const;

    General::Direction direction() const;

	/*! Set the port to be connected or not.
	 *
	 *      @param [in] shouldConnect If true then the port is connected.	 
	 */
	virtual void setConnected(bool shouldConnect);

	/*! Check if the port is connected or not.
	 *
	 *      @return bool True if the port is connected.
	*/
	virtual bool isConnected() const;

	/*! Checks the map for uncommented ports that are used in synthesis.
	 *
	 *      @param [in]     list of ports that are checked.
	 *
	 *      @return True if at least one connected port is found.
	*/
	static bool hasConnectedPorts(QList<QSharedPointer<HDLPort> > const& ports);

private:
	//! No copying
	HDLPort(const HDLPort& other);

	//! No assignment
	HDLPort& operator=(const HDLPort& other);
   
	//! The direction of the port.
	General::Direction direction_;

	//! The left bound of the port.
	int left_;

	//! The right bound of the port.
	int right_;

	//! If true then the port is connected.
	bool connected_;
};

#endif // HDLPort_H
