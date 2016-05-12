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

#include <IPXACTmodels/Component/Component.h>

#include <QString>
#include <QSharedPointer>
#include <QMap>

class Port;
class HWConnectionEndpoint;
class GenericEditProvider;
class AdHocVisibilityEditor;
class AdHocItem;

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
    QMap<QString, bool> getPortAdHocVisibilities() const;

    /*!
     *  Returns the list of ports.
     */
    QSharedPointer<QList<QSharedPointer<Port> > > getPorts() const;

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
    virtual void attach(AdHocVisibilityEditor* editor) = 0;

    /*!
     *  Detaches the data source from the ad-hoc editor.
     */
    virtual void detach(AdHocVisibilityEditor* editor) = 0;

    /*!
     *  Get the name of the ad hoc enabled item.
     *
     *      @return The name of the ad hoc enabled item.
     */
    virtual QString adHocIdentifier() const = 0;

    /*!
     *  Returns true if the ad-hoc visibilities are protected.
     */
    //virtual bool isProtected() const = 0;

    /*!
     *  Returns the ad-hoc port with the given name or null if not found.
     */
    virtual HWConnectionEndpoint* getDiagramAdHocPort(QString const& portName);

    /*!
     *  Create an ad hoc port item with the selected name.
     *
     *      @param [in] portName    The selected port name.
     *
     *      @return 0. Child items should create their own items where desired.
     */
    virtual AdHocItem* createAdhocItem(QString const& portName);

    /*!
     *  Change the visibility of an ad hoc port item.
     *
     *      @param [in] portItem        The selected port item.
     *      @param [in] newVisibility   The new visibility of the port item.
     */
    void changeAdhocVisibility(AdHocItem* portItem, bool newVisibility);

private:
    // Disable copying.
    AdHocEnabled(AdHocEnabled const& rhs);
    AdHocEnabled& operator=(AdHocEnabled const& rhs);

    /*!
     *  Show an ad hoc port item.
     *
     *      @param [in] portItem    The selected item.
     */
    virtual void showAdhocPort(AdHocItem* portItem);

    /*!
     *  Hide an ad hoc port item.
     *
     *      @param [in] portItem    The selected item.
     */
    virtual void hideAdhocPort(AdHocItem* portItem);

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
