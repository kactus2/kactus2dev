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
     */
    QSharedPointer<DesignConfigurationInstantiation> createDesignConfigurationInstantiationFrom
        (QDomNode const& instantiationNode) const;

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
    QString getLanguageFrom(QDomNode const& instantiationNode) const;

    /*!
     *  Get the strictness of the used HDL from XML.
     *
     *      @param [in] instantiationNode   XML description of the instantiation.
     *
     *      @return True, if the language shall be strictly enforced, false otherwise.
     */
    bool getLanguageStrictnessFrom(QDomNode const& instantiationNode) const;
};

#endif // VIEWREADER_H