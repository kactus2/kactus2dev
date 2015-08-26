//-----------------------------------------------------------------------------
// File: ComponentInstance.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.08.2015
//
// Description:
// Describes the ipxact:componentInstance element.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H

#include <IPXACTmodels/vlnv.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/Extendable.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QPointF>
#include <QMap>

//-----------------------------------------------------------------------------
//! IP-XACT component instance class.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentInstance : public Extendable
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] instanceName    The name of the component instance.
     *      @param [in] displayName     The display name.
     *      @param [in] description     Description of the instance.
     *      @param [in] componentRef    The name of the referenced component.
     *      @param [in] position        The position of the component instance.
     *      @param [in] uuid            The identifier of the instance.
     */
    ComponentInstance(QString instanceName, QString displayName, QString description, VLNV const& componentRef,
                      QPointF const& position, const QString& uuid);

    /*!
     *  Copy constructor.
     */
    ComponentInstance(ComponentInstance const& other);

    /*!
     *  The constructor.
     */
    ComponentInstance();

    /*!
     *  The destructor.
     */
    ~ComponentInstance();

    /*!
     *  Checks whether the component instance is in a valid state or not.
     *
     *      @param [out] errorList         The list to add the possible error messages to.
     *      @param [in]  parentIdentifier  String from parent to help to identify the location of the error.
     *
     *      @return True, if the state is valid.
     */
    //bool isValid(QStringList& errorList, QString const& parentIdentifier) const;

    /*!
     *  Checks whether the component instance is in a valid state or not.
     *
     *      @return True, if the state is valid.
     */
    //bool isValid() const;

    /*!
     *  Sets the name of the instance.
     *
     *      @param [in] name The name to set.
     */
    void setInstanceName(QString const& name);

    /*!
     *  Sets the display name of the instance.
     *
     *      @param [in] displayName The display name to set.
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Sets the description of the instance.
     *
     *      @param [in] description The description to set.
     */
    void setDescription(QString const& description);

    /*!
     *  Sets the component reference.
     *
     *      @param [in] vlnv The referenced component VLNV.
     */
    void setComponentRef(QSharedPointer<ConfigurableVLNVReference> newComponentRef);

    /*!
     *  Sets the configurable element values.
     *
     *      @param [in] values The configurable element values to set.
     */
    void setConfigurableElementValues(QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > newValues);

    /*!
     *  Sets the global position of the instance.
     *
     *      @param [in] pos The global position in the design, in pixels.
     */
    void setPosition(QPointF const& pos);

    /*!
     *  Sets the flag whether the instance is an imported one and should be auto-synced.
     *
     *      @param [in] imported If true, the instance is set as imported.
     */
    void setImported(bool imported);

    /*!
     *  Sets the name of the import source instance.
     *
     *      @param [in] nameRef The name of the import source instance.
     */
    void setImportRef(QString const& nameRef);

    /*!
     *  Sets the property values.
     *
     *      @param [in] values The property values.
     */
    void setPropertyValues(QMap<QString, QString> const& values);

    /*!
     *  Updates the position of the bus interface with the given name.
     *
     *      @param [in] name The name of the bus interface.
     *      @param [in] pos  The local position of the bus interface.
     */
    void updateBusInterfacePosition(QString const& name, QPointF const& pos);

    /*!
     *  Updates the position of the ad-hoc port with the given name.
     *
     *      @param [in] name The name of the ad-hoc port.
     *      @param [in] pos  The local position of the ad-hoc port.
     */
    void updateAdHocPortPosition(QString const& name, QPointF const& pos);

    /*!
     *  Updates the position of the API interface with the given name.
     *
     *      @param [in] name The name of the API interface.
     *      @param [in] pos  The local position of the API interface.
     */
    void updateApiInterfacePosition(QString const& name, QPointF const& pos);

    /*!
     *  Updates the position of the API interface with the given name.
     *
     *      @param [in] name The name of the API interface.
     *      @param [in] pos  The local position of the API interface.
     */
    void updateComInterfacePosition(QString const& name, QPointF const& pos);

    /*!
     *  Sets the port ad-hoc visibilities.
     *
     *      @param [in] visibilities The port ad-hoc visibilities to set.
     */
    void setPortAdHocVisibilities(QMap<QString, bool> const& visibilities);

    /*!
     *  Returns the name of the instance.
     */
    QString const& getInstanceName() const;
    
    /*!
     *  Returns the display name of the instance.
     */
    QString const& getDisplayName() const;

    /*!
     *  Returns the description of the instance.
     */
    QString const& getDescription() const;

    /*!
     *  Returns the component reference.
     */
    QSharedPointer<ConfigurableVLNVReference> getComponentRef() const;

    /*!
     *  Returns the configurable element values.
     */
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > getConfigurableElementValues() const;

    /*!
     *  Returns the global position of the instance in the design.
     */
    QPointF const& getPosition() const;

    /*!
     *  Returns true if the instance is an imported one.
     */
    bool isImported() const;

    /*!
     *  Returns the name of the import source instance.
     */
    QString const& getImportRef() const;

    /*!
     *  Returns the property values.
     */
    QMap<QString, QString> const& getPropertyValues() const;

    /*!
     *  Returns the local bus interface positions of the instance in the design.
     */
    QMap<QString, QPointF> const& getBusInterfacePositions() const;

    /*!
     *  Returns the local ad-hoc port positions of the instance in the design.
     */
    QMap<QString, QPointF> const& getAdHocPortPositions() const;

    /*!
     *  Returns the local API interface positions of the instance in the design.
     */
    QMap<QString, QPointF> const& getApiInterfacePositions() const;

    /*!
     *  Returns the local COM interface positions of the instance in the design.
     */
    QMap<QString, QPointF> const& getComInterfacePositions() const;

    /*!
     *  Returns the port ad-hoc visibilities.
     */
    QMap<QString, bool> const& getPortAdHocVisibilities() const;

    /*!
     *  Assignment operator.
     */
    ComponentInstance& operator=(ComponentInstance const& other);

	 /*!
	  *  Get the Uuid of the instance.
	  *
      *     @return QString containing the uuid.
	  */
	 QString getUuid() const;

     /*!
      *  Checks if the component instance is a draft.
      *
      *      @return True, if the instance is a draft, otherwise false.
      */
     bool isDraft() const;

    /*!
     *  Get the presence of the instance.
     *
     *      @return A QString containing the presence.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence of the instance.
     *
     *      @param [in] newIsPresent    The new value for the presence.
     */
    void setIsPresent(QString const& newIsPresent);

private:
    
    /*!
    *  Parses the property values from the given XML node.
    */
    void parsePropertyValues(QDomNode& node);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The instance name.
    QString instanceName_;

    //! The display name.
    QString displayName_;

    //! The description of the instance .
    QString desc_;

    //! The presence.
    QString isPresent_;

    //! The referenced component.
    QSharedPointer<ConfigurableVLNVReference> componentRef_;

    //! The global position of the instance in the design (kactus2:position).
    QPointF pos_;

    //! If true, the instance is an imported one (kactus2:imported).
    bool imported_;

    //! The name of the import source instance (kactus2:importRef).
    QString importRef_;

    //! Port positions for the component instance's bus interface (kactus2:portPositions).
    QMap<QString, QPointF> portPositions_;

    //! Positions for the visible ad-hoc ports.
    QMap<QString, QPointF> adHocPortPositions_;

    //! API interface positions.
    QMap<QString, QPointF> apiInterfacePositions_;

    //! COM interface positions.
    QMap<QString, QPointF> comInterfacePositions_;

    //! Ad-hoc visibility of each port (kactus2:adHocVisibilities).
    QMap<QString, bool> portAdHocVisibilities_;

    //! SW property values for this instance (kactus2:propertyValues).
    QMap<QString, QString> swPropertyValues_;

	//! \brief The unique id used to identify the instance.
	 QString uuid_;
};

//-----------------------------------------------------------------------------

#endif // COMPONENTINSTANCE_H
