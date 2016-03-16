//-----------------------------------------------------------------------------
// File: ComponentInstanceReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.09.2015
//
// Description:
// XML reader class for IP-XACT component instance.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCEREADER_H
#define COMPONENTINSTANCEREADER_H

#include <IPXACTmodels/common/DocumentReader.h>
#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QObject>
#include <QDomNode>

class ComponentInstance;
//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT component instance.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentInstanceReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent object.
     */
    ComponentInstanceReader(QObject* parent = 0);

    /*!
	 *  The destructor.
	 */
    ~ComponentInstanceReader();

    /*!
     *  Creates a component instance from XML definition.
     *
     *      @param [in] instanceNode    The XML description of the component instance.
     *
     *      @return The created component instance.
     */
    QSharedPointer<ComponentInstance> createComponentInstanceFrom(QDomNode const& instanceNode) const;

private:

    // Disable copying.
    ComponentInstanceReader(ComponentInstanceReader const& rhs);
    ComponentInstanceReader& operator=(ComponentInstanceReader const& rhs);

    /*!
     *  Parse extensions of the instance.
     *
     *      @param [in] componentInstanceNode   XML description of the extensions.
     *      @param [in] instance                The component instance whose extensions are read.
     */
    void parseExtensions(const QDomNode& componentInstanceNode, QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Parse the draft flag of the instance.
     *
     *      @param [in] draftNode           XML description of the draft flag.
     *      @param [in] instance            The possessing component instance.
     */
    void parseDraft(QDomElement const& draftNode, QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Parse the position of the instance.
     *
     *      @param [in] positionElement     XML description of the instance position.
     *      @param [in] instance            The possessing component instance.
     */
    void parsePosition(QDomElement const& positionElement, QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Parse import information.
     *
     *      @param [in] importElement   XML description of the instance import.
     *      @param [in] instance        The possessing component instance.
     */
    void parseImport(QDomElement const& importElement, QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Parse port positions.
     *
     *      @param [in] portPositionsElement    XML description of the component instance port positions.
     *      @param [in] instance                The possessing component instance.
     */
    void parsePortPositions(QDomElement const& portPositionsElement,
        QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Parse ad-hoc visibilities.
     *
     *      @param [in] adHocElement    XML description of the ad-hoc visibilities.
     *      @param [in] instance        The possessing component instance.
     */
    void parseAdHocVisibilities(QDomElement const& adHocElement, QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Parse api interface positions.
     *
     *      @param [in] apiElement  XML description of the api interfaces.
     *      @param [in] instance    The possessing component instance.
     */
    void parseApiInterfacePositions(QDomElement const& apiElement,
        QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Parse com interface positions.
     *
     *      @param [in] comElement  XML description of the com interfaces.
     *      @param [in] instance    The possessing component instance.
     */
    void parseComInterfacePositions(QDomElement const& comElement,
        QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Parse property values.
     *
     *      @param [in] propertyElement     XML description of the property values.
     *      @param [in] instance            The possessing component instance.
     */
    void parsePropertyValues(QDomElement const& propertyElement, QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Create mapped positions.
     *
     *      @param [in] positionElement         XML description of the positions.
     *      @param [in] itemIdentifier          Identifier for the containing XML element.
     *      @param [in] referenceIdentifier     Identifier for the reference.
     */
    QMap<QString, QPointF> createMappedPositions(QDomElement const& positionElement, QString const& itemIdentifier,
        QString const& referenceIdentifier) const;
};

#endif // COMPONENTINSTANCEREADER_H
