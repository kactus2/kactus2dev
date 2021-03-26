//-----------------------------------------------------------------------------
// File: AbstractionTypeInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2021
//
// Description:
// Interface for accessing abstraction types.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONTYPEINTERFACE_H
#define ABSTRACTIONTYPEINTERFACE_H

#include <IPXACTmodels/generaldeclarations.h>

#include <editors/ComponentEditor/common/interfaces/CommonInterface.h>

class AbstractionDefinition;
class AbstractionType;
class AbstractionTypeValidator;
class Component;
class ConfigurableVLNVReference;

class PortMapInterface;

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Interface for accessing abstraction types.
//-----------------------------------------------------------------------------
class AbstractionTypeInterface: public CommonInterface
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] portMapInterface    Interface for accessing port maps.
     *      @param [in] validator           Validator for abstraction types.
     *
     *      @return 
     */
    AbstractionTypeInterface(PortMapInterface* portMapInterface,
        QSharedPointer<AbstractionTypeValidator> validator);

    /*!
     *  The destructor.
     */
    virtual ~AbstractionTypeInterface() = default;

    /*!
     *  Setup the abstraction types.
     *
     *      @param [in] newAbstractions     The new abstraction types.
     */
    void setAbstractionTypes(QSharedPointer<QList<QSharedPointer<AbstractionType> > > newAbstractions);

    /*!
     *  Setup the containing port map interface.
     *
     *      @param [in] abstractionDefinition   Abstraction definition containing the logical ports.
     *      @param [in] abstractionTypeIndex    Index of the selected abstraction type.
     *      @param [in] busMode                 The selected interface mode.
     *      @param [in] systemGroup             The selected system group.
     *      @param [in] component               The containing component.
     */
    void setupPortMapInterface(QSharedPointer<const AbstractionDefinition> abstractionDefinition,
        int const& abstractionTypeIndex, General::InterfaceMode const& busMode, std::string const& systemGroup,
        QSharedPointer<Component> component);

    /*!
     *  Get the abstraction reference string of the selected abstraction type.
     *
     *      @param [in] itemIndex   Index of the selected abstraction type.
     *
     *      @return Abstraction reference string of the selected abstraction type.
     */
    std::string getIndexedAbstraction(int const& itemIndex) const;

    /*!
     *  Get the abstraction reference of the selected abstraction type.
     *
     *      @param [in] typeIndex   Index of the selected abstraction type.
     *
     *      @return Abstraction reference of the selected abstraction type.
     */
    QSharedPointer<ConfigurableVLNVReference> getAbstractionReference(int const& typeIndex) const;

    /*!
     *  Get the abstraction reference string.
     *
     *      @return Abstraction reference if only a single abstraction type exists, otherwise [multiple].
     */
    std::string getAbstractionReferenceString() const;

    /*!
     *  Get the list of abstraction references.
     *
     *      @return String list of the abstraction references.
     */
    std::vector<std::string> getAbstractionReferences() const;

    /*!
     *  Check if the selected abstraction type has an abstraction reference.
     *
     *      @param [in] typeIndex   Index of the selected abstraction type.
     *
     *      @return True, if the selected abstraction type has an abstraction reference, false otherwise.
     */
    bool hasAbstractionReference(int const& typeIndex) const;

    /*!
     *  Get the number of available abstraction types.
     *
     *      @return Number of available abstraction types.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the abstraction references of the available abstraction types.
     *
     *      @return Abstraction references of the available abstraction types.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Set a new abstraction reference for the selected abstraction type.
     *
     *      @param [in] abstractionIndex    Index of the selected abstraction type.
     *      @param [in] newVendor           Vendor of the new abstraction reference.
     *      @param [in] newLibrary          Library of the new abstraction reference.
     *      @param [in] newName             Name of the new abstraction reference.
     *      @param [in] newVersion          Version of the new abstraction reference.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setAbstraction(int const& abstractionIndex, std::string const& newVendor, std::string const& newLibrary,
        std::string const& newName, std::string const& newVersion);

    /*!
     *  Get the view references of the selected abstraction type.
     *
     *      @param [in] abstractionIndex    Index of the abstraction type.
     *
     *      @return View references of the selected abstraction type.
     */
    std::vector<std::string> getViewReferences(int const& abstractionIndex) const;

    /*!
     *  Get the view references of the selected abstraction type as a single string.
     *
     *      @param [in] abstractionIndex    Index of the selected abstraction type.
     *
     *      @return String containing the view references of the selected abstraction type.
     */
    std::string getCombinedViews(int const& abstractionIndex) const;

    /*!
     *  Set view references for the selected abstraction type.
     *
     *      @param [in] abstractionIndex    Index of the selected abstraction type.
     *      @param [in] newViews            The new view references.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setViewReferences(int const& abstractionIndex, std::vector<std::string> newViews);

    /*!
     *  Get the physical port names from all the contained port maps of the selected abstraction type.
     *
     *      @param [in] abstractionIndex    Index of the selected abstraction type.
     *
     *      @return Physical port names.
     */
    std::vector<std::string> getPhysicalPortsFromPortMaps(int const& abstractionIndex) const;

    /*!
     *  Validates the contained abstraction types.
     *
     *      @return True, if all the abstraction types are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected abstraction type has a valid abstraction reference.
     *
     *      @param [in] typeIndex   Index of the selected abstraction type.
     *
     *      @return True, if the selected abstraction type has a valid abstraction reference.
     */
    bool hasValidAbstractionReference(int const& typeIndex) const;

    /*!
     *  Check if the selected abstraction type has valid view references.
     *
     *      @param [in] typeIndex   Index of the selected abstraction type.
     *
     *      @return True, if the selected abstraction type has valid view references.
     */
    bool hasValidViewReferences(int const& typeIndex) const;

    /*!
     *  Add an abstraction type.
     *
     *      @param [in] typeIndex   Index of the selected abstraction type.
     */
    void addAbstraction(int const& typeIndex);

    /*!
     *  Add the selected abstraction type.
     *
     *      @param [in] newVendor   Vendor of the new abstraction reference.
     *      @param [in] newLibrary  Library of the new abstraction reference.
     *      @param [in] newName     Name of the new abstraction reference.
     *      @param [in] newVersion  Version of the new abstraction reference.
     */
    void addAbstractionType(std::string const& newVendor, std::string const& newLibrary,
        std::string const& newName, std::string const& newVersion) const;

    /*!
     *  Remove the selected abstraction type.
     *
     *      @param [in] typeIndex   Index of the selected abstraction type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeAbstraction(int const& typeIndex);

    /*!
     *  Get the contained port map interface.
     *
     *      @return The port map interface.
     */
    PortMapInterface* getPortMapInterface() const;

    //! No copying. No assignment.
    AbstractionTypeInterface(const AbstractionTypeInterface& other) = delete;
    AbstractionTypeInterface& operator=(const AbstractionTypeInterface& other) = delete;

private:

    /*!
     *  Get the selected abstraction type.
     *
     *      @param [in] abstractionIndex    Index of the selected abstraction type.
     *
     *      @return The selected abstraction type.
     */
    QSharedPointer<AbstractionType> getAbstraction(int const& abstractionIndex) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the available abstraction types.
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractions_;

    //! Interface for accessing port maps.
    PortMapInterface* portMapInterface_;

    //! Validator for abstraction types.
    QSharedPointer<AbstractionTypeValidator> validator_;
};

#endif // BUSINTERFACEINTERFACE_H
