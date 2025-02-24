//-----------------------------------------------------------------------------
// File: InterconnectionInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.09.2021
//
// Description:
// Interface for accessing design interconnections.
//-----------------------------------------------------------------------------

#ifndef INTERCONNECTIONINTERFACE_H
#define INTERCONNECTIONINTERFACE_H

#include <NameGroupInterface.h>

#include <QSharedPointer>

class Design;
class Interconnection;

//-----------------------------------------------------------------------------
//! Interface for accessing design interconnections.
//-----------------------------------------------------------------------------
class KACTUS2_API InterconnectionInterface : public NameGroupInterface
{
public:

    /*!
     *  The constructor.
     */
    InterconnectionInterface();

    /*!
     *  The destructor.
     */
    virtual ~InterconnectionInterface() = default;

    /*!
     *  Setup the interconnections.
     *
     *    @param [in] newDesign   Design containing the interconnections.
     */
    void setInterconnections(QSharedPointer<Design> newDesign);

    /*!
     *  Get index of the selected interconnection.
     *
     *    @param [in] itemName    Name of the selected interconnection.
     *
     *    @return Index of the selected interconnection.
     */
    virtual int getItemIndex(std::string const& itemName) const override final;

    /*!
     *  Get name of the indexed interconnection.
     *
     *    @param [in] itemIndex   Index of the selected interconnection.
     *
     *    @return Name of the selected interconnection.
     */
    std::string getIndexedItemName(int itemIndex) const final;

    /*!
     *  Get the name of the selected interconnection.
     *
     *    @param [in] startInstanceName   Name of the component instance containing the first bus interface.
     *    @param [in] startBusName        Name of the first bus interface.
     *    @param [in] endInstanceName     Name of the component instance containing the second bus interface.
     *    @param [in] endBusName          Name of the second bus interface.
     *
     *    @return Name of the selected interconnection.
     */
    std::string getConnectionName(std::string const& startInstanceName, std::string const& startBusName,
        std::string const& endInstanceName, std::string const& endBusName) const;

    /*!
     *  Get the name of the selected hierarchical interconnection.
     *
     *    @param [in] instanceName    Name of the component instance containing the selected bus interface.
     *    @param [in] instanceBus     Name of the selected component instance bus interface.
     *    @param [in] topBus          Name of the top component bus interface.
     *
     *    @return Name of the selected interconnection.
     */
    std::string getHierarchicalConnectionName(std::string const& instanceName, std::string const& instanceBus,
        std::string const& topBus) const;

    /*!
     *  Get the number of available interconnections.
     *
     *    @return Number of available interconnections.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the names of the available interconnections.
     *
     *    @return Names of the available interconnections.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Set a new name for the selected interconnection.
     *
     *    @param [in] currentName     Name of the selected interconnection.
     *    @param [in] newName         New name for the interconnection.
     *
     *    @return True, if successful, false otherwise.
     */
    virtual bool setName(std::string const& currentName, std::string const& newName) override final;

    /*!
     *  Get the description of the selected interconnection.
     *
     *    @param [in] itemName    Name of the selected interconnection.
     *
     *    @return Description of the selected interconnection.
     */
    virtual std::string getDescription(std::string const& itemName) const override final;

    /*!
     *  Set a new description for the selected interconnection.
     *
     *    @param [in] itemName        Name of the selected interconnection.
     *    @param [in] newDescription  New description.
     *
     *    @return True, if successful, false otherwise.
     */
    virtual bool setDescription(std::string const& itemName, std::string const& newDescription) override final;

    /*!
     *  Validates the contained interconnections.
     *
     *    @return True, if all the interconnections are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected interconnection has a valid name.
     *
     *    @param [in] itemName    Name of the selected interconnection.
     *
     *    @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const override final;

    /*!
     *  Add a new interconnection.
     *
     *    @param [in] startComponentInstance  Name of the component instance containing the first bus interface.
     *    @param [in] startBus                Name of the first bus interface.
     *    @param [in] endComponentInstance    Name of the component instance containing the second bus interface.
     *    @param [in] endBus                  Name of the second bus interface.
     *    @param [in] connectionName          Name of the connection.
     */
    void addInterconnection(std::string const& startComponentInstance, std::string const& startBus,
        std::string const& endComponentInstance, std::string const& endBus, std::string const& connectionName = "");

    /*!
     *  Add a new hierarchical interconnection.
     *
     *    @param [in] instanceName        Name of the component instance containing the selected bus interface.
     *    @param [in] instanceBus         Name of the selected component instance bus interface.
     *    @param [in] topBus              Name of the selected top component bus interface.
     *    @param [in] connectionName      Name of the connection.
     */
    void addHierarchicalInterconnection(std::string const& instanceName, std::string const& instanceBus,
        std::string const& topBus, std::string const& connectionName = "");

    /*!
     *  Remove the selected interconnection.
     *
     *    @param [in] connectionName  Name of the selected interconnection.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeInterconnection(std::string const& connectionName);

    /*!
     *  Remove interconnections connected to the selected component instance.
     *
     *    @param [in] instanceName    Name of the selected component instance.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeInstanceInterconnections(std::string const& instanceName);

    /*!
     *  Check if the selected interconnection exists.
     *
     *    @param [in] connectionName  Name of the selected interconnection.
     *
     *    @return True, if the interconnection exists, false otherwise.
     */
    bool interconnectionExists(std::string const& connectionName) const;

    /*!
     *  Rename the component references in connections.
     *
     *    @param [in] currentName     Current name of the component instance.
     *    @param [in] newName         New name for the component instance.
     */
    void renameComponentReferences(std::string const& currentName, std::string const& newName);

    //! No copying. No assignment.
    InterconnectionInterface(const InterconnectionInterface& other) = delete;
    InterconnectionInterface& operator=(const InterconnectionInterface& other) = delete;

private:

    /*!
     *  Get the selected interconnection.
     *
     *    @param [in] connectionName  Name of the selected interconnection.
     *
     *    @return The selected interconnection.
     */
    QSharedPointer<Interconnection> getInterconnection(std::string const& connectionName) const;

    /*!
     *  Get the interconnection with the selected name, sliced to a NameGroup.
     *
     *    @param [in] connectionName   Name of the selected parameter.
     *
     *    @return The sliced interconnection with the selected name.
     */
    QSharedPointer<NameGroup> getItem(std::string const& connectionName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The available interconnections.
    QSharedPointer<QList<QSharedPointer<Interconnection> > > interconnections_;
};

#endif // INTERCONNECTIONINTERFACE_H
