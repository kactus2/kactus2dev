//-----------------------------------------------------------------------------
// File: ComponentPreviewBox.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.8.2011
//
// Description:
// Component preview box for visualizing HW components.
//-----------------------------------------------------------------------------

#include "ComponentPreviewBox.h"

#include <designEditors/SystemDesign/SWComponentItem.h>
#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/common/diagramgrid.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <QPainter>
#include <QRectF>
#include "IPXACTmodels/kactusExtensions/SWInstance.h"

//! The minimum size for the preview box.
static const int MIN_BOX_HEIGHT = 120;

//-----------------------------------------------------------------------------

namespace
{
    //-----------------------------------------------------------------------------
    //! GridScene class.
    //-----------------------------------------------------------------------------
    class GridScene : public QGraphicsScene
    {
    public:
        /*!
         *  Constructor.
         */
        GridScene(QWidget* parent) : QGraphicsScene(parent) {}

        /*!
         *  Destructor.
         */
        ~GridScene() {}

    protected:
        //! Draw the scene background.
        void drawBackground(QPainter* painter, QRectF const& rect);
    };

    //-----------------------------------------------------------------------------
    // Function: drawBackground()
    //-----------------------------------------------------------------------------
    void GridScene::drawBackground(QPainter* painter, QRectF const& rect)
    {
        painter->setWorldMatrixEnabled(true);
        painter->setBrush(Qt::lightGray);

        qreal left = int(rect.left()) - (int(rect.left()) % GridSize );
        qreal top = int(rect.top()) - (int(rect.top()) % GridSize );

        for (qreal x = left; x < rect.right(); x += GridSize )
        {
            for (qreal y = top; y < rect.bottom(); y += GridSize )
            {
                painter->drawPoint(x, y);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentPreviewBox::ComponentPreviewBox()
//-----------------------------------------------------------------------------
ComponentPreviewBox::ComponentPreviewBox(LibraryInterface* lh) : lh_(lh), component_(), scene_(0)
{
    // Create the scene.
    scene_ = new GridScene(this);
    setScene(scene_);
    centerOn(0, 0);

	setMinimumHeight(MIN_BOX_HEIGHT);

    // Disable interactivity by default.
    setInteractive(false);
}

//-----------------------------------------------------------------------------
// Function: ComponentPreviewBox::~ComponentPreviewBox()
//-----------------------------------------------------------------------------
ComponentPreviewBox::~ComponentPreviewBox()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentPreviewBox::updatePreview()
//-----------------------------------------------------------------------------
void ComponentPreviewBox::updatePreview()
{
    // Re-create the scene.
    delete scene_;
    scene_ = new GridScene(this);
    setScene(scene_);

    if (component_ != 0)
    {
        ComponentItem* item = 0;

        if (component_->getImplementation() == KactusAttribute::HW)
        {

            QSharedPointer<ComponentInstance> componentInstance(new ComponentInstance());
            componentInstance->setInstanceName(component_->getVlnv().getName());
            componentInstance->setComponentRef(QSharedPointer<ConfigurableVLNVReference>(
                new ConfigurableVLNVReference(component_->getVlnv())));

            item = new HWComponentItem(lh_, componentInstance, component_);
        }
        else if (component_->getImplementation() == KactusAttribute::SW)
		{
			QSharedPointer<SWInstance> swInstance(new SWInstance());
			swInstance->setInstanceName(component_->getVlnv().getName());
			swInstance->setComponentRef(QSharedPointer<ConfigurableVLNVReference>(
				new ConfigurableVLNVReference(component_->getVlnv())));

            item = new SWComponentItem(lh_, component_, swInstance);
        }

        if (item != 0)
        {
            connect(item, SIGNAL(endpointMoved(ConnectionEndpoint*)), this, SIGNAL(endpointsRearranged()));
            scene_->addItem(item);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentPreviewBox::setComponent()
//-----------------------------------------------------------------------------
void ComponentPreviewBox::setComponent(QSharedPointer<Component> component)
{
    component_ = component;
    updatePreview();
}

//-----------------------------------------------------------------------------
// Function: ComponentPreviewBox::setComponent()
//-----------------------------------------------------------------------------
void ComponentPreviewBox::setComponent( const VLNV& vlnv )
{
	QSharedPointer<Component> component;
	
	// if the vlnv belongs to a component
	if (lh_->getDocumentType(vlnv) == VLNV::COMPONENT)
    {
		component = lh_->getModel(vlnv).staticCast<Component>();
	}
	setComponent(component);
}

//-----------------------------------------------------------------------------
// Function: ComponentPreviewBox::itemsBoundingRect()
//-----------------------------------------------------------------------------
QRectF ComponentPreviewBox::itemsBoundingRect() const
{
	return scene_->itemsBoundingRect();
}
