//-----------------------------------------------------------------------------
// File: choice.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.10.2010
//
// Description:
// Equals the spirit:choice element in IP-Xact specification.
//-----------------------------------------------------------------------------

#include "choice.h"
#include "Enumeration.h"
#include "XmlUtils.h"

#include <QList>
#include <QString>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: Choice::Choice()
//-----------------------------------------------------------------------------
Choice::Choice(QDomNode &choice) : choiceName_(), enumerations_(new QList<QSharedPointer<Enumeration> >())
{
    QDomNodeList children = choice.childNodes();
    for (int i = 0; i < children.size(); ++i) {

        // get name
        if (children.at(i).nodeName() == QString("spirit:name"))
        {
            choiceName_ = children.at(i).childNodes().at(0).nodeValue();
            choiceName_ = XmlUtils::removeWhiteSpace(choiceName_);
        }

        // get enumerations
        else if (children.at(i).nodeName() == QString("spirit:enumeration"))
        {
            enumerations_->append(QSharedPointer<Enumeration>(new Enumeration(children.at((i)))));
        }
    }
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
// Function: Choice::write()
//-----------------------------------------------------------------------------
void Choice::write(QXmlStreamWriter& writer) const
{
	writer.writeStartElement("spirit:choice");

	writer.writeTextElement("spirit:name", choiceName_);

    foreach (QSharedPointer<Enumeration> enumeration, *enumerations_)
    {
        enumeration->write(writer);
    }

	writer.writeEndElement(); // spirit:choice
}

//-----------------------------------------------------------------------------
// Function: Choice::isValid()
//-----------------------------------------------------------------------------
bool Choice::isValid( QStringList& errorList, const QString& parentIdentifier ) const
{
	bool valid = true;
	
	if (choiceName_.isEmpty())
    {
		errorList.append(QObject::tr("No name specified for choice within %1").arg(parentIdentifier));
		valid = false;
	}

	if (enumerations_->isEmpty())
    {
		errorList.append(QObject::tr("At least one enumeration is required in choice %1"
            " within %2").arg(choiceName_).arg(parentIdentifier));
		valid = false;
	}

    foreach (QSharedPointer<Enumeration> enumeration, *enumerations_)
    {
        if (enumeration->getValue().isEmpty())
        {
            errorList.append(QObject::tr("No value specified for enumeration in choice %1"
                " within %2").arg(choiceName_).arg(parentIdentifier));
            valid = false;
        }
    }

	return valid;
}

//-----------------------------------------------------------------------------
// Function: Choice::isValid()
//-----------------------------------------------------------------------------
bool Choice::isValid() const
{
    if (choiceName_.isEmpty())
    {
        return false;
    }

    if (enumerations_->isEmpty())
    {
        return false;
    }

    foreach (QSharedPointer<Enumeration> enumeration, *enumerations_)
    {
        if (enumeration->getValue().isEmpty())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: Choice::getName()
//-----------------------------------------------------------------------------
QString Choice::getName() const
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
