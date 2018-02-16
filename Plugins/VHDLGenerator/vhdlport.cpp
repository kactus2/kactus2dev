/* 
 *  	Created on: 28.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlport.cpp
 *		Project: Kactus 2
 */

#include "vhdlport.h"

#include "vhdlgeneral.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/Port.h>

VhdlPort::VhdlPort(QSharedPointer<Port> port, QSharedPointer<ExpressionParser> parser):
VhdlTypedObject(port->name(),
		   port->getTypeName(),
		   port->getDefaultValue(), 
		   port->description()),
direction_(port->getDirection()),
left_(port->getLeftBound()),
right_(port->getRightBound()),
commentOut_(true),
parser_(parser)
{
	// if type is not set then use the defaults
	if (type().isEmpty())
    {
		if (size() == 1)
        {
			setType(QString("std_logic"));
		}
		else
        {
			setType(QString("std_logic_vector"));
		}
	}
}

VhdlPort::~VhdlPort() {
}

void VhdlPort::write( QTextStream& stream ) const {
	if (commentOut_) {
		stream << "-- ";
	}

    stream << getVhdlLegalName().leftJustified(16, ' ');     //align colons (:) at least roughly
	stream << " : " << DirectionTypes::direction2Str(direction_) << " ";

	stream << VhdlGeneral::vhdlType2String(type(), parser_->parseExpression(left_).toInt(), parser_->parseExpression(right_).toInt());
}

int VhdlPort::size() const {
    return parser_->parseExpression(left_).toInt() - parser_->parseExpression(right_).toInt() + 1;
	return 0;
}

QString VhdlPort::left() const {
	return left_;
}

QString VhdlPort::right() const {
	return right_;
}

void VhdlPort::setCommented( bool commentOut ) {
	commentOut_ = commentOut;
}

bool VhdlPort::isCommented() const {
	return commentOut_;
}

bool VhdlPort::hasRealPorts( const QMap<VhdlPortSorter, QSharedPointer<VhdlPort> >& ports )
{
	foreach (QSharedPointer<VhdlPort> port, ports)
    {
		// if at least one port that is uncommented is found
		if (!port->isCommented())
        {
			return true;
		}
	}

	// all ports were commented out so they are not in synthesis
	return false;
}
