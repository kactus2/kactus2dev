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

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/NameGroup.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QPointF>
#include <QMap>

//-----------------------------------------------------------------------------
//! IP-XACT component instance class.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentInstance : public Extendable, public NameGroup
{
public:

    struct PowerDomainLink
    {
        QString externalReference_;
        QString internalReference_;

        PowerDomainLink* clone() const { return new PowerDomainLink({ externalReference_, internalReference_ }); };
    };

    /*!
     *  The default constructor.
     */
    ComponentInstance();

    /*!
     *  The constructor.
     *
     *      @param [in] instanceName    The name of the component instance.
     *      @param [in] componentRef    The referenced component.
     */
    ComponentInstance(QString const& instanceName, QSharedPointer<ConfigurableVLNVReference> componentRef);

    /*!
     *  Copy constructor.
     */
    ComponentInstance(ComponentInstance const& other);

    /*!
     *  The destructor.
     */
    ~ComponentInstance() final = default;

    /*!
     *  Assignment operator.
     */
    ComponentInstance& operator=(ComponentInstance const& other);

    /*!
     *  Sets the name of the instance.
     *
     *      @param [in] name The name to set.
     */
    void setInstanceName(QString const& name);
   
    /*!
     *  Returns the name of the instance.
     */
    QString getInstanceName() const;

    /*!
     *  Sets the component reference.
     *
     *      @param [in] vlnv The referenced component VLNV.
     */
    void setComponentRef(QSharedPointer<ConfigurableVLNVReference> newComponentRef);

    /*!
     *  Returns the component reference.
     */
    QSharedPointer<ConfigurableVLNVReference> getComponentRef() const;

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

    /*!
     *  Sets the configurable element values.
     *
     *      @param [in] values The configurable element values to set.
     */
    void setConfigurableElementValues(QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > newValues);

    /*!
     *  Returns the configurable element values.
     */
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > getConfigurableElementValues() const;

    QSharedPointer<QList<QSharedPointer<PowerDomainLink> > > getPowerDomainLinks() const;

    /*!
     *  Sets the global position of the instance.
     *
     *      @param [in] pos The global position in the design, in pixels.
     */
    void setPosition(QPointF const& pos);

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
     *  Remove bus interface position.
     *
     *      @param [in] interfaceName    Name of the removed bus interface.
     */
    void removeBusInterfacePosition(QString const& interfaceName);

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
     *  Remove API interface position.
     *
     *      @param [in] name    Name of the selected API interface.
     */
    void removeApiInterfacePosition(QString const& name);

    /*!
     *  Updates the position of the API interface with the given name.
     *
     *      @param [in] name The name of the API interface.
     *      @param [in] pos  The local position of the API interface.
     */
    void updateComInterfacePosition(QString const& name, QPointF const& pos);

    /*!
     *  Remove COM interface position.
     *
     *      @param [in] name    Name of the selected COM interface.
     */
    void removeComInterfacePosition(QString const& name);

    /*!
     *  Returns the global position of the instance in the design.
     */
    QPointF getPosition() const;

    /*!
     *  Set the imported value for the component instance.
     *
     *      @param [in] isImported  True if the component is imported, false otherwise.
     */
    void setImported(bool isImported);

    /*!
     *  Returns true if the instance is an imported one.
     *
     *      @return True if the component is imported, false otherwise.
     */
    bool isImported() const;

    /*!
     *  Returns the name of the import source instance.
     */
    QString getImportRef() const;

    /*!
     *  Returns the property values.
     */
    QMap<QString, QString> getPropertyValues() const;

    /*!
     *  Returns the local bus interface positions of the instance in the design.
     */
    QMap<QString, QPointF> getBusInterfacePositions() const;

    /*!
     *  Returns the local ad-hoc port positions of the instance in the design.
     */
    QMap<QString, QPointF> getAdHocPortPositions() const;

    /*!
     *  Returns the local API interface positions of the instance in the design.
     */
    QMap<QString, QPointF> getApiInterfacePositions() const;

    /*!
     *  Returns the local COM interface positions of the instance in the design.
     */
    QMap<QString, QPointF> getComInterfacePositions() const;

    /*!
     *  Returns the port ad-hoc visibilities.
     */
    QMap<QString, bool> getPortAdHocVisibilities() const;
    
    /*!
     *  Get the Uuid of the instance.
	 *
     *     @return QString containing the uuid.
	 */
    QString getUuid() const;
        
    /*!
     *  Set the universally unique identifier (Kactus2 extension).
     *
     *      @param [in] newUuid     The new unique identifier.
     */
    void setUuid(QString const& newUuid);

    /*!
     *  Checks if the component instance is a draft.
     *
     *      @return True, if the instance is a draft, otherwise false.
     */
    bool isDraft() const;
     
    /*!
     *  Set the draft status of a component instance (Kactus2 extension).
     *
     *      @param [in] instanceIsDraft     True for draft component instance, false for normal.
     */
    void setDraft(bool instanceIsDraft);

    /*!
     *  Hides an ad hoc port in the instance.
     *
     *      @param [in] portName   The name of the port to hide.
     */
    void hideAdHocPort(QString const& portName);

    /*!
     *  Sets the file set reference.
     *
     *      @param [in] fileSetName The name of the referenced file set in the top-level component.
     */
    void setFileSetRef(QString const& fileSetName);

    /*!
     *  Sets the HW mapping information for the SW instance.
     *
     *      @param [in] hwRef The name of the HW component instance to which the SW instance is mapped.
     */
    void setMapping(QString const& hwRef);

    /*!
     *  Returns the file set reference (i.e. the name of the referenced file set).
     */
    QString getFileSetRef() const;

    /*!
     *  Returns the name of the referenced HW component instance onto which the SW instance is mapped.
     */
    QString getMapping() const;

private:

    /*!
     *  Update a positions map extension (Kactus2 extension).
     *
     *      @param [in] newReferenceName        The name of the new reference.
     *      @param [in] newPosition             The position of the new reference.
     *      @param [in] groupIdentifier         The identifier for the group.
     *      @param [in] itemIdentifier          The identifier for the item.
     *      @param [in] referenceIdentifier     The identifier for the reference.
     */
    void updatePositionsMap(QString const& newReferenceName, QPointF const& newPosition,
        QString const& groupIdentifier, QString const& itemIdentifier, QString const& referenceIdentifier) const;
 
    void removePosition(QString const& interfaceName, QString const& groupIdentifier, QString const& itemIdentifier,
        QString const& referenceIdentifier);

    /*!
     *  Get a map of positions (Kactus2 extension).
     *
     *      @param [in] groupIdentifier         The identifier of the position group.
     *      @param [in] itemIdentifier          The identifier of the position item.
     *      @param [in] referenceIdentifier     The identifier of the position reference.
     */
    QMap<QString, QPointF> getPositionMap(QString const& groupIdentifier, QString const& itemIdentifier,
        QString const& referenceIdentifier) const;
    

    /*!
     * Sets the Kactus2 vendor extension value. If the extension does not exist, it is created.
     *
     *     @param [in] value            The value to set.
     *     @param [in] extensionType    The type of the vendor extension.
     */
    void setValueExtension(QString const& value, QString const& extensionType);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The presence.
    QString isPresent_;

    //! The referenced component.
    QSharedPointer<ConfigurableVLNVReference> componentRef_;

    //! The power domain links of the component instance.
    QSharedPointer<QList<QSharedPointer<PowerDomainLink> > > powerDomainLinks_ =
        QSharedPointer<QList<QSharedPointer<PowerDomainLink> > >(new QList<QSharedPointer<PowerDomainLink> >());
};

//-----------------------------------------------------------------------------

#endif // COMPONENTINSTANCE_H
