//-----------------------------------------------------------------------------
// File: ComponentInstance.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.6.2012
//
// Description:
// IP-XACT component instance class.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H

#include <library/LibraryManager/vlnv.h>

#include <common/Global.h>

#include <QString>
#include <QSharedPointer>
#include <QPointF>
#include <QMap>
#include <QDomNode>
#include <QXmlStreamWriter>

class VendorExtension;
class Kactus2Position;
class Kactus2Value;
class Kactus2Placeholder;

//-----------------------------------------------------------------------------
//! IP-XACT component instance class.
//-----------------------------------------------------------------------------
class KACTUS2_API ComponentInstance
{
public:
    /*!
     *  Convenience constructor for initializing fields.
     */
    ComponentInstance(QString instanceName, QString displayName,
                      QString description, VLNV const& componentRef,
                      QPointF const& position, const QString& uuid);

    /*!
     *  Copy constructor.
     */
    ComponentInstance(ComponentInstance const& other);

    /*!
     *  Constructor.
     *
     *      @param [in] node A reference to a QDomNode where the information should be parsed from.
     */
    ComponentInstance(QDomNode& node);

    /*!
     *  Assignment operator.
     */
    ComponentInstance& operator=(ComponentInstance const& other);

    /*!
     *  Writes the contents to an XML stream.
     *
     *      @param [in] writer The XML stream writer.
     */
    void write(QXmlStreamWriter& writer) const;

    /*!
     *  Checks whether the component instance is in a valid state or not.
     *
     *      @param [out] errorList         The list to add the possible error messages to.
     *      @param [in]  parentIdentifier  String from parent to help to identify the location of the error.
     *
     *      @return True, if the state is valid.
     */
    bool isValid(QStringList& errorList, QString const& parentIdentifier) const;

    /*!
     *  Checks whether the component instance is in a valid state or not.
     *
     *      @return True, if the state is valid.
     */
    bool isValid() const;

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
    void setComponentRef(VLNV const& vlnv);

    /*!
     *  Sets the configurable element values.
     *
     *      @param [in] values The configurable element values to set.
     */
    void setConfigurableElementValues(QMap<QString, QString> const& values);

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
    VLNV const& getComponentRef() const;

    /*!
     *  Returns the configurable element values.
     */
    QMap<QString, QString> const& getConfigurableElementValues() const;

	 /*! \brief Check if the instance has the specified configurable element value defined.
	  *
	  * Method: 		hasConfElementValue
	  * Full name:	ComponentInstance::hasConfElementValue
	  * Access:		public 
	  *
	  * \param confElementName Identifies the configurable element.
	  *
	  * \return True if the configurable element value is found.
	 */
	 bool hasConfElementValue(const QString& confElementName) const;

	 /*! \brief Get the configurable element value if one exists.
	  *
	  * Method: 		getConfElementValue
	  * Full name:	ComponentInstance::getConfElementValue
	  * Access:		public 
	  *
	  * \param confElementName Identifies the configurable element value.
	  *
	  * \return QString containing the configurable element value, empty if not found.
	 */
	 QString getConfElementValue(const QString& confElementName) const;

    /*!
     *  Returns the global position of the instance in the design.
     */
     QPointF getPosition() const;

    /*!
     *  Returns true if the instance is an imported one.
     */
    bool isImported() const;

    /*!
     *  Returns the name of the import source instance.
     */
    QString getImportRef() const;

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

	 /*! \brief Get the Uuid of the instance.
	  *
	  * Method: 		getUuid
	  * Full name:	ComponentInstance::getUuid
	  * Access:		public 
	  *
	  *
	  * \return QString containing the uuid.
	 */
	 QString getUuid() const;

     /*!
      *  Sets the vendor extensions of the component instance.
      *
      *      @param [in] vendorExtensions   The vendor extensions to set.
      */
     void setVendorExtensions(QList<QSharedPointer<VendorExtension> > const& vendorExtensions);

    /*!
     *  Returns the vendor extensions of the component instance.
     *
     *      @return The vendor extensions.
     */
    QList<QSharedPointer<VendorExtension> > getVendorExtensions() const;

private:
    /*!
    *  Parses the property values from the given XML node.
    */
    void parsePropertyValues(QDomNode& node);

    /*!
     *  Parses the vendor extensions from a DOM node.
     *
     *      @param [in] extensionsNode   The DOM node containing all vendor extensions.
     */
    void parseVendorExtensions(QDomNode const& extensionsNode);

    /*!
     *  Copies vendor extensions from a list of vendor extension.
     *
     *      @param [in] extensions   The list to copy extensions from.          
     */
    void copyVendorExtensions(QList<QSharedPointer<VendorExtension> > const& extensions);

    /*!
     *  Creates a vendor extension for import.
     */
    void createImportVendorExtension();

    /*!
     *  Creates a vendor extension for default position.
     *
     *      @param [in] position   The initial default position.          
     */
    void createPositionVendorExtension(QPointF const& position);

    /*!
     *  Creates a vendor extension for UUID.
     */
    void createUuidVendorExtension();
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The instance name (spirit:instanceName).
    QString instanceName_;

    //! The display name (spirit:displayName).
    QString displayName_;

    //! The description of the instance (spirit:description).
    QString desc_;

    //! The VLNV reference to a component (spirit:componentRef).
    VLNV componentRef_;

    /*! \brief OPTIONAL spirit:configurableElementValues
    *
    * Specifies the configuration for a specific component
    * instance by providing the value of a specific component
    * parameter. Key of the map is a reference to the id
    * attribute of an element in the component instance and the
    * value is the value assigned.
    */
    QMap<QString, QString> configurableElementValues_;

    //! The global position of the instance in the design (kactus2:position).
    QSharedPointer<Kactus2Position> pos_;

    //! If true, the instance is an imported one (kactus2:imported).
    QSharedPointer<Kactus2Placeholder> imported_;

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
	QSharedPointer<Kactus2Value> uuid_;

    /*!
     * OPTIONAL spirit: vendorExtensions
     * Component instance vendor extensions.
     */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;
};

//-----------------------------------------------------------------------------

#endif // COMPONENTINSTANCE_H
