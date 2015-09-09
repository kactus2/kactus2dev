//-----------------------------------------------------------------------------
// File: InstantiationsReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Reader class for IP-XACT instantiations.
//-----------------------------------------------------------------------------

#ifndef INSTANTIATIONSREADER_H
#define INSTANTIATIONSREADER_H

#include "ComponentInstantiation.h"
#include "DesignInstantiation.h"
#include "DesignConfigurationInstantiation.h"

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT instantiations.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT InstantiationsReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    InstantiationsReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~InstantiationsReader();

    /*!
     *  Creates a new design instantiation.
     *
     *      @param [in] instantiationNode   XML description of the instantiation.
     *
     *      @return Pointer to the created design instantiation.
     */
    QSharedPointer<DesignInstantiation> createDesignInstantiationFrom(QDomNode const& instantiationNode) const;

    /*!
     *  Creates a new design configuration instantiation.
     *
     *      @param [in] instantiationNode   XML description of the instantiation.
     *
     *      @return The created design configuration instantiation.
     */
    QSharedPointer<DesignConfigurationInstantiation> createDesignConfigurationInstantiationFrom
        (QDomNode const& instantiationNode) const;

    /*!
     *  Creates a new component instantiation.
     *
     *      @param [in] instantiationNode   XML description of the instantiation.
     *
     *      @return The created component instantiation.
     */
    QSharedPointer<ComponentInstantiation> createComponentInstantiationFrom(QDomNode const& instantiationNode)
        const;

private:

    //! No copying allowed.
    InstantiationsReader(InstantiationsReader const& rhs);
    InstantiationsReader& operator=(InstantiationsReader const& rhs);

    /*!
     *  Get the used HDL from XML.
     *
     *      @param [in] instantiationNode   XML description of the instantiation.
     *
     *      @return The used HDL.
     */
    QString parseLanguageFrom(QDomNode const& instantiationNode) const;

    /*!
     *  Get the strictness of the used HDL from XML.
     *
     *      @param [in] instantiationNode   XML description of the instantiation.
     *
     *      @return True, if the language shall be strictly enforced, false otherwise.
     */
    bool parseLanguageStrictnessFrom(QDomNode const& instantiationNode) const;

    /*!
     *  Reads the name references from XML.
     *
     *      @param [in] instantiationNode   XML description of the instantiation.
     *      @param [in] instantiation       The selected instantiation.
     */
    void parseNameReferences(QDomNode const& instantiationNode,
        QSharedPointer<ComponentInstantiation> instantiation) const;

    /*!
     *  Reads the module parameter from XML.
     *
     *      @param [in] instantiationNode   XML description of the instantiation.
     *      @param [in] instantiation       The selected instantiation.
     */
    void parseModuleParameters(QDomNode const& instantiationNode,
        QSharedPointer<ComponentInstantiation> instantiation) const;

    /*!
     *  Reads the default file builders from XML.
     *
     *      @param [in] instantiationNode   XML description of the instantiation.
     *      @param [in] instantiation       The selected instantiation.
     */
    void parseDefaultFileBuilders(QDomNode const& instantiationNode,
        QSharedPointer<ComponentInstantiation> instantiation) const;

    /*!
     *  Reads the file set references from XLM.
     *
     *      @param [in] instantiationNode   XML description of the instantiation.
     *      @param [in] instantiation       The selected instantiation.
     */
    void parseFileSetReferences(QDomNode const& instantiationNode,
        QSharedPointer<ComponentInstantiation> instantiation) const;
};

#endif // VIEWREADER_H