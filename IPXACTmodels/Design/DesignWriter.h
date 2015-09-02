//-----------------------------------------------------------------------------
// File: DesignWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.08.2015
//
// Description:
// Writer class for IP-XACT Design element.
//-----------------------------------------------------------------------------

#ifndef DESIGNWRITER_H
#define DESIGNWRITER_H

#include "Design.h"

#include <IPXACTmodels/common/DocumentWriter.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Design element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignWriter : public DocumentWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Parent of this writer.
     */
    DesignWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~DesignWriter();

    /*!
     *  Write the design.
     *
     *      @param [in] writer  The used XML writer.
     *      @param [in] design  The selected design.
     */
    void writeDesign(QXmlStreamWriter& writer, QSharedPointer<Design> design) const;

private:

    //! No copying allowed.
    DesignWriter(DesignWriter const& rhs);
    DesignWriter& operator=(DesignWriter const& rhs);

    /*!
     *  Write the start of the XML file.
     *
     *      @param [in] writer  The used XML writer.
     *      @param [in] design  The selected design.
     */
    void writeDesignStart(QXmlStreamWriter& writer, QSharedPointer<Design> design) const;

    /*!
     *  Write the component instances of the design.
     *
     *      @param [in] writer  The used XML writer.
     *      @param [in] design  The selected design.
     */
    void writeComponentInstances(QXmlStreamWriter& writer, QSharedPointer<Design> design) const;

    /*!
     *  Write the design interconnections.
     *
     *      @param [in] writer  The used XML writer.
     *      @param [in] design  The selected design.
     */
    void writeInterconnections(QXmlStreamWriter& writer, QSharedPointer<Design> design) const;

    /*!
     *  Write a single interconnection.
     *
     *      @param [in] writer                  The used XML writer.
     *      @param [in] currentInterconnection  The selected interconnection.
     */
    void writeSingleInterconncetion(QXmlStreamWriter& writer,
        QSharedPointer<Interconnection> currentInterconnection) const;

    /*!
     *  Write an active interface.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] activeInterface     The selected active interface.
     */
    void writeActiveInterface(QXmlStreamWriter& writer, QSharedPointer<ActiveInterface> activeInterface) const;

    /*!
     *  Write a hier interface.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] hierInterface   The selected hier interface.
     */
    void writeHierInterface(QXmlStreamWriter& writer, QSharedPointer<HierInterface> hierInterface) const;

    /*!
     *  Write a monitor interconnection.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] monitorConnection   The selected monitor interconnection.
     */
    void writeMonitorInterconnection(QXmlStreamWriter& writer,
        QSharedPointer<MonitorInterconnection> monitorConnection) const;

    /*!
     *  Write a monitor interface.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] monitorInterface    The selected monitor interface.
     */
    void writeMonitorInterface(QXmlStreamWriter& writer, QSharedPointer<MonitorInterface> monitorInterface) const;

    /*!
     *  Write ad-hoc connections.
     *
     *      @param [in] writer  The used XML writer.
     *      @param [in] design  The selected design.
     */
    void writeAdHocConnections(QXmlStreamWriter& writer, QSharedPointer<Design> design) const;

    /*!
     *  Write a single ad-hoc connection.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] adHocConnection     The selected ad-hoc connection.
     */
    void writeSingleAdHocConnection(QXmlStreamWriter& writer, QSharedPointer<AdHocConnection> adHocConnection) const;

    /*!
     *  Write ad-hoc port references.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] adHocConnection     The selected ad-hoc connection.
     */
    void writeAdHocPortReferences(QXmlStreamWriter& writer, QSharedPointer<AdHocConnection> adHocConnection) const;

    /*!
     *  Write a port reference.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] portRefernce    The selected port reference.
     */
    void writePortReference(QXmlStreamWriter& writer, QSharedPointer<PortReference> portRefernce) const;

    /*!
     *  Write a part select.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] partSelect  The selected part select.
     */
    void writePartSelect(QXmlStreamWriter& writer, QSharedPointer<PartSelect> partSelect) const;
};

#endif // DESIGNWRITER_H
