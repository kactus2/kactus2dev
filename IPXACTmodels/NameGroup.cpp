//-----------------------------------------------------------------------------
// File: NameGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 9.7.2014
//
// Description:
// NameGroup matches the spirit:nameGroup element in IP-Xact.
//-----------------------------------------------------------------------------

#include "NameGroup.h"
#include <IPXACTmodels/generaldeclarations.h>

#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: NameGroup::NameGroup()
//-----------------------------------------------------------------------------
NameGroup::NameGroup(): 
    name_(),
    displayName_(),
    description_() 
{

}

//-----------------------------------------------------------------------------
// Function: NameGroup::NameGroup()
//-----------------------------------------------------------------------------
NameGroup::NameGroup(QDomNode& node): 
    name_(), 
    displayName_(),
    description_()
{
    // search all the node's childNodes and find the right ones.
    for (int i = 0; i < node.childNodes().count(); ++i) {
        QDomNode tempNode = node.childNodes().at(i);

        if (tempNode.nodeName() == QString("spirit:name")) {
            name_ = tempNode.childNodes().at(0).nodeValue();
            name_ = XmlUtils::removeWhiteSpace(name_);
        }
        else if (tempNode.nodeName() == QString("spirit:displayName")) {
            displayName_ = tempNode.childNodes().at(0).nodeValue();
        }
        else if (tempNode.nodeName() == QString("spirit:description")) {
            description_ = tempNode.childNodes().at(0).nodeValue();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: NameGroup::NameGroup()
//-----------------------------------------------------------------------------
NameGroup::NameGroup(QString const& name, QString const& displayName, QString const& description):
    name_(name), 
    displayName_(displayName),
    description_(description)
{

}

//-----------------------------------------------------------------------------
// Function: NameGroup::NameGroup()
//-----------------------------------------------------------------------------
NameGroup::NameGroup( const NameGroup& other ):
    name_(other.name_),
    displayName_(other.displayName_),
    description_(other.description_)
{

}

//-----------------------------------------------------------------------------
// Function: NameGroup::operator=()
//-----------------------------------------------------------------------------
NameGroup& NameGroup::operator=( const NameGroup& other ) {
    if (this != &other) {
        name_ = other.name_;
        displayName_ = other.displayName_;
        description_ = other.description_;
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::name()
//-----------------------------------------------------------------------------
QString NameGroup::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setName()
//-----------------------------------------------------------------------------
void NameGroup::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::displayName()
//-----------------------------------------------------------------------------
QString NameGroup::displayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setDisplayName()
//-----------------------------------------------------------------------------
void NameGroup::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::description()
//-----------------------------------------------------------------------------
QString NameGroup::description() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setDescription()
//-----------------------------------------------------------------------------
void NameGroup::setDescription(QString const& description)
{
    description_ = description;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::write()
//-----------------------------------------------------------------------------
void NameGroup::write(QXmlStreamWriter& writer) const
{
    writer.writeTextElement("spirit:name", name_);

    if (!displayName_.isEmpty()) {
        writer.writeTextElement("spirit:displayName", displayName_);
    }

    if (!description_.isEmpty()) {
        writer.writeTextElement("spirit:description", description_);
    }
}
