//-----------------------------------------------------------------------------
// File: ComponentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 6.11.2011
//
// Description:
// Base class for all graphics items that represent an IP-XACT component.
//-----------------------------------------------------------------------------

#ifndef COMPONENTITEM_H
#define COMPONENTITEM_H

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/Association/Associable.h>

#include <QGraphicsRectItem>
#include <QSharedPointer>

class Component;
class ComponentInstance;
class ConnectionEndpoint;
class LibraryInterface;
class IGraphicsItemStack;

//-----------------------------------------------------------------------------
//! ComponentItem class.
//-----------------------------------------------------------------------------
class ComponentItem : public QObject, public QGraphicsRectItem, public Associable
{
    Q_OBJECT 

public:
    /*!
     *  Constructor.
     *
     *      @param [in] size                      The initial rectangle size.
     *      @param [in] libInterface              The library interface.
     *      @param [in] component                 The actual component.
     *      @param [in] instanceName              The name of the component instance.
     *      @param [in] parent                    The parent graphics item.
     */


    ComponentItem(QRectF const& size,
        LibraryInterface* libInterface,
        QSharedPointer<ComponentInstance> instance,
        QSharedPointer<Component> component,
        QGraphicsItem* parent);

	/*!
     *  Destructor.
     */
	virtual ~ComponentItem();

    /*!
     *  Updates the component item to reflect the current state of the component model.
     */
    virtual void updateComponent();

    /*!
     *  Updates the size of the component box.
     */
    void updateSize();

    /*!
	 *  Returns the height of the component box.
	 */
    virtual qreal getHeight();
    
    /*!
	 *  Returns the width of the component box.
	 */
    virtual qreal getWidth();

	/*!
     *  Sets the instance name.
     *
     *      @param [in] newName The instance name to set.
     */
    void setName(QString const& newName);

    /*!
     *  Sets the display name for the component instance.
     *
     *      @param [in] displayName The display name to set.
	 */
	void setDisplayName(QString const& displayName);

    /*!
     *  Sets the description of the component instance.
     *
     *      @param [in] description The description to set.
     */
	void setDescription(const QString& description);

    /*!
     *  Returns the instance name.
     */
    QString name() const;

	/*!
     *  Returns the display name of the component instance.
     */
	QString displayName() const;

	/*!
     *  Returns the description of the component instance.
     */
	QString description() const;

    /*!
     *  Returns the IP-XACT component model.
     */
    QSharedPointer<Component> componentModel() const;

    /*!
     *  Returns the IP-XACT component instance.
     */
    QSharedPointer<ComponentInstance> getComponentInstance() const;

	/*! \brief Get list of views the component has.
	 *
	 * \return QStringList containing the names of the views for the component.
	*/
	QStringList getViews() const;

    /*!
     *  Returns the list of endpoints in the component.
     */
    QList<ConnectionEndpoint*> getEndpoints() const;

    /*!
     *  Returns the parent graphics item stack.
     */
    IGraphicsItemStack* getParentStack();

	/*! \brief Get the uuid of the instance.
     *
	 * \return QString containing uuid.
	 */
	QString getUuid() const;

    /*!
     *  Defines the connection point for associations in scene coordinates.
     *     
     *      @param [in] otherEnd   The position of the other end connection point.
     *
     *      @return The connection point of the item.
     */
    virtual QPointF connectionPoint(QPointF const& otherEnd = QPointF()) const;

    /*!
     *  Marks the component as a packetized component.
     */
    virtual void setPackaged() = 0;

    /*!
     *  Marks the component as a draft component.
     */
    virtual void setDraft() = 0;
	
    // The basic width of the components.
    static const int COMPONENTWIDTH = 220;

signals:
    //! Emitted when an endpoint has been moved.
    void endpointMoved(ConnectionEndpoint* endpoint);

    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage) const;

	//! \brief Emitted when the name of the component item changes.
	void nameChanged(QString const& newName, QString const& oldName);

	//! \brief Emitted when the display name changes.
	void displayNameChanged(QString const& newName);

	//! \brief Emitted when the description changes.
	void descriptionChanged(QString const& newDescription);

	//! \brief Emitted when the configurable elements change.
	void confElementsChanged(QMap<QString, QString> const& confElements);

	//! \brief Emitted right before this component is destroyed.
	void destroyed(ComponentItem* comp);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    /*!
     *  Returns the library interface.
     */
    LibraryInterface* getLibraryInterface();

    /*!
     *  Updates the name label with the given text.
     *
     *      @param [in] text The text to display in the label.
     */
    virtual void updateNameLabel(QString const& text);

private:
    // Disable copying.
    ComponentItem(ComponentItem const& rhs);
    ComponentItem& operator=(ComponentItem const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libInterface_;

    //! The component model.
    QSharedPointer<Component> component_;

    //! The component instance.
    QSharedPointer<ComponentInstance> componentInstance_;

    //! The name label.
    QGraphicsTextItem* nameLabel_;

};

//-----------------------------------------------------------------------------

#endif // COMPONENTITEM_H
