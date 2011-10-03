/* 
 *
 * 		filename: diagramcomponent.h
 */

#ifndef DIAGRAMCOMPONENT_H
#define DIAGRAMCOMPONENT_H

#include <QGraphicsRectItem>
#include <QSharedPointer>

#include "common/diagramgrid.h"

class Component;
class DiagramPort;
class DiagramColumn;
class LibraryInterface;

/*! \brief DiagramComponent represents graphically an IP-XACT component instance
 *
 */
class DiagramComponent : public QObject, public QGraphicsRectItem
{
    Q_OBJECT 

public:
    enum { Type = UserType + 1 };

    DiagramComponent(LibraryInterface* lh,
                     QSharedPointer<Component> component,
                     const QString &instanceName = QString("unnamed"),
                     const QString &displayName = QString(),
                     const QString &description = QString(),
                     const QMap<QString, QString> &configurableElementValues = QMap<QString, QString>(),
                     QGraphicsItem *parent = 0);

	//! \brief The destructor
	virtual ~DiagramComponent();

    /*!
     *  Adds a new, empty port to the component. This function creates automatically an empty
     *  bus interface to the component model.
     *
     *      @param [in] pos The position hint for the port.
     *
     *      @return The newly created port.
     */
    DiagramPort* addPort(QPointF const& pos);

    /*!
     *  Adds an already created port to the component.
     *
     *      @param [in] port The port to add. Must not be used in any other component.
     */
    void addPort(DiagramPort* port);

    /*!
     *  Removes the given port from the component.
     *
     *      @param [in] port The port to remove.
     */
    void removePort(DiagramPort* port);

    /*! \brief Get the DiagramPort that corresponds to the given bus interface name
     *
     */
    DiagramPort *getBusPort(const QString &name);

    /*! \brief Return the instance name
     *
     */
    QString name();

    /*! \brief Set the instance name
     *
     */
    void setName(QString name);

	/*! \brief Get the display name of the component instance.
	 *
	 * \return QString containing the display name.
	*/
	QString displayName() const;

	/*! \brief Set the display name for the component instance.
	 *
	 * \param displayName The display name for the component instance.
	 *
	*/
	void setDisplayName(const QString& displayName);

	/*! \brief Get the description of the component instance.
	 *
	 * \return QString containing the description for the component instance.
	*/
	QString description() const;

	/*! \brief Set the description for the component instance.
	 *
	 * \param description The description for the component instance.
	 *
	*/
	void setDescription(const QString& description);

    /*! \brief Return the IP-XACT component model
     *
     */
    QSharedPointer<Component> componentModel();

	/*! \brief Get list of views the component has.
	 *
	 * \return QStringList containing the names of the views for the component.
	*/
	QStringList getViews() const;

    int type() const { return Type; }

    /*! Called when a port is being moved.
     *
     *      @param [in] port The port that is being moved.
     */
    void onMovePort(DiagramPort* port);

    /*!
     *  Updates the diagram component to reflect the current state of the component model.
     */
    void updateComponent();

    /*!
     *  Returns true if the connections should not be updated automatically in
     *  the diagram port's itemChange() function. Otherwise false.
     */
    bool isConnectionUpdateDisabled() const;

	/*! \brief Get the configurable elements of this component instance.
	 *
	 * \return QMap containing the name-value pairs of configurable elements.
	*/
	QMap<QString, QString>& getConfigurableElements();

	/*! \brief Set the configurable elements for this component instance.
	 *
	 * \param confElements Reference to QMap that contains the configurable elements to set.
	 *
	*/
	void setConfigurableElements(const QMap<QString, QString>& confElements);

signals:
    //! Signals that the contents of the component have been changed.
    void contentChanged();

	//! \brief Emitted when the name of the diagramComponent changes.
	void nameChanged(const QString& newName, const QString& oldName);

	//! \brief Emitted when the display name changes.
	void displayNameChanged(const QString& newName);

	//! \brief Emitted when the description changes.
	void descriptionChanged(const QString& newDescription);

	//! \brief Emitted when the configurable elements change.
	void confElementsChanged(const QMap<QString, QString>& confElements);

	//! \brief Emitted right before this diagram component is destroyed.
	void destroyed(DiagramComponent* diaComp);

protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    // Called when the user presses the mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
	
	//! \brief No copying
	DiagramComponent(const DiagramComponent& other);

	//! No assignment
	DiagramComponent& operator=(const DiagramComponent& other);

    /*!
     *  Adds a port to the correct port stack.
     *
     *      @param [in] port   The port to add.
     *      @param [in] right  If true, the port is added to the right port stack. If false, it is
     *                         added to the left port stack.
     */
    void onAddPort(DiagramPort* port, bool right);

    /*!
     *  Updates the size of the component based on the port positions.
     */
    void updateSize();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    enum
    {
        SPACING = 8,
        MIN_Y_PLACEMENT = 3 * GridSize
    };

    LibraryInterface* lh_;
    QSharedPointer<Component> component_;
    QGraphicsTextItem *nameLabel_;
    QGraphicsPixmapItem* hierIcon_;

    //! The old column from where the mouse drag event began.
    DiagramColumn* oldColumn_;

    //! The left and right port stacks.
    QList<DiagramPort*> leftPorts_;
    QList<DiagramPort*> rightPorts_;
    bool connUpdateDisabled_;
    QPointF oldPos_;

	//! \brief The display name for the component instance.
	QString displayName_;

	//! \brief The description for the component instance.
	QString description_;

	//! \brief Map containing the configurableElementValues for the instance.
	QMap<QString, QString> configurableValues_;
};

#endif // DIAGRAMCOMPONENT_H
