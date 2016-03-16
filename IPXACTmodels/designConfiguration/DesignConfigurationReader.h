//-----------------------------------------------------------------------------
// File: DesignConfigurationReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.08.2015
//
// Description:
// XML reader class for IP-XACT design configuration element.
//-----------------------------------------------------------------------------

#ifndef DESIGNCONFIGURATIONREADER_H
#define DESIGNCONFIGURATIONREADER_H

#include "DesignConfiguration.h"

#include <IPXACTmodels/common/DocumentReader.h>

#include <QObject>
#include <QDomNode>
#include <QDomNodeList>

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT design configuration element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignConfigurationReader : public DocumentReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor].
     *
     *      @param [in] parent  The parent object.
     */
    DesignConfigurationReader(QObject* parent = 0);

    /*!
	 *  The destructor.
	 */
    ~DesignConfigurationReader();

    /*!
     *  Creates a design configuration from XML definition.
     *
     *      @param [in] document    The XML document to create the design configuration from.
     *
     *      @return The created design configuration.
     */
    QSharedPointer<DesignConfiguration> createDesignConfigurationFrom(
        QDomDocument const& document) const;

private:

    // Disable copying.
    DesignConfigurationReader(DesignConfigurationReader const& rhs);
    DesignConfigurationReader& operator=(DesignConfigurationReader const& rhs);

    /*!
     *  Reads the design reference property from XML to a design configuration.
     *
     *      @param [in] designConfigurationNode     The XML description of the design configuration.
     *      @param [in] newDesignConfiguration      The item where to insert the design reference.
     */
    void parseDesignReference(QDomNode const& designConfigurationNode,
        QSharedPointer<DesignConfiguration> newDesignConfiguration) const;

    /*!
     *  Reads the generator chain configurations from XML to a design configuration.
     *
     *      @param [in] designConfigurationDocument     The XML description of the design configuration.
     *      @param [in] newDesignConfiguration          The item where to insert the chain configurations.
     */
    void parseGeneratorChainConfigurations(QDomDocument const& designConfigurationDocument,
        QSharedPointer<DesignConfiguration> newDesignConfiguration) const;

    /*!
     *  Reads a single generator chain configurations from XML.
     *
     *      @param [in] chainConfigurationNode      The XML description of a generator chain configuration.
     *      @param [in] newDesignConfiguration      The item where to insert the chain configuration.
     */
    void parseSingleGeneratorChainConfiguration(QDomNode const& chainConfigurationNode,
        QSharedPointer<DesignConfiguration> newDesignConfiguration) const;

    /*!
     *  Reads interconnection configurations from XML.
     *
     *      @param [in] designConfigurationDocument     The XML description of a design configuration.
     *      @param [in] newDesignConfiguration          The item where to insert the interconnection configurations.
     */
    void parseInterconnectionConfigurations(QDomDocument const& designConfigurationDocument,
        QSharedPointer<DesignConfiguration> newDesignConfiguration) const;

    /*!
     *  Reads multiple abstractor instances from XML.
     *
     *      @param [in] interconnectionConfigurationNode    The XML description of an interconnection configuration.
     *      @param [in] newInterconnectionConfiguration     The item where to insert multiple abstractor instances.
     */
    void parseMultipleAbstractors(QDomNode const& interconnectionConfigurationNode,
        QSharedPointer<InterconnectionConfiguration> newInterconnectionConfiguration) const;

    /*!
     *  Reads the interface references from XML.
     *
     *      @param [in] multipleAbstractorsNode             The XML description of multiple abstractor instances.
     *      @param [in] newMultipleAbstractorInstances      The item where to insert the interface references.
     */
    void parseInterfaceReferences(QDomNode const& multipleAbstractorsNode,
        QSharedPointer<MultipleAbstractorInstances> newMultipleAbstractorInstances) const;

    /*!
     *  Reads the abstractor instances from XML.
     *
     *      @param [in] multipleAbstractorsNode             The XML description of a multiple abstractor instances.
     *      @param [in] newMultipleAbstractorInstances      The item where to insert the abstractor instances.
     */
    void parseAbstractorInstances(QDomNode const& multipleAbstractorsNode,
        QSharedPointer<MultipleAbstractorInstances> newMultipleAbstractorInstances) const;

    /*!
     *  Get a list of name containing QDomNode child nodes.
     *
     *      @param [in] targetNode      The QDomNode from where to get the child nodes.
     *      @param [in] elementName     The tag of the elements to be searched for.
     *
     *      @return A list of child nodes containing the specified element name.
     */
    QDomNodeList getNamedChildNodes(QDomNode const& targetNode, QString elementName) const;

    /*!
     *  Reads the view configurations from XML.
     *
     *      @param [in] designConfigurationDocument     The XML description of a design configuration.
     *      @param [in] newDesignConfiguration          The item where to insert the view configurations.
     */
    void parseViewConfigurations(QDomDocument const& designConfigurationDocument,
        QSharedPointer<DesignConfiguration> newDesignConfiguration) const;

    /*!
     *  Reads the design configuration extensions.
     *
     *      @param [in] designConfigurationNode     XML description of the design configuration.
     *      @param [in] newDesignConfiguration      The new design configuration item.
     */
    void parseDesignConfigurationExtensions(QDomNode const& designConfigurationNode,
        QSharedPointer<DesignConfiguration> newDesignConfiguration) const;

    /*!
     *  Reads the configurable element values of the component instances.
     *
     *      @param [in] configurableElementsNode    XML description of the configurable elements.
     *      @param [in] newDesignConfiguration      The new design configuration item.
     */
    void parseInstanceConfigurableElementValues(QDomElement const& configurableElementsNode,
        QSharedPointer<DesignConfiguration> newDesignConfiguration) const;

    /*!
     *  Reads the view overrides.
     *
     *      @param [in] viewOverridesNode       XML description of the view overrides.
     *      @param [in] newDesignConfiguration  The new design configuration item.
     */
    void parseViewOverrides(QDomElement const& viewOverridesNode,
        QSharedPointer<DesignConfiguration> newDesignConfiguration) const;
};

#endif // DESIGNCONFIGURATIONREADER_H
