/* 
 *
 * 		filename: blockdiagram.h
 */

#ifndef BLOCKDIAGRAM_H
#define BLOCKDIAGRAM_H

#include "columnview/DiagramColumn.h"

#include <common/DrawMode.h>
#include <models/businterface.h>

#include <QGraphicsScene>
#include <QMap>
#include <QVector>
#include <QSharedPointer>

class LibraryInterface;
class Component;
class Design;
class DesignConfiguration;
class DiagramComponent;
class DiagramPort;
class DiagramInterface;
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
class BlockDiagram : public QGraphicsScene {
    Q_OBJECT
public:
    /*! \brief The constructor
     *
     */
    BlockDiagram(LibraryInterface *lh, GenericEditProvider& editProvider, DesignWidget *parent = 0);

	//! \brief The destructor
	virtual ~BlockDiagram();

    /*! \brief Set the IP-XACT document that is viewed in BlockDiagram
     *
     */
    bool setDesign(QSharedPointer<Component> hierComp, const QString& viewName);

    /*! \brief Get DiagramComponent that has the given instance name
     *
     */
    DiagramComponent *getComponent(const QString &instanceName);

	/*! \brief Get the component instances contained in this scene.
	 *
	 * \return QList containing pointers to the component instances.
	*/
	QList<DiagramComponent*> getInstances() const;

    /*! \brief Set the mode, how BlockDiagran responds to mouse events
     *
     */
    void setMode(DrawMode mode);

    /*!
     *  Sets the block diagram locked/unlocked.
     */
    void setProtection(bool locked);

    /*! \brief Create a Design that represents this BlockDiagram
     *
     * \param vlnv The vlnv to be set for the design
     *
     * \return QSharedPointer<Design> Pointer to the created design.
    */
    QSharedPointer<Design> createDesign(const VLNV &vlnv);

	/*! \brief Get pointer to the design configuration.
	 *
	 * \return Pointer to the design configuration. Null pointer if no design
	 * configuration is used.
	*/
	QSharedPointer<DesignConfiguration> getDesignConfiguration() const;

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

		/*! \brief This function should be called when user removes component instance.
	 *
	 * Removes the instance name from the list so the name can be used again in
	 * the design.
	 *
	 * \param name The name of the removed instance.
	 *
	*/
	void removeInstanceName(const QString& name);

	/*! \brief Update the list of instance names.
	 *
	 * \param oldName The old name of the component instance.
	 * \param newName The new name of the component instance.
	 *
	*/
	void updateInstanceName(const QString& oldName, const QString& newName);

    /*!
     *  Returns the diagram column layout.
     */
    DiagramColumnLayout* getColumnLayout();

    /*!
     *  Returns the edit provider.
     */
    GenericEditProvider& getEditProvider();

	/*! \brief Get pointer to the parent of this scene.
	 *
	 * \return Pointer to the design widget that owns this scene.
	*/
	virtual DesignWidget* parent() const;

    /*!
     *  Returns true if the diagram is in locked state.
     */
    bool isProtected() const;

signals:
    /*! \brief Signal openDesign is emitted when user double clicks on a hierarchical component
     *
     */
    void openDesign(const VLNV& vlnv, const QString& viewName);

	//! \brief Signal is emitted when component with given vlnv should be opened in editor.
	void openComponent(const VLNV& vlnv);

    //! Signaled when the bus with the given vlnv should be opened for editing.
    void openBus(VLNV const& vlnv, VLNV const& absDefVLNV, bool disableBusDef);

	//! \brief Emitted when user changes settings in blockDiagram
	void contentChanged();

	/*! \brief This signal is emitted when user wants to change mode
	 *
	 * \param mode Defines the mode to set
	 *
	*/
	void modeChanged(DrawMode mode);

	//! \brief Send an error message to the user.
	void errorMessage(const QString& errorMessage);

	//! \brief Send a notification to the user.
	void noticeMessage(const QString& noticeMessage);

	/*! \brief Emitted when user selects a component on the draw board.
	 *
	 * \param component Pointer to the DiagramComponent instance that is selected.
	 *
	*/
	void componentSelected(DiagramComponent* component);

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

	/*! \brief Emitted when user de-selects all items.
	 *
	*/
	void clearItemSelection();

	//! \brief Emitted when a new component is instantiated to the design.
	void componentInstantiated(DiagramComponent* comp);

	//! \brief Emitted when a component instance is removed.
	void componentInstanceRemoved(DiagramComponent* comp);

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

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);

    void disableHighlight();

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
    void drawBackground(QPainter* painter, const QRectF& rect);

private:

	//! \brief No copying
	BlockDiagram(const BlockDiagram& other);

	//! No assignment
	BlockDiagram& operator=(const BlockDiagram& other);

    /*! \brief Create an instanceName for the component that is being instantiated
	 *
	 * \param component Pointer to the component being instantiated.
	 *
	 * \return QString containing the name for the component instance
	*/
	QString createInstanceName(QSharedPointer<Component> component);

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

	//! \brief The pointer to the library handler that manages the library
    LibraryInterface *lh_;

	//! \brief Pointer to the parent of this scene.
	DesignWidget* parent_;

	//! \brief Contains the current draw mode of the editor
    DrawMode mode_;

    //! The connection that is being drawn.
    DiagramInterconnection *tempConnection_;

    //! The starting end point of a connection that is being drawn.
    DiagramConnectionEndPoint* tempConnEndPoint_;

    //! The potential end points that can be connected to the starting end point.
    QVector<DiagramConnectionEndPoint*> tempPotentialEndingEndPoints_;

    //! The highlighted end point to which the connection could be snapped automatically.
    DiagramConnectionEndPoint* highlightedEndPoint_;

	/*! \brief The map that contains the instance names for components
	 *
	 * Key = name of the component instance
	 * Value = the number of component instances with this name
	 * 
	 * This map is used to generate a running number for each component 
	 * instance so all instance names are unique but may contain the component's
	 * VLNV name.
	 */
	QStringList instanceNames_;

    //! The component that is being edited in the block diagram.
    QSharedPointer<Component> component_;

	//! \brief The design configuration of the edited design.
	QSharedPointer<DesignConfiguration> designConf_;

    //! The diagram column layout.
    QSharedPointer<DiagramColumnLayout> layout_;

    //! The type of the item being dragged.
    ColumnItemType dragCompType_;
    bool dragBus_;

    //! The edit provider for undo/redo.
    GenericEditProvider& editProvider_;

    //! If true, the diagram is locked and cannot be modified.
    bool locked_;
};

#endif // BLOCKDIAGRAM_H
