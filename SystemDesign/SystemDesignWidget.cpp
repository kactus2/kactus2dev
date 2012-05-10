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

#include "../EndpointDesign/SystemDeleteCommands.h"

#include "SystemDesignDiagram.h"
#include "SWCompItem.h"
#include "SWConnection.h"

#include <LibraryManager/libraryinterface.h>

#include <models/component.h>
#include <models/design.h>
#include <models/designconfiguration.h>

#include <mainwindow/mainwindow.h>

#include <QScrollBar>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: SystemDesignWidget()
//-----------------------------------------------------------------------------
SystemDesignWidget::SystemDesignWidget(LibraryInterface* lh, MainWindow* mainWnd, QWidget* parent)
    : TabDocument(parent, DOC_ZOOM_SUPPORT | DOC_DRAW_MODE_SUPPORT | DOC_PROTECTION_SUPPORT | DOC_EDIT_SUPPORT, 30, 300),
      lh_(lh),
      view_(0),
      diagram_(0),
      editProvider_()
{
    supportedWindows_ = (supportedWindows_ | INSTANCEWINDOW);

    editProvider_ = QSharedPointer<GenericEditProvider>(new GenericEditProvider(EDIT_HISTORY_SIZE));
    diagram_ = new SystemDesignDiagram(lh_, mainWnd, *editProvider_, this);

    connect(diagram_, SIGNAL(openComponent(const VLNV&)),
        this, SIGNAL(openComponent(const VLNV&)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(openDesign(const VLNV&, QString const&)),
        this, SIGNAL(openDesign(const VLNV&, QString const&)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(openSource(ProgramEntityItem*)),
        this, SIGNAL(openSource(ProgramEntityItem*)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(componentSelected(ComponentItem*)),
        this, SIGNAL(componentSelected(ComponentItem*)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(clearItemSelection()),
        this, SIGNAL(clearItemSelection()), Qt::UniqueConnection);

    view_ = new QGraphicsView(this);
    view_->setScene(diagram_);
    view_->centerOn(0, 0);
    view_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_);

    view_->verticalScrollBar()->setTracking(true);
    connect(view_->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onVerticalScroll(int)));
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
bool SystemDesignWidget::setDesign(VLNV const& vlnv)
{
    // Check if the vlnv is not valid.
    if (!vlnv.isValid() || vlnv.getType() != VLNV::COMPONENT)
    {
        return false;
    }

    // Retrieve the model.
    QSharedPointer<Component> system = lh_->getModel(vlnv).staticCast<Component>();

    if (system == 0)
    {
        return false;
    }

    // Check that the component is actually a system.
    KactusAttribute::Implementation implementation = system->getComponentImplementation();

    if (implementation != KactusAttribute::KTS_SYS)
    {
        return false;
    }

    // Retrieve the view and check that it is hierarchical.
    View* view = system->findView("kts_sys_ref");

    if (view == 0 || !view->isHierarchical())
    {
        return false;
    }

    // Open the design to the diagram.
    diagram_->setDesign(system, "kts_sys_ref");

    system_ = system;
    designConf_ = diagram_->getDesignConfiguration();

    connect(diagram_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(modeChanged(DrawMode)),
        this, SIGNAL(modeChanged(DrawMode)), Qt::UniqueConnection);

    setDocumentName(system_->getVlnv()->getName() + 
        " (" + system_->getVlnv()->getVersion() + ")");
    setDocumentType("System");

    // Open in unlocked mode by default only if the version is draft.
    setProtection(vlnv.getVersion() != "draft");

    return true;
}

//-----------------------------------------------------------------------------
// Function: setZoomLevel()
//-----------------------------------------------------------------------------
void SystemDesignWidget::setZoomLevel(int level)
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
void SystemDesignWidget::fitInView()
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
void SystemDesignWidget::setMode(DrawMode mode)
{
    diagram_->setMode(mode);
}

//-----------------------------------------------------------------------------
// Function: getSupportedDrawModes()
//-----------------------------------------------------------------------------
unsigned int SystemDesignWidget::getSupportedDrawModes() const
{
    if (view_->isInteractive())
    {
        return (MODE_SELECT | MODE_CONNECT | MODE_DRAFT);
    }
    else
    {
        return MODE_SELECT;
    }
}

//-----------------------------------------------------------------------------
// Function: save()
//-----------------------------------------------------------------------------
bool SystemDesignWidget::save()
{
    lh_->beginSave();
    
    // Create the design.
    QSharedPointer<Design> design;

    if (designConf_)
    {
        design = diagram_->createDesign(designConf_->getDesignRef());
    }
    else
    {
        design = diagram_->createDesign(system_->getHierRef());
    }

    if (design == 0)
    {
        lh_->endSave();
        return false;
    }

    // Write the files.
    if (designConf_)
    {
        lh_->writeModelToFile(designConf_);
    }

    lh_->writeModelToFile(design);
    lh_->writeModelToFile(system_);

    lh_->endSave();

    return TabDocument::save();
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
        if (diagram_->selectedItems().empty())
        {
            return;
        }

        QGraphicsItem* selected = diagram_->selectedItems().first();

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
            if (del)
            {
                QSharedPointer<QUndoCommand> cmd(new SystemColumnDeleteCommand(diagram_->getColumnLayout(),
                                                                               column));
                editProvider_->addCommand(cmd);
                emit clearItemSelection();
            }
        }
        else if (selected->type() == SWCompItem::Type)
        {
            SWCompItem* component = static_cast<SWCompItem*>(selected);
            diagram_->removeInstanceName(component->name());
            diagram_->clearSelection();

            QSharedPointer<SystemItemDeleteCommand> cmd(new SystemItemDeleteCommand(component));

            connect(cmd.data(), SIGNAL(componentInstanceRemoved(DiagramComponent*)),
                this, SIGNAL(componentInstanceRemoved(DiagramComponent*)), Qt::UniqueConnection);
            connect(cmd.data(), SIGNAL(componentInstantiated(DiagramComponent*)),
                this, SIGNAL(componentInstantiated(DiagramComponent*)), Qt::UniqueConnection);

            editProvider_->addCommand(cmd);

            emit clearItemSelection();
        }
        else if (selected->type() == SWConnection::Type)
        {
            // Delete the connection.
            QSharedPointer<QUndoCommand> cmd(new SWConnectionDeleteCommand(static_cast<SWConnection*>(selected)));
            editProvider_->addCommand(cmd);

            emit clearItemSelection();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: onVerticalScroll()
//-----------------------------------------------------------------------------
void SystemDesignWidget::onVerticalScroll(int y)
{
    QPointF pt(0.0, y);
    QMatrix mat = view_->matrix().inverted();
    diagram_->onVerticalScroll(mat.map(pt).y());
}

//-----------------------------------------------------------------------------
// Function: String()
//-----------------------------------------------------------------------------
QGraphicsView* SystemDesignWidget::getView()
{
    return view_;
}

//-----------------------------------------------------------------------------
// Function: getOpenDocument()
//-----------------------------------------------------------------------------
VLNV const* SystemDesignWidget::getOpenDocument() const
{
    if (system_ == 0)
    {
        return 0;
    }

    return system_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void SystemDesignWidget::setProtection(bool locked)
{
    TabDocument::setProtection(locked);
    diagram_->setProtection(locked);
    diagram_->setMode(MODE_SELECT);
}

//-----------------------------------------------------------------------------
// Function: getComponentVLNV()
//-----------------------------------------------------------------------------
VLNV SystemDesignWidget::getComponentVLNV() const
{
    return *system_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void SystemDesignWidget::addColumn()
{
    diagram_->addColumn("SW Components");
}

//-----------------------------------------------------------------------------
// Function: getEditProvider()
//-----------------------------------------------------------------------------
IEditProvider* SystemDesignWidget::getEditProvider()
{
    return editProvider_.data();
}

//-----------------------------------------------------------------------------
// Function: getGenericEditProvider()
//-----------------------------------------------------------------------------
QSharedPointer<GenericEditProvider> SystemDesignWidget::getGenericEditProvider() const
{
    return editProvider_;
}
