//-----------------------------------------------------------------------------
// File: InstantiationsWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Writer for IP-XACT instantiations element.
//-----------------------------------------------------------------------------

#ifndef INSTANTIATIONSWRITER_H
#define INSTANTIATIONSWRITER_H

#include "ComponentInstantiation.h"
#include "DesignInstantiation.h"
#include "DesignConfigurationInstantiation.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer for IP-XACT view element.
//-----------------------------------------------------------------------------
namespace InstantiationsWriter
{
    /*!
     *  Write a design instantiation to an XML file.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] instantiation   The instantiation to be written.
     *      @param [in] docRevision     The IP-XACT standard revision in use.
     */
    IPXACTMODELS_EXPORT void writeDesignInstantiation(QXmlStreamWriter& writer, 
        QSharedPointer<DesignInstantiation> instantiation, Document::Revision docRevision);

    /*!
     *  Write a design configuration instantiation to an XML file.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] instantiation   The instantiation to be written.
     *      @param [in] docRevision     The IP-XACT standard revision in use.
     */
    IPXACTMODELS_EXPORT void writeDesignConfigurationInstantiation(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfigurationInstantiation> instantiation, Document::Revision docRevision);

    /*!
     *  Writes a component instantiation to an XML file.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] instantiation   The instantiation to be written.
     *      @param [in] docRevision     The IP-XACT standard revision in use.
     */
    IPXACTMODELS_EXPORT void writeComponentInstantiation(QXmlStreamWriter& writer,
        QSharedPointer<ComponentInstantiation> instantiation, Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Write a reference to an IP-XACT item.
         *
         *      @param [in] writer          The used XML writer.
         *      @param [in] reference       The referenced item.
         *      @param [in] elementName     The name of the reference containing element.
         */
        void writeReference(QXmlStreamWriter& writer, QSharedPointer<ConfigurableVLNVReference> reference,
            QString const& elementName);

        /*!
         *  Write the language.
         *
         *      @param [in] writer              The used XML writer.
         *      @param [in] language            The language to be written.
         *      @param [in] languageStrictness  Strictness of the language.
         */
        void writeLanguage(QXmlStreamWriter& writer, QString const& language, bool languageStrictness);

        /*!
         *  Writes the name references of an instantiation.
         *
         *      @param [in] writer              The used XML writer.
         *      @param [in] instantiation       The containing instantiation.
         */
        void writeNameReferences(QXmlStreamWriter& writer, QSharedPointer<ComponentInstantiation> instantiation);

        /*!
         *  Writes the module parameters of an instantiation.
         *
         *      @param [in] writer              The used XML writer.
         *      @param [in] moduleParameters    The module parameters to be written.
         *      @param [in] docRevision         The IP-XACT standard revision in use.
         */
        void writeModuleParameters(QXmlStreamWriter& writer,
            QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters,
            Document::Revision docRevision);

        /*!
         *  Writes the default file builders.
         *
         *      @param [in] writer                  The used XML writer.
         *      @param [in] defautlFileBuilders     The file builders to be written.
         */
        void writeDefaultFileBuilders(QXmlStreamWriter& writer,
            QSharedPointer<QList<QSharedPointer<FileBuilder> > > defautlFileBuilders,
            Document::Revision docRevision);

        /*!
         *  Writes the file set references.
         *
         *      @param [in] writer          The used XML writer.
         *      @param [in] instantiation   The instantiation whose file set refs are written.
         *      @param [in] docRevision     The IP-XACT standard revision in use.
         */
        void writeFileSetReferences(QXmlStreamWriter& writer, QSharedPointer<ComponentInstantiation> instantiation, Document::Revision docRevision);
    }
};

#endif // INSTANTIATIONSWRITER_H