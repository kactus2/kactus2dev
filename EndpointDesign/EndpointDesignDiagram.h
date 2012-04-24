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

#include "EndpointItem.h"
#include "ProgramEntityItem.h"
#include "MappingComponentItem.h"
#include "../SystemDesign/SystemColumnLayout.h"

#include <common/DesignDiagram.h>
#include <common/IDFactory.h>
#include <common/KactusAttribute.h>

class VLNV;
class EndpointDesignWidget;

//-----------------------------------------------------------------------------
//! EndpointDesignDiagram class.
//-----------------------------------------------------------------------------
class EndpointDesignDiagram : public DesignDiagram
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
    EndpointDesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                          GenericEditProvider& editProvider, EndpointDesignWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointDesignDiagram();

    /*! 
     *  Creates a design based on the contents in the diagram.
     *
     *      @param [in] vlnv The vlnv for the design.
     *
     *      @return The created design.
     */
    virtual QSharedPointer<Design> createDesign(VLNV const& vlnv) const;

    /*!
     *  Saves the hierarchy inside the design to components and designs.
     */
    bool saveHierarchy() const;

    /*!
     *  Adds a new column to the diagram.
     *
     *      @param [in] name The name of the column.
     */
    void addColumn(QString const& name);

    /*!
     *  Adds a new SW mapping component to the diagram.
     *
     *      @param [in] column The column where to place the mapping component.
     *      @param [in] pos    The position for the component.
     */
    void addMappingComponent(SystemColumn* column, QPointF const& pos);

    /*!
     *  Adds an already created SW mapping component to the diagram.
     *
     *      @param [in] mappingComp The mapping component to add.
     */
    void addMappingComponent(MappingComponentItem* mappingComp);

    /*!
     *  Removes an SW mapping component from the diagram.
     *
     *      @param [in] mappingComp The mapping component to remove.
     */
    void removeMappingComponent(MappingComponentItem* mappingComp);

    /*!
     * Returns the parent widget.
     */
    EndpointDesignWidget* parent() const;

    /*!
     *  Returns the column layout.
     */
    SystemColumnLayout* getColumnLayout();

    void onVerticalScroll(qreal y);

public slots:
    //! Called when a component instance is added to the diagram.
    virtual void onComponentInstanceAdded(ComponentItem* item);

    //! Called when a component instance is remove from the diagram.
    virtual void onComponentInstanceRemoved(ComponentItem* item);

signals:
    //! \brief Emitted when the user double-clicks a hierarchical component.
    void openDesign(const VLNV& vlnv, const QString& viewName);

    //! \brief Emitted when component with given vlnv should be opened in editor.
    void openComponent(const VLNV& vlnv);

    //! \brief Emitted when a program entity source should be opened in editor.
    void openSource(ProgramEntityItem* progEntity);
     
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
     *  Opens the system for editing.
     */
    void openDesign(QSharedPointer<Design> design);

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
    void packetizeSWComponent(ComponentItem* item, QString const& itemTypeName);
    void onSelected(QGraphicsItem* newSelection);

    //! MappingComponentList type.
    typedef QList<MappingComponentItem*> MappingComponentList;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent widget.
    EndpointDesignWidget* parent_;

    //! The node ID factory.
    IDFactory nodeIDFactory_;

    // Node column layout.
    QSharedPointer<SystemColumnLayout> layout_;

    //! The starting endpoint of a connection that is being drawn.
    EndpointItem* tempConnEndpoint_;

    //! The connection that is being drawn.
    EndpointConnection* tempConnection_;

    //! The potential endpoints that can be connected to the starting endpoint.
    QVector<EndpointItem*> tempPotentialEndingEndPoints_;

    //! The highlighted endpoint to which the connection could be snapped automatically.
    EndpointItem* highlightedEndPoint_;

    //! Boolean flag for indicating that an SW component is being dragged to the diagram.
    bool dragSW_;

    //! The type of the SW component being dragged.
    KactusAttribute::SWType dragSWType_;
};

//-----------------------------------------------------------------------------

#endif // ENDPOINTDESIGNDIAGRAM_H
