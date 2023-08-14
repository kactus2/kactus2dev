//-----------------------------------------------------------------------------
// File: DesignWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.08.2015
//
// Description:
// Writer for IP-XACT Design element.
//-----------------------------------------------------------------------------

#ifndef DESIGNWRITER_H
#define DESIGNWRITER_H

#include "Design.h"

#include <IPXACTmodels/common/DocumentWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer for IP-XACT Design element.
//-----------------------------------------------------------------------------
namespace DesignWriter 
{
    /*!
     *  Write the design.
     *
     *      @param [in] writer  The used XML writer.
     *      @param [in] design  The selected design.
     */
    IPXACTMODELS_EXPORT void writeDesign(QXmlStreamWriter& writer, QSharedPointer<Design> design);

    namespace Details
    {
        /*!
         *  Write the start of the XML file.
         *
         *      @param [in] writer  The used XML writer.
         *      @param [in] design  The selected design.
         */
        void writeDesignStart(QXmlStreamWriter& writer, QSharedPointer<Design> design);

        /*!
         *  Write the component instances of the design.
         *
         *      @param [in] writer  The used XML writer.
         *      @param [in] design  The selected design.
         */
        void writeComponentInstances(QXmlStreamWriter& writer, QSharedPointer<Design> design);

        /*!
         *  Write the design interconnections.
         *
         *      @param [in] writer  The used XML writer.
         *      @param [in] design  The selected design.
         */
        void writeInterconnections(QXmlStreamWriter& writer, QSharedPointer<Design> design);

        /*!
         *  Write a single interconnection.
         *
         *      @param [in] writer                  The used XML writer.
         *      @param [in] currentInterconnection  The selected interconnection.
         *      @param [in] docRevision             The applied IP-XACT standard revision.
         */
        void writeSingleInterconncetion(QXmlStreamWriter& writer,
            QSharedPointer<Interconnection> currentInterconnection, Document::Revision docRevision);

        /*!
         *  Write an active interface.
         *
         *      @param [in] writer              The used XML writer.
         *      @param [in] activeInterface     The selected active interface.
         *      @param [in] docRevision             The applied IP-XACT standard revision.
         */
        void writeActiveInterface(QXmlStreamWriter& writer, QSharedPointer<ActiveInterface> activeInterface,
            Document::Revision docRevision);

        QString componentReferenceAttribute(Document::Revision docRevision);

        /*!
         *  Write a hier interface.
         *
         *      @param [in] writer          The used XML writer.
         *      @param [in] hierInterface   The selected hier interface.
         */
        void writeHierInterface(QXmlStreamWriter& writer, QSharedPointer<HierInterface> hierInterface);

        /*!
         *  Write a monitor interconnection.
         *
         *      @param [in] writer              The used XML writer.
         *      @param [in] monitorConnection   The selected monitor interconnection.
         *      @param [in] docRevision             The applied IP-XACT standard revision.
         */
        void writeMonitorInterconnection(QXmlStreamWriter& writer,
            QSharedPointer<MonitorInterconnection> monitorConnection, Document::Revision docRevision);

        /*!
         *  Write a monitor interface.
         *
         *      @param [in] writer              The used XML writer.
         *      @param [in] monitorInterface    The selected monitor interface.
         *      @param [in] docRevision             The applied IP-XACT standard revision.
         */
        void writeMonitorInterface(QXmlStreamWriter& writer, QSharedPointer<MonitorInterface> monitorInterface,
            Document::Revision docRevision);

        /*!
         *  Write ad-hoc connections.
         *
         *      @param [in] writer  The used XML writer.
         *      @param [in] design  The selected design.
         */
        void writeAdHocConnections(QXmlStreamWriter& writer, QSharedPointer<Design> design);

        /*!
         *  Write a single ad-hoc connection.
         *
         *      @param [in] writer              The used XML writer.
         *      @param [in] adHocConnection     The selected ad-hoc connection.
         *      @param [in] docRevision         The applied IP-XACT standard revision.
         */
        void writeSingleAdHocConnection(QXmlStreamWriter& writer, QSharedPointer<AdHocConnection> adHocConnection,
            Document::Revision docRevision);

        /*!
         *  Write ad-hoc port references.
         *
         *      @param [in] writer              The used XML writer.
         *      @param [in] adHocConnection     The selected ad-hoc connection.
         *      @param [in] docRevision         The applied IP-XACT standard revision.
         */
        void writeAdHocPortReferences(QXmlStreamWriter& writer, QSharedPointer<AdHocConnection> adHocConnection,
            Document::Revision docRevision);

        /*!
         *  Write a port reference.
         *
         *      @param [in] writer          The used XML writer.
         *      @param [in] portRefernce    The selected port reference.
         *      @param [in] docRevision     The applied IP-XACT standard revision.
         */
        void writePortReference(QXmlStreamWriter& writer, QSharedPointer<PortReference> portRefernce,
            Document::Revision docRevision);

    }
};

#endif // DESIGNWRITER_H
