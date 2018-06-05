//-----------------------------------------------------------------------------
// File: ViewConfiguration.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.08.2015
//
// Description:
// Matches the ipxact:viewConfiguration element.
//-----------------------------------------------------------------------------

#ifndef VIEWCONFIGURATION_H
#define VIEWCONFIGURATION_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/ConfigurableElementValue.h>

#include <QString>
#include <QMap>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Matches the ipxact:abstractorInstances element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ViewConfiguration
{
public:

    /*!
     *  The constructor.
     */
    ViewConfiguration();

    /*!
     *  The constructor.
     *
     *      @param [in] instanceName    The name of this instance.
     */
    ViewConfiguration(QString const& instanceName);

    /*!
     *  Copy constructor.
     */
    ViewConfiguration(const ViewConfiguration& other);

    /*!
     *  The destructor.
     */
    ~ViewConfiguration();

    /*!
     *  Get the name of this instance.
     *
     *      @return The name of this instance.
     */
    QString getInstanceName() const;

    /*!
     *  Set the name for this instance.
     *
     *      @param [in] newInstanceName     The new name for this instance.
     */
    void setInstanceName(QString const& newInstanceName);

    /*!
     *  Get the value of isPresent.
     *
     *      @return The value of isPresent.
     */
    QString getIsPresent() const;

    /*!
     *  Set the value for isPresent.
     *
     *      @param [in] newIsPresent    The new value for isPresent.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
     *  Get the referenced view.
     *
     *      @return The referenced view.
     */
    QString getViewReference() const;

    /*!
     *  Set the referenced view.
     *
     *      @param [in] newViewReference    The new view reference.
     */
    void setViewReference(QString const& newViewReference);

    /*!
     *  Get the configurable element values of the referenced view.
     *
     *      @return The configurable element values of the referenced view.
     */
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > getViewConfigurableElements() const;

    /*!
     *  Set the configurable element values of the referenced view.
     *
     *      @param [in] newConfigurableElements     The new configurable element values for the referenced view.
     */
    void setViewConfigurableElements(
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > newConfigurableElements);

private:

    //! The name of this instance.
    QString instanceName_;

    //! Defines the presence of this element.
    QString isPresent_;

    //! The referenced view.
    QString viewRef_;

    //! A list of pointers to the configurable element values of the referenced view.
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > viewConfigurables_;
};

#endif // VIEWCONFIGURATION_H
