//-----------------------------------------------------------------------------
// File: ViewReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Reader class for IP-XACT view element.
//-----------------------------------------------------------------------------

#include "ViewReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>

//-----------------------------------------------------------------------------
// Function: ViewReader::ViewReader()
//-----------------------------------------------------------------------------
ViewReader::ViewReader(QObject* parent /* = 0 */) :
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ViewReader::~ViewReader()
//-----------------------------------------------------------------------------
ViewReader::~ViewReader()
{

}

//-----------------------------------------------------------------------------
// Function: ViewReader::createViewFrom()
//-----------------------------------------------------------------------------
QSharedPointer<View> ViewReader::createViewFrom(QDomNode const& viewNode) const
{
    QSharedPointer<View> newView (new View());

    parseNameGroup(viewNode, newView);

    parseIsPresent(viewNode, newView);

    parseEnvIdentifiers(viewNode, newView);

    parseInstantiationRefs(viewNode, newView);

    return newView;
}

//-----------------------------------------------------------------------------
// Function: ViewReader::parseNameGroup()
//-----------------------------------------------------------------------------
void ViewReader::parseNameGroup(QDomNode const& viewNode, QSharedPointer<View> newView) const
{
    NameGroupReader nameReader;
    nameReader.parseNameGroup(viewNode, newView);
}

//-----------------------------------------------------------------------------
// Function: ViewReader::parseIsPresent()
//-----------------------------------------------------------------------------
void ViewReader::parseIsPresent(QDomNode const& viewNode, QSharedPointer<View> newView) const
{
    QString newIsPresent = viewNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();
    if (!newIsPresent.isEmpty())
    {
        newView->setIsPresent(newIsPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: ViewReader::parseEnvIdentifiers()
//-----------------------------------------------------------------------------
void ViewReader::parseEnvIdentifiers(QDomNode const& viewNode, QSharedPointer<View> newView) const
{
    QDomElement viewElement = viewNode.toElement();

    if (!viewElement.isNull())
    {
        QDomNodeList envIdentifierNodes = viewElement.elementsByTagName(QStringLiteral("ipxact:envIdentifier"));

        for (int i = 0; i < envIdentifierNodes.count(); ++i)
        {
			// Read an environment identifier, identify its parts by the separator.
			QString idString = envIdentifierNodes.at(i).firstChild().nodeValue();
			QStringList idTuple = idString.split(QLatin1Char(':'));

			// Assign the parts to a struct.
			QSharedPointer<View::EnvironmentIdentifier> identifier( new View::EnvironmentIdentifier );

			if ( idTuple.size() > 0 )
			{
				identifier->language = idTuple.at(0);
			}

			if ( idTuple.size() > 1 )
			{
				identifier->tool = idTuple.at(1);
			}

			if ( idTuple.size() > 2 )
			{
				identifier->vendorSpecific = idTuple.at(2);
			}

			// Append to the view.
            newView->addEnvIdentifier(identifier);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ViewReader::parseInstantiationRefs()
//-----------------------------------------------------------------------------
void ViewReader::parseInstantiationRefs(QDomNode const& viewNode, QSharedPointer<View> newView) const
{
    QDomElement componentRefNode = viewNode.firstChildElement(QStringLiteral("ipxact:componentInstantiationRef"));
    if (!componentRefNode.isNull())
    {
        newView->setComponentInstantiationRef(componentRefNode.firstChild().nodeValue());
    }

    QDomElement designRefNode = viewNode.firstChildElement(QStringLiteral("ipxact:designInstantiationRef"));
    if (!designRefNode.isNull())
    {
        newView->setDesignInstantiationRef(designRefNode.firstChild().nodeValue());
    }

    QDomElement designConfigurationRefNode =
        viewNode.firstChildElement(QStringLiteral("ipxact:designConfigurationInstantiationRef"));
    if (!designConfigurationRefNode.isNull())
    {
        newView->setDesignConfigurationInstantiationRef(designConfigurationRefNode.firstChild().nodeValue());
    }
}
