/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgenerator2.h
 *		Project: Kactus 2
 */

#ifndef VHDLGENERATOR2_H
#define VHDLGENERATOR2_H

#include <models/component.h>
#include <models/design.h>
#include <models/designconfiguration.h>

#include <LibraryManager/vlnv.h>

#include "vhdlgeneric.h"
#include "vhdlport.h"
#include "vhdlportsorter.h"
#include "vhdlconnectionendpoint.h"
#include "vhdlsignal.h"
#include "vhdlcomponentdeclaration.h"
#include "vhdlcomponentinstance.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

class LibraryInterface;
class VhdlConnectionEndPoint;
class DesignWidget;

/*! \brief Vhdl Generator generates top-level vhdl for hierarchical component.
 *
 * This class and it's interface provide services to generate vhdl code from
 * a hierarchical view of a component. 
 */
class VhdlGenerator2 : public QObject {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of this generator.
	 *
	*/
	VhdlGenerator2(LibraryInterface* handler, DesignWidget* parent);
	
	//! \brief The destructor
	virtual ~VhdlGenerator2();

	/*! \brief Get pointer to the instance that manages the library.
	 *
	 * \return LibraryInterface* Pointer to the instance that manages the library.
	*/
	LibraryInterface* handler() const;

	/*! \brief Parse the data needed to create the top-level vhdl entity.
	 *
	 * \param topLevelComponent Pointer to the component that's entity is created.
	 * \param viewName The name of the view on the top component to generate the vhdl to.
	 *
	 * \return bool True if the parsing was successful and the vhdl can be generated.
	*/
	bool parse(QSharedPointer<Component> topLevelComponent, const QString& viewName);

	/*! \brief Generate the vhdl that has been parsed.
	 * 
	 * NOTE: Before calling this function the parse() function MUST be called so
	 * there is something to generate.
	 *
	 * \param outputFileName The absolute file path of the vhdl file to be created.
	 * If file exists it is overwritten.
	 *
	*/
	void generateVhdl(const QString& outputFileName);

	/*! \brief Add a new view to the top component's IP-Xact metadata.
	 * 
	 * NOTE: Before calling this function the parse() function must be called and
	 * usually the generateVhdl() also. If generateVhdl() is not called then the
	 * architecture name must be set other way.
	 * 
	 * \param vhdlFileName Absolute file path to the vhdl file to add to the metadata.
	 * This is usually the same file that was set as output file when calling
	 * generateVhdl().
	 *
	 * \return bool True if the view was added successfully.
	*/
	bool addRTLView(const QString& vhdlFileName);

	/*! \brief Check if the generator is able to generate and architecture for top entity.
	 *
	 * \return bool True if component contains a design and is able to create an
	 * architecture for the entity.
	*/
	bool containsArchitecture() const;

signals:

	//! \brief Print a notification to user.
	void noticeMessage(const QString& noticeMessage);

	//! \brief Print a error message to user.
	void errorMessage(const QString& errorMessage);

private:
	//! \brief No copying
	VhdlGenerator2(const VhdlGenerator2& other);

	//! \brief No assignment
	VhdlGenerator2& operator=(const VhdlGenerator2& other);

	//! \brief Contains info on what port to connect.
	struct PortConnection {

		//! \brief Pointer to the instance the port belongs to.
		QSharedPointer<VhdlComponentInstance> instance_;

		//! \brief The name of the port on the instance to connect.
		QString portName_;

		//! \brief The left bound of the port to connect.
		int leftBound_;

		//! \brief The right bound of the port to connect.
		int rightBound_;

		/*! \brief The constructor
		 *
		 * \param instance Pointer to the instance to connect.
		 * \param portName The name of the port on the instance.
		 * \param left The left bound of the port.
		 * \param right The right bound of the port.
		 *
		*/
		PortConnection(QSharedPointer<VhdlComponentInstance> instance, 
			const QString& portName, int left = -1, int right = -1);

		//! \brief Copy constructor
		PortConnection(const PortConnection& other);

		//! \brief Assignment operator
		PortConnection& operator=(const PortConnection& other);
	};

	/*! \brief Parse the design and configuration used.
	 * 
	 * \return True if the design and configuration were parsed successfully.
	*/
	bool parseDesignAndConfiguration();

	/*! \brief Parse the generics for the top entity to create.
	 *
	*/
	void parseTopGenerics();

	/*! \brief Parse the ports for the top entity to create.
	 *
	*/
	void parseTopPorts();

	/*! \brief Parse the component instances from the design.
	 *
	*/
	void parseInstances();

	/*! \brief Parse the interconnections of a design.
	 *
	*/
	void parseInterconnections();

	/*! \brief Parse the ad hoc connections of a design.
	 *
	*/
	void parseAdHocConnections();

	/*! \brief Parse the hierarchical connections of a design.
	 *
	*/
	void parseHierConnections();

	/*! \brief Connect the two interfaces together with signals.
	 *
	 * \param connectionName The name of the connection between the interfaces.
	 * \param description The description of the interconnection.
	 * \param instance1 Pointer to the instance 1.
	 * \param interface1 Pointer to the bus interface of instance 1.
	 * \param instance2 Pointer to the instance 2.
	 * \param interface2 Pointer to the bus interface of instance 2.
	 *
	*/
	void connectInterfaces(const QString& connectionName, 
		const QString& description,
		QSharedPointer<VhdlComponentInstance> instance1, 
		BusInterface* interface1, 
		QSharedPointer<VhdlComponentInstance> instance2, 
		BusInterface* interface2);

	/*! \brief Connect the specified ports together.
	 *
	 * \param connectionName The name of the connection.
	 * \param description Description for the connection.
	 * \param ports Contains the ports to connect.
	 *
	*/
	void connectPorts(const QString& connectionName, 
		const QString& description,
		const QList<VhdlGenerator2::PortConnection>& ports);

	/*! \brief Connect the specified top port to specified instance ports.
	 *
	 * \param topPort The name of the port on top component.
	 * \param leftBound The left bound of the port on top component.
	 * \param rightBound The right bound of the port on top component.
	 * \param ports List of ports to connect to the top port
	 *
	*/
	void connectHierPort(const QString& topPort,
		int leftBound, 
		int rightBound,
		const QList<VhdlGenerator2::PortConnection>& ports);

	/*! \brief Connect the given endPoint with the signal.
	 *
	 * \param endPoint The end point that specified the port to connect.
	 * \param signal The signal the port is connected to.
	 *
	*/
	void connectEndPoint(const VhdlConnectionEndPoint& endPoint,
		const QSharedPointer<VhdlSignal> signal);

	/*! \brief Connect the hierarchical interface to the instance interface.
	 *
	 * \param instance Pointer to the instance that's interface is connected.
	 * \param instanceInterface Pointer to the interface of the instance.
	 * \param topInterface Pointer to the interface of the top component.
	 *
	*/
	void connectHierInterface( QSharedPointer<VhdlComponentInstance> instance, 
		BusInterface* instanceInterface,
		BusInterface* topInterface );

	/*! \brief Map ports of component instances to signals.
	 *
	 * NOTE: This function must be called only after all end points of signals
	 * have been parsed.
	 *
	*/
	void mapPorts2Signals();
	
	/*! \brief Write the header of a vhdl file to be created.
	 *
	 * \param vhdlStream The text stream to write into.
	 * \param fileName The name of the file that is being written.
	 *
	*/
	void writeVhdlHeader( QTextStream& vhdlStream, const QString& fileName );
	
	/*! \brief Write the generics of the top entity.
	 *
	 * \param vhdlStream The text stream to write into.
	 *
	*/
	void writeGenerics( QTextStream& vhdlStream );
	
	/*! \brief Write the ports of the top entity.
	 *
	 * \param vhdlStream The text stream to write into.
	 *
	*/
	void writePorts( QTextStream& vhdlStream );
	
	/*! \brief Write the declarations of the signals.
	 * 
	 * \param vhdlStream The text stream to write into.
	 *
	*/
	void writeSignalDeclarations( QTextStream& vhdlStream );
	
	/*! \brief Write the declarations of components.
	 *
	 * \param vhdlStream The text stream to write into.
	 *
	*/
	void writeComponentDeclarations( QTextStream& vhdlStream );
	
	/*! \brief Write the instances of components.
	 *
	 * \param vhdlStream The text stream to write into.
	 *
	*/
	void writeComponentInstances( QTextStream& vhdlStream );
	
	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the top component.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the top design.
	QSharedPointer<Design> design_;

	//! \brief Pointer to the top configuration
	QSharedPointer<DesignConfiguration> desConf_;

	//! \brief The name of the view used on the top component.
	QString viewName_;

	//! \brief The name of the top level entity to create.
	QString topLevelEntity_;

	//! \brief List of libraries to use
	QStringList libraries_;

	//! \brief The type definitions of the port types used.
	QStringList typeDefinitions_;

	/*! \brief Contains the generics for the top level entity.
	 * 
	 * Key = Name of the generic
	 * Value = Pointer to the generic.
	 */
	QMap<QString, QSharedPointer<VhdlGeneric> > topGenerics_;

	/*! \brief Contains the ports for the top level entity.
	 *
	 * Key = Element used to sort the ports in correct order.
	 * Value = Pointer to the port.
	 */
	QMap<VhdlPortSorter, QSharedPointer<VhdlPort> > topPorts_;

	/*! \brief Contains the signals for the architecture
	 * 
	 * Key = The end point associated with the signal.
	 * Value = Pointer to the signal to use.
	 */
	QMap<VhdlConnectionEndPoint, QSharedPointer<VhdlSignal> > signals_;

	/*! \brief Contains the component declarations of the architecture
	 * 
	 * Key = VLNV of the component IP-Xact model
	 * Value = Pointer to the vhdl component declaration
	 */
	QMap<VLNV, QSharedPointer<VhdlComponentDeclaration> > components_;

	/*! \brief Contains the instances of the architecture
	 * 
	 * Key = Name of the instance
	 * Value = Pointer to the component instance
	 */
	QMap<QString, QSharedPointer<VhdlComponentInstance> > instances_;
};

#endif // VHDLGENERATOR2_H
