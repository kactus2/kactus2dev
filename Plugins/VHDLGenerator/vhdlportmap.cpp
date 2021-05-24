/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlportmap.cpp
 *		Project: Kactus 2
 */

#include "vhdlportmap.h"

VhdlPortMap::VhdlPortMap():
VhdlTypedObject("", "std_logic", "", ""),
left_(),
right_()
{

}

VhdlPortMap::VhdlPortMap(const QString& name, const QString& leftBound, const QString& rightBound, const QString& type) :
    VhdlTypedObject(name, type, "", ""),
    left_(leftBound),
    right_(rightBound)
{

}


bool VhdlPortMap::operator==(const VhdlPortMap& other) const
{
    if (name() == other.name() && left_ == other.left_ &&	right_ == other.right_)
    {
        return true;
    }
    return false;
}

bool VhdlPortMap::operator!=(const VhdlPortMap& other) const
{
    if (name() != other.name())
    {
        return true;
    }
    else if (left_ != other.left_)
    {
        return true;
    }
    else if (right_ != other.right_)
    {
        return true;
    }
    return false;
}

bool VhdlPortMap::operator<( const VhdlPortMap& other ) const 
{
	if (name().compare(other.name(), Qt::CaseInsensitive) == 0) 
    {
		if (left_ == other.left_) 
        {
			return right_ < other.right_;
		}
		else 
        {
			return left_ < other.left_;
		}
	}
	else 
    {
		return name().compare(other.name(), Qt::CaseInsensitive) < 0;
	}
}

bool VhdlPortMap::operator>( const VhdlPortMap& other ) const 
{
	if (name().compare(other.name(), Qt::CaseInsensitive) == 0) 
    {
		if (left_ == other.left_) 
        {
			return right_ > other.right_;
		}
		else 
        {
			return left_ > other.left_;
		}
	}
	else 
    {
		return name().compare(other.name(), Qt::CaseInsensitive) > 0;
	}
}

void VhdlPortMap::write( QTextStream& stream ) const
{
	stream << getVhdlLegalName().leftJustified(16, ' ');
}

QString VhdlPortMap::mappingWith(VhdlPortMap const& value) const
{
    bool writeBounds = (left_.compare(value.left_) != 0 || right_.compare(value.right_) != 0);

    QString result(getVhdlLegalName() + " => " + value.getVhdlLegalName());

    if (writeBounds && value.left_.isEmpty() == false && value.left_.compare(QLatin1String("-1")) != 0 && 
        value.right_.isEmpty() == false && value.right_.compare(QLatin1String("-1")) != 0)
    {
        result += QString("(%1 downto %2)").arg(value.left_).arg(value.right_);
    }
   
    return result;
}
