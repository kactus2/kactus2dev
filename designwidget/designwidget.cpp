/* 
 * 		filename: designwidget.cpp
 */

#include "designwidget.h"
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

#include <models/librarycomponent.h>
#include <models/component.h>
#include <models/design.h>
#include <models/designconfiguration.h>
#include <models/model.h>
#include <models/view.h>
#include <models/generaldeclarations.h>

#include <exceptions/invalid_file.h>

#include <exceptions/vhdl_error.h>
#include <exceptions/write_error.h>

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

DesignWidget::DesignWidget(LibraryInterface *lh, QWidget* parent): 
TabDocument(parent, DOC_ZOOM_SUPPORT | DOC_DRAW_MODE_SUPPORT | DOC_PRINT_SUPPORT |
                    DOC_PROTECTION_SUPPORT | DOC_EDIT_SUPPORT | DOC_VISIBILITY_CONTROL_SUPPORT, 30, 300), 
hierComponent_(NULL), 
viewName_(), 
view_(NULL), 
diagram_(NULL), 
lh_(NULL),
selectionDock_(NULL), 
editProvider_() {

	// update the supported windows 
	supportedWindows_ = (supportedWindows_ | CONFIGURATIONWINDOW |
		CONNECTIONWINDOW | INTERFACEWINDOW |INSTANCEWINDOW);

    lh_ = lh;
    editProvider_ = QSharedPointer<GenericEditProvider>(new GenericEditProvider(EDIT_HISTORY_SIZE));

    diagram_ = new BlockDiagram(lh, *editProvider_, this);

    connect(diagram_, SIGNAL(openDesign(const VLNV&, const QString&)),
		this, SIGNAL(openDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(diagram_, SIGNAL(openComponent(const VLNV&)),
		this, SIGNAL(openComponent(const VLNV&)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(openBus(VLNV const&, VLNV const&, bool)),
        this, SIGNAL(openBus(VLNV const&, VLNV const&, bool)), Qt::UniqueConnection);
	connect(diagram_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(diagram_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	
	connect(diagram_, SIGNAL(portSelected(DiagramPort*)),
		this, SIGNAL(portSelected(DiagramPort*)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(interfaceSelected(DiagramInterface*)),
        this, SIGNAL(interfaceSelected(DiagramInterface*)), Qt::UniqueConnection);
	connect(diagram_, SIGNAL(componentSelected(ComponentItem*)),
		this, SIGNAL(componentSelected(ComponentItem*)), Qt::UniqueConnection);
	connect(diagram_, SIGNAL(connectionSelected(DiagramInterconnection*)),
		this, SIGNAL(connectionSelected(DiagramInterconnection*)), Qt::UniqueConnection);
	connect(diagram_, SIGNAL(clearItemSelection()),
		this, SIGNAL(clearItemSelection()), Qt::UniqueConnection);
	connect(diagram_, SIGNAL(componentInstantiated(DiagramComponent*)),
		this, SIGNAL(componentInstantiated(DiagramComponent*)), Qt::UniqueConnection);
	connect(diagram_, SIGNAL(componentInstanceRemoved(DiagramComponent*)),
		this, SIGNAL(componentInstanceRemoved(DiagramComponent*)), Qt::UniqueConnection);

    diagram_->setProtection(false);
	diagram_->setMode(MODE_SELECT);

    view_ = new QGraphicsView(this);
    view_->setScene(diagram_);
    view_->centerOn(0, 0);
	view_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
   
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view_);

    view_->verticalScrollBar()->setTracking(true);
    connect(view_->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onVerticalScroll(int)));

    addVisibilityControl("Bus Widths", false);
}

DesignWidget::~DesignWidget()
{
}

void DesignWidget::setDesign( const VLNV* vlnv, const QString& viewName) {
	
	Q_ASSERT_X(vlnv, "DesignWidget::setDesign", "Null vlnv pointer given as parameter");

	disconnect(diagram_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()));

	// if vlnv and view name was defined which means that an existing component is opened
	if (vlnv->isValid() && !viewName.isEmpty()) {

		// if vlnv is writeSucceeded and the type is component
		if (vlnv->isValid() && vlnv->getType() == VLNV::COMPONENT) {

			// create model 
			QSharedPointer<LibraryComponent> libComponent = lh_->getModel(*vlnv);
			if (!libComponent)
				return;

			QSharedPointer<Component> comp = libComponent.staticCast<Component>();

			View* view = comp->findView(viewName);

			if (!view || !view->isHierarchical()) {
				return;
				
			}

			// save the name of the view being edited.
			viewName_ = viewName;

			diagram_->setDesign(comp, viewName);
			hierComponent_ = comp;
		}
	}

	// if vlnv was writeSucceeded but view is empty then should create a new design for the component
	else if (vlnv->isValid() && viewName.isEmpty()) {
		Q_ASSERT(lh_->contains(*vlnv));
		Q_ASSERT(lh_->getDocumentType(*vlnv) == VLNV::COMPONENT);

		QSharedPointer<LibraryComponent> libComp = lh_->getModel(*vlnv);
		hierComponent_ = libComp.staticCast<Component>();

		// get the directory path where the component's xml file is located
		const QString xmlPath = lh_->getPath(*vlnv);
		QFileInfo xmlInfo(xmlPath);
		const QString dirPath = xmlInfo.absolutePath();

		createDesignForComponent(hierComponent_, dirPath);
	}

	// if vlnv was not defined (a new hierarchical component is created)
	else {
		hierComponent_ = createEmptyDesign(vlnv);
		if (!hierComponent_) {
			return;
		}
		diagram_->setDesign(hierComponent_, viewName);
		viewName_ = viewName;
	}

	// disable the save at startup
	connect(diagram_, SIGNAL(contentChanged()),
		    this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(diagram_, SIGNAL(modeChanged(DrawMode)),
		    this, SIGNAL(modeChanged(DrawMode)), Qt::UniqueConnection);
	setModified(false);

    setDocumentName(hierComponent_->getVlnv()->getName() + 
		            " (" + hierComponent_->getVlnv()->getVersion() + ")");

    if (hierComponent_->getComponentImplementation() == KactusAttribute::KTS_SW &&
        hierComponent_->getComponentSWType() == KactusAttribute::KTS_SW_PLATFORM)
    {
        setDocumentType("SW Platform Stack");
    }
    else
    {
        setDocumentType("Design");
    }

	emit clearItemSelection();
}

bool DesignWidget::save()
{
	QSharedPointer<Design> design;

	QSharedPointer<DesignConfiguration> designConf = diagram_->getDesignConfiguration();

	// create the design

	// if design configuration is used
	if (designConf) {
		design = diagram_->createDesign(designConf->getDesignRef());
	}
	// if component 
	else {
		design = diagram_->createDesign(hierComponent_->getHierRef(viewName_));
	}

    if (design == 0)
    {
        return false;
    }
    
	// update the hierarchical bus interfaces of the top-component
	diagram_->updateHierComponent(hierComponent_);

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

	Q_ASSERT(hierComponent_);
	if (!hierComponent_->isValid(errorList)) {

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

	lh_->beginSave();

	bool writeSucceeded = true;

	// if design configuration is used then write it.
	if (designConf) {
		if (!lh_->writeModelToFile(designConf)) {
			writeSucceeded = false;
		}
	}

	if (!lh_->writeModelToFile(design)) {
		writeSucceeded = false;
	}
	if (!lh_->writeModelToFile(hierComponent_)) {
		writeSucceeded = false;
	}

	lh_->endSave();

	if (writeSucceeded) {
	    return TabDocument::save();
	}
	else {
		emit errorMessage(tr("Design was not saved to disk."));
		return false;
	}
}

bool DesignWidget::saveAs() {

	VLNV oldVLNV = *hierComponent_->getVlnv();

    // Ask the user for a new VLNV along with attributes and directory.
    KactusAttribute::ProductHierarchy prodHier = hierComponent_->getComponentHierarchy();
    KactusAttribute::Firmness firmness = hierComponent_->getComponentFirmness();

    VLNV vlnv;
    QString directory;

    if (!NewObjectDialog::saveAsDialog(this, lh_, oldVLNV, prodHier, firmness, vlnv, directory))
    {
        // If the user canceled, there was no error.
        return true;
    }

	// create the vlnv for design and design configuration
	VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());
	VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());

	QSharedPointer<Design> design;

	// create the design

	QSharedPointer<Component> oldComponent = hierComponent_;

	// make a copy of the hierarchical component
	hierComponent_ = QSharedPointer<Component>(new Component(*hierComponent_));

	// set the new vlnv for the component
	hierComponent_->setVlnv(vlnv);

	QSharedPointer<DesignConfiguration> designConf = diagram_->getDesignConfiguration();

	// if design configuration is used
	if (designConf) {

		// make a copy of the design configuration
		designConf = QSharedPointer<DesignConfiguration>(new DesignConfiguration(*designConf));

		// set design configuration's vlnv to match new vlnv
		designConf->setVlnv(desConfVLNV);

		// set design configuration to reference to new design vlnv
		designConf->setDesignRef(designVLNV);

		// set component to reference new design configuration
		hierComponent_->setHierRef(desConfVLNV, viewName_);

		// create design with new design vlnv
		design = diagram_->createDesign(designVLNV);
	}
	// if component does not use design configuration then it references directly to design
	else {
		// set component to reference new design
		hierComponent_->setHierRef(designVLNV, viewName_);

		design = diagram_->createDesign(designVLNV);
	}

	if (design == 0)
	{
		return false;
	}

	// update the hierarchical bus interfaces of the top-component
	diagram_->updateHierComponent(hierComponent_);

	// get the paths to the original xml file
	QFileInfo sourceInfo(lh_->getPath(*oldComponent->getVlnv()));
	QString sourcePath = sourceInfo.absolutePath();

	// update the file paths and copy necessary files
	hierComponent_->updateFiles(*oldComponent, sourcePath, directory);

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

	Q_ASSERT(hierComponent_);
	if (!hierComponent_->isValid(errorList)) {

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

	lh_->beginSave();

	// if design configuration is used then write it.
	if (designConf) {
		if (!lh_->writeModelToFile(directory, designConf, false)) {
			writeSucceeded = false;
		}
	}

	if (!lh_->writeModelToFile(directory, design, false)) {
		writeSucceeded = false;
	}
	if (!lh_->writeModelToFile(directory, hierComponent_, false)) {
		writeSucceeded = false;
	}

	lh_->endSave();

	if (writeSucceeded) {
		setDocumentName(hierComponent_->getVlnv()->getName() + " (" + 
			hierComponent_->getVlnv()->getVersion() + ")");
		return TabDocument::saveAs();
	}
	else {
		emit errorMessage(tr("Design was not saved to disk."));
		return false;
	}

}

const VLNV *DesignWidget::getOpenDocument() const
{
	if (hierComponent_)
		return hierComponent_->getVlnv();

    return 0;
}

void DesignWidget::keyPressEvent(QKeyEvent *event)
{
    // If the document is protected, skip all delete events.
    if (isProtected())
    {
        return;
    }

    if (event->key() == Qt::Key_Delete) {
        if (diagram_->selectedItems().empty()) {
            return;
        }
        QGraphicsItem *selected = diagram_->selectedItems().first();

        if (selected->type() == DiagramComponent::Type)
        {
			DiagramComponent* component = static_cast<DiagramComponent*>(selected);
			diagram_->removeInstanceName(component->name());
            diagram_->clearSelection();
            
            QSharedPointer<ComponentDeleteCommand> cmd(new ComponentDeleteCommand(component));

			connect(cmd.data(), SIGNAL(componentInstanceRemoved(DiagramComponent*)),
				this, SIGNAL(componentInstanceRemoved(DiagramComponent*)), Qt::UniqueConnection);
			connect(cmd.data(), SIGNAL(componentInstantiated(DiagramComponent*)),
				this, SIGNAL(componentInstantiated(DiagramComponent*)), Qt::UniqueConnection);

            editProvider_->addCommand(cmd);

            emit clearItemSelection();
        }
        else if (selected->type() == DiagramInterface::Type)
        {
            DiagramInterface* diagIf = static_cast<DiagramInterface*>(selected);
            diagram_->clearSelection();

            // Delete the interface.
            QSharedPointer<QUndoCommand> cmd(new InterfaceDeleteCommand(diagIf));
            editProvider_->addCommand(cmd);
            
            emit clearItemSelection();
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
                editProvider_->addCommand(cmd);

                // Clear the item selection.
                emit clearItemSelection();
            }
        }
        else if (selected->type() == DiagramInterconnection::Type)
        {
            // Delete the interconnection.
            QSharedPointer<QUndoCommand> cmd(new ConnectionDeleteCommand(
                static_cast<DiagramInterconnection*>(selected)));
            editProvider_->addCommand(cmd);

            emit clearItemSelection();
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
            QSharedPointer<QUndoCommand> cmd(new ColumnDeleteCommand(diagram_->getColumnLayout(), column));
            editProvider_->addCommand(cmd);
        }

        // if something was removed then save button must be enabled again
        emit contentChanged();
    }
}

QSharedPointer<Component> DesignWidget::createEmptyDesign(const VLNV* prevlnv) {
    
	Q_ASSERT_X(prevlnv, "DesignWidget::createEmptyDesign",
		"Null vlnv pointer given as parameter.");

	VLNV vlnv;
	QString path;

	if (prevlnv->isValid() && lh_->contains(*prevlnv)) {
		vlnv = *prevlnv;
		path = lh_->getPath(*prevlnv);
		QFileInfo info(path);
		path = info.absolutePath();
	}
	else {
        NewObjectDialog newDesignDialog(lh_, VLNV::COMPONENT, true, parentWidget());
		newDesignDialog.setVLNV(*prevlnv);
		newDesignDialog.exec();

		if (newDesignDialog.result() == QDialog::Rejected)
			return QSharedPointer<Component>();

		vlnv = newDesignDialog.getVLNV();
		path = newDesignDialog.getPath();
	}

	QSharedPointer<Component> newComponent;
	
	if (lh_->contains(*prevlnv)) {
		// find the component
		QSharedPointer<LibraryComponent> libComp = lh_->getModel(*prevlnv);
		newComponent = libComp.staticCast<Component>();

		Q_ASSERT_X(newComponent, "DesignWidget::createEmptyDesign",
			"The selected library item has to be component");
	}
	else {
		// create the component 
		newComponent = QSharedPointer<Component>(new Component(vlnv));
	}

	createDesignForComponent(newComponent, path);

	lh_->writeModelToFile(path, newComponent);

//     QFile compFile(path + "/" + vlnv.getName() + "." + 
// 		designVLNV.getVersion() + ".xml");
//     compFile.open(QFile::WriteOnly | QFile::Truncate);
//     newComponent->write(compFile);
// 	compFile.close();

	// register the vlnvs to the library

	// if the component already existed in the library
// 	if (lh_->contains(*prevlnv)) {
// 		lh_->updatePath(vlnv, compFile.fileName());
// 	}
// 	else {
// 		lh_->registerVLNV(vlnv, compFile.fileName());
// 	}

//     lh_->registerVLNV(designVLNV, designFile.fileName());
// 	lh_->registerVLNV(desConfVLNV, desConfFile.fileName());

    return newComponent;
}

void DesignWidget::createDesignForComponent( QSharedPointer<Component> component,
											const QString& dirPath) {
	VLNV vlnv = *component->getVlnv();

	// create a unique vlnv for the design
	VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());
	int runningNumber = 1;
	QString version = designVLNV.getVersion();
	// if vlnv is reserved then add "(<number>)" to end of version field
	while (lh_->contains(designVLNV)) {
		++runningNumber;
		designVLNV.setVersion(version + "(" + QString::number(runningNumber) + ")");
	}

	// create a unique vlnv for the design configuration
	VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());
	runningNumber = 1;
	version = desConfVLNV.getVersion();
	// if vlnv is reserved then add "(<number>)" to end of version field
	while (lh_->contains(desConfVLNV)) {
		++runningNumber;
		desConfVLNV.setVersion(version + "(" + QString::number(runningNumber) + ")");
	}

	// the name of the view to create
	viewName_ = tr("structural");

	// and a hierarchical view for it
	Model *model = component->getModel();
	Q_ASSERT(model);
	View *hierView = new View(viewName_);
	hierView->setHierarchyRef(desConfVLNV);
	hierView->addEnvIdentifier("");
	model->addView(hierView);

	// create the design configuration
	QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
	designConf->setDesignRef(designVLNV);

	QSharedPointer<Design> newDesign = QSharedPointer<Design>(new Design(designVLNV));

	lh_->writeModelToFile(dirPath, newDesign);
	lh_->writeModelToFile(dirPath, designConf);
    lh_->writeModelToFile(component);

	diagram_->setDesign(component, viewName_);
}

void DesignWidget::onVhdlGenerate() {
	
	if (isModified() && askSaveFile()) {
		save();
	}

	QString fileName = lh_->getPath(*hierComponent_->getVlnv());
	QFileInfo targetInfo(fileName);
	fileName = targetInfo.absolutePath();
	fileName += QString("/");
	fileName += hierComponent_->getVlnv()->getName();
	fileName += QString(".%1").arg(viewName_);
	fileName += QString(".vhd");

	QString path = QFileDialog::getSaveFileName(this,
		tr("Set the directory where the vhdl file is created to"),
		fileName, tr("Vhdl files (*.vhd)"));

	// if user clicks cancel then nothing is created
	if (path.isEmpty())
		return;

	VhdlGenerator2 vhdlGen(lh_, this);
	vhdlGen.parse(hierComponent_, viewName_);
	vhdlGen.generateVhdl(path);

	// check if the file already exists in the metadata
	QString basePath = lh_->getPath(*hierComponent_->getVlnv());
	QString relativePath = General::getRelativePath(basePath, path);
	if (!hierComponent_->hasFile(relativePath)) {

		// ask user if he wants to save the generated vhdl into object metadata
		QMessageBox::StandardButton button = QMessageBox::question(this, 
			tr("Save generated file to metadata?"),
			tr("Would you like to save the generated vhdl-file to IP-Xact"
			" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		// if the generated file is saved
		if (button == QMessageBox::Yes) {

			// add a rtl view to the hierComponent_
			vhdlGen.addRTLView(path);

			// write the hierComponent_ into file system
			lh_->writeModelToFile(hierComponent_);

			emit refresh(this);
		}
	}
}

void DesignWidget::onQuartusGenerate() {

	if (isModified() && askSaveFile()) {
		save();
	}

	QSharedPointer<LibraryComponent> libComp = lh_->getModel(*hierComponent_->getVlnv());
	QSharedPointer<Component> component = libComp.staticCast<Component>();

	QString path = QFileDialog::getExistingDirectory(this,
		tr("Set the directory where the Quartus project is created to"),
		lh_->getPath(*hierComponent_->getVlnv()));

	// if user clicks cancel then nothing is created
	if (path.isEmpty())
		return;

	// create the quartus project
	QuartusGenerator quartusGen(lh_, this);
	connect(&quartusGen, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&quartusGen, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	// try to find the pin maps
	quartusGen.setBoardName(component);

	// parse the files from components
	quartusGen.parseFiles(component, viewName_);

	quartusGen.generateProject(path, component->getVlnv()->getName());
}

void DesignWidget::onModelsimGenerate() {

	if (isModified() && askSaveFile()) {
		save();
	}

	QString fileName = lh_->getPath(*hierComponent_->getVlnv());
	QFileInfo targetInfo(fileName);
	fileName = targetInfo.absolutePath();
	fileName += QString("/%1.%2.create_makefile").arg(
		hierComponent_->getVlnv()->getName()).arg(viewName_);

	// ask user to select a location to save the makefile
	fileName = QFileDialog::getSaveFileName(this, 
		tr("Set the file name for the modelsim script."), fileName,
		tr("Modelsim scripts (*.do);;Shell cripts (*.sh)"));

	// if user clicked cancel
	if (fileName.isEmpty())
		return;


	// construct the generator
	ModelsimGenerator generator(lh_, this);
	connect(&generator, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&generator, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

	// parse the hierComponent_ and view / sub-designs
	generator.parseFiles(hierComponent_, viewName_);

	// create the script file
	generator.generateMakefile(fileName);

	// check if the file already exists in the metadata
	QString basePath = lh_->getPath(*hierComponent_->getVlnv());
	QString relativePath = General::getRelativePath(basePath, fileName);
	if (!hierComponent_->hasFile(relativePath)) {

		// ask user if he wants to save the generated modelsim script into 
		// object metadata
		QMessageBox::StandardButton button = QMessageBox::question(this, 
			tr("Save generated modelsim script to metadata?"),
			tr("Would you like to save the generated modelsim script to IP-Xact"
			" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		// if the generated file is saved
		if (button == QMessageBox::Yes) {

			QString xmlPath = lh_->getPath(*hierComponent_->getVlnv());

			// if the file was successfully added to the library
			if (generator.addMakefile2IPXact(hierComponent_, fileName, xmlPath)) {
				lh_->writeModelToFile(hierComponent_);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: onVerticalScroll()
//-----------------------------------------------------------------------------
void DesignWidget::onVerticalScroll(int y)
{
    QPointF pt(0.0, y);
    QMatrix mat = view_->matrix().inverted();
    diagram_->onVerticalScroll(mat.map(pt).y());
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void DesignWidget::addColumn()
{
    ColumnEditDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted)
    {
        diagram_->addColumn(dlg.getName(), dlg.getContentType(), dlg.getAllowedItems());
    }
}

//-----------------------------------------------------------------------------
// Function: setZoomLevel()
//-----------------------------------------------------------------------------
void DesignWidget::setZoomLevel(int level)
{
    TabDocument::setZoomLevel(level);

    double newScale = getZoomLevel() / 100.0;
    QMatrix oldMatrix = view_->matrix();
    view_->resetMatrix();
    view_->translate(oldMatrix.dx(), oldMatrix.dy());
    view_->scale(newScale, newScale);

    emit zoomChanged();
}

//-----------------------------------------------------------------------------
// Function: fitInView()
//-----------------------------------------------------------------------------
void DesignWidget::fitInView()
{
    QRectF itemRect = diagram_->itemsBoundingRect();
    float scaleX = std::max(0, view_->width() - 10 - view_->verticalScrollBar()->width()) / itemRect.width();
    float scaleY = std::max(0, view_->height() - 10 - view_->horizontalScrollBar()->height()) / itemRect.height();

    int scaleLevel = int(std::min(scaleX, scaleY) * 10) * 10;
    setZoomLevel(scaleLevel);

    view_->centerOn(itemRect.center());
}

//-----------------------------------------------------------------------------
// Function: setMode()
//-----------------------------------------------------------------------------
void DesignWidget::setMode(DrawMode mode)
{
    diagram_->setMode(mode);
}

//-----------------------------------------------------------------------------
// Function: getSupportedDrawModes()
//-----------------------------------------------------------------------------
unsigned int DesignWidget::getSupportedDrawModes() const
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

//-----------------------------------------------------------------------------
// Function: getView()
//-----------------------------------------------------------------------------
QGraphicsView* DesignWidget::getView()
{
    return view_;
}

QString DesignWidget::getOpenViewName() const {
	return viewName_;
}

//-----------------------------------------------------------------------------
// Function: showEvent()
//-----------------------------------------------------------------------------
void DesignWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    diagram_->onShow();
}

VLNV DesignWidget::getComponentVLNV() const {
	return *hierComponent_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void DesignWidget::setProtection(bool locked)
{
    TabDocument::setProtection(locked);
    //view_->setInteractive(!locked).
    diagram_->setProtection(locked);
    diagram_->setMode(MODE_SELECT);
}

//-----------------------------------------------------------------------------
// Function: getEditProvider()
//-----------------------------------------------------------------------------
IEditProvider* DesignWidget::getEditProvider()
{
    return editProvider_.data();
}

//-----------------------------------------------------------------------------
// Function: refresh()
//-----------------------------------------------------------------------------
void DesignWidget::refresh()
{
    Q_ASSERT(!isModified());

	QSharedPointer<LibraryComponent> libComp = lh_->getModel(*hierComponent_->getVlnv());
	hierComponent_ = libComp.staticCast<Component>();

    diagram_->setDesign(hierComponent_, viewName_);

    setModified(false);

	TabDocument::refresh();
}

//-----------------------------------------------------------------------------
// Function: print()
//-----------------------------------------------------------------------------
void DesignWidget::print()
{

	// get the rect that bounds all items on box
	QRectF boundingRect = diagram_->itemsBoundingRect();
	
	// avoid clipping of the edges of the objects by increasing the rect a little
	boundingRect.setHeight(boundingRect.height() + 2);
	boundingRect.setWidth(boundingRect.width() + 2);

	// set the size of the picture
	QPixmap pic(boundingRect.size().toSize());

	// render the picture for the design
	QPainter picPainter(&pic);
	picPainter.fillRect(pic.rect(), QBrush(Qt::white));
	diagram_->render(&picPainter, pic.rect(), boundingRect.toRect());
	
	// create the printer instance, set default settings for it and
	// execute print dialog for user to select the printer and desired options
    QPrinter printer(QPrinter::ScreenResolution);
	printer.setOrientation(QPrinter::Landscape);
	printer.setPaperSize(QPrinter::A4);
    QPrintDialog dialog(&printer, this);

    if (dialog.exec() == QPrintDialog::Accepted)
    {
		// create painter instance to draw the picture for the printer
        QPainter painter(&printer);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
			QPainter::SmoothPixmapTransform, true);

		// scale the picture to match the size of the printer page
		QPixmap scaledPixmap = pic.scaled(printer.pageRect().size(),
			Qt::KeepAspectRatio, Qt::SmoothTransformation);

		// draw the picture on the printer
		painter.drawPixmap(printer.pageRect().topLeft(), scaledPixmap);
    }
}

QSharedPointer<Component> DesignWidget::getHierComponent() const {
	return hierComponent_;
}

QList<DiagramComponent*> DesignWidget::getInstances() const {
	return diagram_->getInstances();
}

QSharedPointer<DesignConfiguration> DesignWidget::getConfiguration() const {
	return diagram_->getDesignConfiguration();
}

QSharedPointer<GenericEditProvider> DesignWidget::getGenericEditProvider() const {
	return editProvider_;
}

bool DesignWidget::isHWImplementation() const {
	return hierComponent_->getComponentImplementation() == KactusAttribute::KTS_HW;
}

QRectF DesignWidget::itemsBoundingRect() const {
	return diagram_->itemsBoundingRect();
}

BlockDiagram* DesignWidget::scene() const {
	return diagram_;
}

QSharedPointer<Design> DesignWidget::createDesign( const VLNV& vlnv ) {
	return diagram_->createDesign(vlnv);
}

//-----------------------------------------------------------------------------
// Function: setVisibilityControlState()
//-----------------------------------------------------------------------------
void DesignWidget::setVisibilityControlState(QString const& name, bool state)
{
    TabDocument::setVisibilityControlState(name, state);

    if (name == "Bus Widths")
    {
        diagram_->setBusWidthsVisible(state);
    }
}
