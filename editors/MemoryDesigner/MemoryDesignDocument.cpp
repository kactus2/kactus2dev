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

#include <editors/common/ImageExporter.h>

#include <editors/MemoryDesigner/MemoryDesignerDiagram.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Component/Component.h>

#include <QScrollBar>
#include <QVBoxLayout>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::MemoryDesignDocument()
//-----------------------------------------------------------------------------
MemoryDesignDocument::MemoryDesignDocument(LibraryInterface* libHandler, QWidget* parent):
TabDocument(parent, libHandler, DOC_ZOOM_SUPPORT | DOC_PRINT_SUPPORT, 30, 300),
view_(new QGraphicsView(this)),
diagram_(new MemoryDesignerDiagram(libHandler, this)),
identifyingVLNV_(),
designViewName_()
{
    connect(diagram_, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)),
        this, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)), Qt::UniqueConnection);

    view_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view_->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_);

    view_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
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
    diagram_->clearScene();
    setDesign(identifyingVLNV_, designViewName_);

    TabDocument::refresh();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::setDesign()
//-----------------------------------------------------------------------------
bool MemoryDesignDocument::setDesign(VLNV const& componentVLNV, QString const& viewName)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    bool designSetupSuccessfull = false;

    if (componentVLNV.isValid() && componentVLNV.getType() == VLNV::COMPONENT && !viewName.isEmpty())
    {
        identifyingVLNV_ = componentVLNV;

        setDocumentName(QString("%1 (%2)").
            arg(getIdentifyingVLNV().getName()).
            arg(getIdentifyingVLNV().getVersion()));
        setDocumentType(DocumentType::MEMORY_DESIGN);

        QSharedPointer<const Document> libraryDocument = getLibHandler()->getModelReadOnly(componentVLNV);
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

    QApplication::restoreOverrideCursor();

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
    QTransform oldMatrix = view_->transform();

    view_->resetTransform();
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
//     QPointF pt(0.0, y);
//     QTransform mat = view_->transform().inverted();
//     diagram_->onVerticalScroll(mat.map(pt).y());
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::showEvent()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::showEvent(QShowEvent* event)
{
    emit helpUrlRequested(QLatin1String("memoryDesign/memoryDesign.html"));
    TabDocument::showEvent(event);
    diagram_->onShow();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::setProtection()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::setProtection(bool /*locked*/)
{
    TabDocument::setProtection(false);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::setCondenseMemoryItems()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::setCondenseMemoryItems(bool condenseItems)
{
    diagram_->setCondenseMemoryItems(condenseItems);
    refresh();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::memoryItemsAreCondensed()
//-----------------------------------------------------------------------------
bool MemoryDesignDocument::memoryItemsAreCondensed() const
{
    return diagram_->memoryItemsAreCondensed();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::filterAddressSpaces()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::filterAddressSpaceChains(bool filterChains)
{
    diagram_->setFilterAddressSpaceChains(filterChains);
    refresh();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::addressSpaceChainsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignDocument::addressSpaceChainsAreFiltered() const
{
    return diagram_->addressSpaceChainsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::condenseFieldItems()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::condenseFieldItems()
{
    diagram_->condenseFieldItems();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::extendFieldItems()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::extendFieldItems()
{
    diagram_->extendFieldItems();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::filterAddressSpaceSegments()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::filterAddressSpaceSegments(bool filterSegments)
{
    diagram_->setFilterAddressSpaceSegments(filterSegments);
    refresh();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::addressSpaceSegmentsAreFilterted()
//-----------------------------------------------------------------------------
bool MemoryDesignDocument::addressSpaceSegmentsAreFilterted() const
{
    return diagram_->addressSpaceSegmentsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::filterAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::filterAddressBlocks(bool filterBlocks)
{
    diagram_->setFilterAddressBlocks(filterBlocks);
    refresh();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::addressBlocksAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignDocument::addressBlocksAreFiltered() const
{
    return diagram_->addressBlocksAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::filterAddressBlockRegisters()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::filterAddressBlockRegisters(bool filterRegisters)
{
    diagram_->setFilterAddressBlockRegisters(filterRegisters);
    refresh();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::addressBlockRegistersAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignDocument::addressBlockRegistersAreFiltered() const
{
    return diagram_->addressBlockRegistersAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::filterFields()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::filterFields(bool filterFields)
{
    diagram_->setFilterFields(filterFields);
    refresh();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::fieldsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignDocument::fieldsAreFiltered() const
{
    return diagram_->fieldsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::unconnectedMemoryItemsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignDocument::unconnectedMemoryItemsAreFiltered() const
{
    return diagram_->unconnectedMemoryItemsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::filterUnconnectedMemoryItems()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::filterUnconnectedMemoryItems(bool filterUnconnected)
{
    diagram_->filterUnconnectedMemoryItems(filterUnconnected);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::filterMemoryOverlapItems()
//-----------------------------------------------------------------------------
void MemoryDesignDocument::filterMemoryOverlapItems(bool filterOverlap)
{
    diagram_->filterMemoryOverlapItems(filterOverlap);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::memoryOverlapItemsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignDocument::memoryOverlapItemsAreFiltered() const
{
    return diagram_->memoryOverlapItemsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDocument::exportImage()
//-----------------------------------------------------------------------------
bool MemoryDesignDocument::exportImage()
{
    QString libraryPath = getLibHandler()->getDirectoryPath(identifyingVLNV_);
    return ImageExporter::exportImage(libraryPath, identifyingVLNV_, diagram_, this);
}
