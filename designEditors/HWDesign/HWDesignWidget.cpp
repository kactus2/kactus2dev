/* 
 * 		filename: designwidget.cpp
 */

#include "HWDesignWidget.h"

#include "HWDesignDiagram.h"
#include "HWComponentItem.h"
#include "HWConnection.h"
#include "BusPortItem.h"
#include "BusInterfaceItem.h"

#include "HWDeleteCommands.h"

#include <designEditors/common/DiagramUtil.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/GenericEditProvider.h>

#include <library/LibraryManager/vlnv.h>
#include <library/LibraryManager/libraryinterface.h>

#include "columnview/ColumnEditDialog.h"
#include "columnview/HWColumn.h"

#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/view.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <kactusGenerators/vhdlGenerator/vhdlgenerator2.h>
#include <kactusGenerators/quartusGenerator/quartusgenerator.h>
#include <kactusGenerators/modelsimGenerator/modelsimgenerator.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsView>
#include <QDockWidget>
#include <QTabWidget>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QKeyEvent>
#include <QScrollBar>
#include <QString>
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>

#include <QDebug>

HWDesignWidget::HWDesignWidget(LibraryInterface *lh, QWidget* parent)
    : DesignWidget(lh, parent)
{
	// update the supported windows 
	supportedWindows_ = (supportedWindows_ | CONFIGURATIONWINDOW |
		CONNECTIONWINDOW | INTERFACEWINDOW |INSTANCEWINDOW | ADHOC_WINDOW | ADDRESS_WINDOW);

    setDiagram(new HWDesignDiagram(lh, *getGenericEditProvider(), this));
    getDiagram()->setProtection(false);
    getDiagram()->setMode(MODE_SELECT);
    
	addVisibilityControl("Bus Widths", false);
}

HWDesignWidget::~HWDesignWidget()
{
}

bool HWDesignWidget::setDesign(VLNV const& vlnv, QString const& viewName)
{	
	disconnect(getDiagram(), SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()));

	// if vlnv and view name was defined which means that an existing component is opened
	if (vlnv.isValid() && !viewName.isEmpty()) {

		// if vlnv is writeSucceeded and the type is component
		if (vlnv.isValid() && vlnv.getType() == VLNV::COMPONENT) {

			// create model 
			QSharedPointer<LibraryComponent> libComponent = getLibraryInterface()->getModel(vlnv);
            QSharedPointer<Component> comp = libComponent.staticCast<Component>();

			if (comp == 0)
            {
				return false;
            }

			if (!setDesign(comp, viewName))
            {
                return false;
            }
		}
	}

	// if vlnv was writeSucceeded but view is empty then should create a new design for the component
	else if (vlnv.isValid() && viewName.isEmpty()) {
		Q_ASSERT(getLibraryInterface()->contains(vlnv));
		Q_ASSERT(getLibraryInterface()->getDocumentType(vlnv) == VLNV::COMPONENT);

		QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(vlnv);
		QSharedPointer<Component> comp = libComp.staticCast<Component>();

		// get the directory path where the component's xml file is located
		const QString xmlPath = getLibraryInterface()->getPath(vlnv);
		QFileInfo xmlInfo(xmlPath);
		const QString dirPath = xmlInfo.absolutePath();

		createDesignForComponent(comp, dirPath);
	}

	// if vlnv was not defined (a new hierarchical component is created)
	else {
		if (!createEmptyDesign(vlnv))
        {
            return false;
        }
	}

	// disable the save at startup
	connect(getDiagram(), SIGNAL(contentChanged()),
		    this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(getDiagram(), SIGNAL(modeChanged(DrawMode)),
		    this, SIGNAL(modeChanged(DrawMode)), Qt::UniqueConnection);
	setModified(false);
	
	setDocumentType("HW Design");
	setDocumentName(QString("%1 (%2)").arg(getIdentifyingVLNV().getName()).arg(getIdentifyingVLNV().getVersion()));

	emit clearItemSelection();

    return true;
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::setDesign()
//-----------------------------------------------------------------------------
bool HWDesignWidget::setDesign(QSharedPointer<Component> comp, const QString& viewName)
{
    View* view = comp->findView(viewName);

    if (!view || !view->isHierarchical()) {
        return false;
    }

    VLNV designVLNV = comp->getHierRef(viewName);

    // Check for a valid VLNV type.
    designVLNV.setType(getLibraryInterface()->getDocumentType(designVLNV));

    if (!designVLNV.isValid())
    {
        emit errorMessage(tr("Component %1 did not contain a hierarchical view").arg(comp->getVlnv()->getName()));
        return false;
    }

    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> designConf;

    // if the component contains a direct reference to a design
    if (designVLNV.getType() == VLNV::DESIGN)
    {
        QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(designVLNV);	
        design = libComp.staticCast<Design>();
    }
    // if component had reference to a design configuration
    else if (designVLNV.getType() == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(designVLNV);
        designConf = libComp.staticCast<DesignConfiguration>();

        designVLNV = designConf->getDesignRef();

        if (designVLNV.isValid())
        {
            QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(designVLNV);	
            design = libComp.staticCast<Design>();
        }

        // if design configuration did not contain a reference to a design.
        if (!design)
        {
            emit errorMessage(tr("Component %1 did not contain a hierarchical view").arg(
                comp->getVlnv()->getName()));
            return false;;
        }
    }

    if (!getDiagram()->setDesign(comp, design, designConf))
    {
        return false;
    }
	setDocumentName(QString("%1 (%2)").arg(getIdentifyingVLNV().getName()).arg(getIdentifyingVLNV().getVersion()));

    return DesignWidget::setDesign(comp, viewName);
}

bool HWDesignWidget::saveAs() {

	VLNV oldVLNV = *getEditedComponent()->getVlnv();

    // Ask the user for a new VLNV along with attributes and directory.
    KactusAttribute::ProductHierarchy prodHier = getEditedComponent()->getComponentHierarchy();
    KactusAttribute::Firmness firmness = getEditedComponent()->getComponentFirmness();

    VLNV vlnv;
    QString directory;

    if (!NewObjectDialog::saveAsDialog(this, getLibraryInterface(), oldVLNV, prodHier, firmness, vlnv, directory))
    {
        return false;
    }

	// create the vlnv for design and design configuration
	VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());
	VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());

	QSharedPointer<Design> design;

	// create the design

	QSharedPointer<Component> oldComponent = getEditedComponent();

	// make a copy of the hierarchical component
	setEditedComponent(QSharedPointer<Component>(new Component(*getEditedComponent())));

	// set the new vlnv for the component
	getEditedComponent()->setVlnv(vlnv);

	QSharedPointer<DesignConfiguration> designConf = getDiagram()->getDesignConfiguration();

	// if design configuration is used
	if (designConf) {

		// make a copy of the design configuration
		designConf = QSharedPointer<DesignConfiguration>(new DesignConfiguration(*designConf));

		// set design configuration's vlnv to match new vlnv
		designConf->setVlnv(desConfVLNV);

		// set design configuration to reference to new design vlnv
		designConf->setDesignRef(designVLNV);

		// set component to reference new design configuration
		getEditedComponent()->setHierRef(desConfVLNV, getOpenViewName());

		// create design with new design vlnv
		design = getDiagram()->createDesign(designVLNV);
	}
	// if component does not use design configuration then it references directly to design
	else {
		// set component to reference new design
		getEditedComponent()->setHierRef(designVLNV, getOpenViewName());

		design = getDiagram()->createDesign(designVLNV);
	}

	if (design == 0)
	{
		return false;
	}

	// update the hierarchical bus interfaces of the top-component
	getDiagram()->updateHierComponent();

	// get the paths to the original xml file
	QFileInfo sourceInfo(getLibraryInterface()->getPath(*oldComponent->getVlnv()));
	QString sourcePath = sourceInfo.absolutePath();

	// update the file paths and copy necessary files
	getEditedComponent()->updateFiles(*oldComponent, sourcePath, directory);

	// create the files for the documents

	bool writeSucceeded = true;

	getLibraryInterface()->beginSave();

	// if design configuration is used then write it.
	if (designConf) {
		if (!getLibraryInterface()->writeModelToFile(directory, designConf, false)) {
			writeSucceeded = false;
		}
	}

	if (!getLibraryInterface()->writeModelToFile(directory, design, false)) {
		writeSucceeded = false;
	}
	if (!getLibraryInterface()->writeModelToFile(directory, getEditedComponent(), false)) {
		writeSucceeded = false;
	}

	getLibraryInterface()->endSave();

	if (writeSucceeded)
    {
		setDocumentName(getEditedComponent()->getVlnv()->getName() + " (" + 
			getEditedComponent()->getVlnv()->getVersion() + ")");
		return TabDocument::saveAs();
	}
	else
    {
		emit errorMessage(tr("Error saving design to disk."));
		return false;
	}
}

void HWDesignWidget::keyPressEvent(QKeyEvent *event)
{
    // Handle delete events if the document is not protected.
    if (!isProtected() && event->key() == Qt::Key_Delete)
    {
        if (getDiagram()->selectedItems().empty())
        {
            return;
        }

        QList<QGraphicsItem*> selectedItems = getDiagram()->selectedItems();
        int type = getDiagram()->getCommonItemType(selectedItems);

        if (type == HWComponentItem::Type)
        {
            getDiagram()->clearSelection();

            QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

            foreach (QGraphicsItem* selected, selectedItems)
            {
			    HWComponentItem* component = static_cast<HWComponentItem*>(selected);
			    getDiagram()->removeInstanceName(component->name());
                getDiagram()->clearSelection();
            
                ComponentDeleteCommand* childCmd = new ComponentDeleteCommand(component, cmd.data());

			    connect(childCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
				    this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
			    connect(childCmd, SIGNAL(componentInstantiated(ComponentItem*)),
				    this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);

                childCmd->redo();
            }

            getGenericEditProvider()->addCommand(cmd, false);
        }
        else if (type == BusInterfaceItem::Type)
        {
            // Enumerate all ports that are part of the selected bus interfaces.
            QList< QSharedPointer<Port> > ports;

            foreach (QGraphicsItem* selected, selectedItems)
            {
                BusInterfaceItem* diagIf = static_cast<BusInterfaceItem*>(selected);

                foreach(QSharedPointer<Port> port, diagIf->getPorts())
                {
                    if (!ports.contains(port))
                    {
                        ports.append(port);
                    }
                }
            }

            // Ask confirmation for port deletion from the user if there were ports in
            // any of the bus interfaces.
            bool removePorts = false;

            if (!ports.isEmpty())
            {
                QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                                   tr("Do you want to delete also the ports that are part of the interfaces?"),
                                   QMessageBox::Yes | QMessageBox::No, this);

                QStringList textList("Interface ports:");

                foreach(QSharedPointer<Port> port, ports)
                {
                    textList.append("* " + port->getName());
                }

                msgBox.setDetailedText(textList.join("\n"));
                removePorts = (msgBox.exec() == QMessageBox::Yes);
            }

            // Delete the interfaces.
            getDiagram()->clearSelection();
            QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

            foreach (QGraphicsItem* selected, selectedItems)
            {
                BusInterfaceItem* diagIf = static_cast<BusInterfaceItem*>(selected);

                InterfaceDeleteCommand* childCmd = new InterfaceDeleteCommand(diagIf, removePorts, cmd.data());
                connect(childCmd, SIGNAL(interfaceDeleted()), this, SIGNAL(clearItemSelection()), Qt::UniqueConnection);            

                childCmd->redo();
            }

            getGenericEditProvider()->addCommand(cmd, false);
        }
        else if (type == BusPortItem::Type)
        {
            getDiagram()->clearSelection();
            QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

            foreach (QGraphicsItem* selected, selectedItems)
            {
                BusPortItem* port = static_cast<BusPortItem*>(selected);
            
                // Ports can be removed only if they are temporary.
                if (port->isTemporary())
                {
                    // Delete the port.
                    QUndoCommand* childCmd = new PortDeleteCommand(port, cmd.data());
                    childCmd->redo();
                }
            }

            getGenericEditProvider()->addCommand(cmd, false);
        }
        else if (type == HWConnection::Type)
        {
            getDiagram()->clearSelection();
            QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

            foreach (QGraphicsItem* selected, selectedItems)
            {
                // Delete the interconnection.
                HWConnection* conn = static_cast<HWConnection*>(selected);
                HWConnectionEndpoint* endpoint1 = static_cast<HWConnectionEndpoint*>(conn->endpoint1());
                HWConnectionEndpoint* endpoint2 = static_cast<HWConnectionEndpoint*>(conn->endpoint2());

                QUndoCommand* childCmd = new ConnectionDeleteCommand(conn, cmd.data());
                childCmd->redo();

                // If the bus ports are invalid, delete them too.
                if (endpoint1->isInvalid())
                {
                    QUndoCommand* childCmd = 0;

                    if (endpoint1->type() == BusPortItem::Type)
                    {
                        childCmd = new PortDeleteCommand(endpoint1, cmd.data());
                    }
                    else
                    {
                        childCmd = new InterfaceDeleteCommand(static_cast<BusInterfaceItem*>(endpoint1), false, cmd.data());
                    }

                    childCmd->redo();
                }

                if (endpoint2->isInvalid())
                {
                    QUndoCommand* childCmd = 0;

                    if (endpoint2->type() == BusPortItem::Type)
                    {
                        childCmd = new PortDeleteCommand(endpoint2, cmd.data());
                    }
                    else
                    {
                        childCmd = new InterfaceDeleteCommand(static_cast<BusInterfaceItem*>(endpoint2), false, cmd.data());
                    }

                    childCmd->redo();
                }
            }

            getGenericEditProvider()->addCommand(cmd, false);
        }
        else if (type == HWColumn::Type)
        {
            // Ask a confirmation if the user really wants to delete the entire column if it is not empty.
            foreach (QGraphicsItem* selected, selectedItems)
            {
                HWColumn* column = static_cast<HWColumn*>(selected);

                if (!column->isEmpty())
                {
                    QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                                       tr("The columns are not empty. Do you want to "
                                          "delete the columns and all of their contents?"),
                                       QMessageBox::Yes | QMessageBox::No, this);

                    if (msgBox.exec() == QMessageBox::No)
                    {
                        return;
                    }

                    break;
                }
            }

            // Delete the columns.
            getDiagram()->clearSelection();
            QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

            foreach (QGraphicsItem* selected, selectedItems)
            {
                HWColumn* column = static_cast<HWColumn*>(selected);
                QUndoCommand* childCmd = new ColumnDeleteCommand(getDiagram()->getColumnLayout(), column, cmd.data());
                childCmd->redo();
            }

            getGenericEditProvider()->addCommand(cmd, false);
        }
    }
    else
    {
        TabDocument::keyPressEvent(event);
    }
}

QSharedPointer<Component> HWDesignWidget::createEmptyDesign(VLNV const& prevlnv) {
    
	VLNV vlnv;
	QString path;

	if (prevlnv.isValid() && getLibraryInterface()->contains(prevlnv)) {
		vlnv = prevlnv;
		path = getLibraryInterface()->getPath(prevlnv);
		QFileInfo info(path);
		path = info.absolutePath();
	}
	else {
        NewObjectDialog newDesignDialog(getLibraryInterface(), VLNV::COMPONENT, true, parentWidget());
		newDesignDialog.setVLNV(prevlnv);
		newDesignDialog.exec();

		if (newDesignDialog.result() == QDialog::Rejected)
			return QSharedPointer<Component>();

		vlnv = newDesignDialog.getVLNV();
		path = newDesignDialog.getPath();
	}

	QSharedPointer<Component> newComponent;
	
	if (getLibraryInterface()->contains(prevlnv)) {
		// find the component
		QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(prevlnv);
		newComponent = libComp.staticCast<Component>();

		Q_ASSERT_X(newComponent, "HWDesignWidget::createEmptyDesign",
			"The selected library item has to be component");
	}
	else {
		// create the component 
		newComponent = QSharedPointer<Component>(new Component(vlnv));
	}

    getLibraryInterface()->writeModelToFile(path, newComponent);

	createDesignForComponent(newComponent, path);

    return newComponent;
}

void HWDesignWidget::createDesignForComponent(QSharedPointer<Component> component,
											const QString& dirPath) {
	VLNV vlnv = *component->getVlnv();

	// create a unique vlnv for the design
	VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());
	int runningNumber = 1;
	QString version = designVLNV.getVersion();
	// if vlnv is reserved then add "(<number>)" to end of version field
	while (getLibraryInterface()->contains(designVLNV)) {
		++runningNumber;
		designVLNV.setVersion(version + "(" + QString::number(runningNumber) + ")");
	}

	// create a unique vlnv for the design configuration
	VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());
	runningNumber = 1;
	version = desConfVLNV.getVersion();
	// if vlnv is reserved then add "(<number>)" to end of version field
	while (getLibraryInterface()->contains(desConfVLNV)) {
		++runningNumber;
		desConfVLNV.setVersion(version + "(" + QString::number(runningNumber) + ")");
	}

	// the name of the view to create
	QString viewName = tr("structural");

	// and a hierarchical view for it
	QSharedPointer<Model> model = component->getModel();
	Q_ASSERT(model);
	View *hierView = new View(viewName);
	hierView->setHierarchyRef(desConfVLNV);
	hierView->addEnvIdentifier("");
	model->addView(hierView);

	// create the design configuration
	QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
	designConf->setDesignRef(designVLNV);

	QSharedPointer<Design> newDesign = QSharedPointer<Design>(new Design(designVLNV));

	getLibraryInterface()->writeModelToFile(dirPath, newDesign);
	getLibraryInterface()->writeModelToFile(dirPath, designConf);
    getLibraryInterface()->writeModelToFile(component);

	setDesign(component, viewName);
}

void HWDesignWidget::onVhdlGenerate() {
	
	if (isModified() && askSaveFile()) {
		save();
	}

	QString fileName = getLibraryInterface()->getPath(*getEditedComponent()->getVlnv());
	QFileInfo targetInfo(fileName);
	fileName = targetInfo.absolutePath();
	fileName += QString("/");
	fileName += getEditedComponent()->getEntityName(getOpenViewName());
	fileName += QString(".vhd");

	QString path = QFileDialog::getSaveFileName(this,
		tr("Set the directory where the vhdl file is created to"),
		fileName, tr("Vhdl files (*.vhd)"));

	// if user clicks cancel then nothing is created
	if (path.isEmpty()) {
		return;
	}

	VhdlGenerator2 vhdlGen(getLibraryInterface(), this);
	
	// if errors are detected during parsing
	if (!vhdlGen.parse(getEditedComponent(), getOpenViewName())) {
		return;
	}

	// generate the vhdl code
	vhdlGen.generateVhdl(path);

	// check if the file already exists in the metadata
	QString basePath = getLibraryInterface()->getPath(*getEditedComponent()->getVlnv());
	QString relativePath = General::getRelativePath(basePath, path);
	if (!getEditedComponent()->hasFile(relativePath)) {

		// ask user if he wants to save the generated vhdl into object metadata
		QMessageBox::StandardButton button = QMessageBox::question(this, 
			tr("Save generated file to metadata?"),
			tr("Would you like to save the generated vhdl-file to IP-Xact"
			" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		// if the generated file is saved
		if (button == QMessageBox::Yes) {

			// add a rtl view to the getEditedComponent()
			vhdlGen.addRTLView(path);

			// write the getEditedComponent() into file system
			getLibraryInterface()->writeModelToFile(getEditedComponent());

			emit refresh(this);
		}
	}
}

void HWDesignWidget::onQuartusGenerate() {

	if (isModified() && askSaveFile()) {
		save();
	}

	QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(*getEditedComponent()->getVlnv());
	QSharedPointer<Component> component = libComp.staticCast<Component>();

	QString path = QFileDialog::getExistingDirectory(this,
		tr("Set the directory where the Quartus project is created to"),
		getLibraryInterface()->getPath(*getEditedComponent()->getVlnv()));

	// if user clicks cancel then nothing is created
	if (path.isEmpty())
		return;

	// create the quartus project
	QuartusGenerator quartusGen(getLibraryInterface(), this);
	connect(&quartusGen, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&quartusGen, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	// try to find the pin maps
	quartusGen.setBoardName(component);

	// parse the files from components
	quartusGen.parseFiles(component, getOpenViewName());

	quartusGen.generateProject(path, component->getVlnv()->getName());
}

void HWDesignWidget::onModelsimGenerate() {

	if (isModified() && askSaveFile()) {
		save();
	}

	QString fileName = getLibraryInterface()->getPath(*getEditedComponent()->getVlnv());
	QFileInfo targetInfo(fileName);
	fileName = targetInfo.absolutePath();
	fileName += QString("/%1.%2.compile.do").arg(
		getEditedComponent()->getVlnv()->getName()).arg(getOpenViewName());

	// ask user to select a location to save the makefile
	fileName = QFileDialog::getSaveFileName(this, 
		tr("Set the file name for the modelsim script."), fileName,
		tr("Modelsim scripts (*.do);;Shell cripts (*.sh)"));

	// if user clicked cancel
	if (fileName.isEmpty())
		return;


	// construct the generator
	ModelsimGenerator generator(getLibraryInterface(), this);
	connect(&generator, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&generator, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

	// parse the getEditedComponent() and view / sub-designs
	generator.parseFiles(getEditedComponent(), getOpenViewName());

	// create the script file
	generator.generateMakefile(fileName);

	// check if the file already exists in the metadata
	QString basePath = getLibraryInterface()->getPath(*getEditedComponent()->getVlnv());
	QString relativePath = General::getRelativePath(basePath, fileName);
	if (!getEditedComponent()->hasFile(relativePath)) {

		// ask user if he wants to save the generated modelsim script into 
		// object metadata
		QMessageBox::StandardButton button = QMessageBox::question(this, 
			tr("Save generated modelsim script to metadata?"),
			tr("Would you like to save the generated modelsim script to IP-Xact"
			" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		// if the generated file is saved
		if (button == QMessageBox::Yes) {

			QString xmlPath = getLibraryInterface()->getPath(*getEditedComponent()->getVlnv());

			// if the file was successfully added to the library
			if (generator.addMakefile2IPXact(getEditedComponent(), fileName, xmlPath)) {
				getLibraryInterface()->writeModelToFile(getEditedComponent());
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void HWDesignWidget::addColumn()
{
    ColumnEditDialog dialog(this, false);

    if (dialog.exec() == QDialog::Accepted)
    {
        int columnWidth = HWDesignDiagram::COMPONENT_COLUMN_WIDTH;

        if (dialog.getContentType() == COLUMN_CONTENT_IO)
        {
            columnWidth = HWDesignDiagram::IO_COLUMN_WIDTH;
        }

        ColumnDesc desc(dialog.getName(), dialog.getContentType(), dialog.getAllowedItems(), columnWidth);
        getDiagram()->addColumn(desc);
    }
}

//-----------------------------------------------------------------------------
// Function: getSupportedDrawModes()
//-----------------------------------------------------------------------------
unsigned int HWDesignWidget::getSupportedDrawModes() const
{
    if (!isProtected())
    {
        return (MODE_SELECT | MODE_CONNECT | MODE_INTERFACE | MODE_DRAFT | MODE_TOGGLE_OFFPAGE);
    }
    else
    {
        // Force to selection mode in read-only mode.
        return MODE_SELECT;
    }
}

QSharedPointer<Design> HWDesignWidget::createDesign( const VLNV& vlnv ) {
	return getDiagram()->createDesign(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::getImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation HWDesignWidget::getImplementation() const
{
    return KactusAttribute::KTS_HW;
}
