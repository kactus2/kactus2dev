//-----------------------------------------------------------------------------
// File: InstantiationsWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Writer class for IP-XACT instantiations element.
//-----------------------------------------------------------------------------

#ifndef INSTANTIATIONSWRITER_H
#define INSTANTIATIONSWRITER_H

#include "DesignInstantiation.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT view element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT InstantiationsWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Owner of this writer.
     */
    InstantiationsWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~InstantiationsWriter();

    /*!
     *  Write a design instantiation to an XML file.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] instantiation   The instantiation to be written.
     */
    void writeDesignInstantiation(QXmlStreamWriter& writer, QSharedPointer<DesignInstantiation> instantiation)
        const;

private:

    //! No copying allowed.
    InstantiationsWriter(InstantiationsWriter const& rhs);
    InstantiationsWriter& operator=(InstantiationsWriter const& rhs);
};

#endif // INSTANTIATIONSWRITER_H