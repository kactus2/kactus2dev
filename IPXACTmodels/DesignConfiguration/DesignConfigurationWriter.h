//-----------------------------------------------------------------------------
// File: DesignConfigurationWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.08.2015
//
// Description:
// Writer class for IP-XACT Design configuration element.
//-----------------------------------------------------------------------------

#ifndef DESIGNCONFIGURATIONWRITER_H
#define DESIGNCONFIGURATIONWRITER_H

#include "DesignConfiguration.h"

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/DocumentWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Design configuration element.
//-----------------------------------------------------------------------------
namespace DesignConfigurationWriter
{


    /*!
     *  Writes the given design configuration into xml.
     *
     *    @param [in] writer                  The xml writer to use.
     *    @param [in] designConfiguration     The selected design configuration.
     */
    IPXACTMODELS_EXPORT void writeDesignConfiguration(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration);

    namespace Details
    {
        /*!
         *  Write the document start elements.
         *
         *    @param [in] writer                  The used xml writer.
         *    @param [in] designConfiguration     The selected design configuration.
         */
        void writeDesignConfigurationStart(QXmlStreamWriter& writer,
            QSharedPointer<DesignConfiguration> designConfiguration);

        /*!
         *  Write the VLNV of the design configuration's referenced design.
         *
         *    @param [in] writer              The used xml writer.
         *    @param [in] designReference     The VLNV of the referenced design.
         */
        void writeDesignReference(QXmlStreamWriter& writer, VLNV const& designReference);

        /*!
         *  Write generator chain configuration.
         *
         *    @param [in] writer                  The used xml writer.
         *    @param [in] designConfiguration     The selected design configuration.
         */
        void writeGeneratorChainConfiguration(QXmlStreamWriter& writer,
            QSharedPointer<DesignConfiguration> designConfiguration);

        /*!
         *  Write the interconnection configurations.
         *
         *    @param [in] writer                  The used xml writer.
         *    @param [in] designConfiguration     The selected design configuration.
         */
        void writeInterConnectionConfiguration(QXmlStreamWriter& writer,
            QSharedPointer<DesignConfiguration> designConfiguration);

        /*!
         *  Write the multiple abstractor instances.
         *
         *    @param [in] writer                  The used xml writer.
         *    @param [in] multipleAbstractors     A list of multipleAbastractors.
         */
        void writeMultipleAbstractorInstances(QXmlStreamWriter& writer,
            QSharedPointer<QList<QSharedPointer<MultipleAbstractorInstances> > > multipleAbstractors,
            Document::Revision docRevision);

        /*!
         *  Write the interface references.
         *
         *    @param [in] writer                  The used xml writer.
         *    @param [in] interfaceReferences     A list of interface references.
         */
        void writeInterfaceReferences(QXmlStreamWriter& writer,
            QSharedPointer<QList<QSharedPointer<InterfaceRef> > > interfaceReferences,
            Document::Revision docRevision);

        /*!
         *  Write the abstractor instances.
         *
         *    @param [in] writer                  The used xml writer.
         *    @param [in] abstractorInstances     A list of abstractor instances.
         */
        void writeAbstractorInstances(QXmlStreamWriter& writer,
            QSharedPointer<QList<QSharedPointer<AbstractorInstance> > > abstractorInstances,
            Document::Revision docRevision);

        /*!
         *  Write the view configurations.
         *
         *    @param [in] writer                  The used xml writer.
         *    @param [in] designConfiguration     The selected design configuration.
         */
        void writeViewConfigurations(QXmlStreamWriter& writer,
            QSharedPointer<DesignConfiguration> designConfiguration);
    };
};

#endif // DESIGNCONFIGURATIONWRITER_H
