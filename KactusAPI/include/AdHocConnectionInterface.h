//-----------------------------------------------------------------------------
// File: AdHocconnectionInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.09.2021
//
// Description:
// Interface for accessing design ad hoc connections.
//-----------------------------------------------------------------------------

#ifndef ADHOCCONNECTIONINTERFACE_H
#define ADHOCCONNECTIONINTERFACE_H

#include <NameGroupInterface.h>

#include <QSharedPointer>

class Design;
class AdHocConnection;

//-----------------------------------------------------------------------------
//! Interface for accessing design ad hoc connections.
//-----------------------------------------------------------------------------
class KACTUS2_API AdHocConnectionInterface : public NameGroupInterface
{
public:

    /*!
     *  The constructor.
     */
    AdHocConnectionInterface();

    /*!
     *  The destructor.
     */
    virtual ~AdHocConnectionInterface() = default;

    /*!
     *  Setup the ad hoc connections.
     *
     *      @param [in] newDesign   Design containing the ad hoc connections.
     */
    void setConnections(QSharedPointer<Design> newDesign);

    /*!
     *  Get index of the selected ad hoc connection.
     *
     *      @param [in] itemName    Name of the selected ad hoc connection.
     *
     *      @return Index of the selected ad hoc connection.
     */
    virtual int getItemIndex(std::string const& itemName) const override final;

    /*!
     *  Get name of the indexed ad hoc connection.
     *
     *      @param [in] itemIndex   Index of the selected ad hoc connection.
     *
     *      @return Name of the selected ad hoc connection.
     */
    virtual std::string getIndexedItemName(int const& itemIndex) const override final;

    /*!
     *  Get the name of the selected ad hoc connection.
     *
     *      @param [in] startInstanceName   Name of the instance containing the first port.
     *      @param [in] startPortName       Name of the first port.
     *      @param [in] endInstanceName     Name of the instance containing the second port.
     *      @param [in] endPortName         Name of the second port.
     *
     *      @return Name of the selected ad hoc connection.
     */
    std::string getConnectionName(std::string const& startInstanceName, std::string const& startPortName,
        std::string const& endInstanceName, std::string const& endPortName) const;

    /*!
     *  Get the name of the selected hierarchical ad hoc connection.
     *
     *      @param [in] instanceName    Name of the component instance containing the selected port.
     *      @param [in] instancePort    Name of the selected component instance port.
     *      @param [in] topPort         Name of the top component port.
     *
     *      @return Name of the selected ad hoc connection.
     */
    std::string getHierarchicalConnectionName(std::string const& instanceName, std::string const& instancePort,
        std::string const& topPort) const;

    /*!
     *  Get the number of available ad hoc connections.
     *
     *      @return Number of available ad hoc connections.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the names of the available bus interfaces.
     *
     *      @return Names of the available bus interfaces.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Set a new name for the selected ad hoc connection.
     *
     *      @param [in] currentName     Name of the selected ad hoc connection.
     *      @param [in] newName         New name for the ad hoc connection.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(std::string const& currentName, std::string const& newName) override final;

    /*!
     *  Get the description of the selected ad hoc connection.
     *
     *      @param [in] itemName    Name of the selected ad hoc connection.
     *
     *      @return Description of the selected ad hoc connection.
     */
    virtual std::string getDescription(std::string const& itemName) const override final;

    /*!
     *  Set a new description for the selected ad hoc connection.
     *
     *      @param [in] itemName        Name of the selected ad hoc connection.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(std::string const& itemName, std::string const& newDescription) override final;

    /*!
     *  Validates the contained ad hoc connections.
     *
     *      @return True, if all the ad hoc connections are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected ad hoc connection has a valid name.
     *
     *      @param [in] itemName    Name of the selected ad hoc connection.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const override final;

    /*!
     *  Add a new ad hoc connection.
     *
     *      @param [in] startComponentInstance  Name of the component instance containing the first port.
     *      @param [in] startPort               Name of the first port.
     *      @param [in] endComponentInstance    Name of the component instance containing the second port.
     *      @param [in] endPort                 Name of the second port.
     *      @param [in] connectionName          Name of the new ad hoc connection.
     */
    void addAdHocConnection(std::string const& startComponentInstance, std::string const& startPort,
        std::string const& endComponentInstance, std::string const& endPort,
        std::string const& connectionName = "");

    /*!
     *  Add a new hierarchical ad hoc connection. 
     *
     *      @param [in] instanceName        Name of the component instance containing the selected port.
     *      @param [in] instancePort        Name of the selected port.
     *      @param [in] topPort             Name of the top component port.
     *      @param [in] connectionName      Name of the new ad hoc connection.
     */
    void addHierarchicalAdHocConnection(std::string const& instanceName, std::string const& instancePort,
        std::string const& topPort, std::string const& connectionName = "");

    /*!
     *  Remove the selected ad hoc connection.
     *
     *      @param [in] busName     Name of the selected ad hoc connection.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeAdHocConnection(std::string const& connectionName);

    /*!
     *  Remove ad hoc connections connected to the selected component instance.
     *
     *      @param [in] instanceName    Name of the selected component instance.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeInstanceAdHocConnections(std::string const& instanceName);

    /*!
     *  Check if the selected ad hoc connection exists.
     *
     *      @param [in] connectionName  Name of the selected ad hoc connection.
     *
     *      @return True, if the ad hoc connection exists, false otherwise.
     */
    bool adHocConnectionExists(std::string const& connectionName) const;

    /*!
     *  Rename the component references in connections.
     *
     *      @param [in] currentName     Current name of the component instance.
     *      @param [in] newName         New name for the component instance.
     */
    void renameComponentReferences(std::string const& currentName, std::string const& newName);

    //! No copying. No assignment.
    AdHocConnectionInterface(const AdHocConnectionInterface& other) = delete;
    AdHocConnectionInterface& operator=(const AdHocConnectionInterface& other) = delete;

private:

    /*!
     *  Get the selected ad hoc connection.
     *
     *      @param [in] connectionName     Name of the selected ad hoc connection.
     *
     *      @return The selected ad hoc connection.
     */
    QSharedPointer<AdHocConnection> getAdHocConnection(std::string const& connectionName) const;

    /*!
     *  Get the ad hoc connection with the selected name, sliced to a NameGroup.
     *
     *      @param [in] connectionName   Name of the selected ad hoc connection.
     *
     *      @return The sliced ad hoc connection with the selected name.
     */
    QSharedPointer<NameGroup> getItem(std::string const& connectionName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The available ad hoc connections.
    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > connections_;
};

#endif // ADHOCCONNECTIONINTERFACE_H
