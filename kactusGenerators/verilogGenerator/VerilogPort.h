//-----------------------------------------------------------------------------
// File: VerilogPort.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// VerilogPort is used within top component entity to print the declaration for one port.
//-----------------------------------------------------------------------------

#ifndef VERILOGPORT_H
#define VERILOGPORT_H

#include <kactusGenerators/HDLGenerator/HDLPort.h>

class Port;

//-----------------------------------------------------------------------------
//! VerilogPort is used within top component entity to print the declaration of one port.
//-----------------------------------------------------------------------------
class VerilogPort : public HDLPort
{
	Q_OBJECT

public:

	/*! The constructor
	 *	 
	 * @param [in]  port    The port to represent.
     * @param [in]  parent  The parent object.
	 */
	VerilogPort(Port* port, QObject* parent = 0);
	
	//! The destructor
	virtual ~VerilogPort();

	/*! Write the contents of the port to the text stream.
	 *
	 *      @param [in] stream   The text stream to write the port into.
	*/
	virtual void write(QTextStream& stream) const;

private:
	//! No copying
	VerilogPort(const VerilogPort& other);

	//! No assignment
	VerilogPort& operator=(const VerilogPort& other);

    /*!
     *  Checks if the port should not be written.
     *
     *      @return True, if the port should not be written, otherwise false.
     */
    bool nonWriteable() const;

    /*!
     *  Converts the port direction to "input", "output" or "inout".
     *
     *      @return The string corresponding to the port direction.
     */
    QString directionString() const;

};

#endif // VERILOGPORT_H
