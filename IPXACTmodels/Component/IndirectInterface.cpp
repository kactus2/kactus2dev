//-----------------------------------------------------------------------------
// File: IndirectInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.08.2017
//
// Description:
// Implementation of the ipxact:indirectInterface element.
//-----------------------------------------------------------------------------

#include "IndirectInterface.h"

#include <IPXACTmodels/Component/TransparentBridge.h>

//-----------------------------------------------------------------------------
// Function: IndirectInterface::IndirectInterface()
//-----------------------------------------------------------------------------
IndirectInterface::IndirectInterface(QString const& name) : NameGroup(name), 
    Extendable(),
    indirectAddressRef_(), 
    indirectDataRef_(),
    memoryMapRef_(),
    transparentBridges_(new QList<QSharedPointer<TransparentBridge> >()),
    bitsInLau_(),
    endianness_(),
    parameters_(new QList<QSharedPointer<Parameter> >())
{

}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::IndirectInterface()
//-----------------------------------------------------------------------------
IndirectInterface::IndirectInterface(IndirectInterface const& other):
NameGroup(other),
    Extendable(other),
    indirectAddressRef_(other.indirectAddressRef_), 
    indirectDataRef_(other.indirectDataRef_),
    memoryMapRef_(other.memoryMapRef_),
    transparentBridges_(new QList<QSharedPointer<TransparentBridge> >()),
    bitsInLau_(other.bitsInLau_),
    endianness_(other.endianness_),
    parameters_(new QList<QSharedPointer<Parameter> >())
{
    foreach (QSharedPointer<TransparentBridge> bridge, *other.getTransparentBridges())
    {
        transparentBridges_->append(QSharedPointer<TransparentBridge>(new TransparentBridge(*bridge.data())));
    }

    foreach (QSharedPointer<Parameter> parameter, *other.getParameters())
    {
        parameters_->append(QSharedPointer<Parameter>(new Parameter(*parameter.data())));
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::getAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> IndirectInterface::getAttributes() const
{
    return attributes_;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::setAttributes()
//-----------------------------------------------------------------------------
void IndirectInterface::setAttributes(QMap<QString, QString> const& attributes)
{
    attributes_ = attributes;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::setIndirectAddressRef()
//-----------------------------------------------------------------------------
void IndirectInterface::setIndirectAddressRef(QString const& addressFieldId)
{
    indirectAddressRef_ = addressFieldId;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::getIndirectAddressRef()
//-----------------------------------------------------------------------------
QString IndirectInterface::getIndirectAddressRef() const
{
    return indirectAddressRef_;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::setIndirectDataRef()
//-----------------------------------------------------------------------------
void IndirectInterface::setIndirectDataRef(QString const& dataFieldId)
{
    indirectDataRef_ = dataFieldId;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::getIndirectDataRef()
//-----------------------------------------------------------------------------
QString IndirectInterface::getIndirectDataRef() const
{
    return indirectDataRef_;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::setMemoryMapRef()
//-----------------------------------------------------------------------------
void IndirectInterface::setMemoryMapRef(QString const& memoryMapName)
{
    memoryMapRef_ = memoryMapName;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::getMemoryMapRef()
//-----------------------------------------------------------------------------
QString IndirectInterface::getMemoryMapRef() const
{
    return memoryMapRef_;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::getTransparentBridges()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<TransparentBridge> > > IndirectInterface::getTransparentBridges() const
{
    return transparentBridges_;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::getBitsInLau()
//-----------------------------------------------------------------------------
void IndirectInterface::setBitsInLau(QString const& bitsInLau) 
{
    bitsInLau_ = bitsInLau;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::getBitsInLau()
//-----------------------------------------------------------------------------
QString IndirectInterface::getBitsInLau() const
{
    return bitsInLau_;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::setEndianess()
//-----------------------------------------------------------------------------
void IndirectInterface::setEndianness(QString const& endianness)
{
    endianness_ = endianness;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::getEndianess()
//-----------------------------------------------------------------------------
QString IndirectInterface::getEndianness() const
{
    return endianness_;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterface::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > IndirectInterface::getParameters() const
{
    return parameters_;
}