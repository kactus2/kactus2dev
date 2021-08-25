//-----------------------------------------------------------------------------
// File: PythonAPI.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.02.2020
//
// Description:
// Interface for accessing Kactus2 data using Python.
//-----------------------------------------------------------------------------

// #pragma once

#include "pythonapi_global.h"

#include <common/KactusAPI.h>

#include <string>
#include <vector>
#include <QSharedPointer>

class LibraryInterface;
class MessageMediator;

class Component;
class Field;
class Register;
class RegisterBase;
class AddressBlock;
class MemoryMap;
class FileSet;

class PortsInterface;
class ParametersInterface;
class MemoryMapInterface;
class FileSetInterface;

class ComponentParameterFinder;
class ExpressionParser;
class PortValidator;
class ExpressionFormatter;

class ParameterValidator;
class MemoryMapValidator;

class Design;
class ComponentInstance;

//-----------------------------------------------------------------------------
//! Interface for accessing Kactus2 data using Python.
//-----------------------------------------------------------------------------
class PythonAPI
{
public:
    
    /*!
     *  The constructor.
     */
    PythonAPI();

    /*!
     *  The destructor.
     */
    ~PythonAPI() = default;
   
    /*!
     *  Get Kactus2 version.
     *
     *      @return The current Kactus2 version.
     */
    std::string getVersion() const;

    std::vector<std::string> getLibraryPaths() const;

    /*!
     * Gets the paths where IP-XACT files are stored.
     *
     *     @return The paths where IP-XACT files are stored.
     */
    void setupLibrary(std::string const& settingsFileString);

    /*!
     * Set the paths where IP-XACT files are stored.
     *
     *     @param [in] paths  The locations that are currently available to store the files.
     */
    void setLibraryPaths(std::vector<std::string> paths) const;

    /*!
     * Get the default library path for IP-XACT files.
     *
     *     @return The default path for IP-XACT files.
     */
    std::string getDefaultLibraryPath() const;

    /*!
     * Set the default library path for IP-XACT files.
     *
     *     @param [in] path  The default path to set for IP-XACT files.
     */
    void setDefaultLibraryPath(std::string const& path) const;
    
    /*!
     * Import a source file (RTL) into the library as a component.
     *
     *     @param [in] filePath     The path to the file to import.
     *     @param [in] targetVLNV   The VLNV to use for the imported component.
     *     @param [in] overwrite    Overwrite the component in the library, if it already exists.
     *
     *     @return The number of imported IP-XACT components.
     */
    int importFile(std::string const& path, std::string vlnv, bool overwrite = false) const;

    /*!
     * Run a file generation for a component.
     *
     *     @param [in] format           The requested output format.
     *     @param [in] vlnv             The component VLNV to run the generation for.
     *     @param [in] viewName         The component view to run the generation for.
     *     @param [in] outputDirectory  The output directory for the generation results.
     */
     void generate(std::string const& format, std::string const& vlnv, std::string const& viewName,
        std::string const& outputDirectory) const;

    /*!
     *  Get the number of files in the library.
     *
     *      @return The number of files in the library.
     */
    int getFileCount() const;

    /*!
     *  List all the VLNVs in the library.
     *
     *      @param[in] vendor   The desired vendor.
     *
     *      @return List of VLNVs in Vendor:Library:Name:Version format.
     */

    std::vector<std::string> listVLNVs(std::string const& vendor = std::string()) const;

    /*!
     *  List all the component VLNVs in the library.
     *
     *     @return List of component VLNVs in Vendor:Library:Name:Version format.
     */
    std::vector<std::string> listComponentVLNVs() const;

    /*!
     *  Check if the selected VLNV exists in the library.
     *
     *      @param [in] vendor      Vendor of the selected VLNV.
     *      @param [in] library     Library of the selected VLNV.
     *      @param [in] name        Name of the selected VLNV.
     *      @param [in] version     Version of the selected VLNV.
     *
     *      @return True, if the VLNV exists in the library, false otherwise.
     */
    bool vlnvExistsInLibrary(std::string const& vendor, std::string const& library, std::string const& name,
        std::string const& version);

    /*!
     *  Create a new component with the selected VLNV.
     *
     *      @param [in] vendor      Vendor of the selected VLNV.
     *      @param [in] library     Library of the selected VLNV.
     *      @param [in] name        Name of the selected VLNV.
     *      @param [in] version     Version of the selected VLNV.
     *
     *      @return True, if the component was created successfully, false otherwise.
     */
    bool createComponent(std::string const& vendor, std::string const& library, std::string const& name,
        std::string const& version);

    /*!
     *  Get the directory path of the selected VLNV document.
     *
     *      @param [in] vendor      Vendor of the selected VLNV.
     *      @param [in] library     Library of the selected VLNV.
     *      @param [in] name        Name of the selected VLNV.
     *      @param [in] version     Version of the selected VLNV.
     *
     *      @return Path of the selected VLNV document.
     */
    std::string getVLNVDirectory(std::string const& vendor, std::string const& library, std::string const& name,
        std::string const& version) const;

    /*!
     *  Get the name of the first view contained within the active component.
     *
     *      @return Name of the first view contained within the active component.
     */
    std::string getFirstViewName();

    /*!
     *  Set the selected component as active component.
     *
     *      @param [in] componentVLNV   VLNV of the selected component.
     *
     *      @return True, if the component exists, false otherwise.
     */
    bool openComponent(std::string const& vlnvString);

    /*!
     *  Remove the active component.
     */
    void closeOpenComponent();

    /*!
     *  Get the name of the active component.
     */

    std::string getComponentName();

    /*!
     *  Get the description of the active component.
     */

    std::string getComponentDescription();

    /*!
     *  Save the component to the library.
     */
    void saveComponent();

    /*!
     *  Get the interface for accessing the component ports.
     *
     *      @return Interface for accessing the component ports.
     */
    PortsInterface* getPortsInterface() const;

    /*!
     *  Get the interface for accessing the component parameters.
     *
     *      @return Interface for accessing the component parameters.
     */
    ParametersInterface* getComponentParameterInterface() const;

    /*!
     *  Get the interface for accessing component memory maps.
     *
     *      @return Interface for accessing component memory maps.
     */
    MemoryMapInterface* getMapInterface();

    /*!
     *  Set the available address blocks for the address block interface.
     *
     *      @param [in] mapName     Name of the memory map containing the desired address blocks.
     */
    void setBlocksForInterface(std::string const& mapName);

    /*!
     *  Set the available registers for the register interface.
     *
     *      @param [in] mapName     Name of the memory map containing the desired address blocks.
     *      @param [in] blockName   Name of the address block containing the desired registers.
     */
    void setRegistersForInterface(std::string const& mapName, std::string const& blockName);

    /*!
     *  Set the available fields for the field interface.
     *
     *      @param [in] mapName         Name of the memory map containing the desired address blocks.
     *      @param [in] blockName       Name of the address block containing the desired registers.
     *      @param [in] registerName    Name of the register containing the desired fields.
     */
    void setFieldsForInterface(std::string const& mapName, std::string const& blockName,
        std::string const& registerName);

    /*!
     *  Set the available resets for the reset interface.
     *
     *      @param [in] mapName         Name of the memory map containing the desired address blocks.
     *      @param [in] blockName       Name of the address block containing the desired registers.
     *      @param [in] registerName    Name of the register containing the desired fields.
     *      @param [in] fieldName       Name of the field containing the desired resets.
     */
    void setResetsForInterface(std::string const& mapName, std::string const& blockName,
        std::string const& registerName, std::string const& fieldName);

    /*!
     *  Get the interface for accessing file sets.
     *
     *      @return Interface for accessing file sets.
     */
    FileSetInterface* getFileSetInterface();

    /*!
     *  Set the available files for the file interface.
     *
     *      @param [in] setName     Name of the file set containing the available files.
     */
    void setFilesForInterface(std::string const& setName);

    /*!
     *  Set the available file builders for the file interface.
     *
     *      @param [in] setName     Name of the file set containing the available file builders.
     */
    void setFileBuildersForInterface(std::string const& setName);

    /*!
     *  Set the selected design as active design.
     *
     *      @param [in] vlnvString  VLNV of the selected design.
     *
     *      @return True, if the design exists, false otherwise.
     */
    bool openDesign(std::string const& vlnvString);

    /*!
     *  Remove the active design.
     */
    void closeOpenDesign();

    /*!
     *  Save the design to the library.
     */
    void saveDesign();

    /*!
     *  Add the selected component instance to the active design.
     *
     *      @param [in] vlnvString      VLNV of the component within the selected component instance.
     *      @param [in] instanceName    Name for the new component instance.
     *
     *      @return True, if successful, false otherwise.
     */
    bool addComponentInstance(std::string const& vlnvString, std::string const& instanceName);

    /*!
     *  Get the names of component instances within the active design.
     *
     *      @return Names of component instances within the active design.
     */
    std::vector<std::string> getInstanceNames() const;

    /*!
     *  Remove the selected component instance from the active design.
     *
     *      @param [in] instanceName    Name of the selected component instance.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeComponentInstance(std::string const& instanceName);

    /*!
     *  Remove connections from the selected component instance.
     *
     *      @param [in] instanceName    Name of the selected component instance.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeInstanceConnections(std::string const& instanceName);

    /*!
     *  Remove ad hoc connections from the selected component instance.
     *
     *      @param [in] instanceName    Name of the selected component instance.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeInstanceAdHocConnections(std::string const& instanceName);

private:

    /*!
     *  Get a unique name for the containing component instance.
     *
     *      @param [in] baseName    The new component instance name.
     *
     *      @return Unique name formed from the basename.
     */
    QString getUniqueInstanceName(QString const& baseName) const;

    /*!
     *  Get the selected document.
     *
     *      @param [in] vlnvString  VLNV of the selected document in string form.
     *
     *      @return The selected document.
     */
    QSharedPointer<Document> getDocument(QString const& vlnvString) const;

    /*!
     *  Get the selected component instance.
     *
     *      @param [in] instanceName    Name of the selected component instance.
     *
     *      @return The selected component instance.
     */
    QSharedPointer<ComponentInstance> getComponentInstance(QString const& instanceName) const;

    /*!
     *  Construct validator for memory items.
     */
    void constructMemoryValidators();

    /*!
     *  Construct interfaces for memory items.
     */
    void constructMemoryInterface();

    /*!
     *  Construct the interfaces for file sets.
     */
    void constructFileSetInterface();

    /*!
     *  Get the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return The selected memory map.
     */
    QSharedPointer<MemoryMap> getMemoryMap(QString const& mapName) const;

    /*!
     *  Get the selected address block.
     *
     *      @param [in] containingMap   Memory map containing the address block.
     *      @param [in] blockName       Name of the selected address block.
     *
     *      @return The selected address block.
     */
    QSharedPointer<AddressBlock> getAddressBock(QSharedPointer<MemoryMap> containingMap, QString const& blockName)
        const;

    /*!
     *  Get the selected register.
     *
     *      @param [in] containingBlock     Address block containing the registers.
     *      @param [in] registerName        Name of the selected register.
     *
     *      @return The selected register.
     */
    QSharedPointer<Register> getRegister(QSharedPointer<AddressBlock> containingBlock, QString const& registerName)
        const;

    /*!
     *  Get the selected field.
     *
     *      @param [in] containingRegister  Register containing the fields.
     *      @param [in] fieldName           Name of the selected field.
     *
     *      @return The selected field.
     */
    QSharedPointer<Field> getField(QSharedPointer<Register> containingRegister, QString const& fieldName) const;

    /*!
     *  Get the selected file set.
     *
     *      @param [in] setName     Name of the selected file set.
     *
     *      @return The selected file set.
     */
    QSharedPointer<FileSet> getFileSet(QString const& setName) const;

    /*!
     *  Send an error text for a non-existing memory map.
     *
     *      @param [in] mapName     Name of the memory map.
     */
    void sendMemoryMapNotFoundError(QString const& mapName) const;

    /*!
     *  Send an error text for a non-existing address block.
     *
     *      @param [in] mapName     Name of the memory map containing the address block.
     *      @param [in] blockName   Name of the address block.
     */
    void sendAddressBlockNotFoundError(QString const& mapName, QString const& blockName) const;

    /*!
     *  Send an error text for a non-existing register.
     *
     *      @param [in] mapName         Name of the memory map containing the address block.
     *      @param [in] blockName       Name of the address block containing the register.
     *      @param [in] registerName    Name of the register.
     */
    void sendRegisterNotFoundError(QString const& mapName, QString const& blockName, QString const& registerName)
        const;

    /*!
     *  Send an error text for a non-existing field.
     *
     *      @param [in] mapName         Name of the memory map containing the address block.
     *      @param [in] blockName       Name of the address block containing the register.
     *      @param [in] registerName    Name of the register containing the field.
     *      @param [in] fieldName       Name of the field.
     */
    void sendFieldNotFoundError(QString const& mapName, QString const& blockName, QString const& registerName,
        QString const& fieldName) const;

    /*!
     *  Send an error text for a non-existing file set.
     *
     *      @param [in] setName     Name of the file set.
     */
    void sendFileSetNotFoundError(QString const& setName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface to the library.
    LibraryInterface* library_;

    //! Message handler.
    MessageMediator* messager_;

    //! Currently active component.
    QSharedPointer<Component> activeComponent_;

    //! Currently active design.
    QSharedPointer<Design> activeDesign_;

    //! Interface for accessing the component ports.
    PortsInterface* portsInterface_;

    //! Interface for accessing the component parameters.
    ParametersInterface* componentParameterInterface_;

    //! Interface for accessing memory maps.
    MemoryMapInterface* mapInterface_;

    //! Interface for accessing file sets.
    FileSetInterface* fileSetInterface_;

    //! Component parameter finder.
    QSharedPointer<ComponentParameterFinder> parameterFinder_;

    //! Parser for expressions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Formatter for expressions.
    QSharedPointer<ExpressionFormatter>expressionFormatter_;

    //! Validator for ports.
    QSharedPointer<PortValidator> portValidator_;

    //! Validator for parameters.
    QSharedPointer<ParameterValidator> parameterValidator_;

    //! Validator for memory maps.
    QSharedPointer<MemoryMapValidator> mapValidator_;
};
