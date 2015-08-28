//-----------------------------------------------------------------------------
// File: DesignReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.08.2015
//
// Description:
// XML reader class for IP-XACT design element.
//-----------------------------------------------------------------------------

#ifndef DESIGNREADER_H
#define DESIGNREADER_H

#include "Design.h"
#include "ActiveInterface.h"
#include "HierInterface.h"
#include "MonitorInterface.h"
#include "PortReference.h"

#include <IPXACTmodels/common/DocumentReader.h>

#include <QObject>
#include <QDomNode>
#include <QDomNodeList>

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT design element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignReader : public DocumentReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent object.
     */
    DesignReader(QObject* parent = 0);

    /*!
	 *  The destructor.
	 */
    ~DesignReader();

    /*!
     *  Creates a design from XML definition.
     *
     *      @param [in] document    The XML document to create the design from.
     *
     *      @return The created design.
     */
    QSharedPointer<Design> createDesignFrom(QDomDocument const& document) const;

private:

    // Disable copying.
    DesignReader(DesignReader const& rhs);
    DesignReader& operator=(DesignReader const& rhs);

    /*!
     *  Reads the component instances from XML to design.
     *
     *      @param [in] designNode  The XML description of the design.
     *      @param [in] newDesign   The new design item.
     */
    void parseComponentInstances(const QDomNode& designNode, QSharedPointer<Design> newDesign) const;

    /*!
     *  Reads the interconnections from XML to design.
     *
     *      @param [in] designNode  The XML description of the design.
     *      @param [in] newDesign   The new design item.
     */
    void parseInterconnections(const QDomNode& designNode, QSharedPointer<Design> newDesign) const;

    /*!
     *  Reads a single interconnection from XML.
     *
     *      @param [in] interconnectionNode     The XML description of the interconnection.
     *      @param [in] newDesign               The new design item.
     */
    void parseSingleInterconnection(const QDomNode& interconnectionNode, QSharedPointer<Design> newDesign) const;

    /*!
     *  Reads an interconnection between components.
     *
     *      @param [in] interconnectionNode     The XML description of the interconnection.
     *      @param [in] newDesign               The new design item.
     *      @param [in] name                    The name of the interconnection.
     *      @param [in] displayName             The display name of the interconnection.
     *      @param [in] description             The description of the interconnection.
     *      @param [in] isPresent               The presence of the interconnection.
     */
    void parseComponentInterconnection(const QDomNode& interconnectionNode, QSharedPointer<Design> newDesign,
        QString const& name, QString const& displayName, QString const& description, QString const& isPresent)
        const;

    /*!
     *  Reads an active interface.
     *
     *      @param [in] interfaceNode   The XML description of the active interface.
     *      @param [in] newInterface    The new interface item.
     */
    void parseActiveInterface(const QDomNode& interfaceNode, QSharedPointer<ActiveInterface> newInterface) const;
    
    /*!
     *  Reads a hierarchical interface.
     *
     *      @param [in] interfaceNode   The XML description of the hierarchical interface.
     *      @param [in] newInterface    The new interface item.
     */
    void parseHierInterface(const QDomNode& interfaceNode, QSharedPointer<HierInterface> newInterface) const;

    /*!
     *  Reads a monitor interconnection.
     *
     *      @param [in] monitorNode     The XML description of the monitor interconnection.
     *      @param [in] newDesign       The new design item.
     *      @param [in] name            The name of the interconnection.
     *      @param [in] displayName     The display name of the interconnection.
     *      @param [in] description     The description of the interconnection.
     *      @param [in] isPresent       The presence of the interconnection.
     */
    void parseMonitorInterconnection(const QDomNode& monitorNode, QSharedPointer<Design> newDesign,
        QString const& name, QString const& displayName, QString const& description, QString const& isPresent)
        const;

    /*!
     *  Reads a monitor interface.
     *
     *      @param [in] interfaceNode   The XML description of the monitor interface.
     *      @param [in] newInterface    The new interface item.
     */
    void parseMonitorInterface(const QDomNode& interfaceNode, QSharedPointer<MonitorInterface> newInterface) const;

    /*!
     *  Reads the ad-hoc connections.
     *
     *      @param [in] designNode  The XML description of the design.
     *      @param [in] newDesign   The new design item.
     */
    void parseAdHocConnections(const QDomNode& designNode, QSharedPointer<Design> newDesign) const;

    /*!
     *  Creates a port reference.
     *
     *      @param [in] portReferenceNode   The XML description of the port.
     */
    QSharedPointer<PortReference> createPortReference(const QDomNode& portReferenceNode) const;
};

#endif // DESIGNREADER_H
