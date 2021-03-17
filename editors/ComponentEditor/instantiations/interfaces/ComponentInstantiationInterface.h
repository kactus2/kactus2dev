//-----------------------------------------------------------------------------
// File: ComponentInstantiationInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.09.2020
//
// Description:
// Interface for editing component instantiations.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANTIATIONINTERFACE_H
#define COMPONENTINSTANTIATIONINTERFACE_H

#include <editors/ComponentEditor/common/interfaces/ParameterizableInterface.h>
#include <editors/ComponentEditor/common/interfaces/NameGroupInterface.h>

class InstantiationsValidator;
class ComponentInstantiation;
class FileBuilderInterface;
class ParametersInterface;
class ModuleParameterInterface;
class FileSetInterface;

//-----------------------------------------------------------------------------
//! Interface for editing component instantiations.
//-----------------------------------------------------------------------------
class ComponentInstantiationInterface : public ParameterizableInterface, public NameGroupInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] validator                   Validator for address blocks.
     *      @param [in] expressionParser            Parser for expressions.
     *      @param [in] expressionFormatter         Formatter for expressions.
     *      @param [in] parameterInterface          Interface for accessing parameters.
     *      @param [in] moduleParameterInterface    Interface for accessing module parameters
     *      @param [in] fileBuilderInterface        Interface for accessing file builders.
     *      @param [in] fileSetInterface            Interface for accessing file sets.
     */
    ComponentInstantiationInterface(QSharedPointer<InstantiationsValidator> validator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        ParametersInterface* parameterInterface,
        ModuleParameterInterface* moduleParameterInterface,
        FileBuilderInterface* fileBuilderInterface,
        FileSetInterface* fileSetInterface);

	/*!
     *  The destructor.
     */
    virtual ~ComponentInstantiationInterface() = default;

    /*!
     *  Set available component instantiations.
     *
     *      @param [in] newInstantiations   The new component instantiations.
     */
    void setComponentInstantiations(
        QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > newInstantiations);

    /*!
     *  Get index of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Index of the selected item.
     */
    virtual int getItemIndex(std::string const& itemName) const override final;

    /*!
     *  Get name of the indexed item.
     *
     *      @param [in] itemIndex   Index of the selected item.
     *
     *      @return Name of the selected item.
     */
    virtual std::string getIndexedItemName(int const& itemIndex) const override final;

    /*!
     *  Get the number of available items.
     *
     *      @return Number of available items.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the names of the available items.
     *
     *      @return Names of the available items.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Set a new name for the selected item.
     *
     *      @param [in] currentName     Name of the selected item.
     *      @param [in] newName         New name for the item.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(std::string const& currentName, std::string const& newName) override final;

    /*!
     *  Get the display name of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Display name of the selected item.
     */
    std::string getDisplayName(std::string const& itemName) const;

    /*!
     *  Set the display name of the selected item.
     *
     *      @param [in] itemName        Name of the selected item.
     *      @param [in] newDisplayName  New display name for the item.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setDisplayName(std::string const& itemName, std::string const& newDisplayName);

    /*!
     *  Get the description of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Description of the selected item.
     */
    virtual std::string getDescription(std::string const& itemName) const override final;

    /*!
     *  Set a new description for the selected item.
     *
     *      @param [in] itemName        Name of the selected item.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(std::string const& itemName, std::string const& newDescription) override final;

    /*!
     *  Calculate all the references to the selected ID in the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references to the selected ID in the selected item.
     */
    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const override
        final;

    /*!
     *  Get the expressions in the selected component instantiations.
     *
     *      @param [in] instantiationNames  Names of the selected component instantiations.
     *
     *      @return Expressions in the selected component instantiations.
     */
    std::vector<std::string> getExpressionsInSelectedItems(std::vector<std::string> instantiationNames) const;

    /*!
     *  Validates the contained items.
     *
     *      @return True, if all the ports are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected item has a valid name.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const override final;

    /*!
     *  Add a new component instantiation.
     *
     *      @param [in] row                     Row of the new component instantiation.
     *      @param [in] newInstantiationName    Name of the new component instantiation.
     */
    void addComponentInstantiation(int const& row, std::string const& newInstantiationName = std::string(""));

    /*!
     *  Remove the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeComponentInstantiation(std::string const& instantiationName);

    /*!
     *  Get the parameter interface.
     *
     *      @return Interface for accessing parameters.
     */
    ParametersInterface* getParameterInterface() const;

    /*!
     *  Get the module parameter interface.
     *
     *      @return Interface for accessing module parameters.
     */
    ModuleParameterInterface* getModuleParameterInterface() const;

    /*!
     *  Get the file builder interface.
     *
     *      @return Interface for accessing file builders.
     */
    FileBuilderInterface* getFileBuilderInterface() const;
    
    /*!
     *  Get the file set interface.
     *
     *      @return Interface for accessing file sets.
     */
    FileSetInterface* getFileSetInterface() const;

    /*!
     *  Get the language of the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *
     *      @return Language of the selected component instantiation.
     */
    std::string getLanguage(std::string const& instantiationName) const;

    /*!
     *  Set a new language for the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *      @param [in] newLanguage         New language.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setLanguage(std::string const& instantiationName, std::string const& newLanguage);

    /*!
     *  Check if the language of the selected component instantiation is strict.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *
     *      @return Language strictness of the selected component instantiation.
     */
    bool isLanguageStrict(std::string const& instantiationName) const;

    /*!
     *  Set language strictness for the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *      @param [in] newStrictness       The new language strictness.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setLanguageStrictness(std::string const& instantiationName, bool newStrictness);

    /*!
     *  Get the library name of the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *
     *      @return Library name of the selected component instantiation.
     */
    std::string getLibraryName(std::string const& instantiationName) const;

    /*!
     *  Set a new library name for the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *      @param [in] newLibraryName      The new library name.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setLibraryName(std::string const& instantiationName, std::string const& newLibraryName);

    /*!
     *  Get the package name of the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *
     *      @return Package name of the selected component instantiation.
     */
    std::string getPackageName(std::string const& instantiationName) const;

    /*!
     *  Set a new package name for the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *      @param [in] newPackageName      The new library name.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setPackageName(std::string const& instantiationName, std::string const& newPackageName);

    /*!
     *  Get the module name of the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *
     *      @return Module name of the selected component instantiation.
     */
    std::string getModuleName(std::string const& instantiationName) const;

    /*!
     *  Set a new module name for the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *      @param [in] newModuleName       The new module name.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setModuleName(std::string const& instantiationName, std::string const& newModuleName);

    /*!
     *  Get the architecture name of the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *
     *      @return Architecture name of the selected component instantiation.
     */
    std::string getArchitectureName(std::string const& instantiationName) const;

    /*!
     *  Set a new architecture name for the selected component instantiation.
     *
     *      @param [in] instantiationName       Name of the selected component instantiation.
     *      @param [in] newArchitectureName     The new module name.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setArchitectureName(std::string const& instantiationName, std::string const& newArchitectureName);

    /*!
     *  Get the configuration name of the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *
     *      @return Configuration name of the selected component instantiation.
     */
    std::string getConfigurationName(std::string const& instantiationName) const;

    /*!
     *  Set a new configuration name for the selected component instantiation.
     *
     *      @param [in] instantiationName       Name of the selected component instantiation.
     *      @param [in] newConfigurationName    The new module name.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setConfigurationName(std::string const& instantiationName, std::string const& newConfigurationName);

    /*!
     *  Get the file set references of the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *
     *      @return File set references of the selected component instantiation.
     */
    std::vector<std::string> getFileSetReferences(std::string const& instantiationName) const;

    /*!
     *  Set new file set references for the selected component instantiation.
     *
     *      @param [in] instantiationName   Name of the selected component instantiation.
     *      @param [in] newReferences       The new file set references.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setFileSetReferences(std::string const& instantiationName, std::vector<std::string> const& newReferences);

private:

    /*!
     *  Get the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return The selected address block.
     */
    QSharedPointer<ComponentInstantiation> getComponentInstantiation(std::string const& itemName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained component instantiations.
    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations_;

    //! Validator for instantiations.
    QSharedPointer<InstantiationsValidator> validator_;

    //! Interface for accessing parameters.
    ParametersInterface* parameterInterface_;

    //! Interface for accessing module parameters.
    ModuleParameterInterface* moduleParameterInterface_;

    //! Interface for accessing file builders.
    FileBuilderInterface* fileBuilderInterface_;

    //! Interface for accessing file sets.
    FileSetInterface* fileSetInterface_;
};

#endif // COMPONENTINSTANTIATIONINTERFACE_H
