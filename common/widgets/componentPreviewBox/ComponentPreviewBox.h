//-----------------------------------------------------------------------------
// File: ComponentPreviewBox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 1.8.2011
//
// Description:
// Component preview box for visualizing HW components.
//-----------------------------------------------------------------------------

#ifndef COMPONENTPREVIEWBOX_H
#define COMPONENTPREVIEWBOX_H

#include <IPXACTmodels/common/VLNV.h>

#include <editors/ComponentEditor/itemvisualizer.h>

#include <QSharedPointer>
#include <QGraphicsView>
#include <QRectF>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! ComponentPreviewBox class.
//-----------------------------------------------------------------------------
class ComponentPreviewBox : public ItemVisualizer
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] lh         The library interface. 
     *      @param [in] parent     The parent widget.
     */
    ComponentPreviewBox(LibraryInterface* lh, QWidget* parent = nullptr);

    /*!
     *  Destructor.
     */
    ~ComponentPreviewBox() override = default;

    // Disable copying.
    ComponentPreviewBox(ComponentPreviewBox const& rhs) = delete;
    ComponentPreviewBox& operator=(ComponentPreviewBox const& rhs) = delete;

    /*!
     *  Sets the component that will be visualized.
     *
     *      @param [in] component  The component to visualize.
     */
    void setComponent(QSharedPointer<Component> component);

    /*!
     *  Updates the preview.
     */
    void updatePreview();

	/*!
     *  Get the bounding rect of all items on the scene.
	 *
	 *      @return The rect that bounds all items.
	 */
	QRectF itemsBoundingRect() const;

    /*!
     *  Get the graphics scene.
     *
     *      @return The graphics scene used by the preview.
     */
    QGraphicsScene* scene() const;

    /*!
     *  Set the preview interactive i.e. user can move interfaces.
     *
     *      @param [in] interactive  Interactive or view-only.
     */
    void setInteractive(bool interactive);



public slots:

	/*! 
	 * Sets the component that will be visualized.
	 * 
	 *		@param [in] vlnv The vlnv of the component. If vlnv does not belong
	 *		to component then the preview box is cleared.
	 */
	void setComponent(const VLNV& vlnv);

signals:
    //! Emitted when the terminal arrangement has changed.
    void endpointsRearranged();

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* lh_;

    //! The component to visualize.
    QSharedPointer<Component> component_ = nullptr;

    QGraphicsView* view_;

    //! The graphics scene.
    QGraphicsScene* scene_;

};

//-----------------------------------------------------------------------------

#endif // COMPONENTPREVIEWBOX_H
