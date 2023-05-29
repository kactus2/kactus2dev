//-----------------------------------------------------------------------------
// File: ComponentInstanceReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.09.2015
//
// Description:
// XML reader for IP-XACT component instance.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCEREADER_H
#define COMPONENTINSTANCEREADER_H

#include <IPXACTmodels/common/DocumentReader.h>
#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QDomNode>

class ComponentInstance;

//-----------------------------------------------------------------------------
//! XML reader for IP-XACT component instance.
//-----------------------------------------------------------------------------
namespace ComponentInstanceReader 
{

    /*!
     *  Creates a component instance from XML definition.
     *
     *      @param [in] instanceNode    The XML description of the component instance.
     *      @param [in] docRevision     The IP-XACT revision to comply.
     *
     *      @return The created component instance.
     */
    IPXACTMODELS_EXPORT QSharedPointer<ComponentInstance> createComponentInstanceFrom(QDomNode const& instanceNode,
        Document::Revision docRevision);

    namespace Details
    {
        /*!
         * Parse the power domain links of the instance.
         *
         *     @param [in] instanceNode  XML description of the instance.
         *     @param [in] instance      The component instance whose power domain links are read.
         */
         void parsePowerDomainLinks(QDomNode const& instanceNode, QSharedPointer<ComponentInstance> instance);

        /*!
         *  Parse extensions of the instance.
         *
         *      @param [in] componentInstanceNode   XML description of the extensions.
         *      @param [in] instance                The component instance whose extensions are read.
         */
        void parseExtensions(const QDomNode& componentInstanceNode, QSharedPointer<ComponentInstance> instance);

        /*!
         *  Parse the draft flag of the instance.
         *
         *      @param [in] draftNode           XML description of the draft flag.
         *      @param [in] instance            The possessing component instance.
         */
        void parseDraft(QDomElement const& draftNode, QSharedPointer<ComponentInstance> instance);

        /*!
         *  Parse the position of the instance.
         *
         *      @param [in] positionElement     XML description of the instance position.
         *      @param [in] instance            The possessing component instance.
         */
        void parsePosition(QDomElement const& positionElement, QSharedPointer<ComponentInstance> instance);

        /*!
         *  Parse import information.
         *
         *      @param [in] importElement   XML description of the instance import.
         *      @param [in] instance        The possessing component instance.
         */
        void parseImport(QDomElement const& importElement, QSharedPointer<ComponentInstance> instance);

        /*!
         *  Parse port positions.
         *
         *      @param [in] portPositionsElement    XML description of the component instance port positions.
         *      @param [in] instance                The possessing component instance.
         */
        void parsePortPositions(QDomElement const& portPositionsElement,
            QSharedPointer<ComponentInstance> instance);

        /*!
         *  Parse ad-hoc visibilities.
         *
         *      @param [in] adHocElement    XML description of the ad-hoc visibilities.
         *      @param [in] instance        The possessing component instance.
         */
        void parseAdHocVisibilities(QDomElement const& adHocElement, QSharedPointer<ComponentInstance> instance);

        /*!
         *  Parse api interface positions.
         *
         *      @param [in] apiElement  XML description of the api interfaces.
         *      @param [in] instance    The possessing component instance.
         */
        void parseApiInterfacePositions(QDomElement const& apiElement,
            QSharedPointer<ComponentInstance> instance);

        /*!
         *  Parse com interface positions.
         *
         *      @param [in] comElement  XML description of the com interfaces.
         *      @param [in] instance    The possessing component instance.
         */
        void parseComInterfacePositions(QDomElement const& comElement,
            QSharedPointer<ComponentInstance> instance);

        /*!
         *  Parse property values.
         *
         *      @param [in] propertyElement     XML description of the property values.
         *      @param [in] instance            The possessing component instance.
         */
        void parsePropertyValues(QDomElement const& propertyElement, QSharedPointer<ComponentInstance> instance);

        /*!
         *  Create mapped positions.
         *
         *      @param [in] positionElement         XML description of the positions.
         *      @param [in] itemIdentifier          Identifier for the containing XML element.
         *      @param [in] referenceIdentifier     Identifier for the reference.
         */
        QMap<QString, QPointF> createMappedPositions(QDomElement const& positionElement, 
            QString const& itemIdentifier, QString const& referenceIdentifier);

        /*!
         *  Parse file set reference.
         *
         *      @param [in] fileSetRefElement       XML description of the file set reference.
         *      @param [in] instance                The possessing component instance.
         */
        void parseFileSetRef(QDomElement const& fileSetRefElement, QSharedPointer<ComponentInstance> instance);

        /*!
         *  Parse hardware mapping.
         *
         *      @param [in] mappingElement          XML description of the mapping.
         *      @param [in] instance                The possessing component instance.
         */
        void parseMapping(QDomElement const& mappingElement, QSharedPointer<ComponentInstance> instance);
    }
};

#endif // COMPONENTINSTANCEREADER_H
