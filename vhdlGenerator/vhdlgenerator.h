/* 
 *  	Created on: 30.10.2010
 *      Author: Antti Kamppi
 * 		filename: vhdlgenerator.h
 *		Project: Kactus 2
 */

#ifndef VHDLGENERATOR_H_
#define VHDLGENERATOR_H_

#include <models/generaldeclarations.h>
#include <models/design.h>
#include <models/designconfiguration.h>

#include "componentinstance.h"
#include "componenttype.h"
#include "connection.h"

#include <QObject>
#include <QString>
#include <QMap>
#include <QTextStream>
#include <QSharedPointer>
#include <QList>
#include <QFileInfo>

class ComponentType;
class ComponentInstance;
class Connection;
class Design;
class Component;
class VLNV;
class LibraryInterface;
class Design;

typedef Connection Signal;

/*! \brief Vhdl Generator generates top-level vhdl for hierarchical component.
 *
 * This class and it's interface provide services to generate vhdl code from
 * a hierarchical view of a component. If the component doesn't contain
 * an rtl view, it is created and added to the component. If rtl view is found
 * it is modified to match the newly generated vhdl.
 */
class VhdlGenerator: public QObject {

	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * Creates a data structure for a top-level vhdl-document to be written.
	 *
	 * \param design A pointer to an IP-Xact design that is used to create the
	 * top-level vhdl-document.
	 * \param fileName The file path to the vhdl file to be written. This must
	 * include the directory path and the file name.
	 * \param parent A pointer to the owner of this instance.
	 *
	 */
	VhdlGenerator(QSharedPointer<Component> topLevelComponent, const QString& viewName,
			LibraryInterface* libraryHandler,	QWidget* parent);

	/*! \brief The destructor
	 *
	 */
	virtual ~VhdlGenerator();

	/*! \brief Parse the information from the top component and design.
	* 
	* Exception guarantee: basic
	* \exception Vhdl_error Occurs when there is an invalid element that
	* prevents the creation of a valid vhdl document.
	*
	* \exception Invalid_file Occurs if an IP-Xact document is found which is
	* not valid.
	 *
	*/
	void parse();

	/*! \brief Generates top-level vhdl file from the contents.
	 *
	 * \param outputFileName Contains the file path of the file to be created.
	 * Must contain both the path to the file and the name of the file. If the
	 * string is empty then the file is created to the IP-Xact library under
	 * the IP-Xact document. The file path of the created file is returned by
	 * this function.
	 *
	 * Exception guarantee: basic
	 * \exception Invalid_file Occurs if the specified file can't be opened
	 * for writing.
	 * \exception Invalid_file Occurs if file type is not defined for a file.
	 * \exception Vhdl_error Occurs if defined port type is invalid.
	 *
	 * \return File path of the created file. If there occurred error during
	 * writing then null string is returned.
	 */
	void generateVhdl(const QString& outputFileName);

	/*! \brief Add an rtl view to the hierarchical component.
	 *
	 * This function can be used to add rtl view to a hierachical component
	 * after the top-level vhdl has been created. If there already exists
	 * another rtl view it is replaced by new one.
	 *
	 * Note: Before calling this function the vhdl file MUST exist, otherwise
	 * false is returned and nothing is done.
	 *
	 * \param vhdlFilePath The name of the output file for the top-level
	 * vhdl. This is the same string that is given to generateVhdl().
	 *
	 * \return True if the view was added successfully
	 */
	bool addRtlView(const QString vhdlFilePath);

signals:

	//! \brief Prints and error message to user.
	void errorMessage(const QString& msg);

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg);

private:

	//! No copying
	VhdlGenerator(const VhdlGenerator& other);

	//! No assignment
	VhdlGenerator& operator=(const VhdlGenerator& other);

	/*! \brief Pointer to the instance that controls the VLNV library.
	 * Not owned by VhdlGenerator
	 */
	LibraryInterface* libraryHandler_;

	//! \brief Pointer to the owner of this generator.
	QWidget* parent_;

	/*! \brief Pointer to the design that is generated.
	 * owned by VhdlGenerator
	 */
	QSharedPointer<Design> design_;

	/*! \brief Pointer to the hierarchical component that contains the view.
	 * Not owned by VhdlGenerator
	 */
	QSharedPointer<Component> component_;

	//! \brief Pointer to the design configuration used (may be null if not used).
	QSharedPointer<DesignConfiguration> desConf_;

	//! \brief The name of the view on the top component.
	const QString viewName_;

	/*! \brief Contains the name of the top-level entity
	 *
	 * The name is the name element of the top component's vlnv.
	 */
	QString topLevelEntity_;

	//! \brief Contains the libraries for components
	QStringList libraries_;

	/*! \brief Contains the top-level generics that are visible to outside.
	 *
	 * Key = name of the generic
	 * Value = Pointer to the generic
	 *
	 * VhdlGenerator owns the generics
	 */
	QMap<QString, QSharedPointer<ComponentType::VhdlGeneric> > generics_;

	/*! \brief Contains the endpoints and signals connection them.
	 *
	 * Key = Endpoint that defines the instance, port and port bounds.
	 * Value = The signal that is connected to the end point.
	 */
	QMap<Connection::EndPoint, QSharedPointer<Signal> > signals_;

	/*! \brief Contains the end point and top port name for hierarchical connection
	 *
	 * Key = Endpoint that defines the instance, port and port bounds
	 * Value = The name of the top port to connect to.
	 */
	QMap<Connection::EndPoint, QString> hierMaps_;

	/*! \brief Contains the component declarations.
	 *
	 * Key = Name of the component type
	 * Value = Pointer to the component type declaration.
	 *
	 * VhdlGenerator owns the component declarations
	 */
	QMap<QString, QSharedPointer<ComponentType> > components_;

	/*! \brief Contains pointers to the component instances.
	 *
	 * Key = name of the instance
	 * Value = Pointer to the component instance.
	 *
	 * VhdlGenerator owns the instances.
	 */
	QMap<QString, QSharedPointer<ComponentInstance> > instances_;

	/*! \brief Contains the component models that are needed
	 *
	 * The LibraryHandler gives the ownership of a component to the caller
	 * when creating a component model so this map is used to save the
	 * pointers to needed components as long as the VhdlGenerator is running.
	 *
	 * Key = Name of the component (VLNV->name)
	 * Value = Pointer to the component model
	 */
	QMap<VLNV, QSharedPointer<Component> > componentModels_;

	/*! \brief Contains the top-level ports that are visible to outside.
	 *
	 * Key = Name of the port
	 * Value = Connection definition of the port
	 *
	 * VhdlGenerator owns the ports
	 */
	QMap<QString, QSharedPointer<Connection> > ports_;

	/*! \brief Parse the generics for the top entity
	 *
	*/
	void parseTopGenerics();

	/*! \brief Connect ports of 2 busInterfaces to each other.
	 *
	 * This function creates a signal that is named:
	 * <connectionName>_<interface1.signalName>_<interface2.signalName>
	 * The signal is connected to corresponding ports of each interface.
	 *
	 * \param connectionName Contains the name of the interconnection name.
	 * \param interface1 A pointer to the busInterface that is connected.
	 * \param interface2 A pointer to the busInterface that is connected.
	 * \param instance1 A pointer to component instance within design.
	 * \param instance2 A pointer to component instance within design. If
	 * undefined the interface2 must be pointer to a bus interface of the
	 * top-level component that is visible to outside of the component.
	 */
	void connectInterfaces(const QString connectionName,
			ComponentInstance* instance1, BusInterface* interface1,
			BusInterface* interface2, ComponentInstance* instance2 = 0);

	/*! \brief Connect ports of component instance and top component.
	 *
	 * \param instance Pointer to the component instance within design.
	 * \param interface Pointer to the instance's interface to connect.
	 * \param hierInterface Pointer to the top-component's interface to connect.
	 *
	*/
	void connectHierInterface(ComponentInstance* instance, BusInterface* interface,
		BusInterface* hierInterface);

	//! \brief Writes the top-level generics into stream.
	void writeGenerics(QTextStream& vhdlStream);

	/*! \brief Write the top-level ports into stream.
	 *
	 * \param vhdlStream A reference to a QTextStream to write into
	 *
	 * Exception guarantee: basic
	 * \exception Vhdl_error Occurs if an invalid port type has been defined.
	 */
	void writePorts(QTextStream& vhdlStream);

	/*! \brief Write the signal mappings
	 *
	 * \param stream A reference to a QTextStream to write into.
	 */
	void writeSignalMaps(QTextStream& stream);

	/*! \brief Write the component declarations
	 *
	 * \param stream A reference to a QTextStream to write into.
	 */
	void writeComponentDeclarations(QTextStream& stream);

	/*! \brief Write the signal declarations
	 *
	 * \param stream A reference to a QTextStream to write into.
	 */
	void writeSignalDeclarations(QTextStream& stream);

	/*! \brief Write the component instances
	 *
	 * \param stream a Reference to QTextStream to write into
	 */
	void writeComponentInstances(QTextStream& stream);

	/*! \brief Get a QMap containing the port maps from an interface
	 *
	 * \param interface A pointer to the BusInterface where the port maps are.
	 *
	 * \return QMap containing the pointers to port maps
	 *  Key = logical name of the port map
	 *  Value = pointer to the port map
	 */
	QMap<QString, General::PortMap*> getInterfacePortMaps(
			BusInterface* interface);

	/*! \brief Write the library compiling script into stream
	 *
	 */
	void writeLibraryCompiles(QTextStream& stream);

	/*! \brief Saves the library names and files to QMap to be used later.
	 *
	 * Purpose of this function is to ask all component for vhdl files and
	 * their library names and convert the relative paths given by
	 * components into absolute file paths.
	 *
	 * Exception guarantee: basic
	 * \exeption Invalid_file Occurs if VLNV doesn't match any component in
	 * LibraryHandler or if file can't be found in file system.
	 */
	//void parseLibraries();

	/*! \brief Writes a header into stream.
	 *
	 * This function should be used when starting to generate a vhdl-file. It
	 * writes the description of the design into stream.
	 *
	 * \param stream A reference to QTextStream to write into.
	 * \param fileName The name of the output file.
	 */
	void writeVhdlHeader(QTextStream& stream, const QString fileName);

	/*! \brief Find all component instances in the design.
	 *
	 * Searches all component instances in the design and makes sure they exist
	 * in the library. Creates component declarations and instances.
	 *
	 * \param design A pointer to the design to find for component instances.
	 * \param handler A pointer to the libraryHandler that controls the VLNV
	 * library.
	 *
	 * Exception guarantee: basic
	 * \exception Vhdl_error Occurs if there are two component instances with
	 * same name.
	 * \exception Vhdl_error Occurs if a there is a ComponentRef to a
	 * component that doesn't exist in the library.
	 * \exception Vhdl_error Occurs if the ComponentRef VLNV is not for a
	 * component.
	 */
	void parseComponentInstances();

	/*! \brief Create a component instance.
	 *
	 * \param instanceName The name for the component instance.
	 * \param compDeclaration A pointer to the component type of the instance.
	 * \param instance A reference to a component instance from the design.
	 * \param component Pointer to the component model.
	 * \param instanceDescription Contains the description for the instance.
	 *
	 * Exception guarantee: basic
	 * \exception Vhdl_error Occurs if there are two component instances with
	 * same name.
	 */
	void createInstance(const QString& instanceName, 
		ComponentType* compDeclaration, 
		const Design::ComponentInstance& instance,
		QSharedPointer<Component> component, 
		const QString& instanceDescription);

	/*! \brief Search the interConnections of a design and make connection
	 *
	 * This function is used to go through all interconnections within a
	 * design and connecting the bus interfaces to each other.
	 */
	void parseInterConnections();

	/*! \brief Search the ad-hoc connections and make connections
	 *
	 * This function is used to go through all ad-hoc connections within a
	 * design and connect the ports and create necessary signals.
	 *
	 * Exception guarantee: basic
	 * \exception Vhdl_error Occurs if the widths of the signals and ports
	 * don't match.
	 * \exception Vhdl_error Occurs if an invalid component reference is found
	 * within adHocConnection.
	 */
	void parseAdHocConnections();

	/*! \brief Search the hierarchical connections and connect them.
	 *
	 * This function is used to go through all the hierConnections within a
	 * design and connect interfaces of components within design
	 * to the top-level interfaces of the hierarchical component containing the
	 * design.
	 *
	 * Exception guarantee: basic
	 * \exception Vhdl_error Occurs if
	 *  	- defined top level bus interface is not found
	 *  	- defined component instance is not found
	 *  	- defined bus interface in component instance's component
	 *  		description is not found.
	 */
	void parseHierConnections();

	/*! \brief Get the libraries needed by the components in this design.
	 *
	 * This function searches the top component and the design and adds all
	 * vhdl libraries that the components use.
	 */
	void getLibraries();

	/*! \brief Create a name for a new signal.
	 *
	 * The parameters are used to create a descriptive name for the signal.
	 *
	 * \param connectionName Name of the connection in the design.
	 * \param instance1Name Name of the component instance where this signal
	 * is coming from.
	 * \param instance1Port Name of the port in instance1 this signal is
	 * connected to.
	 * \param instance2 Pointer to the component instance where this signal is
	 * connected to. This can be null if creating a hier connection.
	 * \param instance2Port Name of the port in instance2 this signal is
	 * connected to.
	 */
	QString createSignalName(const QString connectionName,
			const QString instance1Name,
			const QString instance1Port, ComponentInstance* instance2,
			const QString instance2Port);

	/*! \brief Map ports of component instances and top entity to signals.
	 * 
	 * Call this only after all connections have been parsed to end points.
	*/
	void mapPorts2Signals();

	/*! \brief Parse the libraries needed by given component.
	 *
	 * \param component Pointer to the component that's libraries are wanted
	 * \param viewName Specifies the used view of the component. If no view
	 * is specified then all libraries needed by component is used.
	 *
	*/
	void parseLibraries(QSharedPointer<Component> component, const QString& viewName);

	/*! \brief Connect the end point to given signal
	 *
	 * \param endPoint The endpoint to connect
	 * \param signal The signal to connect the end point to
	 *
	*/
	void connectEndPoint(const Connection::EndPoint& endPoint, QSharedPointer<Signal> signal);

    /* \brief Add quotation marks to string generics if needed
     *
     * \param type Generic type
     * \param value Original generic value
     * \return Quoted string generic
     */
     QString quoteGenericValue(const QString& type, const QString& value) const;
};

#endif /* VHDLGENERATOR_H_ */
