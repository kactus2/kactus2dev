//-----------------------------------------------------------------------------
// File: AdHocEnabled.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.2.2012
//
// Description:
// Base class for adding ad-hoc support to a derived class.
//-----------------------------------------------------------------------------

#ifndef ADHOCENABLED_H
#define ADHOCENABLED_H

#include <QString>
#include <QSharedPointer>
#include <QMap>

class Component;
class Port;
class DiagramConnectionEndpoint;
class GenericEditProvider;
class AdHocEditor;

//-----------------------------------------------------------------------------
//! Base class for adding ad-hoc support to a derived class.
//-----------------------------------------------------------------------------
class AdHocEnabled
{
public:
    /*!
     *  Constructor.
     */
    AdHocEnabled();

    /*!
     *  Sets the ad-hoc data based on the given component and saved port ad-hoc visibilities.
     *
     *      @param [in] component              The component whose ports are considered.
     *      @param [in] portAdHocVisibilities  The saved ad-hoc visibilities for the ports.
     */
    void setAdHocData(QSharedPointer<Component> component,
                      QMap<QString, bool> const& portAdHocVisibilities = QMap<QString, bool>());

    /*!
     *  Sets the ad-hoc visibility of the given port.
     *
     *      @param [in] portName The name of the port.
     *      @param [in] visible  If true, the port is set visible for ad-hoc.
     */
    void setPortAdHocVisible(QString const& portName, bool visible);

    /*!
     *  Returns true if the given port is currently visible for ad-hoc.
     *
     *      @param [in] portName The name of the port.
     */
    bool isPortAdHocVisible(QString const& portName) const;

    /*!
     *  Returns the ad-hoc visibility map.
     */
    QMap<QString, bool> const& getPortAdHocVisibilities() const;

    /*!
     *  Returns the list of ports.
     */
    QList< QSharedPointer<Port> > getPorts() const;

    /*!
     *  Called when a port's ad-hoc visibility has been changed.
     *
     *      @param [in] portName  The name of the port.
     *      @param [in] visible   The new ad-hoc visibility.
     */
    virtual void onAdHocVisibilityChanged(QString const& portName, bool visible);

    /*!
     *  Attaches the data source to an ad-hoc editor.
     */
    virtual void attach(AdHocEditor* editor) = 0;

    /*!
     *  Detaches the data source from the ad-hoc editor.
     */
    virtual void detach(AdHocEditor* editor) = 0;

    /*!
     *  Returns true if the ad-hoc visibilities are protected.
     */
    virtual bool isProtected() const = 0;

    /*!
     *  Returns the edit provider for the data.
     */
    virtual GenericEditProvider& getEditProvider() = 0;

    /*!
     *  Returns the ad-hoc port with the given name or null if not found.
     */
    virtual DiagramConnectionEndpoint* getDiagramAdHocPort(QString const& portName);

private:
    // Disable copying.
    AdHocEnabled(AdHocEnabled const& rhs);
    AdHocEnabled& operator=(AdHocEnabled const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component whose ports to consider.
    QSharedPointer<Component> component_;

    //! The ad-hoc visibility flags for ports.
    QMap<QString, bool> portAdHocVisibilities_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCENABLED_H
