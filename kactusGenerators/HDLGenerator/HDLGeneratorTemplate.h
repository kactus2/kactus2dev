//-----------------------------------------------------------------------------
// File: HDLGeneratorTemplate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.7.2014
//
// Description:
// Creates a structural HDL file with following structure
//    *      1. File header 
//    *      2. Libraries
//    *      3. Entity declaration
//    *      4. Architecture
//    *         - Signals (+ user's own code)
//    *         - Component declarations
//    *         - Component instantiations (+ user's own code)
//-----------------------------------------------------------------------------

#ifndef HDLGENERATORTEMPLATE_H
#define HDLGENERATORTEMPLATE_H

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>

#include <library/LibraryManager/vlnv.h>

#include <kactusGenerators/HDLGenerator/HDLModelParameter.h>
#include <kactusGenerators/HDLGenerator/HDLPort.h>
#include <kactusGenerators/HDLGenerator/vhdlportsorter.h>
#include <kactusGenerators/HDLGenerator/VhdlConnectionEndPoint.h>
#include <kactusGenerators/HDLGenerator/HDLSignal.h>
#include <kactusGenerators/HDLGenerator/HDLcomponentdeclaration.h>
#include <kactusGenerators/HDLGenerator/HDLcomponentinstance.h>

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Template class for HDL generators that generate top-level HDL for a hierarchical component.
//-----------------------------------------------------------------------------
class HDLGeneratorTemplate : public QObject
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *     @param [in]  handler     The instance of the library manager.
	 *     @param [in]  parent      The parent object.
	 *
	 */
	HDLGeneratorTemplate(LibraryInterface* handler, QObject* parent);
	
	//! The destructor
	virtual ~HDLGeneratorTemplate();

	/*! Parse the data needed to create the top-level entity.
	 *
	 *     @param [in]  topLevelComponent   The component whose entity is created.
	 *     @param [in]  viewName            The name of the view on the top component to generate the HDL to.
	 *
	 *     @return True if the parsing was successful and the HDL can be generated.
	 */
	bool parse(QSharedPointer<Component> topLevelComponent, QString const& viewName);

	/*! Generate the HDL that has been parsed.  
	 *
	 *     @param [in]  outputFileName  The absolute file path of the HDL file to be created.
     *                                  If file exists it is overwritten.
     *
     *     @remarks Before calling this function the parse() function MUST be called so there is 
     *              something to generate. 
	 *
	 */
	void generate(QString const& outputFileName);

signals:

	//! Print a notification to user.
	void noticeMessage(QString const& noticeMessage);

	//! Print a error message to user.
	void errorMessage(QString const& errorMessage);

protected:

	//! Contains info on what port to connect.
	struct PortConnection {

		//! Pointer to the instance the port belongs to.
		QSharedPointer<HDLComponentInstance> instance_;

		//! The name of the port on the instance to connect.
		QString portName_;

		//! The left bound of the port to connect.
		int leftBound_;

		//! The right bound of the port to connect.
		int rightBound_;

		/*! The constructor
		 *
		 *     @param [in]  instance    Pointer to the instance to connect.
		 *     @param [in]  portName    The name of the port on the instance.
		 *     @param [in]  left        The left bound of the port.
		 *     @param [in]  right       The right bound of the port.
		 *
		*/
		PortConnection(QSharedPointer<HDLComponentInstance> instance, 
			QString const& portName, int left = -1, int right = -1);

		//! Copy constructor
		PortConnection(const PortConnection& other);

		//! Assignment operator
		PortConnection& operator=(const PortConnection& other);
	};

	/*! Get pointer to the instance that manages the library.
	 *
	 *     @return Pointer to the instance that manages the library.
	 */
	LibraryInterface* handler() const;

    //! Pointer to the top component.
    QSharedPointer<Component> component_;

    //! The name of the view used on the top component.
    QString viewName_;

    //! The name of the top level entity to create.
    QString topLevelEntity_;

    //! List of libraries to use.
    QStringList libraries_;

    //! The type definitions of the port types used.
    QStringList typeDefinitions_;

private:
	//! No copying
	HDLGeneratorTemplate(const HDLGeneratorTemplate& other);

	//! No assignment
	HDLGeneratorTemplate& operator=(const HDLGeneratorTemplate& other);
	
	/*! Write the header of a HDL file to be created.
	 *
	 *     @param [in]  outputStream    The text stream to write into.
	 *     @param [in]  fileName        The name of the file that is being written.
	 */
	virtual void writeFileHeader(QTextStream& outputStream, QString const& fileName) = 0;

    /*!
     *  Writes the includes for all required libraries.
     *
     *      @param [in] outputStream    The output stream to write to.    
     */
    virtual void writeLibraryIncludes(QTextStream& outputStream) = 0;

    /*!
     *  Writes the start tag for the top level entity.
     *
     *      @param [in] outputStream    The output stream to write to.    
     */
    virtual void writeEntityStartTag(QTextStream& outputStream) = 0;

    /*!
     *  Writes the end tag for the top level entity.
     *
     *      @param [in] outputStream    The output stream to write to.    
     */
    virtual void writeEntityEndTag(QTextStream& outputStream) = 0;

    /*!
     *  Writes the start tag for the top level structural description.
     *
     *      @param [in] outputStream    The output stream to write to.    
     */
    virtual void writeStructuralStartTag(QTextStream& outputStream) = 0;

    /*!
     *  Writes the end tag for the top level structural description.
     *
     *      @param [in] outputStream    The output stream to write to.    
     */
    virtual void writeStructuralEndTag(QTextStream& outputStream) = 0;

    /*!
     *  Writes the start tag for the component instantiations.
     *
     *      @param [in] outputStream    The output stream to write to.    
     */
    virtual void writeComponentInstantiationStartTag(QTextStream& outputStream) = 0;


	/*! Parse the design and configuration used.
	 * 
	 *     @return True if the design and configuration were parsed successfully.
	*/
	bool parseDesignAndConfiguration();

	/*! Parse the design.
	 * 
	 *     @return True if the design was parsed successfully.
	*/
    bool parseDesign(VLNV const& hierarchyRef);

	/*! Parse the design configuration.
	 * 
	 *     @return True if the design configuration was parsed successfully.
	*/
    bool parseDesignConfiguration(VLNV const& hierarchyRef);

	/*! Parse the generics for the top entity to create.
	 *
	*/
	void parseTopModelParameters();

    /*!
     *  Creates an object representing one model parameter in HDL.
     *
     *      @param [in] modelParam   The model parameter to represent.
     *
     *      @return The object capturing the HDL representation of the model parameter.
     */
    virtual HDLModelParameter* createModelParameterObject(QSharedPointer<ModelParameter> &modelParam) = 0;

	/*! Parse the ports for the top entity to create.
	 *
	*/
	void parseTopPorts();

    /*!
     *  Creates an object representing one port in HDL.
     *
     *      @param [in] port   The port to represent.
     *
     *      @return The object capturing the HDL representation of the port.
     */
    virtual HDLPort* createPortObject(QSharedPointer<Port>& port) = 0;

	/*! Parse the component instances from the design.
	 *
	*/
	void parseInstances();

    /*!
     *  Creates an object representing one component declaration in HDL.
     *
     *      @param [in] component   The component to represent.
     *
     *      @return The object capturing the HDL representation of the component.
     */
    virtual HDLComponentDeclaration* createComponentDeclarationObject(QSharedPointer<Component> component) = 0;

    /*!
     *  Creates an object representing one component instance in HDL.
     *
     *      @param [in] component   The component to represent.
     *
     *      @return The object capturing the HDL representation of the component instance.
     */
    virtual HDLComponentInstance* createComponentInstanceObject(QSharedPointer<Component> component, 
        ComponentInstance const &instance, QString instanceActiveView) = 0;


    /*!
     *  Finds the libraries required by a given component instance.
     *
     *      @param [in] compInstance        The component instance whose libraries to find.
     *      @param [in] instanceActiveView  The active view of the component instance.
     *
     *      @return The libraries for the component instance.
     */
    virtual QStringList getLibrariesForComponent(QSharedPointer<HDLComponentInstance> compInstance, 
        QString instanceActiveView) = 0;

	/*! Parse the interconnections of a design.
	 *
	*/
	void parseInterconnections();

	/*! Parse the ad hoc connections of a design.
	 *
	*/
	void parseAdHocConnections();

	/*! Parse the hierarchical connections of a design.
	 *
	*/
	void parseHierConnections();

	/*! Connect the two interfaces together with signals.
	 *
	 *     @param [in]  connectionName The name of the connection between the interfaces.
	 *     @param [in]  description The description of the interconnection.
	 *     @param [in]  instance1 Pointer to the instance 1.
	 *     @param [in]  interface1 Pointer to the bus interface of instance 1.
	 *     @param [in]  instance2 Pointer to the instance 2.
	 *     @param [in]  interface2 Pointer to the bus interface of instance 2.
	 *
	*/
	void connectInterfaces(QString const& connectionName, QString const& description, 
        QSharedPointer<HDLComponentInstance> instance1, QSharedPointer<BusInterface> interface1, 
        QSharedPointer<HDLComponentInstance> instance2, QSharedPointer<BusInterface> interface2);

	/*! Connect the specified ports together.
	 *
	 *     @param [in]  connectionName The name of the connection.
	 *     @param [in]  description Description for the connection.
	 *     @param [in]  ports Contains the ports to connect.
	 *
	*/
	void connectPorts(QString const& connectionName, QString const& description, 
        const QList<PortConnection>& ports);

    /*!
     *  Checks if the given port and connection type can be connected.
     *
     *      @param [in] type            The port type.
     *      @param [in] connectionType  The connection type.          
     */
    virtual bool checkPortTypeMatch(QString type, QString connectionType) = 0;


    /*!
     *  Creates an object representing one signal in HDL.
     *
     *      @param [in] connectionName   The name of the connection.
     *      @param [in] type             The type of the connection.
     *      @param [in] left             The left bound of the connection.
     *      @param [in] right            The right bound of the connection.
     *      @param [in] description      The description of the connection.
     *
     *      @return The object capturing the HDL representation of the connection.
     */
    virtual HDLSignal* createSignalObject(QString const& connectionName, QString type, 
        int left, int right, QString const& description) = 0;


    /*!
     *  Gets the type for a scalar signal.
     *
     *      @return The scalar signal type.
     */
    virtual QString scalarSignalType() = 0;
    
    /*!
     *  Gets the type for a vector signal.
     *
     *      @return The vector signal type.
     */
    virtual QString vectorSignalType() = 0;

	/*! Connect the specified top port to specified instance ports.
	 *
	 *     @param [in]  topPort The name of the port on top component.
	 *     @param [in]  leftBound The left bound of the port on top component.
	 *     @param [in]  rightBound The right bound of the port on top component.
	 *     @param [in]  ports List of ports to connect to the top port
	 *
	*/
	void connectHierPort(QString const& topPort, int leftBound, int rightBound,
        const QList<PortConnection>& ports);

    /*!
     *  Checks if the given connection type is a scalar type.
     *
     *      @param [in] type   The type to check.
     *
     *      @return True, if the type is scalar, otherwise false.
     */
    virtual bool isScalarType(QString const& type) const = 0;

	/*! Connect the given endpoint with the signal.
	 *
	 *     @param [in]  endpoint The end point that specified the port to connect.
	 *     @param [in]  signal The signal the port is connected to.
	 *
	*/
	void connectEndPoint(const VhdlConnectionEndPoint& endpoint,
		const QSharedPointer<HDLSignal> signal);

	/*! Connect the hierarchical interface to the instance interface.
	 *
	 *     @param [in]  instance Pointer to the instance that's interface is connected.
	 *     @param [in]  instanceInterface Pointer to the interface of the instance.
	 *     @param [in]  topInterface Pointer to the interface of the top component.
	 *
	*/
	void connectHierInterface( QSharedPointer<HDLComponentInstance> instance, 
        QSharedPointer<BusInterface> instanceInterface, QSharedPointer<BusInterface> topInterface );

	/*! Map ports of component instances to signals.
	 *
	 *      @remarks This function must be called only after all end points of signals have been parsed.
	*/
	void mapPorts2Signals();
	
	/*! Write the model parameters of the top entity.
	 *
	 *     @param [in]  outputStream The text stream to write into.
	*/
	void writeModelParameters( QTextStream& outputStream );

	/*! Writes the start tag for model parameters.
	 *
	 *     @param [in]  outputStream The text stream to write into.
	*/
    virtual void writeModelParametersStartTag(QTextStream& outputStream) = 0;

	/*! Writes the end tag for model parameters.
	 *
	 *     @param [in]  outputStream The text stream to write into.
	*/
    virtual void writeModelParameterEndTag(QTextStream& outputStream) = 0;

	/*! Write the ports of the top entity.
	 *
	 *     @param [in]  outputStream The text stream to write into.
	 *
	*/
	void writePorts( QTextStream& outputStream );

	/*! Writes the start tag for top level ports.
	 *
	 *     @param [in]  outputStream The text stream to write into.
	*/
    virtual void writePortsStartTag(QTextStream& outputStream) = 0;

	/*! Writes the end tag for top level ports.
	 *
	 *     @param [in]  outputStream The text stream to write into.
	*/
    virtual void writePortsEndTag(QTextStream& outputStream) = 0;

    
    /*!
     *  Gets the tag for single-line comment.
     *
     *      @return The comment tag.
     */
    virtual QString commentTag() const = 0;

	/*! Write the declarations of the signals.
	 * 
	 *     @param [in]  outputStream The text stream to write into.
	 *
	*/
	void writeSignalDeclarations( QTextStream& outputStream );
	
	/*! Write the declarations of components.
	 *
	 *     @param [in]  outputStream The text stream to write into.
	 *
	*/
	void writeComponentDeclarations( QTextStream& outputStream );
	
	/*! Write the instances of components.
	 *
	 *     @param [in]  outputStream The text stream to write into.
	 *
	*/
	void writeComponentInstances( QTextStream& outputStream );

	/*! Read an old HDL file and read the user modifiable parts from it.
	 *
	 *     @param [in]  previousFile The file to read.
	 *
	*/
	void readUserModifiablePart(QFile& previousFile);

	/*! Write the user modified declarations read from the previous file.
	 *
	 *     @param [in]  stream The text stream to write into.
	 *
	*/
	void writeUserModifiedDeclarations(QTextStream& stream);

	/*! Write the user modified assignments read from the previous file.
	 *
	 *     @param [in]  stream The text stream to write into.
	 *
	*/
	void writeUserModifiedAssignments(QTextStream& stream);
	
	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! Pointer to the top design.
	QSharedPointer<Design> design_;

	//! Pointer to the top configuration
	QSharedPointer<DesignConfiguration> desConf_;  

	//! Contains the declarations that user has written by hand and that should be saved
	QString userModifiedDeclarations_;

	//! Contains the HDL code that user has written by hand and that should be saved
	QString userModifiedAssignments_;

	/*! Contains the generics for the top level entity.
	 * 
	 * Key = Name of the generic
	 * Value = Pointer to the generic.
	 */
	QMap<QString, QSharedPointer<HDLModelParameter> > topModelParameters_;

	/*! Contains the ports for the top level entity.
	 *
	 * Key = Element used to sort the ports in correct order.
	 * Value = Pointer to the port.
	 */
	QMap<VhdlPortSorter, QSharedPointer<HDLPort> > topPorts_;

	/*! Contains the signals for the architecture
	 * 
	 * Key = The end point associated with the signal.
	 * Value = Pointer to the signal to use.
	 */
	QMap<VhdlConnectionEndPoint, QSharedPointer<HDLSignal> > signals_;

	/*! Contains the component declarations of the architecture
	 * 
	 * Key = VLNV of the component IP-Xact model
	 * Value = Pointer to the HDL component declaration
	 */
	QMap<VLNV, QSharedPointer<HDLComponentDeclaration> > componentDeclarations_;

	/*! Contains the instances of the architecture
	 * 
	 * Key = Name of the instance
	 * Value = Pointer to the component instance
	 */
	QMap<QString, QSharedPointer<HDLComponentInstance> > instances_;
};

#endif // HDLGENERATORTEMPLATE_H
