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

#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT component instance element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentInstanceWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Parent of this writer.
     */
    ComponentInstanceWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ComponentInstanceWriter();

    /*!
     *  Write the component instance.
     *
     *      @param [in] writer  The used XML writer.
     *      @param [in] design  The selected component instance.
     */
    void writeComponentInstance(QXmlStreamWriter& writer, QSharedPointer<ComponentInstance> instance) const;

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
};

#endif // COMPONENTINSTANCEWRITER_H
