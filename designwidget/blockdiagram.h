/* 
 *
 * 		filename: blockdiagram.h
 */

#ifndef BLOCKDIAGRAM_H
#define BLOCKDIAGRAM_H

#include "AdHocEnabled.h"

#include "columnview/DiagramColumn.h"

#include <common/DesignDiagram.h>
#include <models/businterface.h>

#include <QMap>
#include <QVector>
#include <QSharedPointer>
#include <QUndoCommand>

class LibraryInterface;
class Component;
class Design;
class DesignConfiguration;
class DiagramComponent;
class ComponentItem;
class DiagramPort;
class DiagramInterface;
class DiagramAdHocPort;
class DiagramAdHocInterface;
class DiagramInterconnection;
class DiagramConnectionEndPoint;
class DiagramColumnLayout;
class AbstractionDefinition;
class VLNV;
class GenericEditProvider;
class DesignWidget;

/*! \brief BlockDiagram is a graphical view to a design
 *
 */
class BlockDiagram : public DesignDiagram
{
    Q_OBJECT

public:
    /*! \brief The constructor
     *
     */
    BlockDiagram(LibraryInterface *lh, GenericEditProvider& editProvider, DesignWidget *parent = 0);

	//! \brief The destructor
	virtual ~BlockDiagram();

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
     *  Sets the draw mode.
     */
    virtual void setMode(DrawMode mode);


    /*! \brief Set the IP-XACT document that is viewed in BlockDiagram
     *
     */
    void openDesign(QSharedPointer<Design> design);

    /*! \brief Get DiagramComponent that has the given instance name
     *
     */
    DiagramComponent *getComponent(const QString &instanceName);

	/*! \brief Get the component instances contained in this scene.
	 *
	 * \return QList containing pointers to the component instances.
	*/
	QList<DiagramComponent*> getInstances() const;

    /*!
     *  Sets the bus widths visible/invisible.
     *
     *      @param [in] visible If true, the bus widths are set visible. Otherwise false.
     */
    void setBusWidthsVisible(bool visible);

	/*! \brief Update the given hierarchical component to match this BlockDiagram
     *
     */
    void updateHierComponent(QSharedPointer<Component> comp);

    /*!
     *  Adds a column to the diagram's layout.
     *
     *      @param [in] name          The name of the column.
     *      @param [in] type          The content type.
     *      @param [in] allowedItems  The allowed items, if the content type is custom.
     *                                This value is discarded if the content type is something else.
     *                                See ColumnItemType for possible values.
     */
    void addColumn(QString const& name, ColumnContentType contentType,
                   unsigned int allowedItems = CIT_NONE);

    /*!
     *  Removes a column from the diagram's layout.
     *
     *      @param [in] colum The column to remove.
     */
    void removeColumn(DiagramColumn* column);

    /*!
     *  Returns the diagram column layout.
     */
    DiagramColumnLayout* getColumnLayout();

    /*! \brief Get pointer to the parent of this scene.
	 *
	 * \return Pointer to the design widget that owns this scene.
	*/
	virtual DesignWidget* parent() const;

    /*!
     *  Called when a port's ad-hoc visibility has been changed.
     *
     *      @param [in] portName  The name of the port.
     *      @param [in] visible   The new ad-hoc visibility.
     */
    virtual void onAdHocVisibilityChanged(QString const& portName, bool visible);

    /*!
     *  Returns the ad-hoc port with the given name or null if not found.
     */
    virtual DiagramConnectionEndPoint* getDiagramAdHocPort(QString const& portName);

signals:
    /*! \brief Signal openDesign is emitted when user double clicks on a hierarchical component
     *
     */
    void openDesign(const VLNV& vlnv, const QString& viewName);

	//! \brief Signal is emitted when component with given vlnv should be opened in editor.
	void openComponent(const VLNV& vlnv);

    //! Signaled when the bus with the given vlnv should be opened for editing.
    void openBus(VLNV const& vlnv, VLNV const& absDefVLNV, bool disableBusDef);

	/*! \brief Emitted when user selects a port on the draw board.
	 *
	 * \param port Pointer to the DiagramPort instance that is selected.
	 *
	*/
	void portSelected(DiagramPort* port);

    /*! \brief Emitted when user selects an interface on the draw board.
	 *
	 * \param interface Pointer to the DiagramInterface instance that is selected.
	 *
	*/
	void interfaceSelected(DiagramPort* interface);

    /*!
     *  Emitted when user selects an interface on the draw board.
     *
     *      @param [in] interface The selected interface.
     */
    void interfaceSelected(DiagramInterface* interface);

	/*! \brief Emitted when user selects a connection on the draw board.
	 *
	 * \param connection Pointer to the selected connection.
	 *
	*/
	void connectionSelected(DiagramInterconnection* connection);

    //! Emitted when the user selects an ad-hoc port.
    void adHocPortSelected(DiagramAdHocPort* port);

    //! Emitted when the user selects an ad-hoc interface.
    void adHocInterfaceSelected(DiagramAdHocInterface* interface);

public slots:
    /*! \brief Bring the selected item to front
     *
     */
    void selectionToFront();

    /*!
     *  Called when the view has been scrolled vertically.
     */
    void onVerticalScroll(qreal y);

    /*!
     *  Called when the diagram is shown.
     */
    void onShow();

    /*!
     *  Called when the selection changes in the diagram.
     */
    void onSelectionChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

    void endConnect();

    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void wheelEvent(QGraphicsSceneWheelEvent *event);

    void keyReleaseEvent(QKeyEvent *event);
    
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);

    void disableHighlight();
    void createConnection(QGraphicsSceneMouseEvent * mouseEvent);

private:
    // Disable copying.
    BlockDiagram(BlockDiagram const& rhs);
    BlockDiagram& operator=(BlockDiagram const& rhs);

    /*!
     *  Called when an item has been selected in the diagram.
     *
     *      @param [in] newSelection The selected item.
     */
    void onSelected(QGraphicsItem* newSelection);

    /*!
     *  Adds a new interface to the given diagram column.
     *
     *      @param [in] column The column where to add the interface.
     *      @param [in] pos    The interface position.
     */

    void addInterface(DiagramColumn* column, QPointF const& pos);

    /*!
     *  Toggles the connection style of the given connection between normal and off-page style.
     *
     *      @param [in] conn      The connection.
     *      @param [in] parentCmd The parent undo command.
     */
    void toggleConnectionStyle(DiagramInterconnection* conn, QUndoCommand* parentCmd);

    /*!
     *  Hides all visible off-page connections.
     */
    void hideOffPageConnections();

    /*!
     *  Destroys all connections in the diagram.
     */
    void destroyConnections();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! \brief Pointer to the parent of this scene.
	DesignWidget* parent_;

    //! The connection that is being drawn.
    DiagramInterconnection *tempConnection_;

    //! The starting end point of a connection that is being drawn.
    DiagramConnectionEndPoint* tempConnEndPoint_;

    //! The potential end points that can be connected to the starting end point.
    QVector<DiagramConnectionEndPoint*> tempPotentialEndingEndPoints_;

    //! The highlighted end point to which the connection could be snapped automatically.
    DiagramConnectionEndPoint* highlightedEndPoint_;

    //! The diagram column layout.
    QSharedPointer<DiagramColumnLayout> layout_;

    //! The type of the item being dragged.
    ColumnItemType dragCompType_;
    bool dragBus_;

    //! If true, the off-page connection mode is active.
    bool offPageMode_;

    QGraphicsItem* oldSelection_;
};

#endif // BLOCKDIAGRAM_H
