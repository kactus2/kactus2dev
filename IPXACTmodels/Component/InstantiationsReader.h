//-----------------------------------------------------------------------------
// File: InstantiationsReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Reader for IP-XACT instantiations.
//-----------------------------------------------------------------------------

#ifndef INSTANTIATIONSREADER_H
#define INSTANTIATIONSREADER_H

#include "ComponentInstantiation.h"
#include "DesignInstantiation.h"
#include "DesignConfigurationInstantiation.h"

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader for IP-XACT instantiations.
//-----------------------------------------------------------------------------
namespace InstantiationsReader
{
    /*!
     *  Creates a new design instantiation.
     *
     *    @param [in] instantiationNode   XML description of the instantiation.
     *
     *    @return Pointer to the created design instantiation.
     */
    IPXACTMODELS_EXPORT QSharedPointer<DesignInstantiation> createDesignInstantiationFrom(QDomNode const& instantiationNode);

    /*!
     *  Creates a new design configuration instantiation.
     *
     *    @param [in] instantiationNode   XML description of the instantiation.
     *    @param [in] docRevision         The IP-XACT standard revision to comply to.
     *
     *    @return The created design configuration instantiation.
     */
    IPXACTMODELS_EXPORT QSharedPointer<DesignConfigurationInstantiation> createDesignConfigurationInstantiationFrom
        (QDomNode const& instantiationNode, Document::Revision docRevision);

    /*!
     *  Creates a new component instantiation.
     *
     *    @param [in] instantiationNode   XML description of the instantiation.
     *    @param [in] docRevision         The IP-XACT standard to comply to.
     * 
     *    @return The created component instantiation.
     */
    IPXACTMODELS_EXPORT QSharedPointer<ComponentInstantiation> createComponentInstantiationFrom(QDomNode const& instantiationNode,
        Document::Revision docRevision);
    
    namespace Details
    {
        /*!
         *  Get the used HDL from XML.
         *
         *    @param [in] instantiationNode   XML description of the instantiation.
         *
         *    @return The used HDL.
         */
        QString parseLanguageFrom(QDomNode const& instantiationNode);

        /*!
         *  Get the strictness of the used HDL from XML.
         *
         *    @param [in] instantiationNode   XML description of the instantiation.
         *
         *    @return True, if the language shall be strictly enforced, false otherwise.
         */
        bool parseLanguageStrictnessFrom(QDomNode const& instantiationNode);

        /*!
         *  Reads the name references from XML.
         *
         *    @param [in] instantiationNode   XML description of the instantiation.
         *    @param [in] instantiation       The selected instantiation.
         */
        void parseNameReferences(QDomNode const& instantiationNode,
            QSharedPointer<ComponentInstantiation> instantiation);

        /*!
         *  Reads the module parameter from XML.
         *
         *    @param [in] instantiationNode   XML description of the instantiation.
         *    @param [in] instantiation       The selected instantiation.
         *    @param [in] docRevision         The IP-XACT standard revision to comply to.
         */
        void parseModuleParameters(QDomNode const& instantiationNode,
            QSharedPointer<ComponentInstantiation> instantiation, Document::Revision docRevision);

        /*!
         *  Reads the default file builders from XML.
         *
         *    @param [in] instantiationNode   XML description of the instantiation.
         *    @param [in] instantiation       The selected instantiation.
         *    @param [in] docRevision         The IP-XACT standard revision to comply to.
         */
        void parseDefaultFileBuilders(QDomNode const& instantiationNode,
            QSharedPointer<ComponentInstantiation> instantiation,
            Document::Revision docRevision);

        /*!
         *  Reads the file set references from XLM.
         *
         *    @param [in] instantiationNode   XML description of the instantiation.
         *    @param [in] instantiation       The selected instantiation.
         *    @param [in] docRevision         The IP-XACT standard revision to comply to.
         */
        void parseFileSetReferences(QDomNode const& instantiationNode,
            QSharedPointer<ComponentInstantiation> instantiation,
            Document::Revision docRevision);
    }
};

#endif // VIEWREADER_H