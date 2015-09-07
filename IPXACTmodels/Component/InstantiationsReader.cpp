//-----------------------------------------------------------------------------
// File: InstantiationsReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Reader class for IP-XACT instantiations.
//-----------------------------------------------------------------------------

#include "InstantiationsReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::InstantiationsReader()
//-----------------------------------------------------------------------------
InstantiationsReader::InstantiationsReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::~InstantiationsReader()
//-----------------------------------------------------------------------------
InstantiationsReader::~InstantiationsReader()
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::createDesignInstantiationFrom()
//-----------------------------------------------------------------------------
QSharedPointer<DesignInstantiation> InstantiationsReader::createDesignInstantiationFrom(
    QDomNode const& instantiationNode) const
{
    QSharedPointer<DesignInstantiation> newInstantiation (new DesignInstantiation());

    NameGroupReader nameReader;
    nameReader.parseNameGroup(instantiationNode, newInstantiation);

    QDomNode designReferenceNode = instantiationNode.firstChildElement("ipxact:designRef");
    newInstantiation->setDesignReference(parseConfigurableVLNVReference(designReferenceNode, VLNV::DESIGN));

    parseVendorExtensions(instantiationNode, newInstantiation);

    return newInstantiation;
}
