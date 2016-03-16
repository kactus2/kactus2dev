//-----------------------------------------------------------------------------
// File: choice.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.10.2010
//
// Description:
// Equals the ipxact:choice element in IP-Xact specification.
//-----------------------------------------------------------------------------

#include "Choice.h"

#include <IPXACTmodels/common/Enumeration.h>

#include <QList>
#include <QString>

//-----------------------------------------------------------------------------
// Function: Choice::Choice()
//-----------------------------------------------------------------------------
Choice::Choice():
choiceName_(),
enumerations_(new QList<QSharedPointer<Enumeration> >())
{
    
}

//-----------------------------------------------------------------------------
// Function: Choice::Choice()
//-----------------------------------------------------------------------------
Choice::Choice( const Choice& other ):
choiceName_(other.choiceName_),
enumerations_(new QList<QSharedPointer<Enumeration> >())
{
    foreach (QSharedPointer<Enumeration> enumeration, *other.enumerations_)
    {
        QSharedPointer<Enumeration> copy(new Enumeration(*enumeration));
        enumerations_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: Choice::operator=()
//-----------------------------------------------------------------------------
Choice& Choice::operator=( const Choice& other )
{
	if (this != &other)
    {
		choiceName_ = other.choiceName_;
		
        enumerations_->clear();
        foreach (QSharedPointer<Enumeration> enumeration, *other.enumerations_)
        {
            QSharedPointer<Enumeration> copy(new Enumeration(*enumeration));
            enumerations_->append(copy);
        }
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Choice::~Choice()
//-----------------------------------------------------------------------------
Choice::~Choice()
{

}

//-----------------------------------------------------------------------------
// Function: Choice::name()
//-----------------------------------------------------------------------------
QString Choice::name() const
{
	return choiceName_;
}

//-----------------------------------------------------------------------------
// Function: Choice::setName()
//-----------------------------------------------------------------------------
void Choice::setName(const QString& name)
{
	choiceName_ = name;
}

//-----------------------------------------------------------------------------
// Function: Choice::hasEnumeration()
//-----------------------------------------------------------------------------
bool Choice::hasEnumeration(QString const& enumerationValue) const
{
    foreach (QSharedPointer<Enumeration> enumeration, *enumerations_)
    {
        if (enumeration->getValue() == enumerationValue)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Choice::enumerationCount()
//-----------------------------------------------------------------------------
QStringList Choice::getEnumerationValues() const
{
    QStringList enumerationValues;

    foreach (QSharedPointer<Enumeration> enumeration, *enumerations_)
    {
        if (!enumeration->getText().isEmpty())
        {
            enumerationValues.append(enumeration->getText());
        }
        else
        {
            enumerationValues.append(enumeration->getValue());
        }
    }

    return enumerationValues;
}

//-----------------------------------------------------------------------------
// Function: Choice::enumerations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Enumeration> > > Choice::enumerations() const
{
    return enumerations_;
}
