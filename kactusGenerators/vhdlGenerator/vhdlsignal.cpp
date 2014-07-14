/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlsignal.cpp
 *		Project: Kactus 2
 */

#include "vhdlsignal.h"

#include "vhdlgeneral.h"

//-----------------------------------------------------------------------------
// Function: VhdlSignal::VhdlSignal()
//-----------------------------------------------------------------------------
VhdlSignal::VhdlSignal( QObject* parent, 
                       QString const& name /*= QString()*/,
					   QString const& signalType /*= QString()*/,
					   int leftBound /*= -1*/, 
					   int rightBound /*= -1*/, 
					   QString const& description /*= QString()*/,
					   QString const& defaultValue /*= QString()*/):
HDLSignal(parent, name, signalType, leftBound, rightBound, description, defaultValue)
{

	if (signalType.isEmpty()) {
		setType(VhdlGeneral::useDefaultType(left(), right()));
	}	
	Q_ASSERT(!type().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: VhdlSignal::~VhdlSignal()
//-----------------------------------------------------------------------------
VhdlSignal::~VhdlSignal()
{

}

//-----------------------------------------------------------------------------
// Function: VhdlSignal::write()
//-----------------------------------------------------------------------------
void VhdlSignal::write( QTextStream& stream ) const {
	Q_ASSERT(!name().isEmpty());
	Q_ASSERT(!type().isEmpty());

	if (!description().isEmpty()) {
		VhdlGeneral::writeDescription(description(), stream, QString("  "));
	}
	stream << "  signal "; 
	stream << name().leftJustified(16, ' '); //align colons (:) at least roughly
	stream << " : ";
	QString typeDefinition = VhdlGeneral::vhdlType2String(type(), left(), right());
	stream << typeDefinition << ";" << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlSignal::setBounds()
//-----------------------------------------------------------------------------
void VhdlSignal::setBounds( int left, int right )
{    
    HDLSignal::setBounds(left, right);
    
	// if scalar port changed to vectored
	if (type() == "std_logic" && left != right)
    {
		setType("std_logic_vector");
	}

	// if vectored port changed to scalar
	else if (type() == "std_logic_vector" && left == right)
    {
		setType("std_logic");
	}
}
