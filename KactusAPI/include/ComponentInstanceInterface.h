//-----------------------------------------------------------------------------
// File: ComponentInstanceInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.09.2021
//
// Description:
// Interface for accessing component instances.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCEINTERFACE_H
#define COMPONENTINSTANCEINTERFACE_H

#include <NameGroupInterface.h>

#include <QSharedPointer>

class Design;
class ComponentInstance;
class ConfigurableVLNVReference;

class InterconnectionInterface;
class AdHocConnectionInterface;

//-----------------------------------------------------------------------------
//! Interface for accessing component instances.
//-----------------------------------------------------------------------------
class KACTUS2_API ComponentInstanceInterface : public NameGroupInterface
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] connectionInterface     Interface for accessing interconnections.
     *      @param [in] adHocInterface          Interface for accessing ad hoc connections.
     */
    ComponentInstanceInterface(InterconnectionInterface* connectionInterface,
        AdHocConnectionInterface* adHocInterface);

    /*!
     *  The destructor.
     */
    virtual ~ComponentInstanceInterface() = default;

    /*!
     *  Setup the component instances.
     *
     *      @param [in] newDesign   Design containing the component instances.
     */
    void setComponentInstances(QSharedPointer<Design> newDesign);

    /*!
     *  Get index of the selected component instance.
     *
     *      @param [in] itemName    Name of the selected component instance.
     *
     *      @return Index of the selected component instance.
     */
    virtual int getItemIndex(std::string const& itemName) const override final;

    /*!
     *  Get name of the indexed component instance.
     *
     *      @param [in] itemIndex   Index of the selected component instance.
     *
     *      @return Name of the selected component instance.
     */
    virtual std::string getIndexedItemName(int const& itemIndex) const override final;

    /*!
     *  Get the number of available component instances.
     *
     *      @return Number of available component instances.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the names of the available component instances.
     *
     *      @return Names of the available component instances.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Set a new name for the selected component instance.
     *
     *      @param [in] currentName     Name of the selected component instance.
     *      @param [in] newName         New name for the component instance.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(std::string const& currentName, std::string const& newName) override final;

    /*!
     *  Get the description of the selected component instance.
     *
     *      @param [in] itemName    Name of the selected component instance.
     *
     *      @return Description of the selected component instance.
     */
    virtual std::string getDescription(std::string const& itemName) const override final;

    /*!
     *  Set a new description for the selected component instance.
     *
     *      @param [in] itemName        Name of the selected component instance.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(std::string const& itemName, std::string const& newDescription) override final;

    /*!
     *  Validates the contained component instances.
     *
     *      @return True, if all the component instances are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected component instance has a valid name.
     *
     *      @param [in] itemName    Name of the selected component instance.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const override final;

    /*!
     *  Get the VLNV of the referenced component of the selected component instance.
     *
     *      @param [in] instanceName    Name of the selected component instance.
     *
     *      @return VLNV of the referenced component.
     */
    QSharedPointer<ConfigurableVLNVReference> getComponentReference(std::string const& instanceName) const;

    /*!
     *  Set a new component reference for the selected component instance.
     *
     *      @param [in] instanceName    Name of the selected component instance.
     *      @param [in] newVendor       Vendor of the new component reference.
     *      @param [in] newLibrary      Library of the new component reference.
     *      @param [in] newName         Name of the new component reference.
     *      @param [in] newVersion      Version of the new component reference.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setComponentReference(std::string const& instanceName, std::string const& newVendor,
        std::string const& newLibrary, std::string const& newName, std::string const& newVersion);

    /*!
     *  Add a new component instance.
     *
     *      @param [in] instanceName    Name of the new component instance.
     */
    void addComponentInstance(std::string const& instanceName = "");

    /*!
     *  Remove the selected component instance.
     *
     *      @param [in] instanceName    Name of the selected component instance.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeComponentInstance(std::string const& instanceName);

    /*!
     *  Check if the selected component instance exists.
     *
     *      @param [in] instanceName    Name of the selected component instance.
     *
     *      @return True, if the component instance exists, false otherwise.
     */
    bool instanceExists(std::string const& instanceName) const;

    //! No copying. No assignment.
    ComponentInstanceInterface(const ComponentInstanceInterface& other) = delete;
    ComponentInstanceInterface& operator=(const ComponentInstanceInterface& other) = delete;

private:

    /*!
     *  Get the selected component instance.
     *
     *      @param [in] instanceName    Name of the selected component instance.
     *
     *      @return The selected component instance.
     */
    QSharedPointer<ComponentInstance> getComponentInstance(std::string const& instanceName) const;

    /*!
     *  Get the paracomponent instancemeter with the selected name, sliced to a NameGroup.
     *
     *      @param [in] instanceName   Name of the selected component instance.
     *
     *      @return The sliced component instance with the selected name.
     */
    QSharedPointer<NameGroup> getItem(std::string const& instanceName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The available component instances.
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > componentInstances_;

    //! Interface for accessing design interconnections.
    InterconnectionInterface* connectionInterface_;

    //! Interface for accessing design ad hoc connections.
    AdHocConnectionInterface* adHocConnectionInterface_;
};

#endif // COMPONENTINSTANCEINTERFACE_H
