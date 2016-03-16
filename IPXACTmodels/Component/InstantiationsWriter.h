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

#include "ComponentInstantiation.h"
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

    /*!
     *  Writes a component instantiation to an XML file.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] instantiation   The instantiation to be written.
     */
    void writeComponentInstantiation(QXmlStreamWriter& writer,
        QSharedPointer<ComponentInstantiation> instantiation) const;

private:

    //! No copying allowed.
    InstantiationsWriter(InstantiationsWriter const& rhs);
    InstantiationsWriter& operator=(InstantiationsWriter const& rhs);

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

    /*!
     *  Writes the name references of an instantiation.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] instantiation       The containing instantiation.
     */
    void writeNameReferences(QXmlStreamWriter& writer, QSharedPointer<ComponentInstantiation> instantiation) const;

    /*!
     *  Writes the module parameters of an instantiation.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] moduleParameters    The module parameters to be written.
     */
    void writeModuleParameters(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters) const;

    /*!
     *  Writes the default file builders.
     *
     *      @param [in] writer                  The used XML writer.
     *      @param [in] defautlFileBuilders     The file builders to be written.
     */
    void writeDefaultFileBuilders(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<FileBuilder> > > defautlFileBuilders) const;

    /*!
     *  Writes the file set references.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] references  A list of referenced file set names.
     */
    void writeFileSetReferences(QXmlStreamWriter& writer, QSharedPointer<QStringList> references) const;
};

#endif // INSTANTIATIONSWRITER_H