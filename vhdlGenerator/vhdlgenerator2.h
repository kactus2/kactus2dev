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
class MainWindow;

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
	VhdlGenerator2(LibraryInterface* handler, MainWindow* parent);
	
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
	 * \param architectureName The name of the architecture to create for the top
	 * component.
	 *
	*/
	void generateVhdl(const QString& outputFileName, const QString& architectureName);

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

	/*! \brief Set the architecture name for the vhdl to be generated.
	 *
	 * \param architectureName contains the architecture name.
	 *
	*/
	void setArchitectureName(const QString& architectureName);

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

	//! \brief The name of the architecture to create.
	QString architectureName_;

	//! \brief The name of the top level entity to create.
	QString topLevelEntity_;

	//! \brief List of libraries to use
	QStringList libraries_;

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
