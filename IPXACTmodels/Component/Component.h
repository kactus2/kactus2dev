//-----------------------------------------------------------------------------
// File: Component.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 06.10.2015
//
// Description:
// Describes the ipxact:component element.
//-----------------------------------------------------------------------------

#ifndef COMPONENT_H
#define COMPONENT_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QStringList>
#include <QMultiMap>
#include <QMap>

class AddressSpace;
class BusInterface;
class Channel;
class Choice;
class ComponentGenerator;
class ComponentInstantiation;
class Cpu;
class DesignInstantiation;
class DesignConfigurationInstantiation;
class FileSet;
class MemoryMap;
class Mode;
class IndirectInterface;
class OtherClockDriver;
class Port;
class PowerDomain;
class RemapState;
class View;
class ResetType;

// Kactus2 extension classes.

class File;
class ComProperty;
class SystemView;
class ComInterface;
class ApiInterface;
class FileDependency;

//-----------------------------------------------------------------------------
//! Describes the ipxact:component element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Component: public Document
{

public:

	/*!
     *  The constructor.
	 *
	 *      param [in] vlnv     The vlnv that is set for the component.
	 */
	explicit Component(const VLNV &vlnv, Document::Revision revision);

	//! Copy constructor.
	Component(const Component &other);

	//! Assignment operator.
	Component& operator=(const Component &other);

	/*!
     *  The destructor
	 */
	~Component() final = default;

	/*!
     *  Clone this component and return pointer to the copy.
	 * 
	 *      @return Pointer to the cloned component.
	 */
	QSharedPointer<Document> clone() const final;

	/*!
     *  Is this a bus component.
	 *
	 *      @return true if this is a bus component, false otherwise.
	 */
	bool isBus() const;

	/*!
     *  Does the component contain a channel-element.
	 *
	 *      @return True if a channel element is found.
	 */
	bool isChannel() const;

	/*!
     *  Does the component contain a transparent bridge element.
	 *
	 *      @return True if a transparent bridge element is found.
	 */
	bool isTransparentBridge() const;

	/*!
	 *  Does the component contain an opaque bridge.
	 *
	 *      @return True if an opaque bridge is found.
	 */
	bool isOpaqueBridge() const;

  	/*!
     *  Is this component a cpu.
	 *
	 *      @return True, if this is a cpu, otherwise false.
	 */
	bool isCpu() const;

	/*!
     *  Is this component a hierarchical component or not.
	 *
	 *      @return True if the component has design with subcomponents. False if the component is a flat component.
	 */
	bool isHierarchical() const;

    QSharedPointer<QList<QSharedPointer<PowerDomain> > > getPowerDomains() const;

	/*!
     *  Get list of the bus interfaces of a component.
	 *
	 *      @return A list containing pointers to the bus interfaces.
	 */
    QSharedPointer<QList<QSharedPointer<BusInterface> > > getBusInterfaces() const;

	/*!
     *  Set the bus interfaces for the component.
	 *
	 *      @param [in] newBusInterfaces    A list containing the pointers to the new bus interfaces.
	 *
	 */
    void setBusInterfaces(QSharedPointer<QList<QSharedPointer<BusInterface> > > newBusInterfaces);

	/*! \brief Check which interface contains the named port.
	 *
	 * \param portName The name of the port that is searched in interfaces.
	 *
	 * \return The interface that contains the port.
	*/
	QSharedPointer<BusInterface> getInterfaceForPort(const QString& portName) const;

    /*!
     *  Get the bus interfaces containing port maps referencing the selected port.
     *
     *      @param [in] portName    The name of the selected port.
     *
     *      return A list of interfaces containing references to the selected port.
     */
    QSharedPointer<QList<QSharedPointer<BusInterface> > > getInterfacesUsedByPort(QString const& portName) const;

	/*!
     *  Get the indirect interfaces of a component.
	 *
	 *      @return The indirect interfaces of the component.
	 */
    QSharedPointer<QList<QSharedPointer<IndirectInterface> > > getIndirectInterfaces() const;

	/*!
     * Get this component's channels.
	 *
	 *      @return A list containing component's channels.
	 */
    QSharedPointer<QList<QSharedPointer<Channel> > > getChannels() const;

	/*!
     *  Set this component's channels.
	 *
	 *      @param [in] newChannels     A list containing the new channels to be set.
	 */
    void setChannels(QSharedPointer<QList<QSharedPointer<Channel> > > newChannels);

	/*!
     *  Get this component's remap states.
	 *
	 *      @return A list containing component's remap states.
	 */
    QSharedPointer<QList<QSharedPointer<RemapState> > > getRemapStates() const;

    /*!
     *  Get this component's modes.
     *
     *      @return The component's modes.
     */
    QSharedPointer<QList<QSharedPointer<Mode> > > getModes() const;

    /*!
     *  Set the remap states.
     *
     *      @param [in] newRemapStates  A list containing the new remap states.
     */
    void setRemapStates(QSharedPointer<QList<QSharedPointer<RemapState> > > newRemapStates);

	/*!
     *  Get this component's address spaces.
	 *
	 *      @return A list containing component's address spaces.
	 */
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > getAddressSpaces() const;

   	/*!
     *  Set this component's address spaces
	 *
	 *      @param [in] addressSpaces   A list containing the address spaces to be set.
	 */
    void setAddressSpaces(QSharedPointer<QList<QSharedPointer<AddressSpace> > > newAddressSpaces);

	/*!
     *  Get this component's memory maps.
	 *
	 *      @return A list containing component's memory maps.
	 */
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > getMemoryMaps() const;

	/*!
     *  Set this components memory maps.
	 *
	 *      @param [in] newMemoryMaps   A list containing the memory maps to be set.
	 */
    void setMemoryMaps(QSharedPointer<QList<QSharedPointer<MemoryMap> > > newMemoryMaps);

    /*!
     *  Get this component's model information.
	 *
	 *      @return Pointer to this component's model information.
	 */
	QSharedPointer<Model> getModel() const;

	/*!
     *  Set this component's model information.
	 *
	 *      @param [in] model   Pointer to the model information to be set.
	 */
    void setModel(QSharedPointer<Model> newModel);

	/*!
     *  Get the views of the component.
	 *
	 *      @return A list containing pointers to the views.
	 */
    QSharedPointer<QList<QSharedPointer<View> > > getViews() const;

    /*!
     *  Set the views.
     *
     *      @param [in] newViews    A list containing the new views..
     */
    void setViews(QSharedPointer<QList<QSharedPointer<View> > > newViews);

    /*!
     *  Get the component instantiations.
     *
     *      @return A list containing component instantiations.
     */
    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > getComponentInstantiations() const;

    /*!
     *  Set the component instantiations.
     *
     *      @param [in] newInstantiation    A list containing the new component instantiations.
     */
    void setComponentInstantiations(
        QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > newInstantiation);

    /*!
     *  Get the design instantiations.
     *
     *      @return A list containing the design instantiations.
     */
    QSharedPointer<QList<QSharedPointer<DesignInstantiation> > > getDesignInstantiations() const;

    /*!
     *  Set the design instantiations.
     *
     *      @param [in] newInstantiations   A list containing the new design instantiations.
     */
    void setDesignInstantiations(QSharedPointer<QList<QSharedPointer<DesignInstantiation> > > newInstantiations);

    /*!
     *  Get the design configuration instantiations.
     *
     *      @return A list containing the design configuration instantiations.
     */
    QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > >
        getDesignConfigurationInstantiations() const;

    /*!
     *  Set the design configuration instantiations.
     *
     *      @param [in] newInstantiations   A list containing the new design configuration instantiations.
     */
    void setDesignConfigurationInstantiations(
        QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > newInstantiations);

	/*!
     *  Get the ports contained in this component.
	 *
	 *      @return A list containing pointers to the ports.
	 */
    QSharedPointer<QList<QSharedPointer<Port> > > getPorts() const;

    /*!
     *  Set the ports.
     *
     *      @param [in] newPorts    A list containing the new ports.
     */
    void setPorts(QSharedPointer<QList<QSharedPointer<Port> > > newPorts);

	/*!
     *  Get this components component generators.
	 *
	 *      @return A list containing component's component generators.
	 */
    QSharedPointer<QList<QSharedPointer<ComponentGenerator> > > getComponentGenerators() const;

	/*!
     *  Set this component's component generators.
	 *
	 *      @param [in] newGenerators   A list containing the component generators to be set.
	 */
    void setComponentGenerators(QSharedPointer<QList<QSharedPointer<ComponentGenerator> > > newGenerators);

   	/*!
     *  Get this component's choices.
	 *
	 *      @return A list containing component's choices.
	 */
	QSharedPointer<QList<QSharedPointer<Choice> > > getChoices() const;

    /*!
     *  Set the choices.
     *
     *      @param [in] newChoices  A list containing the new choices.
     */
    void setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > newChoices);

	/*!
     *  Get the file sets of this component.
	 *
	 *      @return A list containing pointers to the file sets.
	 */
    QSharedPointer<QList<QSharedPointer<FileSet> > > getFileSets() const;

	/*!
     *  Set this component's file sets.
	 *
	 *      @param [in] newFileSets     A list containing the file sets to be set.
	 */
    void setFileSets(QSharedPointer<QList<QSharedPointer<FileSet> > > newFileSets);
    
	/*!
     *  Get this component's cpus.
	 *
	 *      @return QList containing component's cpus.
	 */
    QSharedPointer<QList<QSharedPointer<Cpu> > >getCpus() const;

	/*!
     *  Set this component's cpus.
	 *
	 *      @param [in] cpus    A list containing the cpus to be set.
	 */
    void setCpus(QSharedPointer<QList<QSharedPointer<Cpu> > > newCpus);

	/*!
     *  Get this component's other clock drivers.
	 *
	 *      @return A list containing component's other clock drivers.
	 */
    QSharedPointer<QList<QSharedPointer<OtherClockDriver> > > getOtherClockDrivers() const;

	/*!
     *  Set other clock drivers for this component.
	 *
	 *      @param [in] newOtherClockDrivers    A list containing the other clock drivers to be set.
	 */
    void setOtherClockDrivers(QSharedPointer<QList<QSharedPointer<OtherClockDriver> > > newOtherClockDrivers);

    /*!
     *  Get this component's reset types.
     *
     *      @return A list containing the reset types.
     */
    QSharedPointer<QList<QSharedPointer<ResetType> > > getResetTypes() const;

    /*!
     *  Set reset types for this component.
     *
     *      @param [in] newResetTypes   A list containing the new reset types.
     */
    void setResetTypes(QSharedPointer<QList<QSharedPointer<ResetType> > > newResetTypes);

    /*!
     *  Returns the list of SW properties.
     *
     *      @return A list of sw properties.
     */
    QList<QSharedPointer<ComProperty> > getSWProperties() const;

    /*!
     *  Sets the SW properties.
     *
     *      @param [in] properties  A list of properties to set.
     */
    void setSWProperties(QList<QSharedPointer<ComProperty> > newProperties);

	/*!
     *  Get the system views of the component.
	 *
	 *      @return QList containing pointers to the system views.
	 */
    QList<QSharedPointer<SystemView> > getSystemViews() const;

    /*!
     *  Set the system views.
     *
     *      @param [in] newSystemViews  List containing the new system views.
     */
    void setSystemViews(QList<QSharedPointer<SystemView> > newSystemViews);

    /*!
     *  Returns the COM interfaces.
     *
     *      @return A list of com interfaces.
     */
    QList<QSharedPointer<ComInterface> > getComInterfaces() const;

    /*!
     *  Set the com interfaces.
     *
     *      @param [in] newComInterfaces    A list of the new com interfaces.
     */
    void setComInterfaces(QList<QSharedPointer<ComInterface> > newComInterfaces);

    /*!
     *  Returns the API interfaces.
     *
     *      @return A list of API interfaces.
     */
    QList<QSharedPointer<ApiInterface> > getApiInterfaces() const;

    /*!
     *  Set the api interfaces.
     *
     *      @param [in] newApiInterfaces    A list of new api interfaces.
     */
    void setApiInterfaces(QList<QSharedPointer<ApiInterface> > newApiInterfaces);
    
    /*!
     *  Get this component's file dependencies.
	 *
	 *      @return QList containing component's file dependencies.
	 */
    QList<QSharedPointer<FileDependency> > getFileDependencies() const;

    /*!
     *  Set the file dependencies.
     *
     *      @param [in] newFileDependencies     A list of new file dependencies.
     */
    void setFileDependendencies(QList<QSharedPointer<FileDependency> > newFileDependencies);

    /*!
     *  Returns the pending dependency list for writing.
     *
     *      @return A list of pending dependencies.
     */
    QList<QSharedPointer<FileDependency> > getPendingFileDependencies() const;

    /*!
     *  Set the pending file dependencies.
     *
     *      @param [in] newFileDependencies     A list of new pending file dependencies.
     */
    void setPendingFileDependencies(QList<QSharedPointer<FileDependency> > newFileDependencies);

    /*!
     *  Returns the list of source directories.
     *
     *      @return A list of source directiories.
     */
    QStringList getSourceDirectories() const;
    
    /*!
     *  Sets the source directories where to automatically scan files to file sets.
     *
     *      @param [in] sourceDirs The source directories to set.
     */
    void setSourceDirectories(QStringList const& sourceDirs);

    /*!
     *  Gets the author of the component.
     *
     *      @return The component author.
     */
    QString getAuthor() const;
    
    /*!
     *  Sets the author of the component.
     *
     *      @param [in] author   The name of the component author.
     */
    void setAuthor(QString const& author);

	/*!
     *  Get list of the files needed by this component.
	 *
	 *      @return A list containing the paths to the files.
	 */
	QStringList getDependentFiles() const final;

	/*!
     *  Get list of the VLNVs needed by this component.
	 *
	 *      @return QList containing pointers to the VLNVs.
	 */
	QList<VLNV> getDependentVLNVs() const final;
    
    QStringList getPowerDomainNames() const;

    /*!
     *  Get the names of the remap states.
     *
     *      @return A list containing the names of the remap states.
     */
    QStringList getRemapStateNames() const;
    

    /*!
     *  Get the names of the memory maps contained within this component.
	 *
	 *      @return QStringList containing the memory map names.
	 */
	QStringList getMemoryMapNames() const;

	/*!
     *  Checks if the component contains views.
	 *
	 *      @return True if views exist.
	 */
	bool hasViews() const;

	/*!
     *  Check if the component has a view with given name.
	 *
	 *      @param [in] viewName    The name of the view to search for.
	 *
	 *      @return True if the view is found.
	 */
	bool hasView(const QString& viewName) const;

	/*!
     *  Get the names of the views of the component.
	 *
	 *      @return QStringList containing the view names.
	 */
	QStringList getViewNames() const;
            
	/*!
     *  Get the names of the CPU elements within component.
	 *
	 *      @return QStringList containing the CPU names.
	 */
	QStringList getCpuNames() const;

	/*!
     *  Get the hierarchical views of a component.
	 *
	 *      return QStringList containing the names of the hierarchical HW views.
	 */
	QStringList getHierViews() const;

	/*!
     *  Get the non-hierarchical views of a component.
	 *
	 *      @return QStringList containing the names of the non-hierarchical HW views.
	 */
	QStringList getFlatViews() const;

	/*!
     *  Get the design of a hierarchical component.
	 *
     *      @return A vlnv of a design or configuration that is used within this component.
	 */
	VLNV getHierRef(const QString viewName = QString()) const;

	/*!
     *  Get the hierarchical references this component contains.
	 *
	 *      @return QList<VLNV> contains the VLNVs of the hierarchical references.
	 */
	QList<VLNV> getHierRefs() const;

    /*!
     *  Checks if the component contains system views.
	 *
	 *      @return True if system views exist.
	 */
	bool hasSystemViews() const;

	/*!
     *  Check if the component has a system view with given name.
	 *
	 *      @param [in] viewName    The name of the system view to search for.
	 *
	 *      @return True if the view is found.
	 */
	bool hasSystemView(const QString& viewName) const;

	/*!
     *  Get the names of the system views of the component.
	 *
	 *      @return QStringList containing the system view names.
	 */
	QStringList getSystemViewNames() const;

	/*!
     *  Find a named system view within a component.
	 *
	 *      @param [in] name    The name of the system view to search for.
	 *
	 *      @return Pointer to the specified system view. Null pointer if the system view was not found.
	 */
    QSharedPointer<SystemView> findSystemView(QString const& name) const;

	/*!
     *  Find a system view which refers to the given VLNV.
	 *
	 *      @param [in] hierRef     The VLNV to the design/configuration that is searched among the views.
	 *
	 *      @return Pointer to the system view. Null pointer if no system view is found.
	 */
	QSharedPointer<SystemView> findSystemView(const VLNV& hierRef) const;

	/*!
     *  Get the system design of a hierarchical component.
	 *
     *      @return A vlnv of a design or configuration that is used within this component.
     */
	VLNV getHierSystemRef(QString const& viewName = QString()) const;

	/*!
     *  Get the hierarchical system references this component contains.
	 *
	 *      @return QList<VLNV> contains the VLNVs of the hierarchical system references.
	 */
	QList<VLNV> getHierSystemRefs() const;

	/*!
     *  Get names of the file sets stored in the component.
	 *
	 *      @return QStringList containing the names of the file sets of the component.
	 */
	QStringList getFileSetNames() const;

	/*!
     *  Get the specified file set if one exists.
	 *
	 *      @param [in] name    Name of the file set that is wanted.
	 *
	 *      @return Pointer to the file set. If file set was not found, creates and returns pointer to it.
	 */
	QSharedPointer<FileSet> getFileSet(const QString& name) const;

	/*!
     *  Check if the component has the named file set or not.
	 *
	 *      @param [in] fileSetName     The name of the file set to search
	 *
	 *      @return True if the file set is found.
	 */
	bool hasFileSet(const QString& fileSetName) const;

	/*!
     *  Checks if the component contains file sets.
	 *
	 *      @return True if file sets exist.
	 */
	bool hasFileSets() const;

	/*!
	 *  Returns all file pointers in all file sets that reference the given file name.
	 *
	 *      @param [in]  filename   The filename to search for.
	 *      @param [out] files      All found files that reference the given file name.
	 */
    QList<QSharedPointer<File> > getFiles(QString const& fileName) const;

	/*!
     *  Find the files that match the specified file type.
	 *
	 *      @param [in] fileType    The file type to search within the files.
	 *
	 *      @return QStringList containing the file paths of files matching the file type.
	 */
	QStringList findFilesByFileType(const QString& fileType) const;

	/*!
     *  Find the file set by the file set id.
	 *
	 *      @param [in] id  Identifies the file set.
	 *
	 *      @return Pointer to the file set found, null pointer if none found.
	 */
	QSharedPointer<FileSet> findFileSetById(QString const& id) const;

	/*!
     *  Get file paths of all specified file sets that match the specified file types.
	 *
	 *      @param [in] fileSetNames    Contains the file sets to search from.
	 *      @param [in] fileTypes       Contains the file types that are searched among the file sets.
	 *
	 *      @return QStringList containing the relative file paths to the files.
	 */
	QStringList getFilesFromFileSets(const QStringList& fileSetNames, const QStringList& fileTypes) const;

	
	/*!
     *  Check if the component has the given file in it's file sets.
	 *
	 *      @param [in] fileName    The name of the file to search.
	 *
	 *      @return True if file is found in one of the file sets.
	 */
	bool hasFile(const QString& fileName) const;

	/*!
     *  Get list of names of all the bus interfaces in this component.
	 *
	 *      @return QStringList containing the names of the bus interfaces.
	 */
	QStringList getBusInterfaceNames() const;

	/*!
     *  Get a A bus interface.
	 *
	 *      @param [in] name    Contains the name of the wanted bus interface.
	 *
	 *      @return A pointer to the busInterface instance or null pointer.
	 */
	QSharedPointer<BusInterface> getBusInterface(const QString& name) const;

	/*!
     *  Get list of the slave interfaces that refer to given memory map.
	 *
	 *      @param [in] memoryMap   The name of the memory map to search for.
	 *
	 *      @return QStringList containing names of the slave interfaces.
	 */
	QStringList getTargetInterfaces(const QString& memoryMap) const;

	/*!
     *  Get list of the master and mirrored master interfaces that refer to given address space.
	 *
	 *      @param [in] addressSpace    The name of the address space to search for.
	 *
	 *      @return QStringList containing names of the master and mirrored master interfaces.
	 */
	QStringList getInitiatorInterfaces(const QString& addressSpace) const;

	/*!
     *  Get list of the master and mirrored master interface names.
	 *
	 *      @return QStringList containing the interface names.
	 */
	QStringList getInitiatorInterfaces() const;

	/*!
     *  Checks if the component contains bus interfaces.
	 *
	 *      @return bool True if bus interfaces exist.
	 */
	bool hasInterfaces() const;

	/*!
     *  Check if the component contains the named interface.
	 *
	 *      @param [in] interfaceName   The name of the interface to search for.
	 *
	 *      @return True if the interface was found.
	 */
	bool hasInterface(const QString& interfaceName) const;

    /*!
     *  Finds the COM interface with the given name.
     *
     *      @param [in] name The name of the COM interface.
     *
     *      @return The COM interface, or 0 if there is no COM interface with the given name.
     */
	QSharedPointer<ComInterface> getComInterface(QString const& name);

    /*!
     *  Finds the API interface with the given name.
     *
     *      @param [in] name The name of the API interface.
     *
     *      @return The API interface, or 0 if there is no API interface with the given name.
     */
	QSharedPointer<ApiInterface> getApiInterface(QString const& name);

	/*!
     *  Get the named port of a component
	 *
	 *      @param [in] name    QString containing the name of the port
	 *
	 *      @return Pointer to the port instance
	 */
	QSharedPointer<Port> getPort(const QString& name) const;

	/*!
     *  Get the ports that are found in specified interface.
	 *
	 *      @param [in] interfaceName   The name of interface that's ports are requested.
	 *
	 *      @return List containing pointers to the ports.
	 */
    QList<QSharedPointer<Port> > getPortsMappedInInterface(const QString& interfaceName) const;

	/*!
     *  Check if the component has the specified port or not.
	 *
	 *      @param [in] name    The name of the port to be searched
	 *
	 *      @return True if the port is found
	 */
	bool hasPort(const QString& name) const;

	/*!
     *  Get list of port names for this component.
	 *
	 *      @return QStringList containing the port names.
	 */
	QStringList getPortNames() const;

	/*!
     *  Checks if the component contains ports.
	 *
	 *      @return bool True if ports exist.
	 */
	bool hasPorts() const;
	
	/*!
	 *	Checks if the component has wire ports.
	 *  
	 * 	    @return True if wire ports exist.
	 */
	bool hasWires() const;

	/*!
	 *	Checks if the component has transactional ports.
	 *  
	 * 	    @return True if transactional ports exist.
	 */
	bool hasTransactionals() const;

	/*!
     *  Get the names of the address spaces stored in this component.
	 *
	 *      @return QStringList containing the names of the address spaces.
	 */
	QStringList getAddressSpaceNames() const;

	/*!
     *  Check if the component contains at least one local memory map.
	 *
	 *      @return True if at least one local memory map is found.
	 */
	bool hasLocalMemoryMaps() const;

	/*!
     *  Get the directories which this component needs.
	 *
	 *      @return QStringList containing the relative directory paths.
	 */
	QStringList getDependentDirs() const final;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Specifies all the power domains for this component.
    QSharedPointer<QList<QSharedPointer<PowerDomain> > > powerDomains_ =
        QSharedPointer<QList<QSharedPointer<PowerDomain> > >(new QList<QSharedPointer<PowerDomain> >());

    //! Specifies all the bus interfaces for this component.
	QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces_ =
        QSharedPointer<QList<QSharedPointer<BusInterface> > >(new QList<QSharedPointer<BusInterface> >());

    //! Specifies all the indirect interfaces for this component.
    QSharedPointer<QList<QSharedPointer<IndirectInterface> > > indirectInterfaces_ = 
        QSharedPointer<QList<QSharedPointer<IndirectInterface> > >(new QList<QSharedPointer<IndirectInterface> >());

    //! Specifies the interconnection between interfaces within component.
	QSharedPointer<QList<QSharedPointer<Channel> > > channels_ =
        QSharedPointer<QList<QSharedPointer<Channel> > >(new QList<QSharedPointer<Channel> >());
	
	//! Contains the remapStates.
	QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates_ =
        QSharedPointer<QList<QSharedPointer<RemapState> > >(new QList<QSharedPointer<RemapState> >());

    //! Contains the modes.
    QSharedPointer<QList<QSharedPointer<Mode> > > modes_ =
        QSharedPointer<QList<QSharedPointer<Mode> > >(new QList<QSharedPointer<Mode> >());

	//! Contains the addressSpaces.
	QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces_ =
        QSharedPointer<QList<QSharedPointer<AddressSpace> > >(new QList<QSharedPointer<AddressSpace> >());

	//! Contains the memoryMaps
	QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps_ =
        QSharedPointer<QList<QSharedPointer<MemoryMap> > >(new QList<QSharedPointer<MemoryMap> >());

	//! Contains the model.
    QSharedPointer<Model> model_ = QSharedPointer<Model> (new Model());

    //! Contains the componentGenerators.
    QSharedPointer<QList<QSharedPointer<ComponentGenerator> > > componentGenerators_ =
        QSharedPointer < QList<QSharedPointer<ComponentGenerator> > >(new QList<QSharedPointer<ComponentGenerator> >());

	//! Contains the choices.
	QSharedPointer<QList<QSharedPointer<Choice> > > choices_ =
        QSharedPointer<QList<QSharedPointer<Choice> > >(new QList<QSharedPointer<Choice> >());

	//! Contains the fileSets.
	QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets_ =
        QSharedPointer<QList<QSharedPointer<FileSet> > >(new QList<QSharedPointer<FileSet> >());
    
	//! Contains the cpus.
	QSharedPointer<QList<QSharedPointer<Cpu> > > cpus_ =
        QSharedPointer<QList<QSharedPointer<Cpu> > >(new QList<QSharedPointer<Cpu> >());
    
	//! Contains the otherClockDrivers.
	QSharedPointer<QList<QSharedPointer<OtherClockDriver> > > otherClockDrivers_ =
        QSharedPointer<QList<QSharedPointer<OtherClockDriver> > >(new QList<QSharedPointer<OtherClockDriver> >());

    //! Contains the reset types.
    QSharedPointer<QList<QSharedPointer<ResetType> > > resetTypes_ =
        QSharedPointer<QList<QSharedPointer<ResetType> > >(new QList<QSharedPointer<ResetType> >());

    //! Contains the pending file dependencies.
    QList<QSharedPointer<FileDependency> > pendingFileDependencies_;

};


#endif // COMPONENT_H
