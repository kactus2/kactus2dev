//-----------------------------------------------------------------------------
// File: Mode.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.08.2023
//
// Description:
// Describes the ipxact:mode element.
//-----------------------------------------------------------------------------

#include "Mode.h"

//-----------------------------------------------------------------------------
// Function: Mode::Mode()
//-----------------------------------------------------------------------------
Mode::Mode(QString const& name):
NameGroup(name),
Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: Mode::Mode()
//-----------------------------------------------------------------------------
Mode::Mode( const Mode& other ):
NameGroup(other),
    Extendable(other)
{
    for (auto portSlice : *other.portSlices_)
    {
        portSlices_->append(QSharedPointer<PortSlice>(new PortSlice(*portSlice)));
    }

    for (auto fieldSlice : *other.fieldSlices_)
    {
        fieldSlices_->append(QSharedPointer<FieldSlice>(new FieldSlice(*fieldSlice)));
    }
}

//-----------------------------------------------------------------------------
// Function: Mode::~Mode()
//-----------------------------------------------------------------------------
Mode::~Mode()
{

}

//-----------------------------------------------------------------------------
// Function: Mode::operator=()
//-----------------------------------------------------------------------------
Mode& Mode::operator=( const Mode& other )
{
	if (this != &other)
    {
		NameGroup::operator=(other);
        Extendable::operator=(other);

		portSlices_->clear();
		for (auto portSlice : *other.portSlices_)
		{
			portSlices_->append(QSharedPointer<PortSlice>(new PortSlice(*portSlice)));
		}

        fieldSlices_->clear();
        for (auto fieldSlice : *other.fieldSlices_)
        {
            fieldSlices_->append(QSharedPointer<FieldSlice>(new FieldSlice(*fieldSlice)));
        }
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Mode::getCondition()
//-----------------------------------------------------------------------------
QString Mode::getCondition() const
{
    return condition_;
}

//-----------------------------------------------------------------------------
// Function: Mode::setCondition()
//-----------------------------------------------------------------------------
void Mode::setCondition(QString const& conditionExpression)
{
    condition_ = conditionExpression;
}

//-----------------------------------------------------------------------------
// Function: Mode::getPortSlices()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PortSlice> > > Mode::getPortSlices() const
{
	return portSlices_;
}

//-----------------------------------------------------------------------------
// Function: Mode::getFieldSlices()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<FieldSlice> > > Mode::getFieldSlices() const
{
    return fieldSlices_;
}
