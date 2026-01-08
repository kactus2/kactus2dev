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

#ifndef PYTHON_API_H
#define PYTHON_API_H

#include "pythonapi_global.h"

#include <KactusAPI/KactusAPI.h>

#include <string>
#include <vector>
#include <QSharedPointer>

#include <KactusAPI/include/ComponentInstanceInterface.h>
#include <KactusAPI/include/InterconnectionInterface.h>
#include <KactusAPI/include/AdHocConnectionInterface.h>

#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/BusInterfaceInterfaceFactory.h>
#include <KactusAPI/include/ComponentAndInstantiationsParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/ExpressionFormatter.h>

#include <KactusAPI/include/PortsInterface.h>
#include <KactusAPI/include/ParametersInterface.h>

#include <IPXACTmodels/Component/validators/PortValidator.h>

class MessageMediator;

class Component;
class Field;
class Register;
class RegisterBase;
class AddressBlock;
class MemoryMap;
class FileSet;

class MemoryMapInterface;
class FileSetInterface;

class ComponentParameterFinder;
class ExpressionParser;
class PortValidator;
class ExpressionFormatter;

class ParameterValidator;
class MemoryMapValidator;

class Design;


//-----------------------------------------------------------------------------
//! Interface for accessing Kactus2 data using Python.
//-----------------------------------------------------------------------------
class PythonAPI
{
public:
    
    /*!
     *  IP-XACT std revision for use in python.
     */
    enum StdRev
    {
        Std14, // IEEE 1685-2014
        Std22  // IEEE 1685-2022
    };

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
     *    @return The current Kactus2 version.
     */
    std::string getVersion() const;

    /*!
     *	Get configured active library paths.
     *
     *    @return The configured active library paths.
     */
    std::vector<std::string> getActiveLibraryPaths() const;

    /*!
     *	Get all configured active and inactive library paths.
     *	    
     *    @return All of the configured library paths.
     */
    std::vector<std::string> getAllLibraryPaths() const;
    
    /*!
     *	Set a given library path as active or inactive. If set as active, also add the path if not found.
     *
     *    @param [in] path         The path to set active.
     *    @param [in] isActive     Indicates if the path should be set active or inactive.
     */
    void setLibraryPathActive(std::string const& path, bool isActive);

    /*!
     *	Add a new library path and set as active by default.
     *  
     *    @param [in] path         The path to add.
     *    @param [in] isActive     Indicates if the new path should be set active or not.
     */
    void addLibraryPath(std::string const& path, bool isActive = true);

    /*!
     *	Remove a library path. Prints error if trying to remove default path.
     *  
     *    @param [in] path     The path to remove.
     */
    void removeLibraryPath(std::string const& path);

    /*!
     *  Get the interface for accessing the component ports.
     *
     *    @return Interface for accessing the component ports.
     */
    PortsInterface* getPortsInterface() const;

    /*!
     *  Get the interface for accessing the component parameters.
     *
     *    @return Interface for accessing the component parameters.
     */
    ParametersInterface* getComponentParameterInterface() const;

    /*!
     *  Get the interface for accessing component memory maps.
     *
     *    @return Interface for accessing component memory maps.
     */
    MemoryMapInterface* getMapInterface();

    /*!
     *  Get the interface for accessing file sets.
     *
     *    @return Interface for accessing file sets.
     */
    FileSetInterface* getFileSetInterface();

    /*!
     *  Get the interface for accessing bus interfaces.
     *
     *    @return Interface for accessing bus interfaces.
     */
    BusInterfaceInterface* getBusInterface();

    /*!
     * Gets the paths where IP-XACT files are stored.
     *
     *    @return The paths where IP-XACT files are stored.
     */
    void setupLibrary(std::string const& settingsFileString);

    /*!
     * Sets new active library paths. First path is set as default.
     *
     *    @param [in] paths  The new locations that are currently available to store the files.
     */
    void setLibraryPaths(std::vector<std::string> const& paths) const;

    /*!
     * Get the default library path for IP-XACT files.
     *
     *    @return The default path for IP-XACT files.
     */
    std::string getDefaultLibraryPath() const;

    /*!
     * Set the default library path for IP-XACT files.
     *
     *    @param [in] path  The default path to set for IP-XACT files.
     */
    void setDefaultLibraryPath(std::string const& path) const;
    
    /*!
     * Import a source file (RTL) into the library as a component.
     *
     *    @param [in] filePath     The path to the file to import.
     *    @param [in] targetVLNV   The VLNV to use for the imported component.
     *    @param [in] overwrite    Overwrite the component in the library, if it already exists.
     *
     *    @return The number of imported IP-XACT components.
     */
    int importFile(std::string const& path, std::string const& vlnv, bool overwrite = false) const;

    /*!
     * Run a file generation for a component.
     *
     *    @param [in] format           The requested output format.
     *    @param [in] vlnv             The component VLNV to run the generation for.
     *    @param [in] viewName         The component view to run the generation for.
     *    @param [in] outputDirectory  The output directory for the generation results.
     */
     void generate(std::string const& format, std::string const& vlnv, std::string const& viewName,
        std::string const& outputDirectory) const;

     void generateInterconnect();

    /*!
     *  Get the number of files in the library.
     *
     *    @return The number of files in the library.
     */
    int getFileCount() const;

    /*!
     *  List all the VLNVs in the library.
     *
     *    @param[in] vendor   The desired vendor.
     *
     *    @return List of VLNVs in Vendor:Library:Name:Version format.
     */

    std::vector<std::string> listVLNVs(std::string const& vendor = std::string()) const;

    /*!
     *  List all the component VLNVs in the library.
     *
     *    @return List of component VLNVs in Vendor:Library:Name:Version format.
     */
    std::vector<std::string> listComponentVLNVs() const;

    /*!
     *  Check if the selected VLNV exists in the library.
     *
     *    @param [in] vendor      Vendor of the selected VLNV.
     *    @param [in] library     Library of the selected VLNV.
     *    @param [in] name        Name of the selected VLNV.
     *    @param [in] version     Version of the selected VLNV.
     *
     *    @return True, if the VLNV exists in the library, false otherwise.
     */
    bool vlnvExistsInLibrary(std::string const& vendor, std::string const& library, std::string const& name,
        std::string const& version) const;

    /*!
     *  Create a new component with the selected VLNV. Defaults to IP-XACT 2022.
     *
     *    @param [in] vendor      Vendor of the selected VLNV.
     *    @param [in] library     Library of the selected VLNV.
     *    @param [in] name        Name of the selected VLNV.
     *    @param [in] version     Version of the selected VLNV.
     *    @param [in] revision    IP-XACT standard revision to use.
     *
     *    @return True, if the component was created successfully, false otherwise.
     */
    bool createComponent(std::string const& vendor, std::string const& library, std::string const& name,
        std::string const& version, StdRev revision = StdRev::Std22);

    /*!
     *  Get the directory path of the selected VLNV document.
     *
     *    @param [in] vendor      Vendor of the selected VLNV.
     *    @param [in] library     Library of the selected VLNV.
     *    @param [in] name        Name of the selected VLNV.
     *    @param [in] version     Version of the selected VLNV.
     *
     *    @return Path of the selected VLNV document.
     */
    std::string getVLNVDirectory(std::string const& vendor, std::string const& library, std::string const& name,
        std::string const& version) const;

    /*!
     *  Get the name of the first view contained within the active component.
     *
     *    @return Name of the first view contained within the active component.
     */
    std::string getFirstViewName() const;

    /*!
     *  Set the selected component as active component.
     *
     *    @param [in] componentVLNV   VLNV of the selected component.
     *
     *    @return True, if the component exists, false otherwise.
     */
    bool openComponent(std::string const& vlnvString);

    /*!
     *  Remove the active component.
     */
    void closeOpenComponent();

    /*!
     *  Get the name of the active component.
     */

    std::string getComponentName() const;

    /*!
     *  Get the description of the active component.
     */

    std::string getComponentDescription() const;

    /*!
     *	Get the IP-XACT standard revision of the active component
     */
    std::string getComponentStdRevision() const;

    /*!
     *  Save the component to the library.
     */
    void saveComponent();


    /*!
     *  Set the available address blocks for the address block interface.
     *
     *    @param [in] mapName     Name of the memory map containing the desired address blocks.
     */
    void setBlocksForInterface(std::string const& mapName);

    /*!
     *  Set the available registers for the register interface.
     *
     *    @param [in] mapName     Name of the memory map containing the desired address blocks.
     *    @param [in] blockName   Name of the address block containing the desired registers.
     */
    void setRegistersForInterface(std::string const& mapName, std::string const& blockName);

    /*!
     *  Set the available fields for the field interface.
     *
     *    @param [in] mapName         Name of the memory map containing the desired address blocks.
     *    @param [in] blockName       Name of the address block containing the desired registers.
     *    @param [in] registerName    Name of the register containing the desired fields.
     */
    void setFieldsForInterface(std::string const& mapName, std::string const& blockName,
        std::string const& registerName);

    /*!
     *  Set the available resets for the reset interface.
     *
     *    @param [in] mapName         Name of the memory map containing the desired address blocks.
     *    @param [in] blockName       Name of the address block containing the desired registers.
     *    @param [in] registerName    Name of the register containing the desired fields.
     *    @param [in] fieldName       Name of the field containing the desired resets.
     */
    void setResetsForInterface(std::string const& mapName, std::string const& blockName,
        std::string const& registerName, std::string const& fieldName);

    /*!
     *  Set the available files for the file interface.
     *
     *    @param [in] setName     Name of the file set containing the available files.
     */
    void setFilesForInterface(std::string const& setName);

    /*!
     *  Set the available file builders for the file interface.
     *
     *    @param [in] setName     Name of the file set containing the available file builders.
     */
    void setFileBuildersForInterface(std::string const& setName);
        
    /*!
     *  Create a new design with the selected VLNV. Defaults to IP-XACT 2022.
     *
     *    @param [in] vendor      Vendor of the selected VLNV.
     *    @param [in] library     Library of the selected VLNV.
     *    @param [in] name        Name of the selected VLNV.
     *    @param [in] version     Version of the selected VLNV.
     *    @param [in] revision    IP-XACT standard revision to use.
     *
     *    @return True, if the design was created successfully, false otherwise.
     */
    bool createDesign(std::string const& vendor, std::string const& library, std::string const& name,
        std::string const& version, StdRev revision = StdRev::Std22);
    
    /*!
     *	Get the IP-XACT standard revision of the active design.
     */
    std::string getDesignStdRevision() const;

    /*!
     *  Set the selected design as active design.
     *
     *    @param [in] vlnvString  VLNV of the selected design.
     *
     *    @return True, if the design exists, false otherwise.
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
     *    @param [in] vlnvString      VLNV of the component within the selected component instance.
     *    @param [in] instanceName    Name for the new component instance.
     *
     *    @return True, if successful, false otherwise.
     */
    bool addComponentInstance(std::string const& vlnvString, std::string const& instanceName);

    /*!
     *  Remove the selected component instance from the active design.
     *
     *    @param [in] instanceName    Name of the selected component instance.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeComponentInstance(std::string const& instanceName);

    /*!
     *  Remove connections from the selected component instance.
     *
     *    @param [in] instanceName    Name of the selected component instance.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeInstanceConnections(std::string const& instanceName);

    /*!
     *  Remove ad hoc connections from the selected component instance.
     *
     *    @param [in] instanceName    Name of the selected component instance.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeInstanceAdHocConnections(std::string const& instanceName);

    /*!
     *  Set a new name for the selected component instance.
     *
     *    @param [in] currentName     Name of the selected component instance.
     *    @param [in] newName         New name for the component instance.
     *
     *    @return True, if successful, false otherwise.
     */
    bool renameInstance(std::string const& currentName, std::string const& newName);

    /*!
     *  Create an interconnection between two bus interfaces.
     *
     *    @param [in] startInstanceName   Name of the component instance containing the first bus interface.
     *    @param [in] startBus            Name of the first bus interface.
     *    @param [in] endInstanceName     Name of the component instance containing the second bus interface.
     *    @param [in] endBus              Name of the second bus interface.
     *
     *    @return True, if the connection was created, false otherwise.
     */
    bool createConnection(std::string const& startInstanceName, std::string const& startBus,
        std::string const& endInstanceName, std::string const& endBus);

    /*!
     *  Create a hierarchical interconnection between two bus interfaces.
     *
     *    @param [in] instanceName    Name of the component instance containing the selected bus interface.
     *    @param [in] instanceBus     Name of the component instance bus interface.
     *    @param [in] topBus          Name of the top component bus interface.
     *
     *    @return True, if the connection was created, false otherwise.
     */
    bool createHierarchicalConnection(std::string const& instanceName, std::string const& instanceBus,
        std::string const& topBus);

    /*!
     *  Remove an interconnection between two bus interfaces.
     *
     *    @param [in] startInstanceName   Name of the component instance containing the first bus interface.
     *    @param [in] startBus            Name of the first bus interface.
     *    @param [in] endInstanceName     Name of the component instance containing the second bus interface.
     *    @param [in] endBus              Name of the second bus interface.
     *
     *    @return True, if the connection was removed, false otherwise.
     */
    bool removeInstanceConnection(std::string const& startInstanceName, std::string const& startBus,
        std::string const& endInstanceName, std::string const& endBus);

    /*!
     *  Remove an interconnection between two bus interfaces.
     *
     *    @param [in] instanceName    Name of the component instance containing the selected bus interface.
     *    @param [in] instanceBus     Name of the component instance bus interface.
     *    @param [in] topBus          Name of the top component bus interface.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeHierarchicalConnection(std::string const& instanceName, std::string const& instanceBus,
        std::string const& topBus);

    /*!
     *  Set a new name for the selected interconnection.
     *
     *    @param [in] currentName     Name of the selected interconnection.
     *    @param [in] newName         New name for the interconnection.
     *
     *    @return True, if successful, false otherwise.
     */
    bool renameConnection(std::string const& currentName, std::string const& newName);

    /*!
     *  Create an ad hoc connection between two ports.
     *
     *    @param [in] startInstanceName   Name of the component instance containing the first port.
     *    @param [in] startPort           Name of the first port.
     *    @param [in] endInstanceName     Name of the component instance containing the second port.
     *    @param [in] endPort             Name of the second port.
     *
     *    @return True, if the connection was created, false otherwise.
     */
    bool createAdHocConnection(std::string const& startInstanceName, std::string const& startPort,
        std::string const& endInstanceName, std::string const& endPort);

    /*!
     *  Create a hierarchical ad hoc connection between two ports.
     *
     *    @param [in] instanceName    Name of the component instance containing the selected port.
     *    @param [in] instancePort    Name of the component instance port.
     *    @param [in] topPort         Name of the top component port.
     *
     *    @return True, if successful, false otherwise.
     */
    bool createHierarchicalAdHocConnection(std::string const& instanceName, std::string const& instancePort,
        std::string const& topPort);

    /*!
     *  Remove an ad hoc connection between two ports.
     *
     *    @param [in] startInstanceName   Name of the component instance containing the first port.
     *    @param [in] startPort           Name of the first port.
     *    @param [in] endInstanceName     Name of the component instance containing the second port.
     *    @param [in] endPort             Name of the second port.
     *
     *    @return True, if the ad hoc connection was removed, false otherwise.
     */
    bool removeInstanceAdHocConnection(std::string const& startInstanceName, std::string const& startPort,
        std::string const& endInstanceName, std::string const& endPort);

    /*!
     *  Remove a hierarchical ad hoc connection between two ports.
     *
     *    @param [in] instanceName    Name of the component instance containing the selected port.
     *    @param [in] instancePort    Name of the component instance port.
     *    @param [in] topPort         Name of the top component port.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeHierarchicalAdHocConnection(std::string const& instanceName, std::string const& instancePort,
        std::string const& topPort);

    /*!
     *  Set a new name for the selected ad hoc connection.
     *
     *    @param [in] currentName     Name of the selected ad hoc connection.
     *    @param [in] newName         New name for the ad hoc connection.
     *
     *    @return True, if successful, false otherwise.
     */
    bool renameAdHocConnection(std::string const& currentName, std::string const& newName);

private:

    /*!
     *  Check if the connection end points exist.
     *
     *    @param [in] startInstanceName   Name of the instance containing the first interface.
     *    @param [in] startBus            Name of the first interface.
     *    @param [in] endInstanceName     Name of the instance containing the second interface.
     *    @param [in] endBus              Name of the second interface.
     *    @param [in] isAdHocConnection   Flag for ad hoc connections.
     *
     *    @return True, if the connection end points exist, false otherwise.
     */
    bool connectionEndsCheck(QString const& startInstanceName, QString const& startBus,
        QString const& endInstanceName, QString const& endBus, bool isAdHocConnection);

    /*!
     *  Check if the selected component instance exists.
     *
     *    @param [in] instanceName    Name of the selected component instance.
     *
     *    @return True, if the component instance exists, false otherwise.
     */
    bool instanceExists(QString const& instanceName) const;

    /*!
     *  Check if the ad hoc connection end points exist.
     *
     *    @param [in] startComponent      Component containing the first port.
     *    @param [in] startBus            Name of the first port.
     *    @param [in] startInstanceName   Name of the instance containing the first port.
     *    @param [in] endComponent        Component containing the second port.
     *    @param [in] endBus              Name of the second port.
     *    @param [in] endInstanceName     Name of the instance containing the second port.
     *
     *    @return True, if the ad hoc connection end points exist, false otherwise.
     */
    bool endsCheckForAdHoc(QSharedPointer<const Component> startComponent, QString const& startBus,
        QString const& startInstanceName, QSharedPointer<const Component> endComponent, QString const& endBus,
        QString const& endInstanceName);

    /*!
     *  Check if the interconnection end points exist.
     *
     *    @param [in] startComponent      Component containing the first bus interface.
     *    @param [in] startBus            Name of the first bus interface.
     *    @param [in] startInstanceName   Name of the instance containing the first bus interface.
     *    @param [in] endComponent        Component containing the second bus interface.
     *    @param [in] endBus              Name of the second bus interface.
     *    @param [in] endInstanceName     Name of the instance containing the second bus interface.
     *
     *    @return True, if the interconnection end points exist, false otherwise.
     */
    bool endsCheckForInterconnection(QSharedPointer<const Component> startComponent, QString const& startBus,
        QString const& startInstanceName, QSharedPointer<const Component> endComponent, QString const& endBus,
        QString const& endInstanceName);

    /*!
     *  Remove the selected interconnection.
     *
     *    @param [in] connectionName  Name of the selected interconnection.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeConnection(std::string const& connectionName);

    /*!
     *  Remove the selected ad hoc connection.
     *
     *    @param [in] connectionName  Name of the selected ad hoc connection.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeAdHocConnection(std::string const& connectionName);

    /*!
     *  Get the selected document.
     *
     *    @param [in] vlnvString  VLNV of the selected document in string form.
     *
     *    @return The selected document.
     */
    QSharedPointer<Document> getDocument(QString const& vlnvString) const;

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
     *    @param [in] mapName     Name of the selected memory map.
     *
     *    @return The selected memory map.
     */
    QSharedPointer<MemoryMap> getMemoryMap(QString const& mapName) const;

    /*!
     *  Get the selected address block.
     *
     *    @param [in] containingMap   Memory map containing the address block.
     *    @param [in] blockName       Name of the selected address block.
     *
     *    @return The selected address block.
     */
    QSharedPointer<AddressBlock> getAddressBock(QSharedPointer<MemoryMap> containingMap, QString const& blockName)
        const;

    /*!
     *  Get the selected register.
     *
     *    @param [in] containingBlock     Address block containing the registers.
     *    @param [in] registerName        Name of the selected register.
     *
     *    @return The selected register.
     */
    QSharedPointer<Register> getRegister(QSharedPointer<AddressBlock> containingBlock, QString const& registerName)
        const;

    /*!
     *  Get the selected field.
     *
     *    @param [in] containingRegister  Register containing the fields.
     *    @param [in] fieldName           Name of the selected field.
     *
     *    @return The selected field.
     */
    QSharedPointer<Field> getField(QSharedPointer<Register> containingRegister, QString const& fieldName) const;

    /*!
     *  Get the selected file set.
     *
     *    @param [in] setName     Name of the selected file set.
     *
     *    @return The selected file set.
     */
    QSharedPointer<FileSet> getFileSet(QString const& setName) const;

    /*!
     *  Send an error text for a non-existing memory map.
     *
     *    @param [in] mapName     Name of the memory map.
     */
    void sendMemoryMapNotFoundError(QString const& mapName) const;

    /*!
     *  Send an error text for a non-existing address block.
     *
     *    @param [in] mapName     Name of the memory map containing the address block.
     *    @param [in] blockName   Name of the address block.
     */
    void sendAddressBlockNotFoundError(QString const& mapName, QString const& blockName) const;

    /*!
     *  Send an error text for a non-existing register.
     *
     *    @param [in] mapName         Name of the memory map containing the address block.
     *    @param [in] blockName       Name of the address block containing the register.
     *    @param [in] registerName    Name of the register.
     */
    void sendRegisterNotFoundError(QString const& mapName, QString const& blockName, QString const& registerName)
        const;

    /*!
     *  Send an error text for a non-existing field.
     *
     *    @param [in] mapName         Name of the memory map containing the address block.
     *    @param [in] blockName       Name of the address block containing the register.
     *    @param [in] registerName    Name of the register containing the field.
     *    @param [in] fieldName       Name of the field.
     */
    void sendFieldNotFoundError(QString const& mapName, QString const& blockName, QString const& registerName,
        QString const& fieldName) const;

    /*!
     *  Send an error text for a non-existing file set.
     *
     *    @param [in] setName     Name of the file set.
     */
    void sendFileSetNotFoundError(QString const& setName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface to the library.
    LibraryInterface* library_{ KactusAPI::getLibrary() };

    //! Message handler.
    MessageMediator* messager_{ KactusAPI::getMessageChannel() };

    //! Component parameter finder.
    QSharedPointer<ComponentParameterFinder> parameterFinder_{ new ComponentAndInstantiationsParameterFinder(nullptr) };

    //! Parser for expressions.
    QSharedPointer<ExpressionParser> expressionParser_{ new IPXactSystemVerilogParser(parameterFinder_) };

    //! Formatter for expressions.
    QSharedPointer<ExpressionFormatter>expressionFormatter_{ new ExpressionFormatter(parameterFinder_) };

    //! Currently active component.
    QSharedPointer<Component> activeComponent_{ nullptr };

    //! Currently active design.
    QSharedPointer<Design> activeDesign_{ nullptr };

    //! Validator for ports.
    QSharedPointer<PortValidator> portValidator_{ new PortValidator(expressionParser_,
        QSharedPointer<QList<QSharedPointer<View> > >()) };

    //! Validator for parameters.
    QSharedPointer<ParameterValidator> parameterValidator_{ new ParameterValidator(expressionParser_,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22) };

    //! Validator for memory maps.
    QSharedPointer<MemoryMapValidator> mapValidator_{ nullptr };

    //! Interface for accessing the component ports. 
    PortsInterface* portsInterface_{ new PortsInterface(portValidator_, expressionParser_, expressionFormatter_) };

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_{ BusInterfaceInterfaceFactory::createBusInterface(parameterFinder_,
        expressionFormatter_, expressionParser_,
        QSharedPointer<Component>(new Component(VLNV(), Document::Revision::Unknown)), library_) };

    //! Interface for accessing the component parameters.
    ParametersInterface* componentParameterInterface_{ new ParametersInterface(parameterValidator_, 
        expressionParser_, expressionFormatter_) };

    //! Interface for accessing memory maps.
    MemoryMapInterface* mapInterface_{ nullptr };

    //! Interface for accessing file sets.
    FileSetInterface* fileSetInterface_{ nullptr };

    //! Interface for accessing interconnections.
    InterconnectionInterface* connectionInterface_{ new InterconnectionInterface() };

    //! Interface for accessing ad hoc connections.
    AdHocConnectionInterface* adhocConnectionInterface_{ new AdHocConnectionInterface() };

    //! Interface for accessing component instances.
    ComponentInstanceInterface* instanceInterface_{ new ComponentInstanceInterface(connectionInterface_, 
        adhocConnectionInterface_) };

};

#endif // !PYTHON_API_H
