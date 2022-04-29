//-----------------------------------------------------------------------------
// File: designwidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWDesignWidget is used to edit and view one design
//-----------------------------------------------------------------------------

#include "HWDesignWidget.h"

#include "HWDesignDiagram.h"
#include "HWComponentItem.h"
#include "HWConnection.h"
#include "AdHocConnectionItem.h"

#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/ComponentParameterFinder.h>

#include <editors/common/Association/Association.h>
#include <editors/common/StickyNote/StickyNote.h>
#include <editors/common/Association/AssociationRemoveCommand.h>

#include <editors/HWDesign/HierarchicalBusInterfaceItem.h>
#include <editors/HWDesign/ActiveBusInterfaceItem.h>
#include <editors/HWDesign/HierarchicalPortItem.h>
#include <editors/HWDesign/ActivePortItem.h>
#include <editors/HWDesign/HWComponentItem.h>

#include <editors/HWDesign/undoCommands/ColumnDeleteCommand.h>
#include <editors/HWDesign/undoCommands/ComponentDeleteCommand.h>
#include <editors/HWDesign/undoCommands/ConnectionDeleteCommand.h>
#include <editors/HWDesign/undoCommands/InterfaceDeleteCommand.h>
#include <editors/HWDesign/undoCommands/PortDeleteCommand.h>
#include <editors/HWDesign/undoCommands/AdHocConnectionDeleteCommand.h>
#include <editors/HWDesign/undoCommands/AdHocVisibilityChangeCommand.h>

#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/GraphicsColumnConstants.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/GenericEditProvider.h>

#include <library/LibraryInterface.h>

#include <common/NameGenerationPolicy.h>

#include "columnview/ColumnEditDialog.h"
#include "columnview/HWColumn.h"

#include <IPXACTmodels/common/TagData.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/View.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::HWDesignWidget()
//-----------------------------------------------------------------------------
HWDesignWidget::HWDesignWidget(LibraryInterface *lh,
    QSharedPointer<MultipleParameterFinder> designAndIsntancesParameterFinder,
    QSharedPointer<ListParameterFinder> designParameterFinder, QWidget *parent):
DesignWidget(lh, parent),
expressionParser_()
{
	// update the supported windows 
	supportedWindows_ = (supportedWindows_ | CONFIGURATIONWINDOW | CONNECTIONWINDOW | INTERFACEWINDOW |
        INSTANCEWINDOW | ADHOCVISIBILITY_WINDOW | ADHOC_WINDOW | ADDRESS_WINDOW | DESIGNPARAMETERSWINDOW);

    setDiagram(new HWDesignDiagram(
        lh, getEditProvider(), designAndIsntancesParameterFinder, designParameterFinder, this));
    getDiagram()->setProtection(false);
    getDiagram()->setMode(MODE_SELECT);
    
    QSharedPointer<ComponentParameterFinder> parameterFinder (new ComponentParameterFinder(getEditedComponent()));
    expressionParser_ = QSharedPointer<IPXactSystemVerilogParser>(new IPXactSystemVerilogParser(parameterFinder));
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::~HWDesignWidget()
//-----------------------------------------------------------------------------
HWDesignWidget::~HWDesignWidget()
{
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::setDesign()
//-----------------------------------------------------------------------------
bool HWDesignWidget::setDesign(VLNV const& vlnv, QString const& viewName)
{	
	disconnect(getDiagram(), SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

	// if vlnv and view name was defined which means that an existing component is opened
	if (vlnv.isValid() && !viewName.isEmpty())
    {
		// if vlnv is writeSucceeded and the type is component
		if (vlnv.isValid() && vlnv.getType() == VLNV::COMPONENT) {

			// create model 
			QSharedPointer<Document> libComponent = getLibraryInterface()->getModel(vlnv);
            QSharedPointer<Component> comp = libComponent.staticCast<Component>();

			if (comp == 0 || !setDesign(comp, viewName))
            {
				return false;
            }
		}
	}
	// if vlnv was writeSucceeded but view is empty then should create a new design for the component
	else if (vlnv.isValid() && viewName.isEmpty())
    {
		Q_ASSERT(getLibraryInterface()->contains(vlnv));
		Q_ASSERT(getLibraryInterface()->getDocumentType(vlnv) == VLNV::COMPONENT);

		QSharedPointer<Document> libComp = getLibraryInterface()->getModel(vlnv);
		QSharedPointer<Component> comp = libComp.staticCast<Component>();

		// get the directory path where the component's xml file is located
		const QString xmlPath = getLibraryInterface()->getPath(vlnv);
		QFileInfo xmlInfo(xmlPath);
		const QString dirPath = xmlInfo.absolutePath();

		createDesignForComponent(comp, dirPath);
	}
	// if vlnv was not defined (a new hierarchical component is created)
	else
    {
		if (!createEmptyDesign(vlnv))
        {
            return false;
        }
	}

	// disable the save at startup
	connect(getDiagram(), SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(getDiagram(), SIGNAL(modeChanged(DrawMode)), this, SIGNAL(modeChanged(DrawMode)), Qt::UniqueConnection);

	setModified(false);
	
	setDocumentType("HW Design");
	setDocumentName(QString("%1 (%2)").arg(getIdentifyingVLNV().getName()).arg(getIdentifyingVLNV().getVersion()));

	emit clearItemSelection();

    return true;
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::setDesign()
//-----------------------------------------------------------------------------
bool HWDesignWidget::setDesign(QSharedPointer<Component> component, QString const& viewName)
{
    QSharedPointer<View> openView(0);
    foreach (QSharedPointer<View> view, *component->getViews())
    {
        if (view->name() == viewName)
        {
            openView = view;
            break;
        }
    }

    if (!openView || !openView->isHierarchical())
    {
        return false;
    }

    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> designConfiguration;
    
    if (!openView->getDesignConfigurationInstantiationRef().isEmpty())
    {
        VLNV configurationVLNV;

        foreach (QSharedPointer<DesignConfigurationInstantiation> configuration, 
            *component->getDesignConfigurationInstantiations())
        {
            if (configuration->name() == openView->getDesignConfigurationInstantiationRef())
            {
                configurationVLNV = *configuration->getDesignConfigurationReference();
            }
        }

        designConfiguration = getLibraryInterface()->getModel(configurationVLNV).dynamicCast<DesignConfiguration>();       
        if (designConfiguration)
        {
            design = getLibraryInterface()->getModel(designConfiguration->getDesignRef()).dynamicCast<Design>();
        }

        if (!design)
        {
            emit errorMessage(tr("Component %1 did not contain a hierarchical view").arg(component->getVlnv().getName()));
            return false;
        }
    }
    else if (!openView->getDesignInstantiationRef().isEmpty())
    {
        VLNV designVLNV;

        foreach (QSharedPointer<DesignInstantiation> designInstantiation, *component->getDesignInstantiations())
        {
            if (designInstantiation->name() == openView->getDesignInstantiationRef())
            {
                designVLNV = *designInstantiation->getDesignReference();
            }
        }

        design = getLibraryInterface()->getModel(designVLNV).dynamicCast<Design>();
        if (!design)
        {
            emit errorMessage(tr("VLNV %1 was not found in library.").arg(designVLNV.toString()));
            return false;
        }
    }

    if (!getDiagram()->setDesign(component, viewName, design, designConfiguration))
    {
        return false;
    }

    DesignWidget::setDesign(component, viewName);
	setDocumentName(QString("%1 (%2)").arg(getIdentifyingVLNV().getName()).arg(getIdentifyingVLNV().getVersion()));

    return true;
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::saveAs()
//-----------------------------------------------------------------------------
bool HWDesignWidget::saveAs()
{
	VLNV oldVLNV = getEditedComponent()->getVlnv();

    // Ask the user for a new VLNV along with attributes and directory.
    KactusAttribute::ProductHierarchy prodHier = getEditedComponent()->getHierarchy();
    KactusAttribute::Firmness firmness = getEditedComponent()->getFirmness();
    QVector<TagData> tags = getEditedComponent()->getTags();

    VLNV vlnv;

    QString directory;
    if (!NewObjectDialog::saveAsDialog(
        this, getLibraryInterface(), oldVLNV, prodHier, firmness, tags, vlnv, directory))
    {
        return false;
    }

	// create the vlnv for design and design configuration
	VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());
	VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());

    // create the design
    QSharedPointer<Design> design;
	QSharedPointer<Component> oldComponent = getEditedComponent();

	// make a copy of the hierarchical component
    QSharedPointer<Component> topComponent(new Component(*oldComponent));
	setEditedComponent(topComponent);

	// set the new vlnv for the component
	topComponent->setVlnv(vlnv);

    QSharedPointer<View> openView(0);
    foreach (QSharedPointer<View> view, *topComponent->getViews())
    {
        if (view->name() == getOpenViewName())
        {
            openView = view;
            break;
        }
    }

	QSharedPointer<DesignConfiguration> designConf = getDiagram()->getDesignConfiguration();

	// if design configuration is used
	if (designConf)
    {
		// make a copy of the design configuration
		designConf = QSharedPointer<DesignConfiguration>(new DesignConfiguration(*designConf));
		designConf->setVlnv(desConfVLNV);
		designConf->setDesignRef(designVLNV);

        foreach (QSharedPointer<DesignConfigurationInstantiation> configuration, 
            *topComponent->getDesignConfigurationInstantiations())
        {
            if (configuration->name() == openView->getDesignConfigurationInstantiationRef())
            {
                configuration->setDesignConfigurationReference(QSharedPointer<ConfigurableVLNVReference>(
                    new ConfigurableVLNVReference(desConfVLNV)));
            }
        }

		design = getDiagram()->getDesign();
	}
	// if component does not use design configuration then it references directly to design
	else
    {
        foreach (QSharedPointer<DesignInstantiation> designInstantiation, *topComponent->getDesignInstantiations())
        {
            if (designInstantiation->name() == openView->getDesignInstantiationRef())
            {
                designInstantiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
                    new ConfigurableVLNVReference(designVLNV)));
            }
        }

		design = getDiagram()->getDesign();
	}

	if (design == 0)
	{
		return false;
	}

    design = QSharedPointer<Design>(new Design(*design));
    design->setVlnv(designVLNV);

	// update the hierarchical bus interfaces of the top-component
	getDiagram()->updateHierComponent();

	// get the paths to the original xml file
	QFileInfo sourceInfo(getLibraryInterface()->getPath(oldComponent->getVlnv()));
	QString sourcePath = sourceInfo.absolutePath();

	// update the file paths and copy necessary files
	updateFiles(topComponent, sourcePath, directory);

	// create the files for the documents

	bool writeSucceeded = true;

	getLibraryInterface()->beginSave();

	// if design configuration is used then write it.
	if (designConf && !getLibraryInterface()->writeModelToFile(directory, designConf))
    {
	    writeSucceeded = false;
	}

	if (!getLibraryInterface()->writeModelToFile(directory, design))
    {
		writeSucceeded = false;
	}

	if (!getLibraryInterface()->writeModelToFile(directory, topComponent))
    {
		writeSucceeded = false;
	}

	getLibraryInterface()->endSave();

	if (writeSucceeded)
    {
		setDocumentName(getEditedComponent()->getVlnv().getName() + " (" + 
            getEditedComponent()->getVlnv().getVersion() + ")");
		return TabDocument::saveAs();
	}
	else
    {
		emit errorMessage(tr("Error saving design to disk."));
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::keyPressEvent()
//-----------------------------------------------------------------------------
void HWDesignWidget::keyPressEvent(QKeyEvent *event)
{
    // Handle delete events if the document is not protected.
    if (!isProtected() && event->key() == Qt::Key_Delete)
    {
        onDeleteSelectedItems();
    }
    else
    {
        TabDocument::keyPressEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::onDeleteSelectedItems()
//-----------------------------------------------------------------------------
void HWDesignWidget::onDeleteSelectedItems()
{
    QList<QGraphicsItem*> selectedItems = getDiagram()->selectedItems();

    if (selectedItems.empty())
    {
        return;
    }

    int type = getDiagram()->getCommonItemType(selectedItems);
    if (type == HWComponentItem::Type)
    {
        deleteSelectedComponentItems(selectedItems);
    }
    else if (type == HierarchicalBusInterfaceItem::Type)
    {
        deleteSelectedBusInterfaceItems(selectedItems);
    }
    else if (type == ActiveBusInterfaceItem::Type)
    {
        deleteSelectedBusPortItems(selectedItems);
    }
    else if (type == HWConnection::Type)
    {
        deleteSelectedHWConnectionItems(selectedItems);
    }
    else if (type == AdHocConnectionItem::Type)
    {
        deleteSelectedAdHocConnectionItems(selectedItems);
    }
    else if (type == HWColumn::Type)
    {
        deleteSelectedHWColumns(selectedItems);
    }
    else if (type == HierarchicalPortItem::Type)
    {
        deleteSelectedAdhocInterfaces(selectedItems);
    }
    else if (type == ActivePortItem::Type)
    {
        deleteSelectedAdHocPorts(selectedItems);
    }
    else if (type == StickyNote::Type)
    {
        removeSelectedNotes();
    }
    else if (type == Association::Type)
    {
        removeSelectedAssociations();
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::deleteSelectedComponentItems()
//-----------------------------------------------------------------------------
void HWDesignWidget::deleteSelectedComponentItems(QList<QGraphicsItem*> selectedItems)
{
    getDiagram()->clearSelection();

    QSharedPointer<QUndoCommand> deleteCommand(new QUndoCommand());

    foreach (QGraphicsItem* selected, selectedItems)
    {
        HWComponentItem* component = static_cast<HWComponentItem*>(selected);
        getDiagram()->clearSelection();

        ComponentDeleteCommand* componentDeleteCommand = new ComponentDeleteCommand(
            getDiagram(), getDiagram()->getLayout()->findColumnAt(component->scenePos()),
            component, deleteCommand.data());

        connect(componentDeleteCommand, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
        connect(componentDeleteCommand, SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);

        componentDeleteCommand->redo();

        foreach(Association* association, component->getAssociations())
        {
            QUndoCommand* associationDeleteCommand = new AssociationRemoveCommand(association,
                getDiagram(), componentDeleteCommand);
            associationDeleteCommand->redo();
        }
    }

    getEditProvider()->addCommand(deleteCommand);
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::deleteSelectedBusInterfaceItems()
//-----------------------------------------------------------------------------
void HWDesignWidget::deleteSelectedBusInterfaceItems(QList<QGraphicsItem*> selectedItems)
{
    // Enumerate all ports that are part of the selected bus interfaces.
    QStringList ports;

    foreach (QGraphicsItem* selected, selectedItems)
    {
        HierarchicalBusInterfaceItem* diagIf = static_cast<HierarchicalBusInterfaceItem*>(selected);

        foreach (QString portName, diagIf->getBusInterface()->getAllMappedPhysicalPorts())
        {
            if (!ports.contains(portName))
            {
                ports.append(portName);
            }
        }
    }

    // Ask confirmation for port deletion from the user if there were ports in any of the bus interfaces.
    bool removePorts = false;

    if (!ports.isEmpty())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("Do you want to delete also the ports that are part of the interfaces?"),
            QMessageBox::Yes | QMessageBox::No, this);

        QStringList textList("Interface ports:");

        foreach(QString port, ports)
        {
            textList.append("* " + port);
        }

        msgBox.setDetailedText(textList.join("\n"));
        removePorts = (msgBox.exec() == QMessageBox::Yes);
    }

    // Delete the interfaces.
    getDiagram()->clearSelection();
    QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

    foreach (QGraphicsItem* selected, selectedItems)
    {
        HierarchicalBusInterfaceItem* diagIf = static_cast<HierarchicalBusInterfaceItem*>(selected);

        InterfaceDeleteCommand* childCmd =
            new InterfaceDeleteCommand(getDiagram(), diagIf, removePorts, cmd.data());
        connect(childCmd, SIGNAL(interfaceDeleted()), this, SIGNAL(clearItemSelection()), Qt::UniqueConnection);            

        childCmd->redo();
    }

    getEditProvider()->addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::deleteSelectedBusPortItems()
//-----------------------------------------------------------------------------
void HWDesignWidget::deleteSelectedBusPortItems(QList<QGraphicsItem*> selectedItems)
{
    getDiagram()->clearSelection();
    QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

    foreach (QGraphicsItem* selected, selectedItems)
    {
        ActiveBusInterfaceItem* port = static_cast<ActiveBusInterfaceItem*>(selected);

        // Ports can be removed only if they are temporary.
        if (port->isTemporary())
        {
            // Delete the port.
            QUndoCommand* childCmd = new PortDeleteCommand(getDiagram(), port, cmd.data());
            childCmd->redo();
        }
    }

    getEditProvider()->addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::deleteSelectedHWConnectionItems()
//-----------------------------------------------------------------------------
void HWDesignWidget::deleteSelectedHWConnectionItems(QList<QGraphicsItem*> selectedItems)
{
    getDiagram()->clearSelection();
    QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

    foreach (QGraphicsItem* selected, selectedItems)
    {
        // Delete the interconnection.
        HWConnection* conn = static_cast<HWConnection*>(selected);
        HWConnectionEndpoint* endpoint1 = static_cast<HWConnectionEndpoint*>(conn->endpoint1());
        HWConnectionEndpoint* endpoint2 = static_cast<HWConnectionEndpoint*>(conn->endpoint2());

        QUndoCommand* childCmd = new ConnectionDeleteCommand(getDiagram(), conn, cmd.data());
        childCmd->redo();

        // If the bus ports are invalid, delete them too.
        if (endpoint1->isInvalid())
        {
            QUndoCommand* childCmd = 0;

            if (endpoint1->type() == ActiveBusInterfaceItem::Type)
            {
                childCmd = new PortDeleteCommand(getDiagram(), endpoint1, cmd.data());
            }
            else
            {
                childCmd = new InterfaceDeleteCommand(
                    getDiagram(), static_cast<HierarchicalBusInterfaceItem*>(endpoint1), false, cmd.data());
            }

            childCmd->redo();
        }

        if (endpoint2->isInvalid())
        {
            QUndoCommand* childCmd = 0;

            if (endpoint2->type() == ActiveBusInterfaceItem::Type)
            {
                childCmd = new PortDeleteCommand(getDiagram(), endpoint2, cmd.data());
            }
            else
            {
                childCmd = new InterfaceDeleteCommand(
                    getDiagram(), static_cast<HierarchicalBusInterfaceItem*>(endpoint2), false, cmd.data());
            }

            childCmd->redo();
        }
    }

    getEditProvider()->addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::deleteSelectedAdHocConnectionItems()
//-----------------------------------------------------------------------------
void HWDesignWidget::deleteSelectedAdHocConnectionItems(QList<QGraphicsItem*> selectedItems)
{
    getDiagram()->clearSelection();
    QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

    foreach (QGraphicsItem* selected, selectedItems)
    {
        AdHocConnectionItem* conn = static_cast<AdHocConnectionItem*>(selected);

        QUndoCommand* childCmd = new AdHocConnectionDeleteCommand(getDiagram(), conn, cmd.data());
        childCmd->redo();
    }

    getEditProvider()->addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::deleteSelectedHWColumns()
//-----------------------------------------------------------------------------
void HWDesignWidget::deleteSelectedHWColumns(QList<QGraphicsItem*> selectedItems)
{
    // Ask a confirmation if the user really wants to delete the entire column if it is not empty.
    foreach (QGraphicsItem* selected, selectedItems)
    {
        HWColumn* column = static_cast<HWColumn*>(selected);
        if (!column->isEmpty())
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("The columns are not empty. Do you want to delete the columns and all of their contents?"),
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
    QSharedPointer<QUndoCommand> parentCommand(new QUndoCommand());

    foreach (QGraphicsItem* selected, selectedItems)
    {
        HWColumn* column = static_cast<HWColumn*>(selected);
        QUndoCommand* columnRemoveCommand = new ColumnDeleteCommand(getDiagram(), 
            getDiagram()->getLayout().data(), column, parentCommand.data());
        columnRemoveCommand->redo();
    }

    getEditProvider()->addCommand(parentCommand);
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::deleteSelectedAdhocInterfaces()
//-----------------------------------------------------------------------------
void HWDesignWidget::deleteSelectedAdhocInterfaces(QList<QGraphicsItem*> selectedItems)
{
    QList<HierarchicalPortItem*> adhocDeleteList;

    foreach (QGraphicsItem* selected, selectedItems)
    {
        HierarchicalPortItem* adhocItem = static_cast<HierarchicalPortItem*>(selected);
        if (adhocItem && !adhocItem->adhocPortIsValid())
        {
            adhocDeleteList.append(adhocItem);
        }
    }

    if (!adhocDeleteList.isEmpty())
    {
        getDiagram()->clearSelection();

        QSharedPointer<QUndoCommand> parentCommand(new QUndoCommand());
        foreach (HierarchicalPortItem* interfaceItem, adhocDeleteList)
        {
            new AdHocVisibilityChangeCommand(getDiagram(), interfaceItem->name(), false, parentCommand.data());
        }

        parentCommand->redo();

        getEditProvider()->addCommand(parentCommand);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::deleteSelectedAdHocPorts()
//-----------------------------------------------------------------------------
void HWDesignWidget::deleteSelectedAdHocPorts(QList<QGraphicsItem*> selectedItems)
{
    QList<ActivePortItem*> adhocDeleteList;
    foreach (QGraphicsItem* selected, selectedItems)
    {
        ActivePortItem* adHocItem = static_cast<ActivePortItem*>(selected);
        if (adHocItem && !adHocItem->adhocPortIsValid())
        {
            adhocDeleteList.append(adHocItem);
        }
    }

    if (!adhocDeleteList.isEmpty())
    {
        HWComponentItem* containingItem = dynamic_cast<HWComponentItem*>(adhocDeleteList.first()->parentItem());
        if (containingItem)
        {
            getDiagram()->clearSelection();

            QSharedPointer<QUndoCommand> parentCommand (new QUndoCommand());
            foreach (ActivePortItem* portItem, adhocDeleteList)
            {
                new AdHocVisibilityChangeCommand(containingItem, portItem->name(), false, parentCommand.data());
            }

            parentCommand->redo();

            getEditProvider()->addCommand(parentCommand);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::createEmptyDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Component> HWDesignWidget::createEmptyDesign(VLNV const& prevlnv)
{    
	VLNV vlnv;
	QString path;

	if (prevlnv.isValid() && getLibraryInterface()->contains(prevlnv)) 
    {
		vlnv = prevlnv;
		path = getLibraryInterface()->getPath(prevlnv);

		QFileInfo info(path);
		path = info.absolutePath();
	}
	else
    {
        NewObjectDialog newDesignDialog(getLibraryInterface(), VLNV::COMPONENT, true, parentWidget());
		newDesignDialog.setVLNV(prevlnv);
		newDesignDialog.exec();

		if (newDesignDialog.result() == QDialog::Rejected)
        {
			return QSharedPointer<Component>();
        }

		vlnv = newDesignDialog.getVLNV();
		path = newDesignDialog.getPath();
	}

	QSharedPointer<Component> newComponent;
	
	if (getLibraryInterface()->contains(prevlnv))
    {
		// find the component
		QSharedPointer<Document> libComp = getLibraryInterface()->getModel(prevlnv);
		newComponent = libComp.staticCast<Component>();

		Q_ASSERT_X(newComponent, "HWDesignWidget::createEmptyDesign",
			"The selected library item has to be component");
	}
	else 
    {
		// create the component 
		newComponent = QSharedPointer<Component>(new Component(vlnv));
	}

    getLibraryInterface()->writeModelToFile(path, newComponent);

	createDesignForComponent(newComponent, path);

    return newComponent;
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::createDesignForComponent()
//-----------------------------------------------------------------------------
void HWDesignWidget::createDesignForComponent(QSharedPointer<Component> component, const QString& dirPath)
{
	VLNV vlnv = component->getVlnv();

	// create a unique vlnv for the design
	VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());

	int runningNumber = 1;
	QString version = designVLNV.getVersion();

	// if vlnv is reserved then add "(<number>)" to end of version field
	while (getLibraryInterface()->contains(designVLNV))
    {
		++runningNumber;
		designVLNV.setVersion(version + "(" + QString::number(runningNumber) + ")");
	}

	// create a unique vlnv for the design configuration
	VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());

	runningNumber = 1;
	version = desConfVLNV.getVersion();

	// if vlnv is reserved then add "(<number>)" to end of version field
	while (getLibraryInterface()->contains(desConfVLNV))
    {
		++runningNumber;
		desConfVLNV.setVersion(version + "(" + QString::number(runningNumber) + ")");
	}

	// the name of the view to create
	QString viewName = NameGenerationPolicy::hierarchicalViewName();
    QString configurationName = NameGenerationPolicy::designConfigurationInstantiationName(viewName);

	// and a hierarchical view for it
	QSharedPointer<Model> model = component->getModel();
	Q_ASSERT(model);

	QSharedPointer<View> hierView(new View(viewName));
    hierView->setDesignConfigurationInstantiationRef(configurationName);
    
    QSharedPointer<DesignConfigurationInstantiation> configuration(new DesignConfigurationInstantiation());
    configuration->setName(configurationName);
    configuration->setDesignConfigurationReference(
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(desConfVLNV)));

	model->getViews()->append(hierView);

	// create the design configuration
	QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
	designConf->setDesignRef(designVLNV);

	QSharedPointer<Design> newDesign = QSharedPointer<Design>(new Design(designVLNV));

	getLibraryInterface()->writeModelToFile(dirPath, newDesign);
	getLibraryInterface()->writeModelToFile(dirPath, designConf);
    getLibraryInterface()->writeModelToFile(component);

	setDesign(component, viewName);
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::updateFiles()
//-----------------------------------------------------------------------------
void HWDesignWidget::updateFiles(QSharedPointer<Component> topComponent, QString const& sourcePath,
    QString const& targetPath) const
{
    QDir sourceDirectory(sourcePath);

    foreach (QSharedPointer<FileSet> fileSet, *topComponent->getFileSets())
    {
        foreach (QSharedPointer<File> componentFile, *fileSet->getFiles())
        {
            QString filePath = componentFile->name();            
            QString absoluteSource = sourceDirectory.absoluteFilePath(filePath);

            // If file is located under the source directory.
            if (!filePath.contains(QLatin1String("../")))
            {
                QDir targetDirectory(targetPath);
                QString absoluteTarget = targetDirectory.absoluteFilePath(filePath);

                QFileInfo targetInfo(absoluteTarget);

                targetDirectory.mkpath(targetInfo.absolutePath());
                QFile::copy(absoluteSource, absoluteTarget);

            }
            // if file is higher in directory hierarchy than the source directory.
            else
            {
                componentFile->setName(absoluteSource);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::addColumn()
//-----------------------------------------------------------------------------
void HWDesignWidget::addColumn()
{
    ColumnEditDialog dialog(this, false);

    if (dialog.exec() == QDialog::Accepted)
    {
        int columnWidth = GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH;

        if (dialog.getContentType() == ColumnTypes::IO)
        {
            columnWidth = GraphicsColumnConstants::IO_COLUMN_WIDTH;
        }

        QSharedPointer<ColumnDesc> desc(new ColumnDesc(dialog.name(), dialog.getContentType(), 
            dialog.getAllowedItems(), columnWidth));

        getDiagram()->addColumn(desc);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignWidget::getSupportedDrawModes()
//-----------------------------------------------------------------------------
unsigned int HWDesignWidget::getSupportedDrawModes() const
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
// Function: HWDesignWidget::getImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation HWDesignWidget::getImplementation() const
{
    return KactusAttribute::HW;
}
