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
#include "DesignConfigurationInstantiation.h"

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

    /*!
     *  Write a design configuration instantiation to an XML file.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] instantiation   The instantiation to be written.
     */
    void writeDesignConfigurationInstantiation(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfigurationInstantiation> instantiation) const;

private:

    /*!
     *  Write a reference to an IP-XACT item.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] reference       The referenced item.
     *      @param [in] elementName     The name of the reference containing element.
     */
    void writeReference(QXmlStreamWriter& writer, QSharedPointer<ConfigurableVLNVReference> reference,
        QString const& elementName) const;

    /*!
     *  Write the language.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] language            The language to be written.
     *      @param [in] languageStrictness  Strictness of the language.
     */
    void writeLanguage(QXmlStreamWriter& writer, QString const& language, bool languageStrictness) const;

    //! No copying allowed.
    InstantiationsWriter(InstantiationsWriter const& rhs);
    InstantiationsWriter& operator=(InstantiationsWriter const& rhs);
};

#endif // INSTANTIATIONSWRITER_H