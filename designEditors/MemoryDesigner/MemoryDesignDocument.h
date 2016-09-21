//-----------------------------------------------------------------------------
// File: MemoryDesignDocument.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.07.2016
//
// Description:
// Declares the memory design tab document class.
//-----------------------------------------------------------------------------

#ifndef MEMORYDESIGNDOCUMENT_H
#define MEMORYDESIGNDOCUMENT_H

#include <common/widgets/tabDocument/TabDocument.h>

#include <QGraphicsView>

class MemoryDesignerDiagram;
class VLNV;
class LibraryInterface;
class Design;
class Component;

//-----------------------------------------------------------------------------
//! Declares the memory design tab document class.
//-----------------------------------------------------------------------------
class MemoryDesignDocument : public TabDocument
{
    Q_OBJECT

public:
   
    /*!
     *  The constructor.
     *
     *      @param [in] library     The library interface.
     *      @param [in] parent      The parent.
     */
    MemoryDesignDocument(LibraryInterface* library, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~MemoryDesignDocument();

    /*!
     *  Reload the current memory design.
     */
    virtual void refresh();

    /*!
     *  Set the memory design.
     *
     *      @param [in] componentVLNV   VLNV of the component.
     *      @param [in] viewName        The selected component view.
     *
     *      @return True, if the memory design was opened successfully, false otherwise.
     */
    bool setDesign(VLNV const& componentVLNV, QString const& viewName);

    /*!
     *  Get the supported draw modes.
     *
     *      @return The supported draw modes.
     */
    virtual unsigned int getSupportedDrawModes() const;

    /*!
     *  Get the VLNV that can be used to identify the document.
	 * 
	 *      @return The VLNV that identifies the document.
	 */
	virtual VLNV getIdentifyingVLNV() const;

    /*!
     *  Get the document VLNV.
     *
     *      @return The VLNV of the containing document.
     */
    virtual VLNV getDocumentVLNV() const;

    /*!
     *  Changes the state of a visibility control.
     *
     *      @param [in] name   The name of the visibility control.
     *      @param [in] state  The new state for the visibility control.
     */
    virtual void setVisibilityControlState(QString const& name, bool state);

    /*!
     *  Sets the zoom level of the design.
     *
     *      @param [in] level The zoom level in percents.
     */
    virtual void setZoomLevel(int level);

    /*!
     *  Fits the whole design in view.
     */
    virtual void fitInView();

    /*!
     *  Centers the current view to a point.
     *
     *      @param [in] centerPoint   The point to center to.
     */
    void centerViewTo(QPointF const& centerPoint);

public slots:

    //! Called when the vertical scrollbar is scrolled.
    void onVerticalScroll(int y);

signals:

    //! Signaled when the zoom level has changed.
    void zoomChanged();

    //! Signaled when all items are deselected.
    void clearItemSelection();

protected:
    
    /*!
     *  Show the document.
     *
     *      @param [in] event   The selected show event.
     */
    void showEvent(QShowEvent* event);

private:
    // Disable copying.
    MemoryDesignDocument(MemoryDesignDocument const& rhs);
    MemoryDesignDocument& operator=(MemoryDesignDocument const& rhs);

    /*!
     *  Get the design from the selected view.
     *
     *      @param [in] component   Component whose views are used.
     *      @param [in] viewName    The name of the selected view.
     */
    QSharedPointer<Design> getDesignFromView(QSharedPointer<Component> component, QString const& viewName);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics view for showing the diagram.
    QGraphicsView* view_;

    //! The memory design diagram.
    MemoryDesignerDiagram* diagram_;

    //! The used library interface.
    LibraryInterface* libraryHandler_;

    //! VLNV identifying the design.
    VLNV identifyingVLNV_;

    //! The name of the view used to construct the memory design diagram.
    QString designViewName_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNDOCUMENT_H
