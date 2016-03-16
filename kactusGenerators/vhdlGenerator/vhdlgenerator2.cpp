/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgenerator2.cpp
 *		Project: Kactus 2
 *
 *      Creates a structural VHDL file with following structure
 *      1. File header 
 *      2. Libraries
 *      3. Entity declaration
 *      4. Architecture
 *         - Signals (+ user's own code)
 *         - Component declarations
 *         - Component instantiations (+ user's own code)
 */

#include "vhdlgenerator2.h"

#include "vhdlgeneral.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/PortAlignment.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>

#include <IPXACTmodels/Design/Interconnection.h>
#include <IPXACTmodels/Design/validator/DesignValidator.h>
#include <IPXACTmodels/designConfiguration/validators/DesignConfigurationValidator.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatterFactoryImplementation.h>

#include <common/utils.h>

#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QDate>
#include <QSettings>

static const QString BLACK_BOX_DECL_START = "-- ##KACTUS2_BLACK_BOX_DECLARATIONS_BEGIN##";
static const QString BLACK_BOX_DECL_END = "-- ##KACTUS2_BLACK_BOX_DECLARATIONS_END##";
static const QString BLACK_BOX_ASSIGN_START = "-- ##KACTUS2_BLACK_BOX_ASSIGNMENTS_BEGIN##";
static const QString BLACK_BOX_ASSIGN_END = "-- ##KACTUS2_BLACK_BOX_ASSIGNMENTS_END##";

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::VhdlGenerator2()
//-----------------------------------------------------------------------------
VhdlGenerator2::VhdlGenerator2(QSharedPointer<ExpressionParser> parser, LibraryInterface* handler, QObject* parent):
QObject(parent),
handler_(handler),
component_(),
design_(),
desConf_(),
viewName_(),
topLevelEntity_(),
libraries_(),
typeDefinitions_(),
userModifiedDeclarations_(),
userModifiedAssignments_(),
topGenerics_(),
topPorts_(),
signals_(),
components_(),
instances_(),
designvalidator_(),
designConfigurationValidator_(),
topComponentParser_()
{
	Q_ASSERT(handler);

    designvalidator_ = QSharedPointer<DesignValidator> (new DesignValidator(parser, handler_));
    designConfigurationValidator_ =
        QSharedPointer<DesignConfigurationValidator>(new DesignConfigurationValidator(parser, handler_));
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::~VhdlGenerator2()
//-----------------------------------------------------------------------------
VhdlGenerator2::~VhdlGenerator2()
{

}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::handler()
//-----------------------------------------------------------------------------
LibraryInterface* VhdlGenerator2::handler() const
{
	return handler_;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::parse()
//-----------------------------------------------------------------------------
bool VhdlGenerator2::parse( QSharedPointer<Component> topLevelComponent, const QString& viewName )
{
	Q_ASSERT(topLevelComponent);
	component_ = topLevelComponent;

    QSharedPointer<ComponentParameterFinder> topFinder (new ComponentParameterFinder(topLevelComponent));
    topComponentParser_ = QSharedPointer<IPXactSystemVerilogParser>(new IPXactSystemVerilogParser(topFinder));

	emit noticeMessage(tr("Parsing the IP-Xact models..."));

	// the name of the top-level entity is the name of the top-level component
	topLevelEntity_ = component_->getVlnv().getName();
	viewName_ = viewName;
	
	// if the parsing fails
	if (!parseDesignAndConfiguration())
    {
		return false;
	}

	// get the types that are used for the ports.
	foreach ( QSharedPointer<Port> port, *component_->getPorts() )
	{
		typeDefinitions_.append( port->getTypeDefinitions() );
	}

	// parse the info for the top entity
	parseTopGenerics(viewName);
	parseTopPorts();

	// if design is used then these can be parsed also
	if (design_)
    {
		parseInstances();
		parseInterconnections();
		parseAdHocConnections();
		mapPorts2Signals();

		// tell each instance to use the default port value for the unconnected ports.
		foreach (QSharedPointer<VhdlComponentInstance> instance, instances_)
        {
			instance->useDefaultsForOtherPorts();
		}

		// tell each component declaration to check for it's ports and uncomment those that are needed
		foreach (QSharedPointer<VhdlComponentDeclaration> comp, components_)
        {
			comp->checkPortConnections();
		}
	}
	// if design is not used then set all ports uncommented.
	else
    {
		for (QMap<VhdlPortSorter, QSharedPointer<VhdlPort> >::iterator i = topPorts_.begin();
			i != topPorts_.end(); ++i)
        {
			i.value()->setCommented(false);
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::generate()
//-----------------------------------------------------------------------------
void VhdlGenerator2::generate( const QString& outputFileName)
{
	if (outputFileName.isEmpty())
    {
		return;
	}

	QFile file(outputFileName);

	// if previous file exists then remove it.
	if (file.exists())
    {
		// read the existing user-modifiable code from the file
		readUserModifiablePart(file);

		file.remove();
	}

	// open the file and erase all old contents if any exists
	// if file could not be opened
	if (!file.open(QFile::Truncate | QFile::WriteOnly))
    {
		QString message(tr("File: "));
		message += outputFileName;
		message += tr(" couldn't be opened for writing");
		emit errorMessage(message);
		return;
	}
	// now file has been opened for writing

	// open file stream to write to
	QTextStream vhdlStream(&file);

	QFileInfo fileInfo(outputFileName);

	emit noticeMessage(tr("Writing the vhdl file..."));

	
	// write header comments of the file
	writeVhdlHeader(vhdlStream, fileInfo.fileName());

	// always add IEEE library
	vhdlStream << "library IEEE;" << endl;

	// write the libraries needed 
	libraries_.append("work");
	libraries_.removeDuplicates();

	// declare the libraries used.
	foreach (QString library, libraries_)
    {
		if (!library.isEmpty())
        {
			vhdlStream << "library " << library <<";" << endl;

			typeDefinitions_.append(QString("%1.all").arg(library));
		}
	}

	// always add the library for std_logic and std_logic_vector
	typeDefinitions_.append("IEEE.std_logic_1164.all");
	typeDefinitions_.removeDuplicates();

	// write all type defs needed
	foreach (QString portTypeDef, typeDefinitions_)
    {
		if (!portTypeDef.isEmpty())
        {
			vhdlStream << "use " << portTypeDef << ";" << endl;
		}
	}
	vhdlStream << endl;

	// write the top-level entity
	vhdlStream << "entity " << topLevelEntity_ << " is" << endl;
	vhdlStream << endl;

	// write the top-level generics
	writeGenerics(vhdlStream);

	// write the top-level ports
	writePorts(vhdlStream);

	// end top-level entity definition
	vhdlStream << "end " << topLevelEntity_ << ";" << endl << endl;


	// if view has description
	QString viewDescription;
	
	foreach ( QSharedPointer<View> currentView, *component_->getViews() )
	{
		if ( currentView->name() == viewName_ )
		{
			viewDescription = currentView->description();
			break;
		}
	}

	VhdlGeneral::writeDescription(viewDescription, vhdlStream);

	QString archName;
	// if view name is not specified then "structural" is used
	if (viewName_.isEmpty()) {
		archName = "structural";
	}
	else {
		archName = viewName_;
	}
	// write the architecture of the entity
	vhdlStream << "architecture " << archName << " of " << topLevelEntity_ << " is"
		<< endl << endl;

	// write declarations for signals connecting the ports
	writeSignalDeclarations(vhdlStream);

	// write all component declarations
	writeComponentDeclarations(vhdlStream);

	// write the user modifiable code 
	writeUserModifiedDeclarations(vhdlStream);

	// start writing architecture component instances
	vhdlStream << endl << "begin" << endl << endl;

	// write the user modifiable code
	writeUserModifiedAssignments(vhdlStream);

	// write the component instances
	writeComponentInstances(vhdlStream);

	vhdlStream << "end " << archName << ";" << endl << endl;

	file.close();

	emit noticeMessage(tr("Done writing the vhdl file."));

	return;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::addRTLView()
//-----------------------------------------------------------------------------
bool VhdlGenerator2::addRTLView( const QString& vhdlFileName )
{
	// ipDir represents the directory where the IP-Xact file is located in.
	QString ipDir(handler_->getPath(component_->getVlnv()));
	if (ipDir.isEmpty())
    {
		emit errorMessage(tr("Path to top-component was not found."));
		return false;
	}

	// get the relative path to add to file set
	QString relativePath = General::getRelativePath(ipDir, vhdlFileName);
	if (relativePath.isEmpty())
    {
		emit errorMessage(tr("Could not create relative path to vhdl file."));
		return false;
	}

	QString fileSetName;
	if (!viewName_.isEmpty())
    {
		fileSetName = QString("%1_vhdlSource").arg(viewName_);
	}
	else
    {
		fileSetName = QString("vhdlSource");
	}
	QSharedPointer<FileSet> topFileSet = component_->getFileSet(fileSetName);

	// if the top vhdl file set was not found. Create one
	if (!topFileSet)
    {
		topFileSet = QSharedPointer<FileSet>(new FileSet(fileSetName, QString("sourceFiles")));
		component_->getFileSets()->append(topFileSet);

		QSharedPointer<FileBuilder> vhdlBuilder(new FileBuilder("vhdlSource"));
		vhdlBuilder->setCommand("vcom");
		vhdlBuilder->setFlags("-quiet -check_synthesis -work work");
		vhdlBuilder->setReplaceDefaultFlags("true");
		topFileSet->getDefaultFileBuilders()->append(vhdlBuilder);

		QSharedPointer<FileBuilder> vhdl87Builder(new FileBuilder("vhdlSource-87"));
		vhdl87Builder->setCommand("vcom");
		vhdl87Builder->setFlags("-quiet -check_synthesis -work work");
		vhdl87Builder->setReplaceDefaultFlags("true");
		topFileSet->getDefaultFileBuilders()->append(vhdl87Builder);

		QSharedPointer<FileBuilder> vhdl93Builder(new FileBuilder("vhdlSource-93"));
		vhdl93Builder->setCommand("vcom");
		vhdl93Builder->setFlags("-quiet -check_synthesis -work work");
		vhdl93Builder->setReplaceDefaultFlags("true");
		topFileSet->getDefaultFileBuilders()->append(vhdl93Builder);
	}

	topFileSet->clearFiles();

	QSettings settings;

	// create a new file
	QSharedPointer<File> topVhdlFile = topFileSet->addFile(relativePath, settings);
	topVhdlFile->setIncludeFile(true);
	topVhdlFile->setLogicalName("work");
	topVhdlFile->setCommand(QString("vcom"));
	topVhdlFile->setBuildFlags(QString("-quiet -check_synthesis -work work"), "true");

	QString newViewName;
	if (!viewName_.isEmpty())
    {
		newViewName = QString("%1_vhd").arg(viewName_);
	}
	else
    {
		newViewName = "rtl";
	}
	QSharedPointer<View> rtlView( new View(newViewName) );

	// add the spirit:envIdentifier. Only language is defined, not tool
	// specific settings
	rtlView->addEnvIdentifier(QString("VHDL:Kactus2:"));

	QSharedPointer<ComponentInstantiation> componentInstantiation(new ComponentInstantiation);

	// Set the language of the instantiation.
	componentInstantiation->setLanguage(QString("vhdl"));

	// set the model name to be the top_level architecture of the top-level
	// entity
	QString archName;
	if (viewName_.isEmpty())
    {
		archName = "rtl";
	}
	else
    {
		archName = viewName_;
	}
	QString topEntity(QString("%1(%2)").arg(topLevelEntity_).arg(archName));
	componentInstantiation->setModuleName(topEntity);

	// set a reference to a file set
	componentInstantiation->getFileSetReferences()->append(fileSetName);

	// Add the view and component instantiation to component.
	component_->getViews()->append(rtlView);
	component_->getComponentInstantiations()->append(componentInstantiation);

	// find the active view used to generate the vhdl
	QSharedPointer<View> activeView;

	foreach (QSharedPointer<View> currentView, *component_->getViews())
	{
		if (currentView->name() == viewName_)
		{
			activeView = currentView;
			break;
		}
	}
	
	// if the view does not exist or it is not hierarchical
	if (!activeView || !activeView->isHierarchical())
    {
		return true;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::parseDesignAndConfiguration()
//-----------------------------------------------------------------------------
bool VhdlGenerator2::parseDesignAndConfiguration()
{
	Q_ASSERT(component_);

	// if view is not specified it is not error it just means that only the top entity should be created.
	if (viewName_.isEmpty())
    {
		return true;
	}

	// if view is specified but it does not exist
	QSharedPointer<View> view;
	
	foreach (QSharedPointer<View> currentView, *component_->getViews())
	{
		if (currentView->name() == viewName_)
		{
			view = currentView;
			break;
		}
	}
	
	// if view is not found
	if (!view)
    {
		return false;
	}
	else if (!view->isHierarchical())
    {
		return false;
	}

	QSharedPointer<DesignInstantiation> DI;
	QSharedPointer<DesignConfigurationInstantiation> DCI;

	// Try to find design instantiation.
	foreach (QSharedPointer<DesignInstantiation> currentInsta, *component_->getDesignInstantiations())
	{
		if (currentInsta->name() == view->getDesignInstantiationRef())
		{
			DI = currentInsta;
			break;
		}
	}

	// Try to find design configuration instantiation.
	foreach (QSharedPointer<DesignConfigurationInstantiation> currentInsta,
		*component_->getDesignConfigurationInstantiations())
	{
		if (currentInsta->name() == view->getDesignConfigurationInstantiationRef())
		{
			DCI = currentInsta;
			break;
		}
	}

	VLNV hierarchyRef;

	// if configuration is used
	if (DCI)
	{
		hierarchyRef = *DCI->getDesignConfigurationReference();
		QSharedPointer<Document> libComp = handler_->getModel(hierarchyRef);
		desConf_ = libComp.staticCast<DesignConfiguration>();
	}
	else if (DI)
	{
		hierarchyRef = *DI->getDesignReference();
	}
	// if the referenced document was not found
	else
	{
		emit errorMessage(tr("The design or design configuration reference within component %1's"
			" view %2 was not found in library.").arg(component_->getVlnv().toString()).arg(viewName_));
		return false;
	}
	
	design_ = handler_->getDesign(hierarchyRef);
	// if design was not found
	if (!design_)
    {
		VLNV designVLNV = handler_->getDesignVLNV(hierarchyRef);
		emit errorMessage(tr("The design %1 referenced in component %2 was not found in library.").arg(
			designVLNV.toString()).arg(component_->getVlnv().toString()));
		return false;
	}

	// if design is found then make sure it is valid
	else
    {
        if (!designvalidator_->validate(design_))
        {
            QVector<QString> errorList;
            designvalidator_->findErrorsIn(errorList, design_);

            emit noticeMessage(tr("The design '%1' contained the following errors:").
                arg(design_->getVlnv().toString()));

            foreach (QString designError, errorList)
            {
                emit errorMessage(designError);
            }

            return false;
        }
	}

	// if design configuration is found the make sure it is also valid
	if (desConf_)
    {
        if (!designConfigurationValidator_->validate(desConf_))
        {
            QVector<QString> errorList;
            designConfigurationValidator_->findErrorsIn(errorList, desConf_);

            emit noticeMessage(tr("The design configuration '%1' contained the following errors:").
                arg(desConf_->getVlnv().toString()));

            foreach (QString configurationError, errorList)
            {
                emit errorMessage(configurationError);
            }

            return false;
        }
	}
	

	// the design and possibly the configuration are now parsed
	return true;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::containsArchitecture()
//-----------------------------------------------------------------------------
bool VhdlGenerator2::containsArchitecture() const
{
	// if design exists then architecture can be created
	return design_;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::parseTopGenerics()
//-----------------------------------------------------------------------------
void VhdlGenerator2::parseTopGenerics(QString const& viewName)
{
	QSharedPointer<View> view;

	foreach (QSharedPointer<View> currentView, *component_->getViews())
	{
		if (currentView->name() == viewName)
		{
			view = currentView;
			break;
		}
	}

    if (view)
    {
        QSharedPointer<ComponentInstantiation> cimp;

        foreach (QSharedPointer<ComponentInstantiation> currentInsta, *component_->getComponentInstantiations())
        {
            if (currentInsta->name() == view->getComponentInstantiationRef())
            {
                cimp = currentInsta;
                break;
            }
        }
        
        if (cimp)
        {
            foreach (QSharedPointer<ModuleParameter> moduleParam, *cimp->getModuleParameters())
            {
                QString name = moduleParam->name();
                QSharedPointer<VhdlGeneric> generic(new VhdlGeneric(moduleParam.data()));

                topGenerics_.insert(name, generic);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::parseTopPorts()
//-----------------------------------------------------------------------------
void VhdlGenerator2::parseTopPorts()
{
	foreach (QSharedPointer<Port> port, *component_->getPorts())
	{
		// do not add ports with invalid direction or phantom direction
		if (port->getDirection() == DirectionTypes::DIRECTION_INVALID ||
			port->getDirection() == DirectionTypes::DIRECTION_PHANTOM)
        {
				continue;
		}

		// create the actual port
		QSharedPointer<VhdlPort> vhdlPort(new VhdlPort(port.data()));

        QString busInterfaceName = getContainingBusInterfaceName(component_, port->name());

		// create the sorter instance
        VhdlPortSorter sorter(busInterfaceName, vhdlPort->name(), port->getDirection());

		// this port can not be created yet
		Q_ASSERT(!topPorts_.contains(sorter));

		topPorts_.insert(sorter, vhdlPort);
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::parseInstances()
//-----------------------------------------------------------------------------
void VhdlGenerator2::parseInstances()
{
	Q_ASSERT(design_);

    ExpressionFormatterFactoryImplementation formatterFactory;

	foreach (QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
		VLNV::IPXactType instanceType = handler_->getDocumentType(*instance->getComponentRef());

		// if vlnv is not found in library
		if (instanceType == VLNV::INVALID)
        {
			emit errorMessage(tr("Component %1 referenced in design %2 was not found in library.").arg(
                instance->getComponentRef()->toString(), component_->getVlnv().toString()));
			continue;
		}
		// if vlnv does not reference a component
		else if (instanceType != VLNV::COMPONENT)
        {
			emit errorMessage(tr("VLNV %1 does not belong to a component.").arg(
				instance->getComponentRef()->toString()));
			continue;
		}

		// pointer to the matching component declaration
		QSharedPointer<VhdlComponentDeclaration> compDeclaration;
		
		// if component declaration is already created
		if (components_.contains(*instance->getComponentRef()))
        {
			compDeclaration = components_.value(*instance->getComponentRef());
		}

		// if component declaration is not yet created then create it
		else
        {
			QSharedPointer<Document> libComp = handler_->getModel(*instance->getComponentRef());
			QSharedPointer<Component> component = libComp.staticCast<Component>();
			Q_ASSERT(component);
			compDeclaration = QSharedPointer<VhdlComponentDeclaration>(new VhdlComponentDeclaration(component,""));

			components_.insert(*instance->getComponentRef(), compDeclaration);
		}
		Q_ASSERT(compDeclaration);

		QString instanceActiveView;
		// if configuration is used then get the active view for the instance
		if (desConf_)
        {
			instanceActiveView = desConf_->getActiveView(instance->getInstanceName());
		}

		// create the instance
		QSharedPointer<VhdlComponentInstance> compInstance(new VhdlComponentInstance(this, handler_,
            compDeclaration.data(), instance->getInstanceName(), instanceActiveView, instance->getDescription()));

        connect(compInstance.data(), SIGNAL(noticeMessage(const QString&)),
            this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
        connect(compInstance.data(), SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString)), Qt::UniqueConnection);

		QSharedPointer<ComponentInstantiation> componentInstantiation;

		foreach (QSharedPointer<ComponentInstantiation> currentInstantiation,
			*compInstance->componentModel()->getComponentInstantiations())
		{
			if (currentInstantiation->name() == instanceActiveView)
			{
				componentInstantiation = currentInstantiation;
				break;
			}
		}

        if (componentInstantiation)
        {
            // add the libraries of the instantiated component to the library list.
            foreach (QString const& fileSetRef, *componentInstantiation->getFileSetReferences())
            {
                QSharedPointer<FileSet> fileSet = compInstance->componentModel()->getFileSet(fileSetRef);

                if (fileSet)
                {
                    libraries_.append( fileSet->getVhdlLibraryNames() );
                }
            }
        }

        ExpressionFormatter* instanceFormatter =
            formatterFactory.makeExpressionFormatter(compInstance->componentModel());

        foreach(QSharedPointer<ConfigurableElementValue> configurableElement, 
            *instance->getConfigurableElementValues())
        {
            QString elementName =
                instanceFormatter->formatReferringExpression(configurableElement->getReferenceId());
            QString elementValue =
                instanceFormatter->formatReferringExpression(configurableElement->getConfigurableValue());

            compInstance->addGenericMap(elementName, elementValue);
		}

		// register the instance to the component declaration
		compDeclaration->addInstantiation(compInstance);

		instances_.insert(instance->getInstanceName(), compInstance);
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::parseInterconnections()
//-----------------------------------------------------------------------------
void VhdlGenerator2::parseInterconnections()
{
	Q_ASSERT(design_);

	QStringList connectionNames;
	foreach (QSharedPointer<Interconnection> interconnection, *design_->getInterconnections())
    {
		bool invalidInterconnection = false;
		QSharedPointer<VhdlComponentInstance> instance1;
		QSharedPointer<BusInterface> interface1;
		
		// if there are several interconnections with same name
		if (connectionNames.contains(interconnection->name()))
        {
			emit errorMessage(tr("Design %1 contained more than one interconnection named %2").arg(
				component_->getVlnv().toString()).arg(interconnection->name()));
		}
		else
        {
			connectionNames.append(interconnection->name());
		}

        QSharedPointer<ActiveInterface> firstInterface = interconnection->getStartInterface();

		// if the instance reference is incorrect
		if (!instances_.contains(firstInterface->getComponentReference()))
        {
			invalidInterconnection = true;
			emit errorMessage(tr("Instance %1 was not found in component %2.").
                arg(firstInterface->getComponentReference()).arg(component_->getVlnv().toString()));
		}
		else
        { 
			instance1 = instances_.value(firstInterface->getComponentReference());
			interface1 = instance1->interface(firstInterface->getBusReference());
		}

		// if the interface is not found
		if (!interface1)
        {
			emit errorMessage(tr("Bus interface %1 was not found in component %2.").
                arg(firstInterface->getBusReference()).arg(instance1->vlnv().toString()));
			invalidInterconnection = true;
		}

        if (!interconnection->getActiveInterfaces()->isEmpty())
        {
            QSharedPointer<ActiveInterface> endInterface = interconnection->getActiveInterfaces()->first();
            parseInstanceInterconnection(
                interconnection, invalidInterconnection, instance1, interface1, endInterface);
        }
        else if (!interconnection->getHierInterfaces()->isEmpty())
        {
            QSharedPointer<HierInterface> endInterface = interconnection->getHierInterfaces()->first();
            parseHierarchicalConnection(instance1, interface1, endInterface);
        }
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::parseInstanceInterconnection()
//-----------------------------------------------------------------------------
void VhdlGenerator2::parseInstanceInterconnection(QSharedPointer<Interconnection> connection,
    bool& invalidInterconnection, QSharedPointer<VhdlComponentInstance> startInstance,
    QSharedPointer<BusInterface> startInterface, QSharedPointer<ActiveInterface> connectionEndInterface)
{
    QSharedPointer<VhdlComponentInstance> endInstance;
    QSharedPointer<BusInterface> endInterface;

    if (!instances_.contains(connectionEndInterface->getComponentReference()))
    {
        invalidInterconnection = true;
        emit errorMessage(tr("Instance %1 was not found in component %2.").
            arg(connectionEndInterface->getComponentReference()).arg(component_->getVlnv().toString()));
    }
    else
    {
        endInstance = instances_.value(connectionEndInterface->getComponentReference());
        endInterface = endInstance->interface(connectionEndInterface->getBusReference());
    }
    // if the interface is not found
    if (!endInterface)
    {
        emit errorMessage(tr("Bus interface %1 was not found in component %2.").
            arg(connectionEndInterface->getBusReference()).arg(endInstance->vlnv().toString()));
        invalidInterconnection = true;
    }

    // if the interconnection is invalid then move on to next interconnection
    if (invalidInterconnection)
    {
        return;
    }
    else
    {
        connectInterfaces(connection->name(), connection->description(), startInstance, startInterface,
            endInstance, endInterface);
    }
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::parseHierarchicalConnection()
//-----------------------------------------------------------------------------
void VhdlGenerator2::parseHierarchicalConnection(QSharedPointer<VhdlComponentInstance> startInstance,
    QSharedPointer<BusInterface> startInterface, QSharedPointer<HierInterface> endInterface)
{
    // search all hierConnections within design
    // find the top-level bus interface
    QSharedPointer<BusInterface> topInterface = component_->getBusInterface(endInterface->getBusReference());

		// if the top level interface couldn't be found
		if (!topInterface)
        {
			emit errorMessage(tr("Interface %1 was not found in top component %2.").
                arg(endInterface->getBusReference()).arg(component_->getVlnv().toString()));
			return;
		}

		// if bus interface couldn't be found
		if (!startInterface)
        {
			emit errorMessage(tr("Interface %1 was not found in instance %2 of type %3").
                arg(startInterface->name()).arg(startInstance->name()).arg(startInstance->typeName()));
			return;
		}

        connectHierInterface(startInstance, startInterface, topInterface);
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::connectInterfaces()
//-----------------------------------------------------------------------------
void VhdlGenerator2::connectInterfaces( const QString& connectionName, const QString& description,
                                        QSharedPointer<VhdlComponentInstance> instance1,
                                        QSharedPointer<BusInterface> interface1, 
                                        QSharedPointer<VhdlComponentInstance> instance2, 
                                        QSharedPointer<BusInterface> interface2 )
{
	Q_ASSERT(instance1);
	Q_ASSERT(interface1);
	Q_ASSERT(instance2);
	Q_ASSERT(interface2);

	// get the IP-XACT models of both instances
	QSharedPointer<Component> component1 = instance1->componentModel();
	QSharedPointer<Component> component2 = instance2->componentModel();

    QSharedPointer<ComponentParameterFinder> firstFinder(new ComponentParameterFinder(component1));
    QSharedPointer<ComponentParameterFinder> secondFinder(new ComponentParameterFinder(component2));

    QSharedPointer<IPXactSystemVerilogParser> firstParser(new IPXactSystemVerilogParser(firstFinder));
    QSharedPointer<IPXactSystemVerilogParser> secondParser(new IPXactSystemVerilogParser(secondFinder));

	// Go through the port maps of both interfaces.
	foreach (QSharedPointer<PortMap> portMap1, *interface1->getPortMaps())
    {
		foreach(QSharedPointer<PortMap> portMap2, *interface2->getPortMaps())
        {
			QSharedPointer<PortMap::PhysicalPort> physPort1 = portMap1->getPhysicalPort();
			QSharedPointer<PortMap::PhysicalPort> physPort2 = portMap2->getPhysicalPort();

			QSharedPointer<Port> port1 = component1->getPort(physPort1->name_);
			QSharedPointer<Port> port2 = component2->getPort(physPort2->name_);

			// if the port maps are not for same logical signal
			if (portMap1->getLogicalPort()->name_ != portMap2->getLogicalPort()->name_)
            {
				continue;
			}
			// if port does not exist in instance 1
			else if (!port1)
            {
				emit errorMessage(tr("Port %1 was not defined in component %2.").
                    arg(physPort1->name_).arg(component1->getVlnv().toString()));
				continue;
			}
			// if port does not exist in instance 2
			else if (!port2)
            {
				emit errorMessage(tr("Port %1 was not defined in component %2.").
                    arg(physPort2->name_).arg(component2->getVlnv().toString()));
				continue;
			}

            General::PortAlignment alignment = calculatePortAlignment(
                portMap1.data(), port1->getLeftBound(), port1->getLeftBound(), firstParser,
                portMap2.data(), port2->getLeftBound(), port2->getRightBound(), secondParser);

			// if the alignment is not valid (port sizes do not match or they do not have any common bits)
			if (alignment.invalidAlignment_)
            {
				continue;
			}

			// create the name for the new signal
			const QString signalName = connectionName + "_" + portMap1->getLogicalPort()->name_;
			
            VhdlGenerator2::PortConnection physPort1Con(instance1, port1->name(), 
                alignment.port1Left_.toInt(), alignment.port1Right_.toInt());
            VhdlGenerator2::PortConnection physPort2Con(instance2, port2->name(),
                alignment.port2Left_.toInt(), alignment.port2Right_.toInt());

            QList<VhdlGenerator2::PortConnection> ports;
			ports.append(physPort1Con);
			ports.append(physPort2Con);

			// connect the port from both port maps
			connectPorts(signalName, description, ports);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::connectPorts()
//-----------------------------------------------------------------------------
void VhdlGenerator2::connectPorts(const QString& connectionName, const QString& description,
    const QList<VhdlGenerator2::PortConnection>& ports)
{
	// at least 2 ports must be found
	Q_ASSERT(ports.size() > 1);

	// the type of the signal
	QString type = ports.first().instance_->portType(ports.first().portName_);

	// the minSize for the signal
	int minSize = ports.first().leftBound_ - ports.first().rightBound_ + 1;
	int maxSize = minSize;

	// first find out the smallest possible minSize for the signal
	foreach (VhdlGenerator2::PortConnection connection, ports)
    {
		// the smallest minSize needed is used
		minSize = qMin(minSize, (connection.leftBound_ - connection.rightBound_ + 1));
	}

	// calculate the bounds for the signal
	//int left = minSize - 1;
	int left = 0;
	int right = 0;

	// create the endpoints
	QList<VhdlConnectionEndPoint> endpoints;
	foreach (VhdlGenerator2::PortConnection connection, ports)
    {
		QString connectionType = connection.instance_->portType(connection.portName_);

		// make sure all ports are for compatible type
		if (!VhdlGeneral::checkVhdlTypeMatch(type, connectionType))
        {
			QString instance1(QString("instance %1 port %2 type %3").
                arg(ports.first().instance_->name()).arg(ports.first().portName_).arg(type));
			QString instance2(QString("instance %1 port %2 type %3").
                arg(connection.instance_->name()).arg(connection.portName_).arg(connectionType));
			emit errorMessage(tr("Type mismatch: %1 - %2.").arg(instance1).arg(instance2));
		}

        VhdlConnectionEndPoint endpoint(connection.instance_->name(), connection.portName_,
            QString::number(connection.leftBound_), QString::number(connection.rightBound_),
            QString::number(maxSize-1), QString::number(right));
        endpoints.append(endpoint);
	}

	// if vectored but minSize is only one then convert to scalar signal type
	if (type == "std_logic_vector" && minSize == 1)
    {
		type = "std_logic";
	}

	// if scalar signal type but minSize is larger than 1 then convert to vectored
	else if (type == "std_logic" && minSize > 1)
    {
		type == "std_logic_vector";
	}

	// the signal that connects the end points
	QSharedPointer<VhdlSignal> signal;

	// find a signal to use for connecting the end points
	foreach (VhdlConnectionEndPoint endpoint, endpoints)
    {
		if (signals_.contains(endpoint))
        {
			signal = signals_.value(endpoint);
            signal->setBounds(qMax(left, signal->left()), qMin(right, signal->right()));
			break;
		}
	}
	// if signal was not found then create a new one 
	if (!signal)
    {
		signal = QSharedPointer<VhdlSignal>(new VhdlSignal(connectionName, type, left, right, description));
		signal->setBounds(left, right);
	}

	// connect each end point to given signal
	foreach (VhdlConnectionEndPoint endpoint, endpoints)
    {
		connectEndPoint(endpoint, signal);
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::connectEndPoint()
//-----------------------------------------------------------------------------
void VhdlGenerator2::connectEndPoint( const VhdlConnectionEndPoint& endpoint,
									  const QSharedPointer<VhdlSignal> signal )
{
	Q_ASSERT(signal);

	// if the end point already has a signal associated with it
	if (signals_.contains(endpoint))
    {
		QSharedPointer<VhdlSignal> oldSignal = signals_.value(endpoint);
		// get all end points associated with signal for endpoint 2
		QList<VhdlConnectionEndPoint> endpoints = signals_.keys(oldSignal);
		// replace each end point with association to signal for endpoint
		foreach (VhdlConnectionEndPoint temp, endpoints)
        {
			signals_.insert(temp, signal);
		}
	}
	// if end point was not yet specified
	else
    {
		signals_.insert(endpoint, signal);
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::parseAdHocConnections()
//-----------------------------------------------------------------------------
void VhdlGenerator2::parseAdHocConnections()
{
	// all adHoc connections are processed	
	foreach (QSharedPointer<AdHocConnection> adHoc, *design_->getAdHocConnections())
    {
		// the data structure to store the ports to connect
		QList<VhdlGenerator2::PortConnection> ports;

		foreach (QSharedPointer<PortReference> portRef, *adHoc->getInternalPortReferences())
        {
			// if the instance is not found
			if (!instances_.contains(portRef->getComponentRef()))
            {
				emit errorMessage(tr("Instance %1 was not found in design.").arg(portRef->getComponentRef()));
				continue;
			}

			QSharedPointer<VhdlComponentInstance> instance = instances_.value(portRef->getComponentRef());
            Q_ASSERT(instance);

            QSharedPointer<ParameterFinder> instanceFinder(
                new ComponentParameterFinder(instance->componentModel()));
            QSharedPointer<ExpressionParser> instanceParser (new IPXactSystemVerilogParser(instanceFinder));

			// if the specified port is not found
			if (!instance->hasPort(portRef->getPortRef()))
            {
				emit errorMessage(tr("Port %1 was not found in instance %2 of type %3").arg(
                    portRef->getPortRef(),
					portRef->getComponentRef(),
					instance->vlnv().toString()));
				continue;
			}

            int left = 0;
            int right = 0;

            QSharedPointer<PartSelect> part = portRef->getPartSelect();
            if (part)
            {
                left = instanceParser->parseExpression(part->getLeftRange()).toInt();
                right = instanceParser->parseExpression(part->getRightRange()).toInt();
            }
            else
            {
                left = instanceParser->parseExpression(
                    instance->getPortPhysLeftBound(portRef->getPortRef())).toInt();
                right = instanceParser->parseExpression(
                    instance->getPortPhysRightBound(portRef->getPortRef())).toInt();
            }

			// create the port specification
			VhdlGenerator2::PortConnection port(instance, portRef->getPortRef(), left, right);

			// add port to the list
			ports.append(port);
		}

		// if the connection contains at least one port of the top component
		if (!adHoc->getExternalPortReferences()->isEmpty())
        {
			// connect each external port to instance port
			foreach (QSharedPointer<PortReference> portRef, *adHoc->getExternalPortReferences())
            {
				QSharedPointer<Port> port = component_->getPort(portRef->getPortRef());

				if ( !port )
				{
					continue;
				}

                QString busInterfaceName = getContainingBusInterfaceName(component_, port->name());

				// check that the port is found 
                VhdlPortSorter sorter(busInterfaceName, portRef->getPortRef(), port->getDirection());

				if (!topPorts_.contains(sorter))
                {
					emit errorMessage(tr("The port %1 is not found in top component %2").arg(
						portRef->getPortRef()).arg(component_->getVlnv().toString()));
					continue;
				}

				QSharedPointer<VhdlPort> vport = topPorts_.value(sorter);
				Q_ASSERT(vport);

                QString left = 0;
                QString right = 0;

                QSharedPointer<PartSelect> part = portRef->getPartSelect();
                if (part)
                {
                    left = part->getLeftRange();
                    right = part->getRightRange();
                }
                else
                {
                    left = vport->left();
                    right = vport->right();
                }

				// connect each instance port to the top port
				connectHierPort(portRef->getPortRef(), left, right, ports);
			}
		}
		// otherwise the connection is just between the ports of the instances
		else 
        {
			// connect the ports 
			connectPorts(adHoc->name(), adHoc->description(), ports);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::connectHierPort()
//-----------------------------------------------------------------------------
void VhdlGenerator2::connectHierPort( const QString& topPortName, QString leftBound, QString rightBound,
    const QList<VhdlGenerator2::PortConnection>& ports )
{
	foreach (VhdlGenerator2::PortConnection port, ports)
	{	
		// if port is scalar then don't add the bit boundaries
        int portLeft = port.leftBound_;
        int portRight = port.rightBound_;
		if (port.instance_->isScalarPort(port.portName_))
        {
			portLeft = -1;
			portRight = -1;
		}

		QSharedPointer<Port> topPort = component_->getPort(topPortName);

        QString busInterfaceName = getContainingBusInterfaceName(component_, topPortName);

        VhdlPortSorter sorter(busInterfaceName, topPortName, topPort->getDirection());

		// if the port was found in top component then set it as uncommented because it is needed
		if (topPorts_.contains(sorter))
        {
			QSharedPointer<VhdlPort> vhdlPort = topPorts_.value(sorter);
			vhdlPort->setCommented(false);

			// if the top port is scalar then don't use the bit boundaries
			if (VhdlGeneral::isScalarType(vhdlPort->type()))
            {
                leftBound = "-1";
                rightBound = "-1";
			}
		}
		else
        {
			emit errorMessage(tr("Port %1 was not found in top component %2.").
                arg(topPortName).arg(component_->getVlnv().toString()));
			continue;
		}

		// tell each instance to create a port map between the ports
		port.instance_->addPortMap(port.portName_, QString::number(portLeft), QString::number(portRight),
            port.instance_->portType(port.portName_), topPortName, leftBound, rightBound,
            topPorts_.value(sorter)->type());
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::connectHierInterface()
//-----------------------------------------------------------------------------
void VhdlGenerator2::connectHierInterface( QSharedPointer<VhdlComponentInstance> instance,
    QSharedPointer<BusInterface> instanceInterface, QSharedPointer<BusInterface> topInterface )
{
	Q_ASSERT(instance);
	Q_ASSERT(instanceInterface);
	Q_ASSERT(topInterface);

	// get the IP-XACT model of the instance
	QSharedPointer<Component> instanceComponent = instance->componentModel();

    QSharedPointer<ComponentParameterFinder> instanceFinder (new ComponentParameterFinder(instanceComponent));
    QSharedPointer<ExpressionParser> instanceParser (new IPXactSystemVerilogParser(instanceFinder));

	foreach (QSharedPointer<PortMap> instancePortMap, *instanceInterface->getPortMaps())
    {
		foreach(QSharedPointer<PortMap> hierPortMap, *topInterface->getPortMaps())
        {
			const QString instancePortName = instancePortMap->getPhysicalPort()->name_;
			const QString hierPortName = hierPortMap->getPhysicalPort()->name_;
            QSharedPointer<Port> instancePort = instanceComponent->getPort(instancePortName);
			QSharedPointer<Port> hierPort = component_->getPort(hierPortName);

			// if the port maps are not for same logical signal
            if (instancePortMap->getLogicalPort()->name_ != hierPortMap->getLogicalPort()->name_)
            {
				continue;
			}
			// if port does not exist in instance 1
			else if (!instanceComponent->hasPort(instancePortName))
            {
				emit errorMessage(tr("Port %1 was not defined in component %2.").
                    arg(instancePortName).arg(instanceComponent->getVlnv().toString()));
				continue;
			}
			// if port does not exist in instance 2
			else if (!component_->hasPort(hierPortName))
            {
				emit errorMessage(tr("Port %1 was not defined in component %2.").
                    arg(hierPortName).arg(component_->getVlnv().toString()));
				continue;
			}

			// get the alignments of the ports
            General::PortAlignment alignment = calculatePortAlignment(
                instancePortMap.data(), instancePort->getLeftBound(), instancePort->getRightBound(), instanceParser,
                hierPortMap.data(), hierPort->getLeftBound(), hierPort->getRightBound(), topComponentParser_);

			// if the alignment is not valid (port sizes do not match or they do not have any common bits)
			if (alignment.invalidAlignment_)
            {
				continue;
			}

			VhdlGenerator2::PortConnection port1(
                instance, instancePortName, alignment.port1Left_.toInt(), alignment.port1Right_.toInt());
			QList<VhdlGenerator2::PortConnection> ports;
			ports.append(port1);

			// connect the port from instance to the top port
			connectHierPort(hierPortName, alignment.port2Left_, alignment.port2Right_, ports);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::mapPorts2Signals()
//-----------------------------------------------------------------------------
void VhdlGenerator2::mapPorts2Signals()
{
	// search each endpoint
	for (QMap<VhdlConnectionEndPoint, QSharedPointer<VhdlSignal> >::iterator i = signals_.begin(); 
		i != signals_.end(); ++i)
    {
		// if the component instance can't be found
		if (!instances_.contains(i.key().instanceName()))
        {
			emit errorMessage(tr("Instance %1 was not found in design.").arg(i.key().instanceName()));
			continue;
		}

		// find the instance for the end point
		QSharedPointer<VhdlComponentInstance> instance = instances_.value(i.key().instanceName());

		// if port is scalar then don't add the bit boundaries
		QString portLeft = i.key().portLeft();
		QString portRight = i.key().portRight();
		if (instance->isScalarPort(i.key().portName()))
        {
			portLeft = "-1";
			portRight = "-1";
		}

		// if signal is scalar then don't add the bit boundaries
		QString signalLeft = i.key().signalLeft();
		QString signalRight = i.key().signalRight();
		if (VhdlGeneral::isScalarType(i.value()->type()))
        {
			signalLeft = "-1";
			signalRight = "-1";
		}

		// add a port map for the instance to connect port to signal
		instance->addPortMap(i.key().portName(), portLeft, portRight, instance->portType(i.key().portName()),
            i.value()->name(), signalLeft, signalRight, i.value()->type());	
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::writeVhdlHeader()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeVhdlHeader( QTextStream& vhdlStream, const QString& fileName )
{
	vhdlStream << "-- ***************************************************" << endl;
	vhdlStream << "-- File         : " << fileName << endl;
	vhdlStream << "-- Creation date: " << QDate::currentDate().toString(QString("dd.MM.yyyy")) << endl;
	vhdlStream << "-- Creation time: " << QTime::currentTime().toString(QString("hh:mm:ss")) << endl;
	vhdlStream << "-- Description  : " << endl;
	VhdlGeneral::writeDescription(component_->getDescription(), vhdlStream, QString(""));
	vhdlStream << "-- " << endl;

	QSettings settings;
	QString userName = settings.value("General/Username", Utils::getCurrentUser()).toString();
	vhdlStream << "-- Created by   : " << userName << endl; 
	vhdlStream << "-- This file was generated with Kactus2 vhdl generator" << endl;
	VLNV vlnv = component_->getVlnv();
	vhdlStream << "-- based on IP-XACT component " << vlnv.toString() << endl;
	vhdlStream << "-- whose XML file is " << handler_->getPath(vlnv) << endl;
	vhdlStream << "-- ***************************************************" << endl;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::writeGenerics()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeGenerics( QTextStream& vhdlStream )
{
	// if generics exist
	if (!topGenerics_.isEmpty())
    {
		// the start tag
		vhdlStream << "  " << "generic (" << endl;

		for (QMap<QString, QSharedPointer<VhdlGeneric> >::iterator i = topGenerics_.begin();
			i != topGenerics_.end(); ++i)
        {
			vhdlStream << "  " << "  ";
			i.value()->write(vhdlStream);

			// if this is not the last generic to write
			if (i + 1 != topGenerics_.end())
            {
				vhdlStream << ";";
			}

			if (!i.value()->description().isEmpty())
            {
				vhdlStream << " ";
				VhdlGeneral::writeDescription(i.value()->description(), vhdlStream);
			}
			else
            {
				vhdlStream << endl;
			}
		}
		vhdlStream << "  " << ");" << endl;
		vhdlStream << endl;
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::writePorts()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writePorts( QTextStream& vhdlStream )
{
	// if ports exist
	if (!topPorts_.isEmpty() && VhdlPort::hasRealPorts(topPorts_))
    {
		vhdlStream << "  " << "port (" << endl;

		QString previousInterface;
		// print each port
		for (QMap<VhdlPortSorter, QSharedPointer<VhdlPort> >::iterator i = topPorts_.begin();
			i != topPorts_.end(); ++i)
        {
            // if the port is first in the interface then introduce it
            if (i.key().interface() != previousInterface)
            {
                const QString interfaceName = i.key().interface();

                vhdlStream << endl << "  " << "  " << "-- ";

                if (interfaceName == QString("none"))
                {
                    vhdlStream << "These ports are not in any interface" << endl;
                }
                else if (interfaceName == QString("several"))
                {
                    vhdlStream << "There ports are contained in many interfaces" << endl;
                }
                else
                {
                    vhdlStream << "Interface: " << interfaceName << endl;
                    const QString description = component_->getBusInterface(interfaceName)->description();
                    if (!description.isEmpty())
                    {
                        VhdlGeneral::writeDescription(description, vhdlStream, QString("    "));
                    }
                }
                previousInterface = interfaceName;
            }

            // write the port name and direction
            vhdlStream << "  " << "  ";
            i.value()->write(vhdlStream);

            // if this is not the last port to write
            if (i + 1 != topPorts_.end())
            {
                vhdlStream << ";";
            }

            if (!i.value()->description().isEmpty())
            {
                vhdlStream << " ";
                VhdlGeneral::writeDescription(i.value()->description(), vhdlStream);
            }
            else
            {
                vhdlStream << endl;
            }
		}
		vhdlStream << "  " << ");" << endl;
		// write extra empty line to make code readable
		vhdlStream << endl;
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::writeSignalDeclarations()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeSignalDeclarations( QTextStream& vhdlStream )
{
	QList<QSharedPointer<VhdlSignal> > signalList;

	// get the unique signals that exist
	foreach (QSharedPointer<VhdlSignal> signal, signals_.values())
    {
		if (!signalList.contains(signal))
        {
			signalList.append(signal);
		}
	}
	// when the list contains only the unique signals then write them
	foreach (QSharedPointer<VhdlSignal> signal, signalList)
    {
		signal->write(vhdlStream);
	}
	vhdlStream << endl;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::writeComponentDeclarations()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeComponentDeclarations( QTextStream& vhdlStream )
{
	foreach (QSharedPointer<VhdlComponentDeclaration> comp, components_)
    {
		comp->write(vhdlStream);
		vhdlStream << endl;
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::writeComponentInstances()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeComponentInstances( QTextStream& vhdlStream )
{
	foreach (QSharedPointer<VhdlComponentInstance> instance, instances_)
    {
		instance->write(vhdlStream);
		vhdlStream << endl;
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::readUserModifiablePart()
//-----------------------------------------------------------------------------
void VhdlGenerator2::readUserModifiablePart( QFile& previousFile )
{
	Q_ASSERT(previousFile.exists());

	// clear the previous stuff
	userModifiedAssignments_.clear();
	userModifiedDeclarations_.clear();

	// open the file
	// if file could not be opened
	if (!previousFile.open(QFile::ReadOnly)) 
    {
		emit errorMessage(tr("File: %1 couldn't be opened for reading").arg(previousFile.fileName()));
		return;
	}

	QTextStream stream(&previousFile);

	bool readingDeclarations = false;
	bool readingAssignments = false;

	// read as long as theres stuff in the stream to read.
	while (!stream.atEnd())
    {
		QString line = stream.readLine();

		QString lineCompare = line.trimmed();

		// if next line starts the declarations
		if (lineCompare == BLACK_BOX_DECL_START)
        {
			readingDeclarations = true;
		}
		// if previous line was the last one of the declarations
		else if (lineCompare == BLACK_BOX_DECL_END)
        {
			readingDeclarations = false;
		}
		// if the next line starts the assignments
		else if (lineCompare == BLACK_BOX_ASSIGN_START)
        {
			readingAssignments = true;
		}
		// if previous line was the last of the assignments
		else if (lineCompare == BLACK_BOX_ASSIGN_END)
        {
			readingAssignments = false;
		}
		// if the line is part of the declarations
		else if (readingDeclarations)
        {
			// add the line to the declarations
			userModifiedDeclarations_ += line;
			userModifiedDeclarations_ += QString("\n");
		}
		// if the line is part of the assignments
		else if (readingAssignments)
        {
			// add the line to the assignments
			userModifiedAssignments_ += line;
			userModifiedAssignments_ += QString("\n");
		}
	}

	previousFile.close();
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::writeUserModifiedDeclarations()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeUserModifiedDeclarations( QTextStream& stream )
{
	stream << "  " <<"-- You can write vhdl code after this tag and it is saved through the generator." << endl;
	stream << "  " << BLACK_BOX_DECL_START << endl;
	stream << userModifiedDeclarations_;
	stream << "  " << BLACK_BOX_DECL_END << endl;
	stream << "  " << "-- Do not write your code after this tag." << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::writeUserModifiedAssignments()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeUserModifiedAssignments( QTextStream& stream )
{
	stream << "  " << "-- You can write vhdl code after this tag and it is saved through the generator." << endl;
	stream << "  " << BLACK_BOX_ASSIGN_START << endl;
	stream << userModifiedAssignments_;
	stream << "  " << BLACK_BOX_ASSIGN_END << endl;
	stream << "  " << "-- Do not write your code after this tag." << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::calculatePortAlignment()
//-----------------------------------------------------------------------------
General::PortAlignment VhdlGenerator2::calculatePortAlignment(const PortMap* portMap1, QString phys1LeftBound,
    QString phys1RightBound, QSharedPointer<ExpressionParser> firstParser, const PortMap* portMap2,
    QString phys2LeftBound, QString phys2RightBound, QSharedPointer<ExpressionParser> secondParser)
{
    General::PortAlignment alignment;

    if (portMap1->getLogicalPort() && portMap2->getLogicalPort() &&
        portMap1->getLogicalPort()->name_ == portMap2->getLogicalPort()->name_)
    {
        QSharedPointer<PortAlignment> firstPhysical =
            getPhysicalAlignment(portMap1, phys1LeftBound, phys1RightBound, firstParser);
        QSharedPointer<PortAlignment> secondPhysical =
            getPhysicalAlignment(portMap2, phys2LeftBound, phys2RightBound, secondParser);

        int firstLogicalMax = getLogicalValue(portMap1, firstParser, true);
        int firstLogicalMin = getLogicalValue(portMap1, firstParser, false);
        int secondLogicalMax = getLogicalValue(portMap2, secondParser, true);
        int secondLogicalMin = getLogicalValue(portMap2, secondParser, false);

        if (portMap1->getLogicalPort()->range_ && portMap2->getLogicalPort()->range_)
        {
            if (firstLogicalMin <= secondLogicalMax && firstLogicalMax >= secondLogicalMin)
            {
                int logicalLeft = qMin(firstLogicalMax, secondLogicalMax);
                int logicalRight = qMax(firstLogicalMin, secondLogicalMin);

                int firstDownSize = abs(firstLogicalMax - logicalLeft);
                int firstUpSize = abs(logicalRight - firstLogicalMin);

                alignment.port1Left_ = QString::number(firstPhysical->getLeftAlignment() - firstDownSize);
                alignment.port1Right_ = QString::number(firstPhysical->getRightAlignment() + firstUpSize);

                int secondDownSize = abs(secondLogicalMax - logicalLeft);
                int secondUpSize = abs(logicalRight - secondLogicalMin);

                alignment.port2Left_ = QString::number(secondPhysical->getLeftAlignment() - secondDownSize);
                alignment.port2Right_ = QString::number(secondPhysical->getRightAlignment() + secondUpSize);
            }
        }
        else if (portMap1->getLogicalPort() && !portMap2->getLogicalPort())
        {
            alignment.port1Left_ = QString::number(firstPhysical->getLeftAlignment());
            alignment.port1Right_ = QString::number(firstPhysical->getRightAlignment());

            alignment.port2Left_ = QString::number(firstLogicalMax);
            alignment.port2Right_ = QString::number(firstLogicalMin);
        }
        else if (!portMap1->getLogicalPort() && portMap2->getLogicalPort())
        {
            alignment.port1Left_ = QString::number(secondLogicalMax);
            alignment.port1Right_ = QString::number(secondLogicalMin);

            alignment.port2Left_ = QString::number(secondPhysical->getLeftAlignment());
            alignment.port2Right_ = QString::number(secondPhysical->getRightAlignment());
        }
        else
        {
            alignment.port1Left_ = QString::number(firstPhysical->getLeftAlignment());
            alignment.port1Right_ = QString::number(firstPhysical->getRightAlignment());
            alignment.port2Left_ = QString::number(secondPhysical->getLeftAlignment());
            alignment.port2Right_ = QString::number(secondPhysical->getRightAlignment());
        }

        int firstPortSize = alignment.port1Left_.toInt() - alignment.port1Right_.toInt() + 1;
        int secondPortSize = alignment.port2Left_.toInt() - alignment.port2Right_.toInt() + 1;

        if (firstPortSize != secondPortSize)
        {
            alignment.invalidAlignment_ = true;
        }
        else
        {
            alignment.invalidAlignment_ = false;
        }
    }

    return alignment;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::getPhysicalAlignment()
//-----------------------------------------------------------------------------
QSharedPointer<PortAlignment> VhdlGenerator2::getPhysicalAlignment(const PortMap* portmap,
    QString const& portLeftBound, QString const& portRightBound, QSharedPointer<ExpressionParser> parser) const
{
    int leftBoundInt = parser->parseExpression(portLeftBound).toInt();
    int rightBoundInt = parser->parseExpression(portRightBound).toInt();

    if (portmap->getPhysicalPort() && portmap->getPhysicalPort()->partSelect_)
    {
        leftBoundInt = parser->parseExpression(portmap->getPhysicalPort()->partSelect_->getLeftRange()).toInt();
        rightBoundInt = parser->parseExpression(portmap->getPhysicalPort()->partSelect_->getRightRange()).toInt();
    }
    
    int alignmentMax = qMax(leftBoundInt, rightBoundInt);
    int alignmentMin = qMin(leftBoundInt, rightBoundInt);
    QSharedPointer<PortAlignment> alignment (new PortAlignment(alignmentMax, alignmentMin));
    return alignment;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::getLogicalValue()
//-----------------------------------------------------------------------------
int VhdlGenerator2::getLogicalValue(const PortMap* portMap, QSharedPointer<ExpressionParser> parser,
    bool isMaximum)
{
    QSharedPointer<PortMap::LogicalPort> logicalPort = portMap->getLogicalPort();

    int logicalValue = 0;

    if (logicalPort->range_)
    {
        int logicalLeft = parser->parseExpression(logicalPort->range_->getLeft()).toInt();
        int logicalRight = parser->parseExpression(logicalPort->range_->getRight()).toInt();

        if (isMaximum)
        {
            logicalValue = qMax(logicalLeft, logicalRight);
        }
        else
        {
            logicalValue = qMin(logicalLeft, logicalRight);
        }
    }

    return logicalValue;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::getContainingBusInterfaceName()
//-----------------------------------------------------------------------------
QString VhdlGenerator2::getContainingBusInterfaceName(QSharedPointer<Component> containingComponent,
    QString const& portName) const
{
    QString busInterfaceName = "none";

    QSharedPointer<BusInterface> containingBus = containingComponent->getInterfaceForPort(portName);
    if (!containingBus.isNull())
    {
        busInterfaceName = containingBus->name();
    }

    return busInterfaceName;
}

//----------------------------------------------------------------------------
// Function: vhdlgenerator2::PortConnection::PortConnection()
//-----------------------------------------------------------------------------
VhdlGenerator2::PortConnection::PortConnection(QSharedPointer<VhdlComponentInstance> instance,
    const QString& portName, int left, int right):
instance_(instance),
portName_(portName),
leftBound_(left),
rightBound_(right)
{

}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::PortConnection::PortConnection()
//-----------------------------------------------------------------------------
VhdlGenerator2::PortConnection::PortConnection( const VhdlGenerator2::PortConnection& other ):
instance_(other.instance_),
portName_(other.portName_),
leftBound_(other.leftBound_),
rightBound_(other.rightBound_)
{

}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::PortConnection::operator=()
//-----------------------------------------------------------------------------
VhdlGenerator2::PortConnection& VhdlGenerator2::PortConnection::operator=(
    const VhdlGenerator2::PortConnection& other )
{
    if (this != &other)
    {
        instance_ = other.instance_;
        portName_ = other.portName_;
        leftBound_ = other.leftBound_;
        rightBound_ = other.rightBound_;
    }
    return *this;
}
