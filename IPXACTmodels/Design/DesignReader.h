//-----------------------------------------------------------------------------
// File: DesignReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.08.2015
//
// Description:
// XML reader for IP-XACT design element.
//-----------------------------------------------------------------------------

#ifndef DESIGNREADER_H
#define DESIGNREADER_H

#include "Design.h"
#include "ActiveInterface.h"
#include "HierInterface.h"
#include "MonitorInterface.h"
#include "PortReference.h"

#include <IPXACTmodels/common/DocumentReader.h>

#include <QDomNode>
#include <QDomNodeList>

//-----------------------------------------------------------------------------
//! XML reader for IP-XACT design element.
//-----------------------------------------------------------------------------
namespace DesignReader
{

    /*!
     *  Creates a design from XML definition.
     *
     *      @param [in] document    The XML document to create the design from.
     *
     *      @return The created design.
     */
    IPXACTMODELS_EXPORT QSharedPointer<Design> createDesignFrom(QDomDocument const& document);

    namespace Details
    {
        /*!
         *  Reads the component instances from XML to design.
         *
         *      @param [in] designNode  The XML description of the design.
         *      @param [in] newDesign   The new design item.
         */
        void parseComponentInstances(QDomNode const& designNode, QSharedPointer<Design> newDesign);

        /*!
         *  Reads the interconnections from XML to design.
         *
         *      @param [in] designNode  The XML description of the design.
         *      @param [in] newDesign   The new design item.
         */
        void parseInterconnections(QDomNode const& designNode, QSharedPointer<Design> newDesign);

        /*!
         *  Reads a single interconnection from XML.
         *
         *      @param [in] interconnectionNode     The XML description of the interconnection.
         *      @param [in] newDesign               The new design item.
         */
        void parseSingleInterconnection(QDomNode const& interconnectionNode, QSharedPointer<Design> newDesign);

        /*!
         *  Reads an interconnection between components.
         *
         *      @param [in] interconnectionNode     The XML description of the interconnection.
         *      @param [in] newDesign               The new design item.
         */
        void parseComponentInterconnection(QDomNode const& interconnectionNode, QSharedPointer<Design> newDesign);

        /*!
         *  Reads interconnection extensions.
         *
         *      @param [in] interconnectionNode     XML description of the interconnection.
         *      @param [in] interconnection         The new interconnection item.
         */
        void parseInterconnectionExtensions(const QDomNode& interconnectionNode,
            QSharedPointer<Interconnection> interconnection);

        /*!
         *  Reads an active interface.
         *
         *      @param [in] interfaceNode   The XML description of the active interface.
         *      @param [in] newInterface    The new interface item.
         *      @param [in] docRevision     The applied IP-XACT standard revision.
         */
        void parseActiveInterface(QDomNode const& interfaceNode, QSharedPointer<ActiveInterface> newInterface,
            Document::Revision docRevision);

        /*!
         * Reads a component reference for active interface, monitor interface and ad-hoc connection.
         *
         *     @param [in] interfaceNode  <Description>
         *     @param [in] docRevision  <Description>
         *
         *     @return The read component reference.
         */
         QString parseComponentReference(QDomNode const& interfaceNode, Document::Revision docRevision);

        /*!
         *  Reads a hierarchical interface.
         *
         *      @param [in] interfaceNode   The XML description of the hierarchical interface.
         *      @param [in] newInterface    The new interface item.
         */
        void parseHierInterface(QDomNode const& interfaceNode, QSharedPointer<HierInterface> newInterface);

        /*!
         *  Reads interface extensions.
         *
         *      @param [in] interfaceNode   XML description of the hierarchical interface.
         *      @param [in] newInterface    The new interface item.
         */
        void parseHierInterfaceExtensions(const QDomNode& interfaceNode, 
            QSharedPointer<HierInterface> newInterface);

        /*!
         *  Reads a monitor interconnection.
         *
         *      @param [in] monitorNode     The XML description of the monitor interconnection.
         *      @param [in] newDesign       The new design item.
         */
        void parseMonitorInterconnection(QDomNode const& monitorNode, QSharedPointer<Design> newDesign);

        /*!
         *  Reads a monitor interface.
         *
         *      @param [in] interfaceNode   The XML description of the monitor interface.
         *      @param [in] newInterface    The new interface item.
         *      @param [in] docRevision     The applied IP-XACT standard revision.
         */
        void parseMonitorInterface(QDomNode const& interfaceNode, QSharedPointer<MonitorInterface> newInterface,
            Document::Revision docRevision);

        /*!
         *  Reads the ad-hoc connections.
         *
         *      @param [in] designNode  The XML description of the design.
         *      @param [in] newDesign   The new design item.
         */
        void parseAdHocConnections(QDomNode const& designNode, QSharedPointer<Design> newDesign);

        /*!
         *  Reads a single ad-hoc connection.
         *
         *      @param [in] adHocNode   XML description of an ad-hoc connection.
         *      @param [in] newDesign   The new design item.
         */
        void parseSingleAdHocConnection(const QDomNode& adHocNode, QSharedPointer<Design> newDesign);

        /*!
         *  Reads the internal port references.
         *
         *      @param [in] internalNodes       XML description of the internal port references.
         *      @param [in] newAdHocConnection  The new ad-hoc connection item.
         *      @param [in] docRevision         The applied IP-XACT standard revision.
         */
        void parseInternalPortReferences(const QDomNodeList& internalNodes,
            QSharedPointer<AdHocConnection> newAdHocConnection, Document::Revision docRevision);

        /*!
         *  Reads the external port references.
         *
         *      @param [in] externalNodes       XML description of the external port references.
         *      @param [in] newAdHocConnection  The new ad-hoc connection item.
         *      @param [in] docRevision         The applied IP-XACT standard revision.
         */
        void parseExternalPortReferences(const QDomNodeList& externalNodes,
            QSharedPointer<AdHocConnection> newAdHocConnection,
            Document::Revision docRevision);

        /*!
         *  Creates a port reference.
         *
         *      @param [in] portReferenceNode   The XML description of the port.
         *      @param [in] docRevision         The applied IP-XACT standard revision.
         */
        QSharedPointer<PortReference> createPortReference(QDomNode const& portReferenceNode,
            Document::Revision docRevision);

        /*!
         *  Reads ad-hoc connection extensions.
         *
         *      @param [in] adHocNode           XML description of the ad-hoc connection.
         *      @param [in] newAdHocConnection  The new ad-hoc connection item.
         */
        void parseAdHocConnectionExtensions(const QDomNode& adHocNode,
            QSharedPointer<AdHocConnection> newAdHocConnection);

        /*!
         *  Reads the design vendor extensions.
         *
         *      @param [in] documentNode    The XML description of the document.
         *      @param [in] design          The new design item.
         */
        void parseDesignExtensions(QDomNode const& documentNode, QSharedPointer<Design> design);

        /*!
         *  Reads the design column layout (Kactus2 extension).
         *
         *      @param [in] columnNode  The XML description of the column layout.
         *      @param [in] design      The new design item.
         */
        void parseColumnLayout(QDomNode const& columnNode, QSharedPointer<Design> design);

        /*!
         * Reads the route for a connection (Kactus2 extension).
         *
         *     @param [in] routesElement    The XML description of the route.
         *     @param [in] design           The new design item.
         *
         *     @return 
         */
         void parseRoutes(QDomElement const& routesElement, QSharedPointer<Design> design);

        /*!
         *  Reads the software instances (Kactus2 extension).
         *
         *      @param [in] swInstancesNode     The XML description of the software instances.
         *      @param [in] design              The new design item.
         */
        void parseSwInstances(QDomNode const& swInstancesNode, QSharedPointer<Design> design);

        /*!
         *  Reads the a single software instance (Kactus2 extension).
         *
         *      @param [in] QDomNode const& node     The XML description of the software instance.
         *
         *      @return The parsed software instance.
         */
        QSharedPointer<ComponentInstance> parseSwInstance(QDomNode const& node);

        /*!
         *  Reads the ad-hoc port (Kactus2:extension).
         *
         *      @param [in] adHocsNode  The XML description of the ad-hoc ports.
         *      @param [in] design      The new design item.
         */
        void parseAdHocPortPositions(QDomNode const& adHocsNode, QSharedPointer<Design> design);

        /*!
         *  Reads the api connections (Kactus2 extension).
         *
         *      @param [in] apiConnectionsNode  The XML description of the api connections.
         *      @param [in] design              The new design item.
         */
        void parseApiConnections(QDomNode const& apiConnectionsNode, QSharedPointer<Design>design);

        /*!
         *  Reads the com connections (Kactus2 extension).
         *
         *      @param [in] comConnectionsNode  The XML description of the com connections.
         *      @param [in] design              The new design item.
         */
        void parseComConnections(QDomNode const& comConnectionsNode, QSharedPointer<Design> design);

        /*!
         *  Reads the interface graphics extensions for the design.
         *
         *      @param [in] extensionsNode      The XML description of the design vendor extensions.
         *      @param [in] design              The new design item.
         */
        void parseInterfaceGraphics(QDomElement const& extensionsNode, QSharedPointer<Design> design);

        /*!
         *  Reads the sticky notes for the design.
         *
         *      @param [in] extensionNode   The XML description of the design vendor extensions.
         *      @param [in] design          The new design item.
         */
        void parseNotes(QDomElement const& extensionNode, QSharedPointer<Design> design);
    }
};

#endif // DESIGNREADER_H
