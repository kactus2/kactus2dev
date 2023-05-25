//-----------------------------------------------------------------------------
// File: ComponentInstanceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.09.2015
//
// Description:
// Writer class for IP-XACT component instance element.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCEWRITER_H
#define COMPONENTINSTANCEWRITER_H

#include "ComponentInstance.h"

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT component instance element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentInstanceWriter : public CommonItemsWriter
{
public:

    /*!
     *  The constructor.
     */
    ComponentInstanceWriter();

    /*!
     *  The destructor.
     */
    ~ComponentInstanceWriter() final = default;

    /*!
     *  Write the component instance.
     *
     *      @param [in] writer  The used XML writer.
     *      @param [in] design  The selected component instance.
     */
    void writeComponentInstance(QXmlStreamWriter& writer, QSharedPointer<ComponentInstance> instance, Document::Revision docRevision) const;

private:

    //! No copying allowed.
    ComponentInstanceWriter(ComponentInstanceWriter const& rhs);
    ComponentInstanceWriter& operator=(ComponentInstanceWriter const& rhs);

    /*!
     *  Write a configurable VLNV.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] VLNVreference   The configurable VLNV.
     *      @param [in] xmlElementName  The name of the XML element.
     */
    void writeConfigurableVLNVReference(QXmlStreamWriter& writer,
        QSharedPointer<ConfigurableVLNVReference> VLNVreference, QString const& xmlElementName) const;

    void writePowerDomainLinks(QXmlStreamWriter& writer, QSharedPointer<ComponentInstance> instance) const;
};

#endif // COMPONENTINSTANCEWRITER_H
