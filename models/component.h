/* 
 *
 *  Created on: 16.7.2010
 *      Author: Antti Kamppi
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <common/KactusAttribute.h>

#include "librarycomponent.h"
#include "generaldeclarations.h"
#include "view.h"
#include "port.h"
#include "modelparameter.h"

#include <QList>
#include <QDomDocument>
#include <QString>
#include <QDomElement>
#include <QSharedPointer>
#include <QStringList>
#include <QFile>
#include <QXmlStreamWriter>

// implemented classes that only need to be declared
class BusInterface;
class ComponentGenerator;
class Cpu;
class FileSet;
class MemoryMap;
class OtherClockDriver;
class Parameter;
class RemapState;
class Model;
class AddressSpace;
class Channel;
class Choice;
class File;

/*! \brief Equals to spirit:component element in IP-Xact specification
 *
 * Class component is a generalization from base class LibraryComponent.
 * Component can be used to describe cores, peripherals and buses.
 */
class Component: public LibraryComponent {

public:

	/*! \brief The constructor
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing
	 *
	 * \param doc A reference to a QDomDocument that holds the information
	 * to parsed into component model.
	 */
	Component(QDomDocument &doc);

	/*! \brief The constructor
	 *
	 * Constructs an empty component which has VLNV information.
	 *
	 * \param vlnv The vlnv that is set for the component.
	 */
	Component(const VLNV &vlnv);

	/*! \brief The default constructor
	 *
	 * Constructs an empty component which is not valid.
	 */
	Component();

	//! \brief Copy constructor
	Component(const Component &other);

	//! \brief Assignment operator
	Component &operator=(const Component &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~Component();

	/*! \brief Clone this component and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit LibraryComponent.
	 *
	 * \return QSharedPointer<LibraryComponent> Pointer to the cloned component.
	*/
	virtual QSharedPointer<LibraryComponent> clone();

	/*! \brief Set the vlnv
	 *
	 * \param vlnv Reference to the vlnv to set
	 *
	*/
	virtual void setVlnv(const VLNV& vlnv);

	/*! \brief Is this component a cpu
	 *
	 * \return boolean value
	 */
	bool isCpu() const;

	/*! \brief Is this a bus component
	 *
	 * \return boolean value
	 */
	bool isBus() const;

	/*! \brief Does the component contain a channel-element.
	 *
	 * \return True if a channel element is found.
	*/
	bool isChannel() const;

	/*! \brief Does the component contain a bridge element.
	 *
	 * \return True if a bridge-element is found.
	*/
	bool isBridge() const;

	/*! \brief Is this component a hierarchical component or not.
	 *
	 * If the component contains a view with a hierarchyRef, this is true
	 *
	 * \return True if the component has design with subcomponents. False if
	 * the component is a flat component.
	 */
	bool isHierarchical() const;

	/*! \brief Get this component's address spaces
	 *
	 * \return QList containing component's adress spaces
	 */
	const QList<QSharedPointer<AddressSpace> >& getAddressSpaces() const;

	/*! \brief Get the names of the address spaces stored in this component.
	 *
	 * \return QStringList containing the names of the address spaces.
	*/
	const QStringList getAddressSpaceNames() const;

	//QMap<QString, QSharedPointer<BusInterface> >* getBusIfPointers();

	/*! \brief Get this component's channels
	 *
	 * \return QList containing component's channels
	 */
	const QList<QSharedPointer<Channel> >& getChannels() const;

	/*! \brief Get this component's choices
	 *
	 * \return QList containing component's choices
	 */
	const QList<QSharedPointer<Choice> >& getChoices() const;

	/*! \brief Get this components component generators
	 *
	 * \return QList containing component's component generators
	 */
	const QList<QSharedPointer<ComponentGenerator> >& getCompGenerators() const;

	/*! \brief Get this component's cpus
	 *
	 * \return QList containing component's cpus
	 */
	const QList<QSharedPointer<Cpu> >& getCpus() const;

	/*! \brief Get this component's memory maps
	 *
	 * \return QList containing component's memory maps
	 */
	const QList<QSharedPointer<MemoryMap> >& getMemoryMaps() const;

	/*! \brief Get this component's model information
	 *
	 * \return a Pointer to this component's model information.
	 */
	Model *getModel();

	/*! \brief Get this component's other clock drivers
	 *
	 * \return QList containing component's other clock drivers
	 */
	QList<QSharedPointer<OtherClockDriver> >& getOtherClockDrivers();

	/*! \brief Get this component's parameters
	 *
	 * \return QList containing component's parameters
	 */
	QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Get this component's remap states
	 *
	 * \return QList containing component's remap states
	 */
	const QList<QSharedPointer<RemapState> >& getRemapStates() const;

	/*! \brief Get the attributes for the component
	 *
	 * \return A reference to QMap containing the attributes
	 */
	const QMap<QString, QString>& getAttributes() const;

	/*! \brief Set the attributes for the component
	 *
	 * \param attributes A reference to a QMap containing the attributes.
	 */
	void setAttributes(const QMap<QString, QString>& attributes);

	/*! \brief Set this component's address spaces
	 *
	 * \param addressSpaces QList containing the address spaces to be set
	 */
	void setAddressSpaces(const QList<QSharedPointer<AddressSpace> > &addressSpaces);

	/*! \brief Set this component's channels
	 *
	 * \param channels QList containing the channels to be set
	 */
	void setChannels(const QList<QSharedPointer<Channel> > &channels);

	/*! \brief Set this component's choices
	 *
	 * \param choices QList containing the choices to be set
	 */
	void setChoices(const QList<QSharedPointer<Choice> > &choices);

	/*! \brief Set this component's component generators
	 *
	 * \param compGererators QList containing the component generators to be set
	 */
	void setCompGenerators(const
	QList<QSharedPointer<ComponentGenerator> > &compGenerators);

	/*! \brief Set this component's cpus
	 *
	 * \param cpus QList containing the cpus to be set
	 */
	void setCpus(const QList<QSharedPointer<Cpu> > &cpus);

	/*! \brief Set this components memory maps
	 *
	 * \param memoryMaps QList containing the memory maps to be set
	 */
	void setMemoryMaps(const QList<QSharedPointer<MemoryMap> > &memoryMaps);

	/*! \brief Set this component's model information
	 *
	 * \param model A pointer to the model information to be set
	 */
	void setModel(Model *model);

	/*! \brief Set other clock drivers for this component
	 *
	 * \param otherClockDrivers QList containing the other clock drivers to be set
	 */
	void setOtherClockDrivers(const
	QList<QSharedPointer<OtherClockDriver> > &otherClockDrivers);

	/*! \brief Set parameters for this component
	 *
	 * \param parameters QList containing the parameters to be set
	 */
	void setParameters(const QList<QSharedPointer<Parameter> > &parameters);

	/*! \brief Set remap states for this component
	 *
	 * \param remapStates QList containing the remapStates to be set
	 */
	void setRemapStates(const QList<QSharedPointer<RemapState> > &remapStates);

	/*! \brief Get list of the files needed by this component.
	 *
	 * \return QStringList A list containing the paths to the files.
	 */
	virtual const QStringList getDependentFiles() const;

	/*! \brief Get list of the VLNVs needed by this component.
	 *
	 * \return QList containing pointers to the VLNVs.
	 */
	virtual const QList<VLNV> getDependentVLNVs() const;

	/*! \brief Write the document contents into file.
	 *
	 * Prints all information stored in this document into specified file
	 * generating a valid IP-Xact document.
	 *
	 * \param file A reference to QFile instance representing the file to write
	 * the data into.
	 *
	 * \exception Write_error Occurs if the class or one of it's subclasses is
	 * not valid IP-Xact at the moment of writing.
	 */
	virtual void write(QFile& file);

	/*! \brief Get the vhdl files and their libraries used in this component.
	 *
	 * \return QList containing the file paths and library names
	 *
	 * Exception guarantee: basic
	 * \exception Invalid_file Occurs if file type is not defined for a file.
	 */
	QList<General::LibraryFilePair> getVhdlLibraries() const;

	/*! \brief Get the vhdl libraries needed by given view.
	 *
	 * \param viewName The name of the view that specifies used file sets.
	 *
	 * \return QList containing the file names and library names.
	*/
	QList<General::LibraryFilePair> getLibraries(const QString& viewName) const;

	/*! \brief Get the verilog files and their libraries used in this component.
	 *
	 * \return QList<General::LibraryFilePair> containing the libraries and files.
	 * 
	 * Exception guarantee: basic
	 * \exception Invalid_file Occurs if file type is not defined for a file.
	*/
	QList<General::LibraryFilePair> getVerilogLibraries() const;

	/*! \brief Create an empty channel and return a pointer to it.
	 *
	 * \return Pointer to the created channel element.
	*/
	Channel* createChannel();

	/*! \brief Get the model parameters of the component
	 *
	 * \return QMap containing the model parameters.
	*/
	const QMap<QString, QSharedPointer<ModelParameter> > getModelParameters() const;

	/*! \brief Remove a channel from the component
	 *
	 * \param channelName The name of the channel to remove
	 *
	*/
	void removeChannel(const QString& channelName);

	/*! \brief Get the file that wraps the quartus pin map.
	 *
	 * \return Pointer to the file wrapper.
	*/
	File* getQuartusPinmap() const;

    /*!
     *  Sets the product hierarchy level of the component.
     *
     *      @param [in] prodHier The product hierarchy.
     */
    void setComponentHierarchy(KactusAttribute::ProductHierarchy prodHier);

	/*! \brief Set the implementation type of the component.
	 *
	 * \param implementation The implementation type.
	 *
	*/
	void setComponentImplementation(KactusAttribute::Implementation implementation);

    /*!
     *  Sets the firmness level of the component.
     *
     *      @param [in] firmness The firmness of the component.
     */
    void setComponentFirmness(KactusAttribute::Firmness firmness);

    /*!
     *  Sets the SW type of the component.
     *
     *      @param [in] type The SW type of the component.
     */
    void setComponentSWType(KactusAttribute::SWType type);


	/*! \brief Get the hierarchy level of the component.
	 * 
	 * If the hierarchy is undefined then ProductHierarchy::KTS_IP is returned
	 * as default.
	 *
	 * \return KactusAttribute::ProductHierarchy specifies the hierarchy level.
	*/
	KactusAttribute::ProductHierarchy getComponentHierarchy() const;

	/*! \brief Get the component implementation type.
	 *
	 * If the implementation is not defined then Implementation::HW is returned
	 * as default.
	 *
	 * \return KactusAttribute::Implementation specifies the implementation.
	*/
	KactusAttribute::Implementation getComponentImplementation() const;

	/*! \brief Get the firmness of the component.
	 * 
	 * If the firmness is not defined the Firmness::KTS_MUTABLE is returned as default.
	 *
	 * \return KactusAttribute::Firmness specifies the firmness of the component.
	*/
	KactusAttribute::Firmness getComponentFirmness() const;

    /*!
     *  Returns the SW type of the component.
     *
     *      @remarks If the firmness is not defined, SWType::KTS_SW_MAPPING is returned as the default value.
	 */
	KactusAttribute::SWType getComponentSWType() const;

	/*! \brief Checks if the component contains model parameters.
	 *
	 * \return bool True if model parameters exist.
	*/
	bool hasModelParameters() const;

	/*! \brief Checks if the component contains parameters.
	 *
	 * \return bool True if parameters exist.
	*/
	bool hasParameters() const;

	/*! \brief Checks if the component contains views.
	*
	* \return bool True if views exist.
	*/
	bool hasViews() const;

	/*! \brief Get the description of specified view.
	* 
	* If view is not found a null string is returned.
	*
	* \param viewName Identifies the view that's description is wanted.
	*
	* \return QString Contains the description for the view.
	*/
	QString getViewDescription(const QString& viewName) const;

	/*! \brief Remove view from the component.
	*
	* \param viewName The name of the view to remove
	*
	*/
	void removeView(const QString& viewName);

	/*! \brief Get the number of views the component has.
	*
	* \return The number of views found in the meta data.
	*/
	int viewCount() const;

	/*! \brief Create an empty view and return a pointer to it.
	*
	* \return Pointer to the created view element.
	*/
	View* createView();

	/*! \brief Get the views of the component
	*
	* \return QList containing pointers to the views.
	*/
	const QList<QSharedPointer<View> > getViews() const;

	/*! \brief Get the names of the views of the component.
	*
	* \return QStringList containing the view names.
	*/
	QStringList getViewNames() const;

	/*! \brief Find a named view within a component.
	*
	* \param name The name of the view to search for.
	*
	* \return Pointer to the specified view. Null pointer if the view was not
	* found.
	*/
	View* findView(const QString name) const;

	/*! \brief Get the hierarchical views of a component
	*
	* \return QStringList containing the names of the hierarchical views.
	*/
	QStringList getHierViews() const;

	/*! \brief Add a new view to the component's model.
	*
	* \param newView A pointer to the component's new view.
	*
	*/
	void addView(View* newView);

	/*! \brief Get the design of a hierarchical component.
	*
	* This function searches the views of the component for a named view.
	* If view if a given name is not found then invalid vlnv is returned.
	* If the component is not a hierarchical component then invalid vlnv is
	* returned.
	* If name is not given then the first found VLNV is returned.
	*
	* \return A vlnv of a design or configuration that is used within this
	* component. Invalid vlnv if design is not found.
	*/
	VLNV getHierRef(const QString viewName = QString()) const;

	/*! \brief Get the hierarchical references this component contains.
	*
	* \return QList<VLNV> contains the VLNVs of the hierarchical references.
	*/
	QList<VLNV> getHierRefs() const;

	/*! \brief Set hierarchical reference for given view.
	* 
	* If view name is not specified then the first hierarchical view is used.
	*
	* \param vlnv The reference to set as hierarchical reference.
	* \param viewName The name of the view to set the hierarchical reference for.
	*
	*/
	void setHierRef(const VLNV& vlnv, const QString& viewName = QString());

	/*! \brief Get this component's file sets
	*
	* \return QList containing component's file sets
	*/
	const QList<QSharedPointer<FileSet> >& getFileSets() const;

	/*! \brief Get names of the file sets stored in the component.
	*
	* \return QStringList containing the names of the file sets of the component.
	*/
	QStringList getFileSetNames() const;

	/*! \brief Set this component's file sets
	*
	* \param fileSets QList containing the file sets to be set
	*/
	void setFileSets(const QList<QSharedPointer<FileSet> > &fileSets);

	/*! \brief Get the specified file set if one exists
	*
	* \param name Name of the file set that is wanted.
	*
	* \return pointer to the file set. Null pointer if file set was not found.
	*/
	FileSet* getFileSet(const QString& name) const;

	/*! \brief Add a new file set to the component
	*
	* \param fileSet A pointer to the file set to be added.
	*/
	void addFileSet(FileSet* fileSet);

	/*! \brief Remove the specified filetSet from the component.
	*
	* \param fileSetName Contains the name of the file set to be removed.
	*
	*/
	void removeFileSet(const QString& fileSetName);

	/*! \brief Create a new empty file set for the component
	*
	* \return Pointer to the created file set.
	*/
	FileSet* createFileSet();

	/*! \brief Check if the component has the named file set or not.
	*
	* \param fileSetName The name of the file set to search
	*
	* \return bool True if the file set is found.
	*/
	bool hasFileSet(const QString& fileSetName) const;

	/*! \brief Checks if the component contains file sets.
	*
	* \return bool True if file sets exist.
	*/
	bool hasFileSets() const;

	/*! \brief Get the files that are referenced by the specified file set.
	*
	* Note: The files paths returned by this function are relative to the
	* containing xml-file.
	*
	* \param fileSetName The name of the file set that's files are needed.
	*
	* \return QStringList containing the files referenced by the file set.
	*/
	QStringList getFiles(const QString fileSetName);

	/*! \brief Get all files referenced by this component.
	*
	* \return QStringList containing the file paths.
	*/
	QStringList getFiles() const;

	/*! \brief Get all the RTL files listed in component's file sets.
	*
	* \return QList containing pointers to the files.
	*/
	QList<QSharedPointer<File> > getRTLFiles() const;

	/*! \brief Update the file paths within file sets to match the new location.
	*
	* \param other Reference to the component to copy file paths from
	* \param sourcePath The path to the location of other component's xml.
	* \param targetPath The path to the location of this component's xml.
	*
	*/
	void updateFiles(const Component& other, const QString& sourcePath,
		const QString& targetPath);

	/*! \brief Change a file's name within a file set.
	*
	* \param from QString containing current file name (file path)
	* \param to QString containing the new name for file (file path)
	*
	*/
	void changeFileName(const QString& from, const QString& to);

	/*! \brief Check if the component has the given file in it's file sets.
	*
	* \param fileName The name of the file to search.
	*
	* \return bool True if file is found in one of the file sets.
	*/
	bool hasFile(const QString& fileName) const;

	/*! \brief Get this component's bus interfaces
	*
	* \return QMap containing component's bus interfaces
	*/
	const QMap<QString, QSharedPointer<BusInterface> >& getBusInterfaces() const;

	/*! \brief Get list of names of all the bus interfaces in this component.
	*
	* \return QStringList containing the names of the bus interfaces.
	*/
	QStringList getBusInterfaceNames() const;

	/*! \brief Set this component's bus interfaces
	*
	* \param busInterfaces QMap containing the bus interfaces to be set
	*/
	void setBusInterfaces(const QMap<QString, QSharedPointer<BusInterface> >
		&busInterfaces);

	/*! \brief Get a pointer to a bus interface.
	*
	* If named bus interface is not found then null pointer is returned.
	*
	* \param name Contains the name of the wanted bus interface.
	*
	* \return A pointer to the busInterface instance or null pointer.
	*/
	BusInterface* getBusInterface(const QString& name);

	/*! \brief Add a new bus interface to the component.
	*
	* \param busInterface Pointer to the new bus interface to be added.
	*
	* \return True if bus interface was successfully added. False if a
	* bus interface with same name already existed.
	*/
	bool addBusInterface(QSharedPointer<BusInterface> busInterface);

	/*! \brief Update the given busInterface to the component.
	* 
	* If the bus interface already exists in the component then this function
	* updates the key of the map so it stays valid with bus interface's name.
	* 
	* If the bus interface does not belong to the component then it is added
	* to the map containing bus interfaces.
	* 
	* \param busInterface Pointer to the bus interface to update.
	*
	*/
	void updateBusInterface(BusInterface* busInterface);

	/*! \brief Remove the specified bus interface from the component.
	*
	* \param busifName The name of the bus interface to remove.
	*
	*/
	void removeBusInterface(const QString& busifName);

	/*! \brief Remove the specified bus interface from the component.
	*
	* \param busInterface Pointer to the bus interface to remove.
	*
	*/
	void removeBusInterface(const BusInterface* busInterface);

	/*! \brief Create an empty bus interface to the component
	*
	* \return Pointer to the created empty bus interface.
	*/
	BusInterface* createBusInterface();

	/*! \brief Checks if the component contains bus interfaces.
	*
	* \return bool True if bus interfaces exist.
	*/
	bool hasInterfaces() const;

	/*! \brief Get the named port of a component
	*
	* This function can be used to find a specific port of a component. If
	* named port can't be found a null pointer is returned.
	*
	* \param name QString containing the name of the port
	*
	* \return Pointer to the port instance
	*/
	Port* getPort(const QString& name) const;

	/*! \brief Get the ports contained in this component.
	*
	* \return QMap containing pointers to the ports.
	*/
	const QMap<QString, QSharedPointer<Port> > getPorts() const;

	/*! \brief Get the direction of the given port.
	* 
	* If port is not found on the component then invalid direction is given.
	* 
	* \param portName The name of the port.
	*
	* \return General::Direction specifying the port direction.
	*/
	General::Direction getPortDirection(const QString& portName) const;

	/*! \brief Get the width of the given port.
	*
	* \param port Identifies the port in the component.
	*
	* \return int The number of bits in the port. -1 if unspecified or port is
	* not found.
	*/
	int getPortWidth(const QString& port) const;

	/*! \brief Get the left bound of the specified port.
	*
	* If port is not found then -1 is returned.
	*
	* \param port The name of the port.
	*
	* \return int the left bound of the port.
	*/
	int getPortLeftBound(const QString& port) const;

	/*! \brief Get the right bound of the specified port.
	* 
	* If port is not found the -1 is returned.
	* 
	* \param port The name of the port.
	*
	* \return int the right bound of the port.
	*/
	int getPortRightBound(const QString& port) const;

	/*! \brief Check if the component has the specified port or not.
	*
	* \param name The name of the port to be searched
	*
	* \return True if the port is found
	*/
	bool hasPort(const QString& name) const;

	/*! \brief Get list of port names for this component.
	*
	* \return QStringList containing the port names.
	*/
	QStringList getPortNames() const;

	/*! \brief Add a new port to the model.
	*
	* \param port Pointer to the port to add
	*
	* \return True if port was successfully added. False if a port with same 
	* name already existed in the model. If port exists in the model then 
	* nothing is added.
	*/
	bool addPort(QSharedPointer<Port> port);

	/*! \brief Remove a port from the model.
	*
	* \param portName The name of the port to remove.
	*
	*/
	void removePort(const QString& portName);

	/*! \brief Rename a port in the model
	*
	* \param oldName The old name of the port.
	* \param newName The new name to be set for the port.
	*
	* \return True if renaming was successful. False if port with old name was
	* not found.
	*/
	bool renamePort(const QString& oldName, const QString& newName);

	/*! \brief Checks if the component contains ports.
	*
	* \return bool True if ports exist.
	*/
	bool hasPorts() const;

private:

	/*! \brief Specifies all the interfaces for this component.
	 * OPTIONAL spirit:busInterfaces
	 *
	 * Key = name of the interface.
	 * Value = Pointer to the bus interface instance.
	 */
	QMap<QString, QSharedPointer<BusInterface> > busInterfaces_;

	/*! \brief specifies the interconnection between interfaces within component
	 * OPTIONAL spirit:channels
	 */
	QList<QSharedPointer<Channel> > channels_;

	/*! \brief Contains the remapStates
	 * OPTIONAL spirit:remapStates
	 * Specifies the combination of logic states on the component ports and
	 * translates them into a logical name.
	 */
	QList<QSharedPointer<RemapState> > remapStates_;

	/*! \brief Contains the addressSpaces
	 * OPTIONAL spirit:addressSpaces
	 * Specifies the addressable as seen from businterfaces with an interface
	 * mode of slave.
	 */
	QList<QSharedPointer<AddressSpace> > addressSpaces_;

	/*! \brief Contains the memoryMaps
	 * OPTIONAL spirit:memoryMaps
	 * Specifies the addressable area as seen from businterfaces with an
	 * interfacemode of slave.
	 */
	QList<QSharedPointer<MemoryMap> > memoryMaps_;

	/*! \brief Contains the pointer to a model.
	 * OPTIONAL spirit:model
	 * Specifies all the different views, ports, and model configuration
	 * parameters of a component.
	 */
	QSharedPointer<Model> model_;

	/*! \brief Contains the componentGenerators.
	 * OPTIONAL spirit:componentGenerators
	 * Specifies a list of generator programs attached to this component
	 */
	QList<QSharedPointer<ComponentGenerator> > compGenerators_;

	/*! \brief Contains the choices.
	 * OPTIONAL spirit:choices
	 * Specifies multiple enumerated lists.
	 */
	QList<QSharedPointer<Choice> > choices_;

	/*! \brief Contains the fileSets.
	 * OPTIONAL spirit:fileSets
	 * Specifies groups of files and possibly their function for reference by
	 * other sections of this component description.
	 */
	QList<QSharedPointer<FileSet> > fileSets_;

	/*! \brief Contains the cpus.
	 * OPTIONAL spirit:cpus
	 * Indicates this component contains programmable processors.
	 */
	QList<QSharedPointer<Cpu> > cpus_;

	/*! \brief contains the otherClockDrivers.
	 * OPTIONAL spirit:otherClockDrivers
	 * Specifies any clock signals which are referenced by implementation
	 * constraints but are not external ports of the component.
	 */
	QList<QSharedPointer<OtherClockDriver> > otherClockDrivers_;

	/*! \brief Contains the parameters.
	 * OPTIONAL spirit:parameters
	 * Describes anu parameter that can be used to configure or hold information
	 * related to this component.
	 */
	QList<QSharedPointer<Parameter> > parameters_;

	//! \brief Contains the attributes for the spirit:component element.
	//! OPTIONAL
	QMap<QString, QString> attributes_;
};


#endif /* COMPONENT_H_ */
