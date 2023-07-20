//-----------------------------------------------------------------------------
// File: ViewReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Reader for IP-XACT view element.
//-----------------------------------------------------------------------------

#include "ViewReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
// Function: ViewReader::createViewFrom()
//-----------------------------------------------------------------------------
QSharedPointer<View> ViewReader::createViewFrom(QDomNode const& viewNode, Document::Revision docRevision)
{
    QSharedPointer<View> newView (new View());

    NameGroupReader::parseNameGroup(viewNode, newView);

    if (docRevision == Document::Revision::Std14)
    {
        Details::parseIsPresent(viewNode, newView);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        CommonItemsReader::parseVendorExtensions(viewNode, newView);
    }

    Details::parseEnvIdentifiers(viewNode, newView);

    Details::parseInstantiationRefs(viewNode, newView);

    return newView;
}

//-----------------------------------------------------------------------------
// Function: ViewReader::Details::parseIsPresent()
//-----------------------------------------------------------------------------
void ViewReader::Details::parseIsPresent(QDomNode const& viewNode, QSharedPointer<View> newView)
{
    QString newIsPresent = viewNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();
    if (!newIsPresent.isEmpty())
    {
        newView->setIsPresent(newIsPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: ViewReader::Details::parseEnvIdentifiers()
//-----------------------------------------------------------------------------
void ViewReader::Details::parseEnvIdentifiers(QDomNode const& viewNode, QSharedPointer<View> newView)
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
// Function: ViewReader::Details::parseInstantiationRefs()
//-----------------------------------------------------------------------------
void ViewReader::Details::parseInstantiationRefs(QDomNode const& viewNode, QSharedPointer<View> newView)
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
