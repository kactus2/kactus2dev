/* 
 * 		filename: designwidget.cpp
 */

#include "HWDesignWidget.h"

#include "blockdiagram.h"
#include "diagramcomponent.h"
#include "diagraminterconnection.h"
#include "diagramport.h"
#include "diagraminterface.h"

#include "DiagramDeleteCommands.h"

#include <common/DiagramUtil.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/GenericEditProvider.h>

#include "LibraryManager/vlnv.h"
#include "LibraryManager/libraryinterface.h"

#include "columnview/ColumnEditDialog.h"
#include "columnview/DiagramColumn.h"

#include <models/librarycomponent.h>
#include <models/component.h>
#include <models/design.h>
#include <models/designconfiguration.h>
#include <models/model.h>
#include <models/view.h>
#include <models/generaldeclarations.h>

#include <vhdlGenerator/vhdlgenerator2.h>
#include <quartusGenerator/quartusgenerator.h>
#include <modelsimGenerator/modelsimgenerator.h>

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
		CONNECTIONWINDOW | INTERFACEWINDOW |INSTANCEWINDOW | ADHOC_WINDOW);

    setDiagram(new BlockDiagram(lh, *getGenericEditProvider(), this));
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

    setDocumentName(getEditedComponent()->getVlnv()->getName() + 
		            " (" + getEditedComponent()->getVlnv()->getVersion() + ")");
    setDocumentType("Design");

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

    return DesignWidget::setDesign(comp, viewName);
}

// bool HWDesignWidget::save()
// {
// 	QSharedPointer<Design> design;
// 
// 	QSharedPointer<DesignConfiguration> designConf = getDiagram()->getDesignConfiguration();
// 
// 	// create the design
// 
// 	// if design configuration is used
// 	if (designConf) {
// 		design = getDiagram()->createDesign(designConf->getDesignRef());
// 	}
// 	// if component 
// 	else {
// 		design = getDiagram()->createDesign(getEditedComponent()->getHierRef(getOpenViewName()));
// 	}
// 
//     if (design == 0)
//     {
//         return false;
//     }
//     
// 	// update the hierarchical bus interfaces of the top-component
// 	getDiagram()->updateHierComponent(getEditedComponent());
// 
// 	bool writePossible = true;
// 	QStringList errorList;
// 
// 	// if design config is used then check that it can be written
// 	if (designConf && !designConf->isValid(errorList)) {
// 
// 		emit noticeMessage(tr("The configuration contained the following errors:"));
// 		foreach (QString error, errorList) {
// 			emit errorMessage(error);
// 		}
// 		errorList.clear();
// 		writePossible = false;
// 	}
// 
// 	// check that the design is in valid state and can be written
// 
// 	Q_ASSERT(design);
// 	if (!design->isValid(errorList)) {
// 
// 		emit noticeMessage(tr("The design contained the following errors:"));
// 		foreach (QString error, errorList) {
// 			emit errorMessage(error);
// 		}
// 		errorList.clear();
// 		writePossible = false;
// 	}
// 
// 	// check the component validity
// 
// 	Q_ASSERT(getEditedComponent());
// 	if (!getEditedComponent()->isValid(errorList)) {
// 
// 		emit noticeMessage(tr("The component contained the following errors:"));
// 		foreach (QString error, errorList) {
// 			emit errorMessage(error);
// 		}
// 		errorList.clear();
// 		writePossible = false;
// 	}
// 
// 	// if there were errors then don't write anything
// 	if (!writePossible) {
// 		emit noticeMessage(tr("Nothing was written. Fix the errors and try saving again."));
// 		return false;
// 	}
// 
// 	getLibraryInterface()->beginSave();
// 
// 	bool writeSucceeded = true;
// 
// 	// if design configuration is used then write it.
// 	if (designConf) {
// 		if (!getLibraryInterface()->writeModelToFile(designConf)) {
// 			writeSucceeded = false;
// 		}
// 	}
// 
// 	if (!getLibraryInterface()->writeModelToFile(design)) {
// 		writeSucceeded = false;
// 	}
// 	if (!getLibraryInterface()->writeModelToFile(getEditedComponent())) {
// 		writeSucceeded = false;
// 	}
// 
// 	getLibraryInterface()->endSave();
// 
// 	if (writeSucceeded) {
// 	    return TabDocument::save();
// 	}
// 	else {
// 		emit errorMessage(tr("Design was not saved to disk."));
// 		return false;
// 	}
// }

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
	getEditedComponent() = QSharedPointer<Component>(new Component(*getEditedComponent()));

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
	static_cast<BlockDiagram*>(getDiagram())->updateHierComponent(getEditedComponent());

	// get the paths to the original xml file
	QFileInfo sourceInfo(getLibraryInterface()->getPath(*oldComponent->getVlnv()));
	QString sourcePath = sourceInfo.absolutePath();

	// update the file paths and copy necessary files
	getEditedComponent()->updateFiles(*oldComponent, sourcePath, directory);

	// create the files for the documents

	bool writePossible = true;
	QStringList errorList;

	// if design config is used then check that it can be written
	if (designConf && !designConf->isValid(errorList)) {
		
		emit noticeMessage(tr("The configuration contained the following errors:"));
		foreach (QString error, errorList) {
			emit errorMessage(error);
		}
		errorList.clear();
		writePossible = false;
	}

	// check that the design is in valid state and can be written

	Q_ASSERT(design);
	if (!design->isValid(errorList)) {
		
		emit noticeMessage(tr("The design contained the following errors:"));
		foreach (QString error, errorList) {
			emit errorMessage(error);
		}
		errorList.clear();
		writePossible = false;
	}

	// check the component validity

	Q_ASSERT(getEditedComponent());
	if (!getEditedComponent()->isValid(errorList)) {

		emit noticeMessage(tr("The component contained the following errors:"));
		foreach (QString error, errorList) {
			emit errorMessage(error);
		}
		errorList.clear();
		writePossible = false;
	}

	// if there were errors then don't write anything
	if (!writePossible) {
		emit noticeMessage(tr("Nothing was written. Fix the errors and try saving again."));
		return false;
	}

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

	if (writeSucceeded) {
		setDocumentName(getEditedComponent()->getVlnv()->getName() + " (" + 
			getEditedComponent()->getVlnv()->getVersion() + ")");
		return TabDocument::saveAs();
	}
	else {
		emit errorMessage(tr("Design was not saved to disk."));
		return false;
	}

}

void HWDesignWidget::keyPressEvent(QKeyEvent *event)
{
    // Handle delete events if the document is not protected.
    if (!isProtected() && event->key() == Qt::Key_Delete) {
        if (getDiagram()->selectedItems().empty()) {
            return;
        }
        QGraphicsItem *selected = getDiagram()->selectedItems().first();

        if (selected->type() == DiagramComponent::Type)
        {
			DiagramComponent* component = static_cast<DiagramComponent*>(selected);
			getDiagram()->removeInstanceName(component->name());
            getDiagram()->clearSelection();
            
            QSharedPointer<ComponentDeleteCommand> cmd(new ComponentDeleteCommand(component));

			connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
				this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
			connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
				this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);

            getGenericEditProvider()->addCommand(cmd);

            emit clearItemSelection();
        }
        else if (selected->type() == DiagramInterface::Type)
        {
            DiagramInterface* diagIf = static_cast<DiagramInterface*>(selected);

            // Ask the user if he/she wants to delete the ports too.
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("Do you want to delete also the ports that are part of the interface?"),
                               QMessageBox::Yes | QMessageBox::No, this);
            bool removePorts = (msgBox.exec() == QMessageBox::Yes);

            getDiagram()->clearSelection();
            emit clearItemSelection();

            // Delete the interface.
            QSharedPointer<QUndoCommand> cmd(new InterfaceDeleteCommand(diagIf, removePorts));
            getGenericEditProvider()->addCommand(cmd);
        }
        else if (selected->type() == DiagramPort::Type)
        {
            DiagramPort* port = static_cast<DiagramPort*>(selected);
            DiagramComponent* comp = static_cast<DiagramComponent*>(port->parentItem());

            // Ports can be removed only if the parent component is not
            // yet packaged (i.e. has an invalid VLNV).
            if (!comp->componentModel()->getVlnv()->isValid())
            {
                // Delete the port.
                QSharedPointer<QUndoCommand> cmd(new PortDeleteCommand(port));
                getGenericEditProvider()->addCommand(cmd);

                // Clear the item selection.
                emit clearItemSelection();
            }
        }
        else if (selected->type() == DiagramInterconnection::Type)
        {
            emit clearItemSelection();

            // Delete the interconnection.
            QSharedPointer<QUndoCommand> cmd(new ConnectionDeleteCommand(
                static_cast<DiagramInterconnection*>(selected)));
            getGenericEditProvider()->addCommand(cmd);
        }
        else if (selected->type() == DiagramColumn::Type)
        {
            // Ask a confirmation if the user really wants to delete the entire column
            // if it is not empty.
            DiagramColumn* column = static_cast<DiagramColumn*>(selected);

            bool del = true;

            if (!column->isEmpty())
            {
                QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                                   tr("The column is not empty. Do you want to "
                                   "delete the column and all of its contents?"),
                                   QMessageBox::Yes | QMessageBox::No, this);

                if (msgBox.exec() == QMessageBox::No)
                {
                    del = false;
                }
            }

            if (!del)
            {
                return;
            }

            // Delete the column if requested.
            QSharedPointer<QUndoCommand> cmd(new ColumnDeleteCommand(getDiagram()->getColumnLayout(), column));
            getGenericEditProvider()->addCommand(cmd);
        }

        // If something was removed then save button must be enabled again.
        emit contentChanged();
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
	Model *model = component->getModel();
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
	fileName += QString("/%1.%2.create_makefile").arg(
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
    ColumnEditDialog dlg(this, false);

    if (dlg.exec() == QDialog::Accepted)
    {
        ColumnDesc desc(dlg.getName(), dlg.getContentType(), dlg.getAllowedItems());
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
