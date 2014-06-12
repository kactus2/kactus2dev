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
#include <designEditors/common/Associable.h>

#include <QGraphicsRectItem>
#include <QSharedPointer>

class Component;
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
     *      @param [in] displayName               The component instance's display name.
     *      @param [in] description               The component instance's description.
     *      @param [in] uuid							  The uuid of the instance.
     *      @param [in] configurableElementValue  The component instance's configurable element values.
     *      @param [in] parent                    The parent graphics item.
     */
    ComponentItem(QRectF const& size,
                  LibraryInterface* libInterface,
                  QSharedPointer<Component> component,
                  QString const& instanceName = QString("instance"),
                  QString const& displayName = QString(),
                  QString const& description = QString(),
                  QString const& uuid = QString(),
                  QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>(),
                  QGraphicsItem *parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~ComponentItem();

    /*!
     *  Updates the component item to reflect the current state of the component model.
     */
    virtual void updateComponent();

    /*!
     *  Sets the instance name.
     *
     *      @param [in] name The instance name to set.
     */
    void setName(QString const& name);

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
     *  Sets the configurable elements of the component instance.
     *
     *      @param [in] confElements The map of elements to set.
     */
	void setConfigurableElements(const QMap<QString, QString>& confElements);

    /*!
     *  Returns the instance name.
     */
    QString name() const;

	/*!
     *  Returns the display name of the component instance.
     */
	QString const& displayName() const;

	/*!
     *  Returns the description of the component instance.
     */
	QString const& description() const;

    /*!
     *  Returns the configurable elements of the component instance.
     */
	QMap<QString, QString>& getConfigurableElements();

    /*!
     *  Returns the configurable elements of the component instance.
     */
	QMap<QString, QString> const& getConfigurableElements() const;

	/*!
     *  Returns the IP-XACT component model.
     */
    QSharedPointer<Component> componentModel();

    /*!
     *  Returns the IP-XACT component model.
     */
    QSharedPointer<Component const> componentModel() const;

    /*!
     *  Returns the library interface.
     */
    LibraryInterface* getLibraryInterface();

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

    /*!
     *  Returns true if the connections should not be updated automatically in
     *  the port's itemChange() function. Otherwise false.
     */
    virtual bool isConnectionUpdateDisabled() const = 0;

	/*! \brief Get the uuid of the instance.
	 *
	 * Method: 		getUuid
	 * Full name:	ComponentItem::getUuid
	 * Access:		public 
	 *
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

    //! The instance name.
    QString name_;

    //! The name label.
    QGraphicsTextItem* nameLabel_;

    //! \brief The display name for the component instance.
	QString displayName_;

	//! \brief The description for the component instance.
	QString description_;

	//! \brief Map containing the configurableElementValues for the instance.
	QMap<QString, QString> configurableValues_;

	//! \brief The uuid identifying the instance.
	QString uuid_;
};

//-----------------------------------------------------------------------------

#endif // COMPONENTITEM_H
