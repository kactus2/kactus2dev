//-----------------------------------------------------------------------------
// File: ComponentPreviewBox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.8.2011
//
// Description:
// Component preview box for visualizing HW components.
//-----------------------------------------------------------------------------

#ifndef COMPONENTPREVIEWBOX_H
#define COMPONENTPREVIEWBOX_H

#include <IPXACTmodels/vlnv.h>

#include <QSharedPointer>
#include <QGraphicsView>
#include <QRectF>

#include <common/Global.h>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! ComponentPreviewBox class.
//-----------------------------------------------------------------------------
class KACTUS2_API ComponentPreviewBox : public QGraphicsView
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] lh         The library interface.
     */
    ComponentPreviewBox(LibraryInterface* lh);

    /*!
     *  Destructor.
     */
    ~ComponentPreviewBox();

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

	/*! \brief Get the bounding rect of all items on the scene.
	 *
	 *
	 * \return QRectF The rect that bounds all items.
	*/
	QRectF itemsBoundingRect() const;

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
    // Disable copying.
    ComponentPreviewBox(ComponentPreviewBox const& rhs);
    ComponentPreviewBox& operator=(ComponentPreviewBox const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* lh_;

    //! The component to visualize.
    QSharedPointer<Component> component_;

    //! The graphics scene.
    QGraphicsScene* scene_;
};

//-----------------------------------------------------------------------------

#endif // COMPONENTPREVIEWBOX_H
