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

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>

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
     *      @param [in] type        The ipxact type.
     *      @param [in] vendor      The vendor.
     *      @param [in] library     The library.
     *      @param [in] name        The name.
     *      @param [in] version     The version.
     */
    ConfigurableVLNVReference(const IPXactType& type, const QString& vendor, const QString& library,
        const QString& name, const QString& version);

    /*!
     *  Constructor with VLNV information.
     *
     *      @param [in] configurableVLNV    The configurable VLNV.
     */
    ConfigurableVLNVReference(const VLNV& configurableVLNV);

    /*!
     *  The destructor.
     */
    ~ConfigurableVLNVReference();

    /*!
     *  Sets the VLNV reference.
     *
     *      @param [in] vlnv   The VLNV to set.
     */
    void setVLNV(VLNV const& vlnv);

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

    /*!
     *  Check if the configurable vlnv contains the given configurable element.
     *
     *      @param [in] referenceId     The ID of the configurable element.
     *
     *      @return True, if the configurable element is found, false otherwise.
     */
    bool hasConfigurableElementValue(QString const& referenceId) const;

    /*!
     *  Get a single configurable element value.
     *
     *      @param [in] referenceId     The id of the configurable element value.
     *
     *      @return The value of the configurable element.
     */
    QString getSingleConfigurableElementValue(QString const& referenceId) const;
    
private:

    //! A list of configurable element values
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElementValues_;
};

#endif // CONFIGURABLEVLNVREFERENCE_H
