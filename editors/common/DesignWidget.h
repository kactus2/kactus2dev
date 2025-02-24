//-----------------------------------------------------------------------------
// File: DesignWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 12.6.2012
//
// Description:
// Base class for all design widgets.
//-----------------------------------------------------------------------------

#ifndef DESIGNWIDGET_H
#define DESIGNWIDGET_H

#include <common/widgets/tabDocument/TabDocument.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <common/GenericEditProvider.h>

#include <QGraphicsView>
#include <QWheelEvent>

class LibraryInterface;
class IEditProvider;
class GenericEditProvider;
class MainWindow;
class ComponentItem;
class ConnectionEndpoint;
class GraphicsConnection;
class Component;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! Base class for all design widgets that are used to show design diagrams.
//-----------------------------------------------------------------------------
class DesignWidget : public TabDocument
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    DesignWidget(LibraryInterface* libHandler, QWidget* parent = nullptr);

    /*!
     *  Destructor.
     */
    virtual ~DesignWidget();

	/*! \brief Get the VLNV that can be used to identify the document.
	 * 
	 * Design widget returns the VLNV of the design being edited. 
	 *
	 * \return The VLNV that identifies the document.
	*/
	virtual VLNV getIdentifyingVLNV() const;

    /*!
     *  Sets the diagram used to visualize the design.
     */
    void setDiagram(DesignDiagram* diagram);

    /*!
     *  Sets a design to be displayed in the design diagram.
     *
     *    @param [in] vlnv      The VLNV of the top-level component.
     *    @param [in] viewName  The name of the view to open.
     *
     *    @return False if there were errors opening the design; otherwise true.
     */
    virtual bool setDesign(VLNV const& vlnv, QString const& viewName) = 0;

    /*!
     *  Changes the state of a visibility control.
     *
     *    @param [in] name   The name of the visibility control.
     *    @param [in] state  The new state for the visibility control.
     */
    virtual void setVisibilityControlState(QString const& name, bool state);

    /*!
     *  Refreshes the document view.
     */
    virtual void refresh();

    /*!
     *  Prints the design.
     */
    virtual void print();

    /*!
     *  Exports a screenshot of the document as an image file.
     */
    virtual bool exportImage();

    /*!
     *  Sets the zoom level of the design.
     *
     *    @param [in] level The zoom level in percents.
     */
    virtual void setZoomLevel(int level);

    /*!
     *  Fits the whole design in view.
     */
    virtual void fitInView();

    /*!
     *  Sets the draw mode of the design.
     *
     *    @param [in] mode The draw mode.
     */
    virtual void setMode(DrawMode mode);

    /*!
     *  Sets the protection state of the document.
     *
     *    @param [in] locked True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

    /*!
     *  Sets the edited component.
     *
     *    @param [in] component The component to set as the edited one.
     */
    void setEditedComponent(QSharedPointer<Component> component);

    /*!
     *  Returns the VLNV of the currently open system.
     */
    VLNV getOpenDocument() const;

    /*! 
     *  Returns the vlnv of the edited component.
	 */
	virtual VLNV getDocumentVLNV() const;

    /*!
     *  Returns the the top component being edited.
	*/
	QSharedPointer<Component> getEditedComponent() const;

    /*!
     *  Returns the name of the view that is currently open.
	 */
	QString getOpenViewName() const;

    /*!
     *  Returns the component instances currently in the design.
     */
    QList<ComponentItem*> getInstances() const;

    /*!
     *  Returns the edit provider.
     *
     *    @return Base class implementation returns null.
     *
     *      @remarks Edit support should be queried with getFlags().
     */
    QSharedPointer<IEditProvider> getEditProvider() const;

    /*!
     *  Returns the design diagram.
     */
    DesignDiagram* getDiagram() const;

    /*!
     *  Returns the implementation attribute.
     */
    virtual KactusAttribute::Implementation getImplementation() const = 0;

    /*!
     *  Centers the current view to a point.
     *
     *    @param [in] centerPoint   The point to center to.
     */
    void centerViewTo(QPointF const& centerPoint);

public slots:
    /*! 
     *  Saves the design to disk.
     */
    virtual bool save();

    /*!
     *  Adds a new column to the diagram.
     */
    virtual void addColumn() = 0;

    //! Called when the vertical scrollbar is scrolled.
    void onVerticalScroll(int y);

    /*!
     *  Delete the selected items.
     */
    virtual void onDeleteSelectedItems() = 0;

signals:

    //! Emitted when component with given vlnv should be opened in editor.
    void openComponent(const VLNV& vlnv);

    //! Emitted when a C source file should be opened for editing.
    void openCSource(ComponentItem* compItem);

    //! Signaled when the draw mode of the diagram has changed.
    void modeChanged(DrawMode mode);

    //! Signaled when the zoom level has changed.
    void zoomChanged();

    //! Signaled when the user selects a component on the draw board.
    void componentSelected(ComponentItem* component);

    //! Signaled when the user selects an interface on the draw board.
    void interfaceSelected(ConnectionEndpoint* endpoint);

    //! Signaled when the user selects a connection on the draw board.
    void connectionSelected(GraphicsConnection* connection);

    //! Signaled when all items are deselected.
    void clearItemSelection();

    //! \brief Emitted when a new component is instantiated to the design.
    void componentInstantiated(ComponentItem* comp);

    //! \brief Emitted when a component instance is removed from the design.
    void componentInstanceRemoved(ComponentItem* comp);

protected:
    void showEvent(QShowEvent* event);

    virtual bool setDesign(QSharedPointer<Component> component, const QString& viewName);

    //! Removes all selected sticky notes.
    void removeSelectedNotes();

    //! Removes all selected associations.
    void removeSelectedAssociations();

private:
    // Disable copying.
    DesignWidget(DesignWidget const& rhs);
    DesignWidget& operator=(DesignWidget const& rhs);

    enum
    {
        EDIT_HISTORY_SIZE = 50
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics view for showing the diagram.
    QGraphicsView* view_ = new QGraphicsView(this);

    //! The component being edited.
    QSharedPointer<Component> editedComponent_ = nullptr;

    //! The name of the hierarchical view being edited.
    QString viewName_;

    //! The edit provider.
    QSharedPointer<GenericEditProvider> editProvider_ = 
        QSharedPointer<GenericEditProvider>(new GenericEditProvider(EDIT_HISTORY_SIZE));

    //! The design diagram.
    DesignDiagram* diagram_ = nullptr;
};

//-----------------------------------------------------------------------------

#endif // DESIGNWIDGET_H
