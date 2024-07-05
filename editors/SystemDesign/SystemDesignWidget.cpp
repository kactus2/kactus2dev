//-----------------------------------------------------------------------------
// File: SystemDesignWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 23.4.2012
//
// Description:
// System design widget.
//-----------------------------------------------------------------------------

#include "SystemDesignWidget.h"

#include "SystemColumn.h"
#include "SystemDesignDiagram.h"
#include "SWComponentItem.h"
#include "SWInterfaceItem.h"
#include "HWMappingItem.h"

#include <common/GenericEditProvider.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnConstants.h>

#include <editors/HWDesign/columnview/ColumnEditDialog.h>
#include <editors/common/Association/Association.h>
#include <editors/common/StickyNote/StickyNote.h>
#include <editors/common/Association/AssociationRemoveCommand.h>
#include <editors/SystemDesign/ComGraphicsConnection.h>
#include <editors/SystemDesign/UndoCommands/ComConnectionDeleteCommand.h>
#include <editors/SystemDesign/UndoCommands/SWPortDeleteCommand.h>
#include <editors/SystemDesign/UndoCommands/SWInterfaceDeleteCommand.h>
#include <editors/SystemDesign/ApiGraphicsConnection.h>
#include <editors/SystemDesign/UndoCommands/ApiConnectionDeleteCommand.h>
#include <editors/SystemDesign/UndoCommands/SystemMoveCommands.h>
#include <editors/SystemDesign/UndoCommands/SystemDeleteCommands.h>
#include <editors/SystemDesign/UndoCommands/SystemComponentDeleteCommand.h>

#include <KactusAPI/include/LibraryInterface.h>
#include <library/LibraryUtils.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <IPXACTmodels/kactusExtensions/SystemView.h>

#include <QKeyEvent>
#include <QMessageBox>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget()
//-----------------------------------------------------------------------------
SystemDesignWidget::SystemDesignWidget(bool onlySW, LibraryInterface* lh, QWidget* parent):
DesignWidget(lh, parent),
onlySW_(onlySW)
{
    supportedWindows_ |= INSTANCEWINDOW | INTERFACEWINDOW | CONNECTIONWINDOW | CONFIGURATIONWINDOW;

    if (!onlySW_)
    {
        supportedWindows_ |= SYSTEM_DETAILS_WINDOW;
    }

    setDiagram(new SystemDesignDiagram(onlySW, lh, getEditProvider(), this));
}


//-----------------------------------------------------------------------------
// Function: setDesign()
//-----------------------------------------------------------------------------
bool SystemDesignWidget::setDesign(VLNV const& vlnv, QString const& viewName)
{
    disconnect(getDiagram(), SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    // Check if the vlnv is not valid.
    if (!vlnv.isValid() || vlnv.getType() != VLNV::COMPONENT)
    {
        return false;
    }

    // Retrieve the model.
    QSharedPointer<Component> system = getLibraryInterface()->getModel(vlnv).staticCast<Component>();

    if (system == nullptr)
    {
        return false;
    }

    // Open the design to the diagram.
    if (!setDesign(system, viewName))
    {
        return false;
    }

    connect(getDiagram(), SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(getDiagram(), SIGNAL(modeChanged(DrawMode)),
        this, SIGNAL(modeChanged(DrawMode)), Qt::UniqueConnection);

	setDocumentName(QString("%1 (%2)").arg(getIdentifyingVLNV().getName()).arg(getIdentifyingVLNV().getVersion()));
    if (onlySW_)
    {
        setDocumentType(DocumentType(DocumentTypes::SW_DESIGN));
    }
    else
    {
        setDocumentType(DocumentType(DocumentTypes::SYSTEM_DESIGN));
    }

    // Open in unlocked mode by default only if the version is draft.
    setProtection(vlnv.getVersion() != "draft");

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::setDesign()
//-----------------------------------------------------------------------------
bool SystemDesignWidget::setDesign(QSharedPointer<Component> comp, const QString& viewName)
{
    VLNV designVLNV;
    QString hwViewRef = "";

    if (onlySW_)
    {
        QSharedPointer<View> view = comp->getModel()->findView(viewName);

        if (!view)
        {
            return false;
        }

        designVLNV = comp->getHierRef(viewName);
    }
    else
    {
		QSharedPointer<SystemView> view = comp->findSystemView(viewName);

        if (!view)
        {
            return false;
        }

        designVLNV = comp->getHierSystemRef(viewName);
        hwViewRef = view->getHWViewRef();
    }

    // Check for a valid VLNV type.
    designVLNV.setType(getLibraryInterface()->getDocumentType(designVLNV));

    if (!designVLNV.isValid())
    {
        emit errorMessage(tr("Component %1 did not contain a view").arg(comp->getVlnv().getName()));
        return false;
    }

    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> designConf;

    // if the component contains a direct reference to a design
    if (designVLNV.getType() == VLNV::DESIGN)
    {
        QSharedPointer<Document> libComp = getLibraryInterface()->getModel(designVLNV);	
        design = libComp.staticCast<Design>();
    }
    // if component had reference to a design configuration
    else if (designVLNV.getType() == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<Document> libComp = getLibraryInterface()->getModel(designVLNV);
        designConf = libComp.staticCast<DesignConfiguration>();

        designVLNV = designConf->getDesignRef();

        if (designVLNV.isValid())
        {
            QSharedPointer<Document> libComp = getLibraryInterface()->getModel(designVLNV);	
            design = libComp.staticCast<Design>();
        }

        // if design configuration did not contain a reference to a design.
        if (!design)
        {
            emit errorMessage(tr("Component %1 did not contain a view").arg(
                comp->getVlnv().getName()));
            return false;
        }
    }

    if (!onlySW_)
    {
        // Update the design.
        updateSystemDesignV2(getLibraryInterface(), comp->getHierRef(hwViewRef), *design, designConf);
    }

    if (!getDiagram()->setDesign(comp, viewName, design, designConf))
    {
        return false;
    }

    return DesignWidget::setDesign(comp, viewName);
}

//-----------------------------------------------------------------------------
// Function: getSupportedDrawModes()
//-----------------------------------------------------------------------------
unsigned int SystemDesignWidget::getSupportedDrawModes() const
{
    if (isProtected())
    {
        return MODE_SELECT;
    }
    else
    {
        return (MODE_SELECT | MODE_CONNECT | MODE_DRAFT | MODE_TOGGLE_OFFPAGE | MODE_LABEL);
    }
}

//-----------------------------------------------------------------------------
// Function: keyPressEvent()
//-----------------------------------------------------------------------------
void SystemDesignWidget::keyPressEvent(QKeyEvent* event)
{
    // If the document is protected, skip all delete events.
    if (isProtected())
    {
        return;
    }

    if (event->key() == Qt::Key_Delete)
    {
        onDeleteSelectedItems();
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget::onDeleteSelectedItems()
//-----------------------------------------------------------------------------
void SystemDesignWidget::onDeleteSelectedItems()
{
    if (getDiagram()->selectedItems().empty())
    {
        return;
    }

    QList<QGraphicsItem*> selectedItems = getDiagram()->selectedItems();
    int type = getDiagram()->getCommonItemType(selectedItems);

    if (type == SystemColumn::Type)
    {
        deleteSelectedSystemColumns(selectedItems);
    }
    else if (type == SWComponentItem::Type)
    {
        deleteSelectedSWComponentItems(selectedItems);
    }
    else if (type == SWPortItem::Type)
    {
        deleteSelectedSWPortItems(selectedItems);
    }
    else if (type == SWInterfaceItem::Type)
    {
        deleteSelectedSWInterfaceItems(selectedItems);
    }
    else if (type == ComGraphicsConnection::Type)
    {
        deleteSelectedComConnectionItems(selectedItems);
    }
    else if (type == ApiGraphicsConnection::Type)
    {
        deleteSelectedApiConnectionItems(selectedItems);
    }
    else if (type == GraphicsConnection::Type)
    {
        deleteSelectedGraphicsConnectionItems(selectedItems);
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
// Function: SystemDesignWidget::deleteSelectedSystemColumns()
//-----------------------------------------------------------------------------
void SystemDesignWidget::deleteSelectedSystemColumns(QList<QGraphicsItem*> selectedItems)
{
    // Ask a confirmation if the user really wants to delete the entire column if it is not empty.
    for (QGraphicsItem* selected : selectedItems)
    {
        SystemColumn* column = static_cast<SystemColumn*>(selected);

        if (!column->isEmpty())
        {
            // Column cannot be deleted if it contains HW mapping items.
            for (QGraphicsItem* childItem: column->getItems())
            {
                if (childItem->type() == HWMappingItem::Type)
                {
                    QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                        tr("The columns cannot be removed because they contain underlying HW. "
                        "Move underlying HW components to another column before deletion."),
                        QMessageBox::Ok, this);
                    msgBox.exec();
                    return;
                }
            }

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

    for (QGraphicsItem* selected : selectedItems)
    {
        SystemColumn* column = static_cast<SystemColumn*>(selected);
        QUndoCommand* childCmd = new SystemColumnDeleteCommand(getDiagram(), column, cmd.data());
        childCmd->redo();
    }

    getEditProvider()->addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget::deleteSelectedSWComponentItems()
//-----------------------------------------------------------------------------
void SystemDesignWidget::deleteSelectedSWComponentItems(QList<QGraphicsItem*> selectedItems)
{
    getDiagram()->clearSelection();
    QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

    for (QGraphicsItem* selected : selectedItems)
    {
        SWComponentItem* component = static_cast<SWComponentItem*>(selected);

        // Only non-imported SW component instances can be deleted.
        if (!component->isImported())
        {
            QSharedPointer<Design> containingDesign = getDiagram()->getDesign();

            SystemComponentDeleteCommand* childCmd = new SystemComponentDeleteCommand(component,
                getDiagram()->getDesign(), cmd.data());

            connect(childCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
                this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
            connect(childCmd, SIGNAL(componentInstantiated(ComponentItem*)),
                this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);

            childCmd->redo();

            for(Association* association : component->getAssociations())
            {
                QUndoCommand* associationRemoveCmd =
                    new AssociationRemoveCommand(association, getDiagram(), childCmd);
                associationRemoveCmd->redo();
            }
        }
    }

    getEditProvider()->addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget::deleteSelectedSWPortItems()
//-----------------------------------------------------------------------------
void SystemDesignWidget::deleteSelectedSWPortItems(QList<QGraphicsItem*> selectedItems)
{
    getDiagram()->clearSelection();
    QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

    for (QGraphicsItem* selected : selectedItems)
    {
        SWPortItem* port = static_cast<SWPortItem*>(selected);

        // Ports can be removed only if they are temporary.
        if (port->isTemporary())
        {
            // Delete the port.
            QUndoCommand* childCmd = new SWPortDeleteCommand(port, getDiagram()->getDesign(), cmd.data());
            childCmd->redo();
        }
    }

    getEditProvider()->addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget::deleteSelectedSWInterfaceItems()
//-----------------------------------------------------------------------------
void SystemDesignWidget::deleteSelectedSWInterfaceItems(QList<QGraphicsItem*> selectedItems)
{
    getDiagram()->clearSelection();
    QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

    for (QGraphicsItem* selected : selectedItems)
    {
        SWInterfaceItem* interface = static_cast<SWInterfaceItem*>(selected);

        QUndoCommand* childCmd = new SWInterfaceDeleteCommand(
            interface, getDiagram()->getDesign(), getDiagram()->getEditedComponent(), cmd.data());
        childCmd->redo();
    }

    getEditProvider()->addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget::deleteSelectedComConnectionItems()
//-----------------------------------------------------------------------------
void SystemDesignWidget::deleteSelectedComConnectionItems(QList<QGraphicsItem*> selectedItems)
{
    getDiagram()->clearSelection();
    QSharedPointer<QUndoCommand> undoCommand(new QUndoCommand());

    for (QGraphicsItem* selected: selectedItems)
    {
        auto comConnection = static_cast<ComGraphicsConnection*>(selected);
        auto endPoint1 = static_cast<SWConnectionEndpoint*>(comConnection->endpoint1());
        auto endPoint2 = static_cast<SWConnectionEndpoint*>(comConnection->endpoint2());

        QUndoCommand* childCommand =
            new ComConnectionDeleteCommand(comConnection, getDiagram()->getDesign(), undoCommand.data());
        childCommand->redo();

        deleteConnectedEndPoint(endPoint1, undoCommand);
        deleteConnectedEndPoint(endPoint2, undoCommand);
    }

    getEditProvider()->addCommand(undoCommand);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget::deleteSelectedApiConnectionItems()
//-----------------------------------------------------------------------------
void SystemDesignWidget::deleteSelectedApiConnectionItems(QList<QGraphicsItem*> selectedItems)
{
    getDiagram()->clearSelection();
    QSharedPointer<QUndoCommand> undoCommand(new QUndoCommand());

    for (QGraphicsItem* selected : selectedItems)
    {
        auto apiConnection = static_cast<ApiGraphicsConnection*>(selected);
        auto endPoint1 = static_cast<SWConnectionEndpoint*>(apiConnection->endpoint1());
        auto endPoint2 = static_cast<SWConnectionEndpoint*>(apiConnection->endpoint2());

        QUndoCommand* childCommand =
            new ApiConnectionDeleteCommand(apiConnection, getDiagram()->getDesign(), undoCommand.data());
        childCommand->redo();

        deleteConnectedEndPoint(endPoint1, undoCommand);
        deleteConnectedEndPoint(endPoint2, undoCommand);
    }

    getEditProvider()->addCommand(undoCommand);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget::deleteSelectedGraphicsConnectionItems()
//-----------------------------------------------------------------------------
void SystemDesignWidget::deleteSelectedGraphicsConnectionItems(QList<QGraphicsItem*> selectedItems)
{
    getDiagram()->clearSelection();
    QSharedPointer<QUndoCommand> undoCommand(new QUndoCommand());

    for (QGraphicsItem* selected : selectedItems)
    {
        auto connection = static_cast<GraphicsConnection*>(selected);
        auto endPoint1 = static_cast<SWConnectionEndpoint*>(connection->endpoint1());
        auto endPoint2 = static_cast<SWConnectionEndpoint*>(connection->endpoint2());

        QUndoCommand* childCommand = new QUndoCommand(undoCommand.data());
        childCommand->redo();

        deleteConnectedEndPoint(endPoint1, undoCommand);
        deleteConnectedEndPoint(endPoint2, undoCommand);
    }

    getEditProvider()->addCommand(undoCommand);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget::deleteConnectedEndPoint()
//-----------------------------------------------------------------------------
void SystemDesignWidget::deleteConnectedEndPoint(SWConnectionEndpoint* endPoint,
    QSharedPointer<QUndoCommand> parentCommand)
{
    if (endPoint->isInvalid())
    {
        QUndoCommand* endPointCommand = nullptr;
        if (endPoint->type() == SWPortItem::Type)
        {
            endPointCommand = new SWPortDeleteCommand(
                static_cast<SWPortItem*>(endPoint), getDiagram()->getDesign(), parentCommand.data());
        }
        else
        {
            endPointCommand = new SWInterfaceDeleteCommand(static_cast<SWInterfaceItem*>(endPoint),
                getDiagram()->getDesign(), getDiagram()->getEditedComponent(), parentCommand.data());
        }

        endPointCommand->redo();
    }
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void SystemDesignWidget::addColumn()
{
    if (onlySW_)
    {
        ColumnEditDialog dialog(this, true);

        if (dialog.exec() == QDialog::Accepted)
        {
            if (dialog.getContentType() == ColumnTypes::IO)
            {
                QSharedPointer<ColumnDesc> desc(new ColumnDesc(
                    dialog.name(), dialog.getContentType(), 0, GraphicsColumnConstants::IO_COLUMN_WIDTH));
                getDiagram()->addColumn(desc);
            }
            else
            {
                QSharedPointer<ColumnDesc> desc(new ColumnDesc(
                    dialog.name(), dialog.getContentType(), 0, GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH));
                getDiagram()->addColumn(desc);
            }
        }
    }
    else
    {
        ColumnEditDialog dialog(this, true);
        dialog.hideContentSettings();

        if (dialog.exec() == QDialog::Accepted)
        {
            getDiagram()->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(dialog.name(),
                ColumnTypes::COMPONENTS, 0, GraphicsColumnConstants::SYSTEM_COLUMN_WIDTH)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget::getImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation SystemDesignWidget::getImplementation() const
{
    if (onlySW_)
    {
        return KactusAttribute::SW;
    }
    else
    {
        return KactusAttribute::SYSTEM;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget::saveAs()
//-----------------------------------------------------------------------------
bool SystemDesignWidget::saveAs()
{
    VLNV oldVLNV = getEditedComponent()->getVlnv();

    // Ask the user for a new VLNV and directory.

    VLNV vlnv;
    QString directory;

    if (!NewObjectDialog::saveAsDialog(this, getLibraryInterface(), oldVLNV, vlnv, directory))
    {
        return false;
    }

    // create the vlnv for design and design configuration
    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
                    vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
                     vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());

    // Make a copy of the hierarchical component.
    QSharedPointer<Component> oldComponent = getEditedComponent();

    setEditedComponent(QSharedPointer<Component>(new Component(*getEditedComponent())));
    getEditedComponent()->setVlnv(vlnv);

    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> designConf = getDiagram()->getDesignConfiguration();

	// Find the open view.
	QSharedPointer<SystemView> openView;

	for (QSharedPointer<SystemView> currentView : getEditedComponent()->getSystemViews())
	{
		if ( getOpenViewName() == currentView->name() )
		{
			openView = currentView;
			break;
		}
	}

    // If design configuration is used.
    if (designConf)
    {
        // Make a copy of the design configuration with the new VLNVs.
        designConf = QSharedPointer<DesignConfiguration>(new DesignConfiguration(*designConf));
        designConf->setVlnv(desConfVLNV);
        designConf->setDesignRef(designVLNV);

		openView->setHierarchyRef( desConfVLNV );

        // Create design with new design vlnv.
        design = getDiagram()->getDesign();
        design->setVlnv(designVLNV);
    }
    // If component does not use design configuration then it references directly to design.
    else
    {
		// Set component to reference new design.
		openView->setHierarchyRef( designVLNV );
        design = getDiagram()->getDesign();
        design->setVlnv(designVLNV);
    }

    if (design == 0)
    {
        return false;
    }

    getDiagram()->updateHierComponent();

    // get the paths to the original xml file
    QFileInfo sourceInfo(getLibraryInterface()->getPath(oldComponent->getVlnv()));
    QString sourcePath = sourceInfo.absolutePath();

	// Update the file paths and copy necessary files.
	for (QSharedPointer<FileSet> fileSet : *oldComponent->getFileSets())
	{
		for (QSharedPointer<File> file: *fileSet->getFiles())
		{
			// Get the absolute path to the file.
			QDir source(sourcePath);
			QString absoluteSource = source.absoluteFilePath(file->name());

			// If file is located under the source directory.
			if (!file->name().contains(QString("../")))
			{
				QDir target(directory);
				QString absoluteTarget = target.absoluteFilePath(file->name());

				QFileInfo targetInfo(absoluteTarget);

				target.mkpath(targetInfo.absolutePath());
				QFile::copy(absoluteSource, absoluteTarget);

			}
			// If file is higher in directory hierarchy than the source directory.
			else
			{
				// Update the file name.
				fileSet->changeFileName(file->name(), absoluteSource);
			}
		}
	}


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
    if (!getLibraryInterface()->writeModelToFile(directory, getEditedComponent()))
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
// Function: SystemDesignWidget::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV SystemDesignWidget::getIdentifyingVLNV() const
{
    const QSharedPointer<DesignConfiguration> designConf = getDiagram()->getDesignConfiguration();

    if (designConf)
    {
        return designConf->getDesignRef();
    }
    else
    {
        return getEditedComponent()->getHierRef(getOpenViewName());
    }
}
