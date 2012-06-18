//-----------------------------------------------------------------------------
// File: SystemDesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.4.2012
//
// Description:
// System design diagram to work as a drawing canvas.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDESIGNDIAGRAM_H
#define SYSTEMDESIGNDIAGRAM_H

#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/IDFactory.h>
#include <common/KactusAttribute.h>
#include <common/DesignDiagram.h>

#include <QVector>

class MainWindow;
class Component;
class Design;
class DesignConfiguration;
class LibraryInterface;
class VLNV;
class GenericEditProvider;
class SystemDesignWidget;
class HWMappingItem;
class SystemComponentItem;
class SWConnectionEndpoint;
class GraphicsConnection;

//-----------------------------------------------------------------------------
//! SystemDesignDiagram class.
//-----------------------------------------------------------------------------
class SystemDesignDiagram : public DesignDiagram
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] lh            The library interface.
     *      @param [in] mainWnd       The main window.
     *      @param [in] editProvider  The edit provider.
     *      @param [in] parent        The parent widget.
     */
    SystemDesignDiagram(bool onlySW, LibraryInterface* lh, MainWindow* mainWnd,
                        GenericEditProvider& editProvider, SystemDesignWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemDesignDiagram();

    /*!
     *  Clears the scene.
     */
    virtual void clearScene();

    /*! 
     *  Creates a design based on the contents in the diagram.
     *
     *      @param [in] vlnv The vlnv for the design.
     *
     *      @return The created design.
     */
    virtual QSharedPointer<Design> createDesign(VLNV const& vlnv) const;

    /*!
     *  Reflects the changes in the design to the top-level component.
     */
    virtual void updateHierComponent();

    /*!
     *  Sets the draw mode.
     */
    virtual void setMode(DrawMode mode);

    /*!
     *  Adds a new column to the diagram.
     *
     *      @param [in] desc The column description.
     */
    void addColumn(ColumnDesc const& desc);

    /*!
     * Returns the parent widget.
     */
    SystemDesignWidget* parent() const;

    /*!
     *  Returns the column layout.
     */
    GraphicsColumnLayout* getColumnLayout();

    void onVerticalScroll(qreal y);

public slots:
    //! Called when a component instance is added to the diagram.
    virtual void onComponentInstanceAdded(ComponentItem* item);

    //! Called when a component instance is remove from the diagram.
    virtual void onComponentInstanceRemoved(ComponentItem* item);

protected:
    //! Called when the user presses a mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user moves the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release a mouse button.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the mouse is double-clicked.
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Called when the mouse wheel has scrolled.
    void wheelEvent(QGraphicsSceneWheelEvent *event);

    //! Called when an drag enters the diagram.
    void dragEnterEvent(QGraphicsSceneDragDropEvent * event);

    //! Called when an object is dragged within the diagram.
    void dragMoveEvent(QGraphicsSceneDragDropEvent * event);

    //! Called when the drag leaves the diagram.
    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);

    //! Called when an object is dropped to the diagram.
    void dropEvent(QGraphicsSceneDragDropEvent *event);

private:
    // Disable copying.
    SystemDesignDiagram(SystemDesignDiagram const& rhs);
    SystemDesignDiagram& operator=(SystemDesignDiagram const& rhs);

    /*!
     *  Called when an item has been selected in the diagram.
     *
     *      @param [in] newSelection The selected item.
     */
    virtual void onSelected(QGraphicsItem* newSelection);

    /*!
     *  Opens the system design for editing.
     */
    void loadDesign(QSharedPointer<Design> design);

    /*!
     *  Loads the COM connections from the given design.
     */
    void loadComConnections(QSharedPointer<Design> design);

    /*!
     *  Loads the API dependencies from the given design.
     */
    void loadApiDependencies(QSharedPointer<Design> design);

    /*!
     *  Returns the HW component instance with the given name.
     */
    HWMappingItem* getHWComponent(QString const& instanceName);

    /*!
     *  Returns the component with the given name (either HW or SW).
     *
     *      @param [in] instanceName The name of the instance to search for.
     *
     *      @return The corresponding component, or null if not found.
     */
    SystemComponentItem* getComponent(QString const& instanceName);

    /*!
     *  Creates the currently drawn connection.
     *
     *      @param [in] event The ending mouse press event.
     */
    void createConnection(QGraphicsSceneMouseEvent* event);

    /*!
     *  Discards and destroys the temporary (currently drawn) connection.
     */
    void discardConnection();

    /*!
     *  Destroys all connections in the diagram.
     */
    void destroyConnections();

    /*!
     *  Disables all highlights.
     */
    void disableHighlights();

    /*!
     *  Disables the current highlight. In case of a potential ending endpoint, the highlight is
     *  set to allowed state.
     */
    void disableCurrentHighlight();
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! If true, the widget is used for editing SW designs.
    bool onlySW_;

    //! The parent widget.
    SystemDesignWidget* parent_;

    // Graphics column layout.
    QSharedPointer<GraphicsColumnLayout> layout_;

    //! Boolean flag for indicating that an SW component is being dragged to the diagram.
    bool dragSW_;

    //! Boolean flag for indicating that a API/COM definition is being dragged to the diagram.
    bool dragDefinition_;

    //! The connection that is being drawn.
    GraphicsConnection* tempConnection_;

    //! The starting endpoint of a connection that is being drawn.
    SWConnectionEndpoint* tempConnEndpoint_;

    //! The potential endpoints that can be connected to the starting end point.
    QVector<SWConnectionEndpoint*> tempPotentialEndingEndpoints_;
    
    //! The highlighted endpoint to which the connection could be snapped automatically.
    SWConnectionEndpoint* highlightedEndpoint_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDESIGNDIAGRAM_H
