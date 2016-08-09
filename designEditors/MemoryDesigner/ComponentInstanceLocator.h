//-----------------------------------------------------------------------------
// File: ComponentInstanceLocator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.07.2016
//
// Description:
// Locator for component instances within a selected design.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCELOCATOR_H
#define COMPONENTINSTANCELOCATOR_H

#include <IPXACTmodels/common/VLNV.h>

class LibraryInterface;
class ConfigurableVLNVReference;
class Design;
class DesignConfiguration;
class ComponentInstance;
class Component;
class View;

#include <QString>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Locator for component instances within a selected design.
//-----------------------------------------------------------------------------
class ComponentInstanceLocator
{

public:

    //! The target instance data.
    struct MatchingInstance
    {
        //! Name of the component instance.
        QString instanceName_;

        //! Unique ID of the component instance.
        QString instanceUUID_;

        //! The selected active view of the component instance.
        QString activeView_;

        //! The VLNV reference to the instantiated component.
        QSharedPointer<ConfigurableVLNVReference> componentVLNV_;
    };

    /*!
     *  Constructor.
     *
     *      @param [in] library     The library interface.
     */    
    ComponentInstanceLocator(LibraryInterface* library);

    /*!
     *  Destructor.
     */
    ~ComponentInstanceLocator();

    /*!
     *  Finds the component instances from a given design and its design configuration. Includes component
     *  instances from hierarchical designs.
     *
     *      @param [in] design                  The selected design.
     *      @param [in] designConfiguration     The selected design configuration.
     *
     *      @return A list of instance data.
     */
    QList<MatchingInstance> getMatchingInstances(QSharedPointer<Design> design,
        QSharedPointer<const DesignConfiguration> designConfiguration) const;

private:
    // Disable copying.
    ComponentInstanceLocator(ComponentInstanceLocator const& rhs);
    ComponentInstanceLocator& operator=(ComponentInstanceLocator const& rhs);

    /*!
     *  Get the required data from a given component instance.
     *
     *      @param [in] instance                The selected component instance.
     *      @param [in] designConfiguration     The design configuration of the design containing the component
     *                                          instance.
     *
     *      @return Instance data from the selected component instance.
     */
    MatchingInstance getInstanceData(QSharedPointer<ComponentInstance> instance,
        QSharedPointer<const DesignConfiguration> designConfiguration) const;

    /*!
     *  Get the child instances of a hierarchical component instance.
     *
     *      @param [in] instance                The selected component instance.
     *      @param [in] instancedComponent      The instanced component.
     *      @param [in] designConfiguration     Design configuration of the design containing the component
     *                                          instance.
     *
     *      @return A list of instance data.
     */
    QList<MatchingInstance> getChildInstancesFromComponent(QSharedPointer<ComponentInstance> instance,
        QSharedPointer<const Component> instancedComponent,
        QSharedPointer<const DesignConfiguration> designConfiguration) const;

    /*!
     *  Get the design configuration from the selected view.
     *
     *      @param [in] component           The component containing the selected view.
     *      @param [in] hierarchicalView    The selected view.
     *
     *      @return The design configuration referenced by the selected view.
     */
    QSharedPointer<const DesignConfiguration> getHierarchicalDesignConfiguration(
        QSharedPointer<const Component> component, QSharedPointer<View> hierarchicalView) const;

    /*!
     *  Get the design from the selected view.
     *
     *      @param [in] component               Component containing the selected view.
     *      @param [in] hierarchicalView        The selected view.
     *      @param [in] designConfiguration     Design configuration referenced by the selected view.
     *
     *      @return The design referenced by the selected view.
     */
    QSharedPointer<Design> getHierarchicalDesign(QSharedPointer<const Component> component,
        QSharedPointer<View> hierarchicalView, QSharedPointer<const DesignConfiguration> designConfiguration)
        const;

    /*!
     *  Gets the VLNV of the design referenced by the selected view.
     *
     *      @param [in] component               Component containing the selected view.
     *      @param [in] hierarchicalView        The selected view.
     *      @param [in] designConfiguration     Design configuration referenced by the selected view.
     *
     *      @return The VLNV of the design referenced by the selected view.
     */
    VLNV getHierarchicalDesignVLNV(QSharedPointer<const Component> component,
        QSharedPointer<View> hierarchicalView, QSharedPointer<const DesignConfiguration> designConfiguration)
        const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libraryHandler_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNERDIAGRAM_H
