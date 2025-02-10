//-----------------------------------------------------------------------------
// File: IpxactFile.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.01.2017
//
// Description:
// Implementation for ipxact:ipxactFile element.
//-----------------------------------------------------------------------------

#include "IpxactFile.h"

#include <QString>
#include <QDomNamedNodeMap>
#include <QDomNode>
#include <QDomNodeList>
#include <QObject>
#include <QSharedPointer>
#include <QStringList>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: IpxactFile::IpxactFile()
//-----------------------------------------------------------------------------
IpxactFile::IpxactFile(): 
Extendable(),
    vlnv_(),
    name_()
{

}

//-----------------------------------------------------------------------------
// Function: IpxactFile::IpxactFile()
//-----------------------------------------------------------------------------
IpxactFile::IpxactFile(IpxactFile const& other):
Extendable(other),
    vlnv_(other.vlnv_),
    name_(other.name_)
{

}

//-----------------------------------------------------------------------------
// Function: IpxactFile::operator=()
//-----------------------------------------------------------------------------
IpxactFile & IpxactFile::operator=(IpxactFile const& other)
{
    if (this != &other)
    {
        Extendable::operator=(other);
        vlnv_ = other.vlnv_;
        name_ = other.name_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: IpxactFile::~IpxactFile()
//-----------------------------------------------------------------------------
IpxactFile::~IpxactFile()
{

}

//-----------------------------------------------------------------------------
// Function: IpxactFile::setVlnv()
//-----------------------------------------------------------------------------
void IpxactFile::setVlnv(VLNV const& vlnv)
{
    vlnv_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: IpxactFile::getVlnv()
//-----------------------------------------------------------------------------
VLNV IpxactFile::getVlnv() const
{
    return vlnv_;
}

//-----------------------------------------------------------------------------
// Function: IpxactFile::setName()
//-----------------------------------------------------------------------------
void IpxactFile::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: IpxactFile::getName()
//-----------------------------------------------------------------------------
QString IpxactFile::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: IpxactFile::setDescription()
//-----------------------------------------------------------------------------
void IpxactFile::setDescription(QString const& description)
{
    description_ = description;
}

//-----------------------------------------------------------------------------
// Function: IpxactFile::getDescription()
//-----------------------------------------------------------------------------
QString IpxactFile::getDescription() const
{
    return description_;
}
