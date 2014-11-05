/* 
 *
 *  Created on: 16.7.2010
 *      Author: Antti Kamppi
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include "librarycomponent.h"
#include "generaldeclarations.h"
#include "view.h"
#include "port.h"
#include "modelparameter.h"

#include "ipxactmodels_global.h"

#include <QList>
#include <QDomDocument>
#include <QString>
#include <QDomElement>
#include <QSharedPointer>
#include <QStringList>
#include <QFile>
#include <QXmlStreamWriter>
#include <QMultiMap>
#include <QMap>

// implemented classes that only need to be declared
class SWView;
class SystemView;
class BusInterface;
class ComInterface;
class ApiInterface;
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
class ComProperty;
class FileDependency;

/*! \brief Equals to spirit:component element in IP-Xact specification
 *
 * Class component is a generalization from base class LibraryComponent.
 * Component can be used to describe cores, peripherals and buses.
 */
class IPXACTMODELS_EXPORT Component: public LibraryComponent {

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
	virtual QSharedPointer<LibraryComponent> clone() const;

	/*! \brief Write the document contents into file.
	*
	* Prints all information stored in this document into specified file
	* generating a valid IP-Xact document.
	*
	* \param file A reference to QFile instance representing the file to write
	* the data into.
	*/
	virtual void write(QFile& file);

	/*! \brief Check the validity of the component.
	 * 
	 * This function should be used to check if the component is in valid state
	 * before writing the xml to the disk.
	 * 
	 * \param errorList The error messages of the detected errors are appended to
	 * this list.
	 *
	 * \return bool True if the component was valid.
	*/
	virtual bool isValid(QStringList& errorList) const;

	/*! \brief Check the validity of the component.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid() const;

	/*! \brief Set the vlnv
	 *
	 * \param vlnv Reference to the vlnv to set
	 *
	*/
	virtual void setVlnv(const VLNV& vlnv);

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

    /*!
     *  Returns true if the component has hierarchical SW.
     */
    bool isHierarchicalSW() const;

	/*! \brief Get this component's channels
	 *
	 * \return QList containing component's channels
	 */
	const QList<QSharedPointer<Channel> >& getChannels() const;

	/*! \brief Get the channels of this component.
	 *
	 * \return QList containing the pointers to the channels.
	*/
	QList<QSharedPointer<Channel> >& getChannels();

	/*! \brief Get this component's choices
	 *
	 * \return QList containing component's choices
	 */
	QSharedPointer<QList<QSharedPointer<Choice> > > getChoices() const;

	/*! \brief Get this components component generators
	 *
	 * \return QList containing component's component generators
	 */
	const QList<QSharedPointer<ComponentGenerator> >& getCompGenerators() const;

	/*! \brief Get this component's cpus
	 *
	 * \return QList containing component's cpus
	 */
	QList<QSharedPointer<Cpu> >& getCpus();

	/*! \brief Get component's cpus
	 *
	 * \return QList containing component's cpus
	*/
	const QList<QSharedPointer<Cpu> >& getCpus() const;

	/*! \brief Get the names of the CPU elements within component.
	 *
	 * Method: 		getCpuNames
	 * Full name:	Component::getCpuNames
	 * Access:		public 
	 *
	 *
	 * \return QStringList containing the CPU names.
	*/
	QStringList getCpuNames() const;

	/*! \brief Is this component a cpu
	*
	* \return boolean value
	*/
	bool isCpu() const;

	/*! \brief Remove the named cpu from the component.
	 *
	 * \param cpuName The name of the cpu to remove.
	 *
	*/
	void removeCpu(const QString& cpuName);

	/*! \brief Create a new empty cpu-element.
	 *
	 * \return Pointer to the created cpu.
	*/
	Cpu* createCpu();

	/*! \brief Get this component's memory maps
	 *
	 * \return QList containing component's memory maps
	 */
	const QList<QSharedPointer<MemoryMap> >& getMemoryMaps() const;

	/*! \brief Get component's memory maps
	 *
	 * \return QList containing pointers to the component's memory maps.
	*/
	QList<QSharedPointer<MemoryMap> >& getMemoryMaps();

	/*! \brief Get the names of the memory maps contained within this component.
	 *
	 * \return QStringList containing the memory map names.
	*/
	QStringList getMemoryMapNames() const;

    /*!
     *  Retrieves the memory map with the given name.
     *
     *      @param [in] name The name of the memory map to retrieve.
     *
     *      @return The corresponding memory map, or null pointer if not found.
     */
    QSharedPointer<MemoryMap> getMemoryMap(QString const& name) const;

	/*! \brief Set this components memory maps
	*
	* \param memoryMaps QList containing the memory maps to be set
	*/
	void setMemoryMaps(const QList<QSharedPointer<MemoryMap> > &memoryMaps);

	/*! \brief Get this component's model information
	 *
	 * \return a Pointer to this component's model information.
	 */
	QSharedPointer<Model> getModel();

	/*! \brief Get this component's other clock drivers
	 *
	 * \return QList containing component's other clock drivers
	 */
	QList<QSharedPointer<OtherClockDriver> >& getOtherClockDrivers();

	/*! \brief Get this component's other clock drivers.
	 *
	 * \return QList containing component's other clock driver.
	*/
	const QList<QSharedPointer<OtherClockDriver> >& getOtherClockDrivers() const;

	/*! \brief Get this component's parameters
	 *
	 * \return QList containing component's parameters
	 */
	QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Get this component's parameters
	 *
	 * \return QList containing component's parameters
	 */
	const QList<QSharedPointer<Parameter> >& getParameters() const;

	/*! \brief Get names of the parameters stored in the component.
	 *
	 * Method: 		getParameterNames
	 * Full name:	Component::getParameterNames
	 * Access:		public 
	 *
	 *
	 * \return QStringList containing the parameter names.
	*/
	QStringList getParameterNames() const;

	/*! \brief Set parameters for this component
	*
	* \param parameters QList containing the parameters to be set
	*/
	void setParameters(const QList<QSharedPointer<Parameter> > &parameters);

	/*! \brief Get the model parameters of the component.
	*
	* \return QList containing the model parameters.
	*/
	QList<QSharedPointer<ModelParameter> >& getModelParameters();

	/*! \brief Get the model parameters of the component.
	*
	* \return QList containing the model parameters.
	*/
	const QList<QSharedPointer<ModelParameter> >& getModelParameters() const;

	/*! \brief Get the names of the model parameters of the component.
	*
	* \return QStringList containing the model parameter names.
	*/
	QStringList getModelParameterNames() const;

	/*! \brief Get the names of the parameters and model parameters contained in the component.
	*
	* Method: 		getAllParameterNames
	* Full name:	Component::getAllParameterNames
	* Access:		public 
	*
	*
	* \return QStringList containing parameter and model parameter names.
	*/
	QStringList getAllParameterNames() const;

	/*! \brief Get the default value set for specified parameter or model parameter.
	 *
	 * Method: 		getAllParametersDefaultValue
	 * Full name:	Component::getAllParametersDefaultValue
	 * Access:		public 
	 * 
	 * First model parameters is searched and then parameters. The first 
	 * parameter/model parameter matching the paramName is selected.
	 * 
	 * \param paramName Identifies the parameter or model parameter.
	 *
	 * \return QString contains the specified default value.
	*/
	QString getAllParametersDefaultValue(const QString& paramName) const;

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

	/*! \brief Set this component's channels
	 *
	 * \param channels QList containing the channels to be set
	 */
	void setChannels(const QList<QSharedPointer<Channel> > &channels);

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

	/*! \brief Get the vhdl files and their libraries used in this component.
	 *
	 * \return QList containing the file paths and library names
	 */
	QList<General::LibraryFilePair> getVhdlLibraries() const;

	/*! \brief Get the library names of vhdl files contained in given view.
	 *
	 * \param viewName The name of the view that's file sets are searched.
	 *
	 * \return QStringList contains the names of the libraries.
	*/
	QStringList getVhdlLibraries(const QString& viewName) const;

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
	*/
	QList<General::LibraryFilePair> getVerilogLibraries() const;

	/*! \brief Create an empty channel and return a pointer to it.
	 *
	 * \return Pointer to the created channel element.
	*/
	Channel* createChannel();

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

    /*! \brief Get the hierarchy level of the component.
	 * 
	 * If the hierarchy is undefined then ProductHierarchy::KTS_IP is returned
	 * as default.
	 *
	 * \return KactusAttribute::ProductHierarchy specifies the hierarchy level.
	*/
	KactusAttribute::ProductHierarchy getComponentHierarchy() const;

	/*! \brief Get the implementation of the component.
	 *
	 * Method: 		getComponentImplementation
	 * Full name:	Component::getComponentImplementation
	 * Access:		public 
	 *
	 *
	 * \return KactusAttribute::Implementation specifying the component implementation.
	*/
	KactusAttribute::Implementation getComponentImplementation() const;

	/*! \brief Get the firmness of the component.
	 * 
	 * If the firmness is not defined the Firmness::KTS_MUTABLE is returned as default.
	 *
	 * \return KactusAttribute::Firmness specifies the firmness of the component.
	*/
	KactusAttribute::Firmness getComponentFirmness() const;

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

	/*! \brief Check all HW, SW and System views of the component for the given view name.
	 *
	 * Method: 		checkAllViews
	 * Full name:	Component::checkAllViews
	 * Access:		public 
	 *
	 * \param viewName The view name which is searched.
	 *
	 * \return True if the named view is found in at least one view category.
	*/
	bool checkAllViews(const QString& viewName) const;

	/*! \brief Checks if the component contains views.
	*
	* \return bool True if views exist.
	*/
	bool hasViews() const;

	/*! \brief Check if the component has a view with given name.
	 *
	 * \param viewName The name of the view to search for.
	 *
	 * \return bool True if the view is found.
	*/
	bool hasView(const QString& viewName) const;

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

	/*! \brief Get the views of the component
	 *
	 * \return QList containing pointers to the views.
	*/
	QList<QSharedPointer<View> >& getViews();

	/*! \brief Get the names of the views of the component.
	*
	* \return QStringList containing the view names.
	*/
	QStringList getViewNames() const;

	/*! \brief Get the type of the view that uses the given VLNV.
	 * 
	 * If a view that uses the given vlnv is not found then KTS_IMPLEMENTATION_COUNT
	 * is returned.
	 *
	 * \param vlnv The vlnv that is searched among the views.
	 *
	 * \return KactusAttribute::Implementation specifies the type of the view.
	*/
	KactusAttribute::Implementation getViewType(const VLNV& vlnv) const;

	/*! \brief Get the name of the view that uses the given VLNV.
	 * 
	 * If a view that uses the given vlnv is not found then empty string is returned.
	 *
	 * \param vlnv The vlnv that is searched among the views.
	 *
	 * \return The name of the view that refers to the vlnv.
	*/
	QString getViewName(const VLNV& vlnv) const;

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
	* \return QStringList containing the names of the hierarchical HW views.
	*/
	QStringList getHierViews() const;

	/*! \brief Get the non-hierarchical views of a component.
	 *
	 * Method: 		getFlatViews
	 * Full name:	Component::getFlatViews
	 * Access:		public 
	 *
	 *
	 * \return QStringList containing the names of the non-hierarchical HW views.
	*/
	QStringList getFlatViews() const;

	/*! \brief Get the entity name of the component.
	 *
	 * \param viewName The name of the view that's entity name is requested.
	 *
	 * \return QString contains the name of the entity.
	*/
	QString getEntityName(const QString& viewName) const;

	/*! \brief Get the name of the architecture for the component.
	 *
	 * \param viewName The name of the view that's architecture is wanted.
	 *
	 * \return QString contains the architecture name.
	*/
	QString getArchitectureName(const QString& viewName) const;

	/*! \brief Add a new view to the component's model.
	*
	* \param newView A pointer to the component's new view.
	*
	*/
	void addView(View* newView);

	/*! \brief Create empty non-hierarchical view for the component.
	 * 
	 * The name for the view is "rtl" in IP and SoC hierarchy levels and "flat" for others.
	 *
	*/
	void createEmptyFlatView();

	/*! \brief Create a hierarchical view for the component.
	 * 
	 * The name for the view is "structural" in IP and SoC levels and "hierarchical" for other.
	 * 
	 * \param hierRef The reference to a design or design configuration.
	 *
	*/
	void createHierarchicalView(const VLNV& hierRef);

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

	/*! \brief Get the hierarchical references this component contains.
	 *
	 * \return QMap containing the hierarchical references with the matching view names.
	 *		Key = The name of the view.
	 *		Value = The hierarchical reference of the view.
	*/
	QMap<QString, VLNV> getHierRefNames() const;

	/*! \brief Set hierarchical reference for given view.
	* 
	* If view name is not specified then the first hierarchical view is used.
	*
	* \param vlnv The reference to set as hierarchical reference.
	* \param viewName The name of the view to set the hierarchical reference for.
	*
	*/
	void setHierRef(const VLNV& vlnv, const QString& viewName = QString());

    /*! \brief Checks if the component contains SW views.
	*
	* \return bool True if SW views exist.
	*/
	bool hasSWViews() const;

	/*! \brief Check if the component has a SW view with given name.
	 *
	 * \param viewName The name of the SW view to search for.
	 *
	 * \return bool True if the view is found.
	*/
	bool hasSWView(const QString& viewName) const;

	/*! \brief Get the description of specified view.
	* 
	* If view is not found a null string is returned.
	*
	* \param viewName Identifies the view that's description is wanted.
	*
	* \return QString Contains the description for the view.
	*/
	QString getSWViewDescription(const QString& viewName) const;

	/*! \brief Remove view from the component.
	*
	* \param viewName The name of the view to remove
	*
	*/
	void removeSWView(const QString& viewName);

	/*! \brief Get the number of SW views the component has.
	*
	* \return The number of SW views found in the meta data.
	*/
	int getSWViewCount() const;

	/*! \brief Create an empty SW view and return a pointer to it.
	*
	* \return Pointer to the created SW view element.
	*/
	SWView* createSWView();

	/*! \brief Get the SW views of the component
	*
	* \return QList containing pointers to the SW views.
	*/
	const QList<QSharedPointer<SWView> >& getSWViews() const;

	/*! \brief Get the SW views of the component
	 *
	 * \return QList containing pointers to the SW views.
	*/
	QList<QSharedPointer<SWView> >& getSWViews();

	/*! \brief Get the names of the SW views of the component.
	*
	* \return QStringList containing the SW view names.
	*/
	QStringList getSWViewNames() const;

	/*! \brief Find a named SW view within a component.
	*
	* \param name The name of the SW view to search for.
	*
	* \return Pointer to the specified SW view. Null pointer if the SW view was not
	* found.
	*/
	QSharedPointer<SWView> findSWView(const QString name) const;

	/*! \brief Get the SW view by the specified name.
	 * 
	 * If the SW view does not exist then the SW view is created.
	 * 
	 * Method: 		getSWView
	 * Full name:	Component::getSWView
	 * Access:		public 
	 *
	 * \param viewName The name of the view to search/create.
	 *
	 * \return Pointer to the found/created SW view.
	*/
	QSharedPointer<SWView> getSWView(const QString& viewName);

	/*! \brief Add a new SW view to the component's model.
	*
	* \param newView A pointer to the component's new SW view.
	*
	*/
	void addSWView(SWView* newView);

	/*! \brief Create a SW view for the component.
	 * 
	 * \param hierRef The reference to a design or design configuration.
	 *
	*/
	void createSWView(const VLNV& hierRef);

	/*! \brief Get the SW design of a hierarchical component.
	*
	* This function searches the SW views of the component for a named SW view.
	* If SW view if a given name is not found then invalid vlnv is returned.
	* If the component is not a hierarchical component then invalid vlnv is
	* returned.
	* If name is not given then the first found VLNV is returned.
	*
	* \return A vlnv of a design or configuration that is used within this
	* component. Invalid vlnv if design is not found.
	*/
	VLNV getHierSWRef(const QString viewName = QString()) const;

	/*! \brief Get the hierarchical SW references this component contains.
	*
	* \return QList<VLNV> contains the VLNVs of the hierarchical SW references.
	*/
	QList<VLNV> getHierSWRefs() const;

	/*! \brief Set hierarchical reference for given SW view.
	* 
	* If SW view name is not specified then the first SW view is used.
	*
	* \param vlnv The reference to set as hierarchical SW reference.
	* \param viewName The name of the SW view to set the hierarchical SW reference for.
	*
	*/
	void setHierSWRef(const VLNV& vlnv, const QString& viewName = QString());

    /*! \brief Checks if the component contains system views.
	*
	* \return bool True if system views exist.
	*/
	bool hasSystemViews() const;

	/*! \brief Check if the component has a system view with given name.
	 *
	 * \param viewName The name of the system view to search for.
	 *
	 * \return bool True if the view is found.
	*/
	bool hasSystemView(const QString& viewName) const;

	/*! \brief Get the description of specified view.
	* 
	* If view is not found a null string is returned.
	*
	* \param viewName Identifies the view that's description is wanted.
	*
	* \return QString Contains the description for the view.
	*/
	QString getSystemViewDescription(const QString& viewName) const;

	/*! \brief Remove view from the component.
	*
	* \param viewName The name of the view to remove
	*
	*/
	void removeSystemView(const QString& viewName);

	/*! \brief Get the number of system views the component has.
	*
	* \return The number of system views found in the meta data.
	*/
	int getSystemViewCount() const;

	/*! \brief Create an empty system view and return a pointer to it.
	*
	* \return Pointer to the created system view element.
	*/
	SystemView* createSystemView();

	/*! \brief Get the system views of the component
	*
	* \return QList containing pointers to the system views.
	*/
	const QList<QSharedPointer<SystemView> > getSystemViews() const;

	/*! \brief Get the system views of the component
	 *
	 * \return QList containing pointers to the system views.
	*/
	QList<QSharedPointer<SystemView> >& getSystemViews();

	/*! \brief Get the names of the system views of the component.
	*
	* \return QStringList containing the system view names.
	*/
	QStringList getSystemViewNames() const;

	/*! \brief Find a named system view within a component.
	*
	* \param name The name of the system view to search for.
	*
	* \return Pointer to the specified system view. Null pointer if the system view was not
	* found.
	*/
	SystemView* findSystemView(const QString name) const;

	/*! \brief Find a system view which refers to the given VLNV.
	 *
	 * Method: 		findSystemView
	 * Full name:	Component::findSystemView
	 * Access:		public 
	 *
	 * \param hierRef The VLNV to the design/configuration that is searched among the views.
	 *
	 * \return Pointer to the system view. Null pointer if no system view is found.
	*/
	QSharedPointer<SystemView> findSystemView(const VLNV& hierRef) const;

	/*! \brief Add a new system view to the component's model.
	*
	* \param newView A pointer to the component's new system view.
	*
	*/
	void addSystemView(SystemView* newView);

	/*! \brief Create a system view for the component.
	 * 
	 * \param hierRef The reference to a design or design configuration.
	 *
	*/
	void createSystemView(const VLNV& hierRef);

	/*! \brief Get the system design of a hierarchical component.
	*
	* This function searches the system views of the component for a named system view.
	* If system view if a given name is not found then invalid vlnv is returned.
	* If the component is not a hierarchical component then invalid vlnv is
	* returned.
	* If name is not given then the first found VLNV is returned.
	*
	* \return A vlnv of a design or configuration that is used within this
	* component. Invalid vlnv if design is not found.
	*/
	VLNV getHierSystemRef(const QString viewName = QString()) const;

	/*! \brief Get the hierarchical system references this component contains.
	*
	* \return QList<VLNV> contains the VLNVs of the hierarchical system references.
	*/
	QList<VLNV> getHierSystemRefs() const;

	/*! \brief Set hierarchical reference for given system view.
	* 
	* If system view name is not specified then the first system view is used.
	*
	* \param vlnv The reference to set as hierarchical system reference.
	* \param viewName The name of the system view to set the hierarchical system reference for.
	*
	*/
	void setHierSystemRef(const VLNV& vlnv, const QString& viewName = QString());

	/*! \brief Get this component's file sets
	*
	* \return QList containing component's file sets
	*/
	const QList<QSharedPointer<FileSet> >& getFileSets() const;

	/*! \brief Get the file sets of this component.
	 *
	 * \return QList containing pointers to the file sets.
	*/
	QList<QSharedPointer<FileSet> >& getFileSets();

    /*! \brief Get this component's file dependencies
	*
	* \return QList containing component's file dependencies
	*/
	const QList<QSharedPointer<FileDependency> >& getFileDependencies() const;

    /*!
     *  Returns the pending dependency list for writing.
     */
    QList< QSharedPointer<FileDependency> >& getPendingFileDependencies();

    /*!
     *  Returns the list of source directories.
     */
    QStringList const& getSourceDirectories() const;

    /*!
     *  Returns the list of ignored files.
     */
    QStringList const& getIgnoredFiles() const;

	/*! \brief Get the file dependencies of this component.
	 *
	 * \return QList containing pointers to the file dependencies.
	*/
	QList<QSharedPointer<FileDependency> >& getFileDependencies();

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

    /*!
     *  Sets the source directories where to automatically scan files to file sets.
     *
     *      @param [in] sourceDirs The source directories to set.
     */
    void setSourceDirectories(QStringList const& sourceDirs);

    /*!
     *  Sets ignored files.
     *
     *      @param [in] ignoredFiles The names of the ignored files.
     */
    void setIgnoredFiles(QStringList const& ignoredFiles);

    /*!
     *  Adds a file to the ignore list.
     *
     *      @param [in] filename The name of the ignored file.
     */
    void addIgnoredFile(QString const& filename);

    /*!
     *  Removes a file from the ignore list.
     *
     *      @param [in] filename The name of the ignored file.
     */
    void removeIgnoredFile(QString const& filename);

	/*! \brief Get the specified file set if one exists
	*
	* \param name Name of the file set that is wanted.
	*
	* \return pointer to the file set. If file set was not found, creates and returns pointer to it.
	*/
	QSharedPointer<FileSet> getFileSet(const QString& name) const;

	/*! \brief Get the specified file set.
	 * 
	 * If file set with given name does not exist then one is created and pointer
	 * to it returned.
	 * 
	 * Method: 		getFileSet
	 * Full name:	Component::getFileSet
	 * Access:		public 
	 *
	 * \param name The name of the file set.
	 *
	 * \return Pointer to the file set.
	*/
	QSharedPointer<FileSet> getFileSet(const QString& name);

	/*! \brief Add a new file set to the component
	*
	* \param fileSet A pointer to the file set to be added.
	*/
	void addFileSet(QSharedPointer<FileSet> fileSet);

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
	QSharedPointer<FileSet> createFileSet();

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

	/*!
	*  Returns all file pointers in all file sets that reference the given file name.
	*
	*      @param [in]  filename The filename to search for.
	*      @param [out] files    All found files that reference the given file name.
	*/
	void getFiles(QString const& filename, QList<File*>& files);

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

	/*! \brief Find the files that match the specified file type.
	 *
	 * Method: 		findFilesByFileType
	 * Full name:	Component::findFilesByFileType
	 * Access:		public 
	 *
	 * \param fileType The file type to search within the files.
	 *
	 * \return QStringList containing the file paths of files matching the file type.
	*/
	QStringList findFilesByFileType(const QString& fileType) const;

	/*! \brief Find the file set by the file set id.
	*
	* Method: 		findFileSetById
	* Full name:	Component::findFileSetById
	* Access:		public 
	*
	* \param id Identifies the file set.
	*
	* \return Pointer to the file set found, null pointer if none found.
	*/
	QSharedPointer<const FileSet> findFileSetById(const QString& id) const;

	/*! \brief Get file paths of all specified file sets that match the specified file types.
	*
	* Method: 		getFilesFromFileSets
	* Full name:	Component::getFilesFromAllFileSets
	* Access:		public 
	*
	* \param fileSetNames Contains the file sets to search from.
	* \param fileTypes Contains the file types that are searched among the file sets.
	*
	* \return QStringList containing the relative file paths to the files.
	*/
	QStringList getFilesFromFileSets(const QStringList& fileSetNames, const QStringList& fileTypes) const;

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

	/*! \brief Get list of the bus interfaces of a component.
	 *
	 * \return QList containing pointers to the bus interfaces.
	*/
	QList<QSharedPointer<BusInterface> >& getBusInterfaces();

	/*! \brief Get list of the bus interfaces of a component.
	 *
	 * \return QList containing pointers to the bus interfaces.
	*/
	const QList<QSharedPointer<BusInterface> >& getBusInterfaces() const;

	/*! \brief Get list of bus interfaces that are connected to the given interface through channel.
	 *
	 * Method: 		getChannelConnectedInterfaces
	 * Full name:	Component::getChannelConnectedInterfaces
	 * Access:		public 
	 *
	 * \param sourceInterfaceName The name of the interface that's channels are checked.
	 *
	 * \return QList containing pointers to the bus interfaces connected via channel.
	*/
	QList<QSharedPointer<const BusInterface> > getChannelConnectedInterfaces(const QString& sourceInterfaceName) const;

	/*! \brief Set the bus interfaces for the component.
	 *
	 * \param busif QList containing the pointers to the bus interfaces.
	 *
	*/
	void setBusInterfaces(QList<QSharedPointer<BusInterface> >& busInterfaces);

	/*! \brief Get list of names of all the bus interfaces in this component.
	*
	* \return QStringList containing the names of the bus interfaces.
	*/
	QStringList getBusInterfaceNames() const;

	/*! \brief Get a pointer to a bus interface.
	*
	* If named bus interface is not found then null pointer is returned.
	*
	* \param name Contains the name of the wanted bus interface.
	*
	* \return A pointer to the busInterface instance or null pointer.
	*/
	QSharedPointer<BusInterface> getBusInterface(const QString& name);

    /*! \brief Get a pointer to a bus interface.
	*
	* If named bus interface is not found then null pointer is returned.
	*
	* \param name Contains the name of the wanted bus interface.
	*
	* \return A pointer to the busInterface instance or null pointer.
	*/
	QSharedPointer<BusInterface const> getBusInterface(const QString& name) const;

	/*! \brief Get list of the slave interfaces that refer to given memory map.
	*
	* \param memoryMap The name of the memory map to search for.
	*
	* \return QStringList containing names of the slave interfaces.
	*/
	QStringList getSlaveInterfaces(const QString& memoryMap) const;

	/*! \brief Get list of the master and mirrored master interfaces that refer to given address space.
	 *
	 * \param addressSpace The name of the address space to search for.
	 *
	 * \return QStringList containing names of the master and mirrored master interfaces.
	*/
	QStringList getMasterInterfaces(const QString& addressSpace) const;

	/*! \brief Get list of the master and mirrored master interface names.
	 *
	 * Method: 		getMasterInterfaces
	 * Full name:	Component::getMasterInterfaces
	 * Access:		public 
	 *
	 *
	 * \return QStringList containing the interface names.
	*/
	QStringList getMasterInterfaces() const;

	/*! \brief Add a new bus interface to the component.
	*
	* \param busInterface Pointer to the new bus interface to be added.
	*
	* \return True if bus interface was successfully added. False if a
	* bus interface with same name already existed.
	*/
	bool addBusInterface(QSharedPointer<BusInterface> busInterface);

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

	/*! \brief Check if the component contains the named interface.
	 *
	 * \param interfaceName The name of the interface to search for.
	 *
	 * \return bool True if the interface was found.
	*/
	bool hasInterface(const QString& interfaceName) const;

	/*! \brief Get the interface mode of the specified interface.
	 *
	 * \param interfaceName The name of the interface to check.
	 *
	 * If the specified interface does not exist then General::INTERFACE_MODE_COUNT is returned.
	 *
	 * \return General::InterfaceMode The interface mode of the specified interface.
	*/
	General::InterfaceMode getInterfaceMode(const QString& interfaceName) const;

    /*!
     *  Returns the COM interfaces.
     */
    QList<QSharedPointer<ComInterface> > const& getComInterfaces() const;

	/*! \brief Get the COM interfaces of the component.
	 *
	 * \return QMap containing the pointers to COM interfaces.
	*/
	QList<QSharedPointer<ComInterface> >& getComInterfaces();

    /*!
     *  Returns the names of the COM interfaces.
     */
    QStringList getComInterfaceNames() const;

    /*!
     *  Finds the COM interface with the given name.
     *
     *      @param [in] name The name of the COM interface.
     *
     *      @return The COM interface, or 0 if there is no COM interface with the given name.
     */
	QSharedPointer<ComInterface> getComInterface(QString const& name);

    /*!
     *  Creates a new empty COM interface to the component.
     *
     *      @return The created COM interface.
     */
	QSharedPointer<ComInterface> createComInterface();

    /*!
     *  Adds a new COM interface to the component.
     *
     *      @param [in] comInterface The COM interface to add.
     *
     *      @return True, if the COM interface was added successfully. False, if a COM interface
     *              with the same name already exists.
     */
    bool addComInterface(QSharedPointer<ComInterface> comInterface);

    /*!
     *  Removes a COM interface from the component.
     *
     *      @param [in] name The name of the COM interface to remove.
     */
    void removeComInterface(QString const& name);

    /*!
     *  Removes a COM interface from the component.
     *
     *      @param [in] comInterface The COM interface to remove.
     */
    void removeComInterface(ComInterface* comInterface);

    /*!
     *  Returns the API interfaces.
     */
    QList<QSharedPointer<ApiInterface> > const& getApiInterfaces() const;

	/*! \brief Get the API interfaces of the component.
	 *
	 * \return QMap containing pointers to the API interfaces.
	*/
	QList<QSharedPointer<ApiInterface> >& getApiInterfaces();

    /*!
     *  Returns the names of the API interfaces.
     */
    QStringList getApiInterfaceNames() const;

    /*!
     *  Finds the API interface with the given name.
     *
     *      @param [in] name The name of the API interface.
     *
     *      @return The API interface, or 0 if there is no API interface with the given name.
     */
	QSharedPointer<ApiInterface> getApiInterface(QString const& name);

    /*!
     *  Creates a new empty API interface to the component.
     *
     *      @return The created API interface.
     */
	QSharedPointer<ApiInterface> createApiInterface();

    /*!
     *  Adds a new API interface to the component.
     *
     *      @param [in] apiInterface The API interface to add.
     *
     *      @return True, if the API interface was added successfully. False, if a API interface
     *              with the same name already exists.
     */
    bool addApiInterface(QSharedPointer<ApiInterface> apiInterface);

    /*!
     *  Removes a API interface from the component.
     *
     *      @param [in] name The name of the API interface to remove.
     */
    void removeApiInterface(QString const& name);

    /*!
     *  Removes an API interface from the component.
     *
     *      @param [in] apiInterface The API interface to remove.
     */
    void removeApiInterface(ApiInterface* apiInterface);


	/*! \brief Find the interface that contains the physical port and return it's abs def vlnv and logical name.
	 *
	 * \param physicalPortName The name of the physical port that's interface is searched.
	 *
	 * \return QMultiMap<QString, VLNV> Identifies the abstraction definition(s) used in the interface(s).
	 *  Key: the logical name of the port.
	 *  Value: The VLNV of the interface.
	*/
	QMultiMap<QString, VLNV> getInterfaceAbsDefForPort(const QString& physicalPortName) const;

	/*! \brief Check which interface contains the named port.
	 * 
	 * If port is contained in more than one interface then "several" is returned.
	 * If port is not contained in any interface then "none" is returned.
	 *
	 * \param portName The name of the port that is searched in interfaces.
	 *
	 * \return QString The name of the interface that contains the port.
	*/
	QString getInterfaceNameForPort(const QString& portName) const;

	/*! \brief Get the description of the specified interface.
	 *
	 * \param interfaceName The name of the interface that's description is wanted.
	 *
	 * \return QString The description of the given interface.
	*/
	QString getInterfaceDescription(const QString& interfaceName) const;

	/*! \brief Get the named port of a component
	*
	* This function can be used to find a specific port of a component. If
	* named port can't be found a null pointer is returned.
	*
	* \param name QString containing the name of the port
	*
	* \return Pointer to the port instance
	*/
	QSharedPointer<Port> getPort(const QString& name) const;

	/*! \brief Get the ports contained in this component
	 *
	 * \return QList containing pointers to the ports.
	*/
	const QList<QSharedPointer<Port> >& getPorts() const;

	/*! \brief Get the ports contained in this component.
	*
	* \return QList containing pointers to the ports.
	*/
	QList<QSharedPointer<Port> >& getPorts();

	/*! \brief Get the ports that are found in specified interface.
	 *
	 * \param interfaceName The name of interface that's ports are requested.
	 *
	 * \return List containing pointers to the ports.
	*/
	const QList<QSharedPointer<Port> > getPorts(const QString& interfaceName) const;

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

	/*! \brief Get the default values of in and inout ports.
	 *
	 * \return QMap<QString, QString> contains the port names and default values.
	 * Key: Name of the port.
	 * Value: The default value for the specified port.
	*/
	QMap<QString, QString> getPortDefaultValues() const;

	/*! \brief Get the type definitions for the ports of the component.
	*
	* \return QStringList contains the type definitions for the ports.
	*/
	QStringList getPortTypeDefinitions() const;

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

	/*! \brief Checks if the component has at least one port that's type is defined.
	 * 
	 * This function can be used to check if component needs to have at least one
	 * view because if port type is defined then there has to be view reference.
	 *
	 * \return bool True if at least one port with defined port type exists.
	*/
	bool hasPortTypes() const;

	/*! \brief Check if the specified port exists on the implementation of the component.
	 *
	 * This function checks the direction of a wired port and returns true for
	 * directions: in, out and inout.
	 * 
	 * If port is not found then false is returned.
	 * 
	 * \param portName The name of the port to check.
	 *
	 * \return bool True if port's direction is in, out or inout.
	*/
	bool isPhysicalPort(const QString& portName) const;

	/*! \brief Get the list of component's physical ports and their bounds.
	 *
	 * \return QList containing the physical port names and their bounds.
	*/
	QList<General::PortBounds> getPortBounds() const;

	/*! \brief Get this component's address spaces
	*
	* \return QList containing component's address spaces
	*/
	QList<QSharedPointer<AddressSpace> >& getAddressSpaces();

	/*! \brief Get address spaces of this component.
	 *
	 * \return QList containing pointers to component's address spaces.
	*/
	const QList<QSharedPointer<AddressSpace> >& getAddressSpaces() const;

	/*! \brief Get the names of the address spaces stored in this component.
	*
	* \return QStringList containing the names of the address spaces.
	*/
	const QStringList getAddressSpaceNames() const;

	/*! \brief Set this component's address spaces
	*
	* \param addressSpaces QList containing the address spaces to be set
	*/
	void setAddressSpaces(const QList<QSharedPointer<AddressSpace> > &addressSpaces);

	/*! \brief Check if the component contains at least one local memory map.
	 *
	 * Method: 		hasLocalMemoryMaps
	 * Full name:	Component::hasLocalMemoryMaps
	 * Access:		public 
	 *
	 *
	 * \return True if at least one local memory map is found.
	*/
	bool hasLocalMemoryMaps() const;

	/*! \brief Remove the specified address space from the component.
	 *
	 * \param addrSpaceName The name of the address space to remove.
	 *
	*/
	void removeAddressSpace(const QString& addrSpaceName);

	/*! \brief Create a new empty address space to the component.
	 *
	 * \return Pointer to the created address space.
	*/
	AddressSpace* createAddressSpace();

	/*! \brief Check if the component contains an address space with given name.
	 *
	 * \param addrSpaceName The name of the address space to search for.
	 *
	 * \return bool True if an address space with given name is found.
	*/
	bool hasAddressSpace(const QString& addrSpaceName) const;

    /*!
     *  Sets the SW properties.
     *
     *      @param [in] properties A list of properties to set.
     */
    void setSWProperties(QList< QSharedPointer<ComProperty> > const& properties);

    /*!
     *  Returns the list of SW properties.
     */
    QList< QSharedPointer<ComProperty> >& getSWProperties();

	/*! \brief Get the list of SW properties.
	 *
	 * \return QList containing pointers to the SW properties.
	*/
	const QList<QSharedPointer<ComProperty> >& getSWProperties() const;

	/*! \brief Check if the address blocks within memory maps contains registers with given names.
	 *
	 * Method: 		uniqueRegisterNames
	 * Full name:	AddressBlock::uniqueRegisterNames
	 * Access:		public 
	 *  
	 * The register names are checked against the given list and appended to it. 
	 * If all register names are unique then true is returned.
	 *
	 * \param regNames List of strings to be checked.
	 *
	 * \return True if all register names are unique.
	*/
	bool uniqueRegisterNames(QStringList& regNames) const;

	/*! \brief Check if the names of memory typed address blocks are contained in the given list.
	 *
	 * Method: 		uniqueMemoryNames
	 * Full name:	MemoryMap::uniqueMemoryNames
	 * Access:		public 
	 * 
	 * The memory names are checked against the given list and appended to it.
	 * If all memory names are unique then true is returned.
	 * 
	 * \param memNames List of strings to be checked.
	 *
	 * \return True if all memory names are unique.
	*/
	bool uniqueMemoryNames(QStringList& memNames) const;

	/*! \brief Get the directories which this component needs.
	 *
	 * Method: 		getDependentDirs
	 * Full name:	Component::getDependentDirs
	 * Access:		virtual public 
	 *
	 *
	 * \return QStringList containing the relative directory paths.
	*/
	virtual const QStringList getDependentDirs() const;


    void setAuthor(QString const& author);

    QString getAuthor() const;

private:

    /*!
     *  Parses COM interfaces from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseComInterfaces(QDomNode& node);

    /*!
     *  Parses API interfaces from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseApiInterfaces(QDomNode& node);

    /*!
     *  Parses properties from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseSWProperties(QDomNode& childNode);

    /*!
     *  Parses SW views from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseSWViews(QDomNode& node);

    /*!
     *  Parses system views from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseSystemViews(QDomNode& node);

    /*!
     *  Parses file dependencies from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseFileDependencies(QDomNode& node);

    /*!
     *  Parses source directories from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseSourceDirectories(QDomNode& node);

    /*!
     *  Parses ignored files from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseIgnoredFiles(QDomNode& node);
    
    /*! \brief Specifies all the interfaces for this component.
	 * OPTIONAL spirit:busInterfaces
	 *
	 */
	QList<QSharedPointer<BusInterface> > busInterfaces_;

    //! The communication interfaces (extension).
    QList<QSharedPointer<ComInterface> > comInterfaces_;

    //! The API interfaces (extension).
    QList<QSharedPointer<ApiInterface> > apiInterfaces_;

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
	QSharedPointer<QList<QSharedPointer<Choice> > > choices_;

	/*! \brief Contains the fileSets.
	 * OPTIONAL spirit:fileSets
	 * Specifies groups of files and possibly their function for reference by
	 * other sections of this component description.
	 */
	QList<QSharedPointer<FileSet> > fileSets_;

    //! The list of file dependencies.
    //! OPTIONAL kactus2:fileDependencies
    QList< QSharedPointer<FileDependency> > fileDependencies_;
    QList< QSharedPointer<FileDependency> > pendingFileDependencies_;

    //! The list of source directories.
    //! OPTIONAL kactus2:sourceDirectories
    QStringList sourceDirs_;

    //! The list of ignored files.
    //! OPTIONAL kactus2:ignoredFiles
    QStringList ignoredFiles_;

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
	 * Describes any parameter that can be used to configure or hold information
	 * related to this component.
	 */
	QList<QSharedPointer<Parameter> > parameters_;

	//! \brief Contains the attributes for the spirit:component element.
	//! OPTIONAL
	QMap<QString, QString> attributes_;

    //! The list of properties.
    QList< QSharedPointer<ComProperty> > swProperties_;

    //! The list of SW views.
    QList< QSharedPointer<SWView> > swViews_;

    //! The list of system views.
    QList< QSharedPointer<SystemView> > systemViews_;

    //! The author of the component.
    QString author_;
};


#endif /* COMPONENT_H_ */
