//-----------------------------------------------------------------------------
// File: MemoryRemap.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.04.2015
//
// Description:
// Memory Remap describes additional memory map items of a slave bus interface in a specific remap state.
//-----------------------------------------------------------------------------

#include "MemoryRemap.h"

#include "GenericVendorExtension.h"

#include <QDomNode>
#include <QXmlStreamWriter>
#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: MemoryRemap::MemoryRemap()
//-----------------------------------------------------------------------------
MemoryRemap::MemoryRemap(QDomNode& memoryRemapNode):
AbstractMemoryMap(memoryRemapNode),
remapState_()
{
    for (int i = 0; i < memoryRemapNode.childNodes().count(); ++i)
    {
        QDomNode tempNode = memoryRemapNode.childNodes().at(i);

        if (tempNode.nodeName() == QString("spirit:remapState"))
        {
            remapState_ = tempNode.childNodes().at(0).nodeValue();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::MemoryRemap()
//-----------------------------------------------------------------------------
MemoryRemap::MemoryRemap():
AbstractMemoryMap(),
remapState_()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::MemoryRemap()
//-----------------------------------------------------------------------------
MemoryRemap::MemoryRemap(const MemoryRemap& other):
AbstractMemoryMap(other),
remapState_(other.remapState_)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::operator=()
//-----------------------------------------------------------------------------
MemoryRemap& MemoryRemap::operator=(const MemoryRemap& other)
{
    if (this != &other)
    {
        AbstractMemoryMap::operator=(other);
        remapState_ = other.remapState_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::~MemoryRemap()
//-----------------------------------------------------------------------------
MemoryRemap::~MemoryRemap()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::write()
//-----------------------------------------------------------------------------
void MemoryRemap::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement("spirit:memoryRemap");

    AbstractMemoryMap::write(writer);

    if (!remapState_.isNull())
    {
        writer.writeTextElement("spirit:remapState", remapState_);
    }

    writer.writeEndElement(); //spirit:memoryRemap
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::setRemapState()
//-----------------------------------------------------------------------------
void MemoryRemap::setRemapState(QString newRemapState)
{
    remapState_ = newRemapState;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::getRemapState()
//-----------------------------------------------------------------------------
QString MemoryRemap::getRemapState() const
{
    return remapState_;
}


//-----------------------------------------------------------------------------
// Function: MemoryRemap::isValid()
//-----------------------------------------------------------------------------
bool MemoryRemap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList remapStateNames, QStringList& errorList, const QString& parentIdentifier) const
{
    bool valid = AbstractMemoryMap::isValid(componentChoices, errorList, parentIdentifier);

    if (!remapStateNames.contains(remapState_))
    {
        if (remapState_.isEmpty())
        {
            errorList.append(QObject::tr("No remap state assigned in memory remap %1 of %2").arg(getName()).
                arg(parentIdentifier));
        }
        else
        {
            errorList.append(QObject::tr("Remap state %1 in memory remap %2 of %3 is not a remap state of component").
                arg(remapState_).arg(getName()).arg(parentIdentifier));
        }
    }

    return valid && !remapState_.isEmpty() && remapStateNames.contains(remapState_);
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::isValid()
//-----------------------------------------------------------------------------
bool MemoryRemap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList componentRemapStateNames) const
{
    bool valid = AbstractMemoryMap::isValid(componentChoices);

    return valid && !remapState_.isEmpty() && componentRemapStateNames.contains(remapState_);
}
