//-----------------------------------------------------------------------------
// File: HWDesignWidgetMock.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.02.2015
//
// Description:
// Mock class for hardware design widget.
//-----------------------------------------------------------------------------

#include "HWDesignWidgetMock.h"

#include <designEditors/HWDesign/HWDesignDiagram.h>
#include <designEditors/common/StickyNote/StickyNote.h>

#include <common/GenericEditProvider.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/view.h>

#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: HWDesignWidgetMock::HWDesignWidgetMock()
//-----------------------------------------------------------------------------
HWDesignWidgetMock::HWDesignWidgetMock(LibraryInterface *lh, QWidget* parent)
    : DesignWidget(lh, parent)
{
	// update the supported windows 
	supportedWindows_ = (supportedWindows_ | CONFIGURATIONWINDOW |
		CONNECTIONWINDOW | INTERFACEWINDOW |INSTANCEWINDOW | ADHOC_WINDOW | ADDRESS_WINDOW | NOTES_WINDOW);

    setDiagram(new HWDesignDiagram(lh, *getGenericEditProvider(), this));
    getDiagram()->setProtection(false);
    getDiagram()->setMode(MODE_SELECT);
    
	addVisibilityControl("Bus Widths", false);
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidgetMock::~HWDesignWidgetMock()
//-----------------------------------------------------------------------------
HWDesignWidgetMock::~HWDesignWidgetMock()
{
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidgetMock::setDesign()
//-----------------------------------------------------------------------------
bool HWDesignWidgetMock::setDesign(VLNV const& vlnv, QString const& viewName)
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
// Function: HWDesignWidgetMock::setDesign()
//-----------------------------------------------------------------------------
bool HWDesignWidgetMock::setDesign(QSharedPointer<Component> comp, const QString& viewName)
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
    DesignWidget::setDesign(comp, viewName);
	setDocumentName(QString("%1 (%2)").arg(getIdentifyingVLNV().getName()).arg(getIdentifyingVLNV().getVersion()));

    return true;
}

bool HWDesignWidgetMock::saveAs()
{
    return false;
}

void HWDesignWidgetMock::keyPressEvent(QKeyEvent *event)
{

}

QSharedPointer<Component> HWDesignWidgetMock::createEmptyDesign(VLNV const& prevlnv) {
    
	VLNV vlnv;
	QString path;

	if (prevlnv.isValid() && getLibraryInterface()->contains(prevlnv)) {
		vlnv = prevlnv;
		path = getLibraryInterface()->getPath(prevlnv);
		QFileInfo info(path);
		path = info.absolutePath();
	}
	else {
        Q_ASSERT(false);
	}

	QSharedPointer<Component> newComponent;
	
	if (getLibraryInterface()->contains(prevlnv)) {
		// find the component
		QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(prevlnv);
		newComponent = libComp.staticCast<Component>();

		Q_ASSERT_X(newComponent, "HWDesignWidgetMock::createEmptyDesign",
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

void HWDesignWidgetMock::createDesignForComponent(QSharedPointer<Component> component,
											const QString& dirPath)
{
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

void HWDesignWidgetMock::onVhdlGenerate()
{

}

void HWDesignWidgetMock::onQuartusGenerate()
{

}

void HWDesignWidgetMock::onModelsimGenerate()
{

}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void HWDesignWidgetMock::addColumn()
{

}

//-----------------------------------------------------------------------------
// Function: getSupportedDrawModes()
//-----------------------------------------------------------------------------
unsigned int HWDesignWidgetMock::getSupportedDrawModes() const
{
    if (isProtected())
    {
        // Force to selection mode in read-only mode.
        return MODE_SELECT;        
    }
    else
    {
        return (MODE_SELECT | MODE_CONNECT | MODE_INTERFACE | MODE_DRAFT | MODE_TOGGLE_OFFPAGE | MODE_LABEL);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidgetMock::createDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> HWDesignWidgetMock::createDesign( const VLNV& vlnv )
{
	return getDiagram()->createDesign(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidgetMock::getImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation HWDesignWidgetMock::getImplementation() const
{
    return KactusAttribute::KTS_HW;
}
