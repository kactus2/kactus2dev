//-----------------------------------------------------------------------------
// File: EndpointDesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.2.2011
//
// Description:
// Endpoint design diagram to work as a drawing canvas.
//-----------------------------------------------------------------------------

#ifndef ENDPOINTDESIGNDIAGRAM_H
#define ENDPOINTDESIGNDIAGRAM_H

#include <QGraphicsScene>

#include "EndpointItem.h"
#include "ProgramEntityItem.h"
#include "MappingComponentItem.h"
#include "SystemColumnLayout.h"

#include <common/IDFactory.h>
#include <common/DrawMode.h>
#include <common/KactusAttribute.h>

class MainWindow;
class Component;
class Design;
class DesignConfiguration;
class LibraryInterface;
class VLNV;
class GenericEditProvider;

//-----------------------------------------------------------------------------
//! EndpointDesignDiagram class.
//-----------------------------------------------------------------------------
class EndpointDesignDiagram : public QGraphicsScene
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] lh            The library interface.
     *      @param [in] mainWnd       The main window.
     *      @param [in] editProvider  The edit provider.
     *      @param [in] parent        The parent object.
     */
    EndpointDesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                          GenericEditProvider& editProvider, QObject* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointDesignDiagram();

    /*!
     *  Saves the hierarchy inside the design to components and designs.
     */
    bool saveHierarchy() const;

    /*! 
     *  Creates a design based on the contents in the diagram.
     *
     *      @param [in] vlnv     The vlnv for the design.
     *
     *      @return The created system design.
     */
    QSharedPointer<Design> createDesign(VLNV const& vlnv);

    /*!
     *  Opens the system for editing.
     */
    bool setDesign(QSharedPointer<Component> system);

    /*!
     *  Adds a new column to the diagram.
     *
     *      @param [in] name The name of the column.
     */
    void addColumn(QString const& name);

    /*!
     *  Returns the design configuration.
     */
    QSharedPointer<DesignConfiguration> getDesignConfiguration() const;

    
    /*!
     *  Removes the instance name from the list so the name can be used again in the design.
	 *
	 *      @param name The name of the removed instance.
	 *
     *      @remarks This function should be called when user removes component instance.
	*/
	void removeInstanceName(const QString& name);

	/*!
     *  Updates the list of instance names.
	 *
	 *      @param oldName The old name of the component instance.
	 *      @param newName The new name of the component instance.
	 *
	*/
	void updateInstanceName(const QString& oldName, const QString& newName);


    /*!
     *  Adds a new SW mapping component to the diagram.
     *
     *      @param [in] column The column where to place the mapping component.
     *      @param [in] pos    The position for the component.
     */
    void addMappingComponent(SystemColumn* column, QPointF const& pos);

    /*!
     *  Draws the diagram background.
     */
     void drawBackground(QPainter* painter, QRectF const& rect);

     /*!
      * Returns the main window.
      */
     MainWindow* getMainWindow() const;

     /*!
     *  Sets the draw mode of the diagram.
     *
     *      @param [in] mode The draw mode.
     */
    void setMode(DrawMode mode);

    /*!
     *  Returns the column layout.
     */
    SystemColumnLayout* getColumnLayout();

    /*!
     *  Returns the edit provider.
     */
    GenericEditProvider& getEditProvider();

    void onVerticalScroll(qreal y);

public slots:
    //! Called when a component instance is added to the diagram.
    void onComponentInstanceAdded(SWComponentItem* item);

    //! Called when a component instance is remove from the diagram.
    void onComponentInstanceRemoved(SWComponentItem* item);

signals:
    //! \brief Emitted when component with given vlnv should be opened in editor.
    void openComponent(const VLNV& vlnv);

    //! \brief Emitted when a program entity source should be opened in editor.
    void openSource(ProgramEntityItem* progEntity);

    //! Signaled when the draw mode has changed.
    void modeChanged(DrawMode mode);

    //! Signaled when all items are deselected.
    void clearItemSelection();

    //! Signaled when the contents of the diagram have changed.
    void contentChanged();

    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage);

    //! Sends a notification to the user.
    void noticeMessage(const QString& noticeMessage);

    //! Signaled when a new component is instantiated to the design.
    void componentInstantiated(SWComponentItem* item);

    //! Signaled when a component instance is removed from the design.
    void componentInstanceRemoved(SWComponentItem* item);
     
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
    EndpointDesignDiagram(EndpointDesignDiagram const& rhs);
    EndpointDesignDiagram& operator=(EndpointDesignDiagram const& rhs);
    void disableHighlight();

    /*!
     *  Creates a unique instance name for the given component.
     *
     *      @param [in] component The component whose instance is being created.
     *
     *      @return The name for the component instance.
     */
    QString createInstanceName(QSharedPointer<Component> component);

    /*!
     *  Creates a unique instance name with the given base name.
     *
     *      @param [in] baseName The base name for the instance.
     */
    QString createInstanceName(QString const& baseName);

    /*!
     *  Retrieves the SW mapping component item with the given instance name.
     *
     *      @param [in] instanceName The name of the mapping component instance to search.
     *
     *      @return The SW mapping component, of null if not found.
     */
    MappingComponentItem* getMappingComponent(QString const& instanceName);

    /*!
     *  Creates an application for the given program entity.
     *
     *      @param [in] progEntity The program entity.
     */
    void createApplication(ProgramEntityItem* progEntity);

    /*!
     *  Creates an unpackaged platform component and assigns it to the given mapping component.
     *
     *      @param [in] mappingCompItem The mapping component.
     */
    void createPlatformComponent(MappingComponentItem* mappingCompItem);

    /*!
     *  Packetizes the given SW component.
     *
     *      @param [in] item          The SW component item to packetize.
     *      @param [in] itemTypeName  The item type name (e.g. "Application").
     */
    void packetizeSWComponent(SWComponentItem* item, QString const& itemTypeName);
    
    //! MappingComponentList type.
    typedef QList<MappingComponentItem*> MappingComponentList;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Library interface.
    LibraryInterface* lh_;

    //! A pointer to the main window.
    MainWindow* mainWnd_;

    //! The system that is being edited.
    QSharedPointer<Component> system_;

    //! The design configuration.
    QSharedPointer<DesignConfiguration> designConf_;

    //! The node ID factory.
    IDFactory nodeIDFactory_;

    // Node column layout.
    QSharedPointer<SystemColumnLayout> layout_;

    // The current draw mode.
    DrawMode mode_;

    //! The starting endpoint of a connection that is being drawn.
    EndpointItem* tempConnEndpoint_;

    //! The connection that is being drawn.
    EndpointConnection* tempConnection_;

    //! The potential endpoints that can be connected to the starting endpoint.
    QVector<EndpointItem*> tempPotentialEndingEndPoints_;

    //! The highlighted endpoint to which the connection could be snapped automatically.
    EndpointItem* highlightedEndPoint_;

    //! List of instance names already in use.
    QStringList instanceNames_;

    //! Boolean flag for indicating that an SW component is being dragged to the diagram.
    bool dragSW_;

    //! The type of the SW component being dragged.
    KactusAttribute::SWType dragSWType_;

    //! The edit provider for undo/redo.
    GenericEditProvider& editProvider_;
};

//-----------------------------------------------------------------------------

#endif // ENDPOINTDESIGNDIAGRAM_H
