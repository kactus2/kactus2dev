//-----------------------------------------------------------------------------
// File: MemoryDesignDocument.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.07.2016
//
// Description:
// Declares the memory design tab document class.
//-----------------------------------------------------------------------------

#include "MemoryDesignDocument.h"

#include <designEditors/MemoryDesigner/MemoryDesignerDiagram.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Component/Component.h>

#include <QScrollBar>
#include <QVBoxLayout>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::MemoryDesignDocument()
//-----------------------------------------------------------------------------
MemoryDesignDocument::MemoryDesignDocument(LibraryInterface* library, QWidget* parent):
TabDocument(parent, DOC_ZOOM_SUPPORT | DOC_DRAW_MODE_SUPPORT | DOC_PRINT_SUPPORT | DOC_PROTECTION_SUPPORT |
    DOC_VISIBILITY_CONTROL_SUPPORT, 30, 300),
view_(new QGraphicsView(this)),
diagram_(new MemoryDesignerDiagram(library, this)),
libraryHandler_(library),
identifyingVLNV_(),
designViewName_()
{
    view_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_);

    view_->verticalScrollBar()->setTracking(true);
    connect(view_->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onVerticalScroll(int)));

    view_->setScene(diagram_);

    centerViewTo(QPointF(0,0));
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::~MemoryDesignDocument()
//-----------------------------------------------------------------------------
MemoryDesignDocument::~MemoryDesignDocument()
{
    view_->verticalScrollBar()->disconnect(this);

    if (diagram_)
    {
        delete diagram_;
        diagram_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::refresh()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::refresh()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    diagram_->clearScene();
    setDesign(identifyingVLNV_, designViewName_);

    TabDocument::refresh();

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::setDesign()
//-----------------------------------------------------------------------------
bool MemoryDesignDocument::setDesign(VLNV const& componentVLNV, QString const& viewName)
{
    bool designSetupSuccessfull = false;

    if (componentVLNV.isValid() && componentVLNV.getType() == VLNV::COMPONENT && !viewName.isEmpty())
    {
        identifyingVLNV_ = componentVLNV;

        setDocumentName(QString("%1 (%2)").
            arg(getIdentifyingVLNV().getName()).
            arg(getIdentifyingVLNV().getVersion()));
        setDocumentType("Memory Design");

        QSharedPointer<const Document> libraryDocument = libraryHandler_->getModelReadOnly(componentVLNV);
        QSharedPointer<const Component> component = libraryDocument.dynamicCast<const Component>();
        if (component)
        {
            if (diagram_->loadDesignFromCurrentView(component, viewName))
            {
                designViewName_ = viewName;
                designSetupSuccessfull = true;
            }
            else
            {
                emit errorMessage(QString("Could not setup memory design from %1.").arg(componentVLNV.toString()));
            }
        }
        else
        {
            emit errorMessage(QString("Could not find component %1.").arg(componentVLNV.toString()));
        }

    }
    else
    {
        emit errorMessage(QString("Selected VLNV %1 is not valid.").arg(componentVLNV.toString()));
    }

    return designSetupSuccessfull;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::getSupportedDrawModes()
//-----------------------------------------------------------------------------
unsigned int MemoryDesignDocument::getSupportedDrawModes() const
{
    return MODE_SELECT;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV MemoryDesignDocument::getIdentifyingVLNV() const
{
    return identifyingVLNV_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV MemoryDesignDocument::getDocumentVLNV() const
{
    return identifyingVLNV_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::setVisibilityControlState()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::setVisibilityControlState(QString const& name, bool state)
{
    TabDocument::setVisibilityControlState(name, state);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::setZoomLevel()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::setZoomLevel(int level)
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
// Function: MemoryDesignDocument::fitInView()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::fitInView()
{
    QRectF itemRect = diagram_->itemsBoundingRect();
    float scaleX = qMax(0, view_->width() - 10 - view_->verticalScrollBar()->width()) / itemRect.width();
    float scaleY = qMax(0, view_->height() - 10 - view_->horizontalScrollBar()->height()) / itemRect.width();

    int scaleLevel = int(qMin(scaleX, scaleY) * 10) * 10;
    setZoomLevel(scaleLevel);

    centerViewTo(itemRect.center());
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::centerViewTo()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::centerViewTo(QPointF const& centerPoint)
{
    view_->centerOn(centerPoint);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::onVerticalScroll()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::onVerticalScroll(int y)
{
    QPointF pt(0.0, y);
    QMatrix mat = view_->matrix().inverted();
    diagram_->onVerticalScroll(mat.map(pt).y());
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::showEvent()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::showEvent(QShowEvent* event)
{
    TabDocument::showEvent(event);
    diagram_->onShow();
}
