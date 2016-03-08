//-----------------------------------------------------------------------------
// File: vhdlgenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 26.10.2011
//
// Description:
// Vhdl Generator generates top-level vhdl for hierarchical component.
//-----------------------------------------------------------------------------

#ifndef VHDLGENERATOR2_H
#define VHDLGENERATOR2_H

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/common/VLNV.h>

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
class HWDesignWidget;
class PortMap;
class DesignValidator;
class DesignConfigurationValidator;
class ExpressionParser;
class PortAlignment;

//-----------------------------------------------------------------------------
//! Vhdl Generator generates top-level vhdl for hierarchical component.
//-----------------------------------------------------------------------------
class VhdlGenerator2 : public QObject
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] handler     Pointer to the instance that manages the library.
	 *      @param [in] parent      Pointer to the owner of this generator.
	 */
	VhdlGenerator2(QSharedPointer<ExpressionParser> parser, LibraryInterface* handler,
        QObject* parent);
	
	//! The destructor.
	virtual ~VhdlGenerator2();

	/*!
     *  Get pointer to the instance that manages the library.
	 *
	 *      @return LibraryInterface* Pointer to the instance that manages the library.
	 */
	LibraryInterface* handler() const;

	/*!
     *  Parse the data needed to create the top-level vhdl entity.
	 *
	 *      @param [in] topLevelComponent   Pointer to the component that's entity is created.
	 *      @param [in] viewName            The name of the view on the top component to generate the vhdl to.
	 *
	 *      @return bool True if the parsing was successful and the vhdl can be generated.
	 */
	bool parse(QSharedPointer<Component> topLevelComponent, const QString& viewName);

	/*!
     *  Generate the vhdl that has been parsed.
	 * 
	 *  NOTE: Before calling this function the parse() function MUST be called so there is something to generate.
	 *
	 *      @param [in] outputFileName  The absolute file path of the vhdl file to be created.
     *                                  If file exists it is overwritten.
	 */
	void generate(const QString& outputFileName);

	/*!
     *  Add a new view to the top component's IP-Xact metadata.
	 * 
	 *  NOTE: Before calling this function the parse() function must be called and usually the generateVhdl() also.
     *  If generateVhdl() is not called then the architecture name must be set other way.
	 * 
	 *      @param [in] vhdlFileName    Absolute file path to the vhdl file to add to the metadata. This is 
     *                                  usually the same file that was set as output file when calling
     *                                  generateVhdl().
	 *
	 *      @return bool True if the view was added successfully.
	 */
	bool addRTLView(const QString& vhdlFileName);

	/*!
     *  Check if the generator is able to generate and architecture for top entity.
	 *
	 *      @return True if component contains a design and is able to create an architecture for the entity.
	 */
	bool containsArchitecture() const;

signals:

	//! Print a notification to user.
	void noticeMessage(const QString& noticeMessage);

	//! Print a error message to user.
	void errorMessage(const QString& errorMessage);

private:

	//! No copying.
	VhdlGenerator2(const VhdlGenerator2& other);

	//! No assignment.
	VhdlGenerator2& operator=(const VhdlGenerator2& other);

	//! Contains info on what port to connect.
	struct PortConnection
    {
		//! Pointer to the instance the port belongs to.
		QSharedPointer<VhdlComponentInstance> instance_;

		//! The name of the port on the instance to connect.
		QString portName_;

		//! The left bound of the port to connect.
// 		QString leftBound_;
        int leftBound_;

		//! The right bound of the port to connect.
// 		QString rightBound_;
        int rightBound_;

		/*!
         *  The constructor
		 *
		 *      @param [in] instance    Pointer to the instance to connect.
		 *      @param [in] portName    The name of the port on the instance.
		 *      @param [in] left        The left bound of the port.
		 *      @param [in] right       The right bound of the port.
		 */
// 		PortConnection(QSharedPointer<VhdlComponentInstance> instance, const QString& portName,
//             QString left = "-1", QString right = "-1");
        PortConnection(QSharedPointer<VhdlComponentInstance> instance, const QString& portName,
            int left = -1, int right = -1);

		//! Copy constructor.
		PortConnection(const PortConnection& other);

		//! Assignment operator.
		PortConnection& operator=(const PortConnection& other);
	};

	/*!
     *  Parse the design and configuration used.
	 * 
	 *      @return True if the design and configuration were parsed successfully.
	 */
	bool parseDesignAndConfiguration();

	/*!
     *  Parse the generics for the top entity to create.
     *
     *      @param [in] viewName    Name of the selected view.
	 */
	void parseTopGenerics( QString const& viewName );

	/*!
     *  Parse the ports for the top entity to create.
	 */
	void parseTopPorts();

	/*!
     *  Parse the component instances from the design.
	 */
	void parseInstances();

	/*!
     *  Parse the interconnections of a design.
	 */
	void parseInterconnections();

    void parseInstanceInterconnection(QSharedPointer<Interconnection> connection, bool& invalidInterconnection,
        QSharedPointer<VhdlComponentInstance> startInstance, QSharedPointer<BusInterface> startInterface,
        QSharedPointer<ActiveInterface> connectionEndInterface);

    void parseHierarchicalConnection(QSharedPointer<VhdlComponentInstance> startInstance,
        QSharedPointer<BusInterface> startInterface, QSharedPointer<HierInterface> endInterface);

	/*!
     *  Parse the ad hoc connections of a design.
	 */
	void parseAdHocConnections();

	/*!
     *  Parse the hierarchical connections of a design.
	 */
// 	void parseHierConnections();

	/*!
     *  Connect the two interfaces together with signals.
	 *
	 *      @param [in] connectionName  The name of the connection between the interfaces.
	 *      @param [in] description     The description of the interconnection.
	 *      @param [in] instance1       Pointer to the instance 1.
	 *      @param [in] interface1      Pointer to the bus interface of instance 1.
	 *      @param [in] instance2       Pointer to the instance 2.
	 *      @param [in] interface2      Pointer to the bus interface of instance 2.
	 */
	void connectInterfaces(const QString& connectionName, const QString& description,
        QSharedPointer<VhdlComponentInstance> instance1, QSharedPointer<BusInterface> interface1,
        QSharedPointer<VhdlComponentInstance> instance2, QSharedPointer<BusInterface> interface2);

	/*!
     *  Connect the specified ports together.
	 *
	 *      @param [in] connectionName  The name of the connection.
	 *      @param [in] description     Description for the connection.
	 *      @param [in] ports           Contains the ports to connect.
	 */
	void connectPorts(const QString& connectionName, const QString& description,
        const QList<VhdlGenerator2::PortConnection>& ports);

	/*!
     *  Connect the specified top port to specified instance ports.
	 *
	 *      @param [in] topPort     The name of the port on top component.
	 *      @param [in] leftBound   The left bound of the port on top component.
	 *      @param [in] rightBound  The right bound of the port on top component.
	 *      @param [in] ports       List of ports to connect to the top port
	 */
	void connectHierPort(const QString& topPortName, QString leftBound, QString rightBound,
        const QList<VhdlGenerator2::PortConnection>& ports);

	/*!
     *  Connect the given endpoint with the signal.
	 *
	 *      @param [in] endpoint    The end point that specified the port to connect.
	 *      @param [in] signal      The signal the port is connected to.
	 */
	void connectEndPoint(const VhdlConnectionEndPoint& endpoint, const QSharedPointer<VhdlSignal> signal);

	/*!
     *  Connect the hierarchical interface to the instance interface.
	 *
	 *      @param [in] instance            Pointer to the instance that's interface is connected.
	 *      @param [in] instanceInterface   Pointer to the interface of the instance.
	 *      @param [in] topInterface        Pointer to the interface of the top component.
	 */
	void connectHierInterface( QSharedPointer<VhdlComponentInstance> instance,
        QSharedPointer<BusInterface> instanceInterface, QSharedPointer<BusInterface> topInterface );

	/*!
     *  Map ports of component instances to signals.
	 *
	 *  NOTE: This function must be called only after all end points of signals have been parsed.
	 */
	void mapPorts2Signals();
	
	/*!
     *  Write the header of a vhdl file to be created.
	 *
	 *      @param [in] vhdlStream  The text stream to write into.
	 *      @param [in] fileName    The name of the file that is being written.
	 */
	void writeVhdlHeader( QTextStream& vhdlStream, const QString& fileName );
	
	/*!
     *  Write the generics of the top entity.
	 *
	 *      @param [in] vhdlStream  The text stream to write into.
	 */
	void writeGenerics( QTextStream& vhdlStream );
	
	/*!
     *  Write the ports of the top entity.
	 *
	 *      @param [in] vhdlStream  The text stream to write into.
	 */
	void writePorts( QTextStream& vhdlStream );
	
	/*!
     *  Write the declarations of the signals.
	 * 
	 *      @param [in] vhdlStream  The text stream to write into.
	 */
	void writeSignalDeclarations( QTextStream& vhdlStream );
	
	/*!
     *  Write the declarations of components.
	 *
	 *      @param [in] vhdlStream  The text stream to write into.
	 */
	void writeComponentDeclarations( QTextStream& vhdlStream );
	
	/*!
     *  Write the instances of components.
	 *
	 *      @param [in] vhdlStream  The text stream to write into.
	 */
	void writeComponentInstances( QTextStream& vhdlStream );

	/*!
     *  Read an old vhdl file and read the user modifiable parts from it.
	 *
	 *      @param [in] previousFile    The file to read.
	 */
	void readUserModifiablePart(QFile& previousFile);

	/*!
     *  Write the user modified declarations read from the previous file.
	 *
	 *      @param [in] stream  The text stream to write into.
	 */
	void writeUserModifiedDeclarations(QTextStream& stream);

	/*!
     *  Write the user modified assignments read from the previous file.
	 *
	 *      @param [in] stream  The text stream to write into.
	 */
	void writeUserModifiedAssignments(QTextStream& stream);
	
    /*!
     *  Calculate the port alignment.
     *
     *      @param [in] portMap1            First port map.
     *      @param [in] phys1LeftBound      Physical left bound of the first port.
     *      @param [in] phys1RightBound     Physical right bound of the first port.
     *      @param [in] portMap2            The second port map.
     *      @param [in] phys2LeftBound      Physical left bound of the second port.
     *      @param [in] phys2RightBound     Physical right bound of the second port.
     */
    General::PortAlignment calculatePortAlignment(const PortMap* portMap1, QString phys1LeftBound,
        QString phys1RightBound, QSharedPointer<ExpressionParser> firstParser, const PortMap* portMap2,
        QString phys2LeftBound, QString phys2RightBound, QSharedPointer<ExpressionParser> secondParser);

    /*!
     *  Get the alignment of the physical port mapping.
     *
     *      @param [in] portmap         The containing port map.
     *      @param [in] portLeftBound   The left bound of the selected port.
     *      @param [in] portRightBound  The right bound of the selected port.
     *      @param [in] parser          Expression parser for the containing component.
     *
     *      @return The alignment of the physical port mapping.
     */
    QSharedPointer<PortAlignment> getPhysicalAlignment(const PortMap* portmap, QString const& portLeftBound,
        QString const& portRightBound, QSharedPointer<ExpressionParser> parser) const;

    /*!
     *  Get the logical minimum or maximum port mapping value.
     *
     *      @param [in] portMap     Port map containing the logical port.
     *      @param [in] parser      Expression parser for the containing component.
     *      @param [in] isMaximum   Flag value for determining whether the maximum of minimum value is wanted.
     *
     *      @return A logical bound of the logical port mapping.
     */
    int getLogicalValue(const PortMap* portMap, QSharedPointer<ExpressionParser> parser, bool isMaximum);

    /*!
     *  Get the name of the containing bus interface.
     *
     *      @param [in] containingComponent     The component containing the port and the bus interface.
     *      @param [in] portName                The name of the selected port.
     *
     *      @return The name of the containing bus interface, or "none" if one is not found.
     */
    QString getContainingBusInterfaceName(QSharedPointer<Component> containingComponent, QString const& portName)
        const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! Pointer to the top component.
	QSharedPointer<Component> component_;

	//! Pointer to the top design.
	QSharedPointer<Design> design_;

	//! Pointer to the top configuration
	QSharedPointer<DesignConfiguration> desConf_;

	//! The name of the view used on the top component.
	QString viewName_;

	//! The name of the top level entity to create.
	QString topLevelEntity_;

	//! List of libraries to use
	QStringList libraries_;

	//! The type definitions of the port types used.
	QStringList typeDefinitions_;

	//! Contains the declarations that user has written by hand and that should be saved
	QString userModifiedDeclarations_;

	//! Contains the vhdl code that user has written by hand and that should be saved
	QString userModifiedAssignments_;

	/*! Contains the generics for the top level entity.
	 *      Key = Name of the generic
	 *      Value = Pointer to the generic.
	 */
	QMap<QString, QSharedPointer<VhdlGeneric> > topGenerics_;

	/*! Contains the ports for the top level entity.
	 *      Key = Element used to sort the ports in correct order.
	 *      Value = Pointer to the port.
	 */
	QMap<VhdlPortSorter, QSharedPointer<VhdlPort> > topPorts_;

	/*! Contains the signals for the architecture
	 *      Key = The end point associated with the signal.
	 *      Value = Pointer to the signal to use.
	 */
	QMap<VhdlConnectionEndPoint, QSharedPointer<VhdlSignal> > signals_;

	/*! Contains the component declarations of the architecture
	 *      Key = VLNV of the component IP-Xact model
	 *      Value = Pointer to the vhdl component declaration
	 */
	QMap<VLNV, QSharedPointer<VhdlComponentDeclaration> > components_;

	/*! Contains the instances of the architecture
	 *      Key = Name of the instance
	 *      Value = Pointer to the component instance
	 */
	QMap<QString, QSharedPointer<VhdlComponentInstance> > instances_;

    //! The used design validator.
    QSharedPointer<DesignValidator> designvalidator_;

    //! The used design configuration validator.
    QSharedPointer<DesignConfigurationValidator> designConfigurationValidator_;

    //! The expression parser for the top component parser.
    QSharedPointer<ExpressionParser> topComponentParser_;
};

#endif // VHDLGENERATOR2_H
