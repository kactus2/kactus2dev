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
#include "SWCompItem.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/GenericEditProvider.h>

#include <designwidget/columnview/ColumnEditDialog.h>

#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/LibraryUtils.h>

#include <models/component.h>
#include <models/design.h>
#include <models/designconfiguration.h>

#include <mainwindow/mainwindow.h>

#include <QScrollBar>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget()
//-----------------------------------------------------------------------------
SystemDesignWidget::SystemDesignWidget(bool onlySW, LibraryInterface* lh, MainWindow* mainWnd, QWidget* parent)
    : DesignWidget(lh, parent),
      onlySW_(onlySW)
{
    supportedWindows_ = (supportedWindows_ | INSTANCEWINDOW | INTERFACEWINDOW | CONNECTIONWINDOW | CONFIGURATIONWINDOW);

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

    setDocumentName(system->getVlnv()->getName() + " (" + system->getVlnv()->getVersion() + ")");

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

    if (onlySW_)
    {
        SWView* view = comp->findSWView(viewName);

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
        updateSystemDesignV2(getLibraryInterface(), comp->getHierRef(""), *design, designConf);
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
        return (MODE_SELECT | MODE_CONNECT | MODE_DRAFT);
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
        else if (selected->type() == SWCompItem::Type)
        {
            SWCompItem* component = static_cast<SWCompItem*>(selected);

            // Only non-imported SW component instances can be deleted.
            if (!component->isImported())
            {
                getDiagram()->removeInstanceName(component->name());
                getDiagram()->clearSelection();

                QSharedPointer<SystemItemDeleteCommand> cmd(new SystemItemDeleteCommand(component));

                connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
                        this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
                connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
                        this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);

                getGenericEditProvider()->addCommand(cmd);
                emit clearItemSelection();
            }
        }
        else if (selected->type() == GraphicsConnection::Type)
        {
            emit clearItemSelection();

            // Delete the connection.
            QSharedPointer<QUndoCommand> cmd(new SWConnectionDeleteCommand(static_cast<GraphicsConnection*>(selected)));
            getGenericEditProvider()->addCommand(cmd);
        }

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void SystemDesignWidget::addColumn()
{
    if (onlySW_)
    {
        ColumnEditDialog dlg(this, true);

        if (dlg.exec() == QDialog::Accepted)
        {
            if (dlg.getContentType() == COLUMN_CONTENT_IO)
            {
                ColumnDesc desc(dlg.getName(), dlg.getContentType(), 0, IO_COLUMN_WIDTH);
                getDiagram()->addColumn(desc);
            }
            else
            {
                ColumnDesc desc(dlg.getName(), dlg.getContentType(), 0, SW_COLUMN_WIDTH);
                getDiagram()->addColumn(desc);
            }
        }
    }
    else
    {
        getDiagram()->addColumn(ColumnDesc("SW Components", COLUMN_CONTENT_COMPONENTS, 0, SYSTEM_COLUMN_WIDTH));
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
