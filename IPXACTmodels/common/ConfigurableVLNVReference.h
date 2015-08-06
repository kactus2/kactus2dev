//-----------------------------------------------------------------------------
// File: ConfigurableVLNVReference.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.08.2015
//
// Description:
// Configurable VLNV reference matching ipxact type configurableLibraryReference.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLEVLNVREFERENCE_H
#define CONFIGURABLEVLNVREFERENCE_H

#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>

#include <QDomNode>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Configurable VLNV reference matching ipxact type configurableLibraryReference.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ConfigurableVLNVReference : public VLNV
{

public:

    /*!
     *  The constructor.
     */
    ConfigurableVLNVReference();

    /*!
     *  The copy constructor.
     */
    ConfigurableVLNVReference(const ConfigurableVLNVReference& other);

    /*!
     *  The constructor.
     *
     *      @param [in] vlnvReferenceNode   A reference to the node to parse from.
     *      @param [in] type                The ipxact type.
     */
    ConfigurableVLNVReference(const QDomNode& vlnvReferenceNode, IPXactType type);

    /*!
     *  The constructor.
     *
     *      @param [in] type        The ipxact type.
     *      @param [in] vendor      The vendor.
     *      @param [in] library     The library.
     *      @param [in] name        The name.
     *      @param [in] version     The version.
     */
    ConfigurableVLNVReference(const IPXactType& type, const QString& vendor, const QString& library,
        const QString& name, const QString& version);

    /*!
     *  The destructor.
     */
    ~ConfigurableVLNVReference();

    /*!
     *  Get the list of configurable element values.
     *
     *      @return A pointer to the list of configurable element values.
     */
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > getConfigurableElementValues();

    /*!
     *  Set the configurable element values.
     *
     *      @param [in] newConfigurableElementValues    The new list of configurable element values.
     */
    void setConfigurableElementValues(
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > newConfigurableElementValues);

private:

    //! A list of configurable element values
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElementValues_;
};

#endif // CONFIGURABLEVLNVREFERENCE_H
