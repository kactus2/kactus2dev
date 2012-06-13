//-----------------------------------------------------------------------------
// File: DesignWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.6.2012
//
// Description:
// Base class for all design widgets.
//-----------------------------------------------------------------------------

#ifndef DESIGNWIDGET_H
#define DESIGNWIDGET_H

#include <common/widgets/tabDocument/TabDocument.h>
#include <common/KactusAttribute.h>

#include <QGraphicsView>

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
    DesignWidget(LibraryInterface* lh, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~DesignWidget();

    /*!
     *  Sets the diagram used to visualize the design.
     */
    void setDiagram(DesignDiagram* diagram);

    /*!
     *  Sets a design to be displayed in the design diagram.
     *
     *      @param [in] vlnv      The VLNV of the top-level component.
     *      @param [in] viewName  The name of the view to open.
     *
     *      @return False if there were errors opening the design; otherwise true.
     */
    virtual bool setDesign(VLNV const& vlnv, QString const& viewName) = 0;

    /*!
     *  Changes the state of a visibility control.
     *
     *      @param [in] name   The name of the visibility control.
     *      @param [in] state  The new state for the visibility control.
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
     *  Sets the draw mode of the design.
     *
     *      @param [in] mode The draw mode.
     */
    virtual void setMode(DrawMode mode);

    /*!
     *  Sets the protection state of the document.
     *
     *      @param [in] locked True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

    /*!
     *  Returns the graphics view.
     */
    QGraphicsView* getView();

    /*!
     *  Returns the graphics view.
     */
    QGraphicsView const* getView() const;

    /*!
     *  Returns the VLNV of the currently open system.
     */
    VLNV const* getOpenDocument() const;

    /*! 
     *  Returns the vlnv of the edited component.
	 */
	virtual VLNV getComponentVLNV() const;

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
     *      @return Base class implementation returns null.
     *
     *      @remarks Edit support should be queried with getFlags().
     */
    IEditProvider* getEditProvider();

    /*!
     *  Returns the generic edit provider that manages the undo stack.
     */
    QSharedPointer<GenericEditProvider> getGenericEditProvider() const;

    /*!
     *  Returns the design diagram.
     */
    DesignDiagram* getDiagram();

    /*!
     *  Returns the design diagram.
     */
    DesignDiagram const* getDiagram() const;

    /*!
     *  Returns the implementation attribute.
     */
    virtual KactusAttribute::Implementation getImplementation() const = 0;

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

signals:
    //! Emitted when the user double-clicks a hierarchical SW/mapping component.
    void openSWDesign(const VLNV& vlnv, const QString& viewName);

    //! Emitted when user double clicks a hierarchical HW component.
    void openDesign(const VLNV& vlnv, const QString& viewName);

    //! Emitted when component with given vlnv should be opened in editor.
    void openComponent(const VLNV& vlnv);

    //! Emitted when a C source file should be opened for editing.
    void openCSource(ComponentItem* compItem);

    //! Signaled when the bus with the given vlnv should be opened for editing.
    void openBus(VLNV const& vlnv, VLNV const& absDefVLNV, bool disableBusDef);

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

    /*!
     *  Returns the library interface.
     */
    LibraryInterface* getLibraryInterface();

    virtual bool setDesign(QSharedPointer<Component> component, const QString& viewName);

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

    //! The library interface.
    LibraryInterface* lh_;

    //! The graphics view for showing the diagram.
    QGraphicsView* view_;

    //! The component being edited.
    QSharedPointer<Component> editedComponent_;

    //! The name of the hierarchical view being edited.
    QString viewName_;

    //! The design diagram.
    DesignDiagram* diagram_;

    //! The edit provider.
    QSharedPointer<GenericEditProvider> editProvider_;
};

//-----------------------------------------------------------------------------

#endif // DESIGNWIDGET_H
