//-----------------------------------------------------------------------------
// File: SystemDesignWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.4.2012
//
// Description:
// System design widget.
//-----------------------------------------------------------------------------

#include "SystemDesignWidget.h"

#include "SystemDeleteCommands.h"

#include "SystemColumn.h"
#include "SystemDesignDiagram.h"
#include "SWComponentItem.h"
#include "SWInterfaceItem.h"
#include "HWMappingItem.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/GenericEditProvider.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <designwidget/columnview/ColumnEditDialog.h>

#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/LibraryUtils.h>

#include <models/component.h>
#include <models/design.h>
#include <models/designconfiguration.h>
#include <models/SystemView.h>

#include <mainwindow/mainwindow.h>

#include <QScrollBar>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileInfo>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget()
//-----------------------------------------------------------------------------
SystemDesignWidget::SystemDesignWidget(bool onlySW, LibraryInterface* lh, MainWindow* mainWnd, QWidget* parent)
    : DesignWidget(lh, parent),
      onlySW_(onlySW)
{
    supportedWindows_ |= INSTANCEWINDOW | INTERFACEWINDOW | CONNECTIONWINDOW | CONFIGURATIONWINDOW;

    if (!onlySW_)
    {
        supportedWindows_ |= SYSTEM_DETAILS_WINDOW;
    }

    setDiagram(new SystemDesignDiagram(onlySW, lh, mainWnd, *getGenericEditProvider(), this));
}

//-----------------------------------------------------------------------------
// Function: ~SystemDesignWidget()
//-----------------------------------------------------------------------------
SystemDesignWidget::~SystemDesignWidget()
{
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

    if (system == 0)
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

/*    setDocumentName(system->getVlnv()->getName() + " (" + system->getVlnv()->getVersion() + ")");*/
	setDocumentName(QString("%1 (%2)").arg(getIdentifyingVLNV().getName()).arg(getIdentifyingVLNV().getVersion()));
    if (onlySW_)
    {
        setDocumentType("SW Design");
    }
    else
    {
        setDocumentType("System Design");
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
        QSharedPointer<SWView> view = comp->findSWView(viewName);

        if (!view)
        {
            return false;
        }

        designVLNV = comp->getHierSWRef(viewName);
    }
    else
    {
        SystemView* view = comp->findSystemView(viewName);

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
        emit errorMessage(tr("Component %1 did not contain a view").arg(comp->getVlnv()->getName()));
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
            emit errorMessage(tr("Component %1 did not contain a view").arg(
                comp->getVlnv()->getName()));
            return false;
        }
    }

    if (!onlySW_)
    {
        // Update the design.
        updateSystemDesignV2(getLibraryInterface(), comp->getHierRef(hwViewRef), *design, designConf);
    }

    if (!getDiagram()->setDesign(comp, design, designConf))
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
    if (getView()->isInteractive())
    {
        return (MODE_SELECT | MODE_CONNECT | MODE_DRAFT | MODE_TOGGLE_OFFPAGE);
    }
    else
    {
        return MODE_SELECT;
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
        if (getDiagram()->selectedItems().empty())
        {
            return;
        }

        QGraphicsItem* selected = getDiagram()->selectedItems().first();

        if (selected->type() == SystemColumn::Type)
        {
            // Ask a confirmation if the user really wants to delete the entire column if it is not empty.
            SystemColumn* column = static_cast<SystemColumn*>(selected);

            bool del = true;

            if (!column->isEmpty())
            {
                // Column cannot be deleted if it contains HW mapping items.
                foreach (QGraphicsItem* childItem, column->getItems())
                {
                    if (childItem->type() == HWMappingItem::Type)
                    {
                        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                                           tr("The column cannot be removed because it contains underlying HW. "
                                              "Move underlying HW components to another column before deletion."),
                                           QMessageBox::Ok, this);
                        msgBox.exec();
                        return;
                    }
                }

                QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                                   tr("The column is not empty. Do you want to "
                                      "delete the column and all of its contents?"),
                                   QMessageBox::Yes | QMessageBox::No, this);

                if (msgBox.exec() == QMessageBox::No)
                {
                    del = false;
                }
            }

            // Delete the column if requested.
            if (!del)
            {
                return;
            }

            QSharedPointer<QUndoCommand> cmd(new SystemColumnDeleteCommand(getDiagram()->getColumnLayout(),
                                                                           column));
            getGenericEditProvider()->addCommand(cmd);
            emit clearItemSelection();
        }
        else if (selected->type() == SWComponentItem::Type)
        {
            SWComponentItem* component = static_cast<SWComponentItem*>(selected);

            // Only non-imported SW component instances can be deleted.
            if (!component->isImported())
            {
                getDiagram()->removeInstanceName(component->name());
                getDiagram()->clearSelection();

                QSharedPointer<SystemComponentDeleteCommand> cmd(new SystemComponentDeleteCommand(component));

                connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
                        this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
                connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
                        this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);

                getGenericEditProvider()->addCommand(cmd);
                emit clearItemSelection();
            }
        }
        else if (selected->type() == SWPortItem::Type)
        {
            SWPortItem* port = static_cast<SWPortItem*>(selected);
            ComponentItem* comp = port->encompassingComp();

            // Ports can be removed only if they are temporary.
            if (port->isTemporary())
            {
                // Delete the port.
                QSharedPointer<QUndoCommand> cmd(new SWPortDeleteCommand(port));
                getGenericEditProvider()->addCommand(cmd);

                // Clear the item selection.
                emit clearItemSelection();
            }
        }
        else if (selected->type() == SWInterfaceItem::Type)
        {
            SWInterfaceItem* interface = static_cast<SWInterfaceItem*>(selected);
            getDiagram()->clearSelection();
            emit clearItemSelection();

            QSharedPointer<QUndoCommand> cmd(new SWInterfaceDeleteCommand(interface));
            getGenericEditProvider()->addCommand(cmd);
        }
        else if (selected->type() == GraphicsConnection::Type)
        {
            emit clearItemSelection();

            // Delete the connection.
            GraphicsConnection* conn = static_cast<GraphicsConnection*>(selected);
            SWConnectionEndpoint* endpoint1 = static_cast<SWConnectionEndpoint*>(conn->endpoint1());
            SWConnectionEndpoint* endpoint2 = static_cast<SWConnectionEndpoint*>(conn->endpoint2());

            QSharedPointer<QUndoCommand> cmd(new SWConnectionDeleteCommand(conn));
            getGenericEditProvider()->addCommand(cmd);

            // If the bus ports are invalid, delete them too.
            if (endpoint1->isInvalid())
            {
                QUndoCommand* childCmd = 0;

                if (endpoint1->type() == SWPortItem::Type)
                {
                    childCmd = new SWPortDeleteCommand(static_cast<SWPortItem*>(endpoint1), cmd.data());
                }
                else
                {
                    childCmd = new SWInterfaceDeleteCommand(static_cast<SWInterfaceItem*>(endpoint1), cmd.data());
                }

                childCmd->redo();
            }

            if (endpoint2->isInvalid())
            {
                QUndoCommand* childCmd = 0;

                if (endpoint2->type() == SWPortItem::Type)
                {
                    childCmd = new SWPortDeleteCommand(static_cast<SWPortItem*>(endpoint2), cmd.data());
                }
                else
                {
                    childCmd = new SWInterfaceDeleteCommand(static_cast<SWInterfaceItem*>(endpoint2), cmd.data());
                }

                childCmd->redo();
            }
        }
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
            if (dialog.getContentType() == COLUMN_CONTENT_IO)
            {
                ColumnDesc desc(dialog.getName(), dialog.getContentType(), 0, SystemDesignDiagram::IO_COLUMN_WIDTH);
                getDiagram()->addColumn(desc);
            }
            else
            {
                ColumnDesc desc(dialog.getName(), dialog.getContentType(), 0, SystemDesignDiagram::SW_COLUMN_WIDTH);
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
            getDiagram()->addColumn(ColumnDesc(dialog.getName(), COLUMN_CONTENT_COMPONENTS, 0,
                                               SystemDesignDiagram::SYSTEM_COLUMN_WIDTH));
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
        return KactusAttribute::KTS_SW;
    }
    else
    {
        return KactusAttribute::KTS_SYS;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget::saveAs()
//-----------------------------------------------------------------------------
bool SystemDesignWidget::saveAs()
{
    VLNV oldVLNV = *getEditedComponent()->getVlnv();

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

    // If design configuration is used.
    if (designConf)
    {
        // Make a copy of the design configuration with the new VLNVs.
        designConf = QSharedPointer<DesignConfiguration>(new DesignConfiguration(*designConf));
        designConf->setVlnv(desConfVLNV);
        designConf->setDesignRef(designVLNV);

        getEditedComponent()->setHierRef(desConfVLNV, getOpenViewName());

        // Create design with new design vlnv.
        design = getDiagram()->createDesign(designVLNV);
    }
    // If component does not use design configuration then it references directly to design.
    else
    {
        // Set component to reference new design.
        getEditedComponent()->setHierRef(designVLNV, getOpenViewName());
        design = getDiagram()->createDesign(designVLNV);
    }

    if (design == 0)
    {
        return false;
    }

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
