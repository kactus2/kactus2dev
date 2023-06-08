//-----------------------------------------------------------------------------
// File: ComponentInstanceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.09.2015
//
// Description:
// Writer for IP-XACT component instance element.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCEWRITER_H
#define COMPONENTINSTANCEWRITER_H

#include "ComponentInstance.h"

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer for IP-XACT component instance element.
//-----------------------------------------------------------------------------
namespace  ComponentInstanceWriter
{
    /*!
     *  Write the component instance.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] design      The selected component instance.
     *      @param [in] docRevision The IP-XACT standard revision to apply.
     */
    IPXACTMODELS_EXPORT void writeComponentInstance(QXmlStreamWriter& writer, 
        QSharedPointer<ComponentInstance> instance, Document::Revision docRevision);

    namespace Details
    {
        /*!
         *  Write a configurable VLNV.
         *
         *      @param [in] writer          The used XML writer.
         *      @param [in] VLNVreference   The configurable VLNV.
         *      @param [in] xmlElementName  The name of the XML element.
         */
        void writeConfigurableVLNVReference(QXmlStreamWriter& writer,
            QSharedPointer<ConfigurableVLNVReference> VLNVreference, QString const& xmlElementName);

        /*!
         * Write the power domain links of the component instance
         *
         *     @param [in] writer       The used XML writer.
         *     @param [in] instance     The instance whose power domain links to write.
         */
         void writePowerDomainLinks(QXmlStreamWriter& writer, QSharedPointer<ComponentInstance> instance);
    }
};

#endif // COMPONENTINSTANCEWRITER_H
