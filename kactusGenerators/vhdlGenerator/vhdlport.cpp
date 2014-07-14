/* 
 *  	Created on: 28.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlport.cpp
 *		Project: Kactus 2
 */

#include "vhdlport.h"

#include "vhdlgeneral.h"

#include <IPXACTmodels/port.h>

//-----------------------------------------------------------------------------
// Function: VhdlPort::VhdlPort()
//-----------------------------------------------------------------------------
VhdlPort::VhdlPort(Port* port, QObject* parent):
HDLPort(port, parent)
{
	// if type is not set then use the defaults
	if (type().isEmpty())
    {

		// if port is scalar
		if (size() == 1) {
			setType(QString("std_logic"));
		}
		else {
			setType(QString("std_logic_vector"));
		}
	}
}

//-----------------------------------------------------------------------------
// Function: VhdlPort::VhdlPort()
//-----------------------------------------------------------------------------
VhdlPort::~VhdlPort()
{

}

//-----------------------------------------------------------------------------
// Function: VhdlPort::write()
//-----------------------------------------------------------------------------
void VhdlPort::write( QTextStream& stream ) const {
	if (!isConnected())
    {
		stream << "-- ";
	}

	stream << name().leftJustified(16, ' '); //align colons (:) at least roughly
	stream << " : " << General::direction2Str(direction()) << " ";

	stream << VhdlGeneral::vhdlType2String(type(), left(), right());
}
