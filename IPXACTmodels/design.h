#ifndef DESIGN_H
#define DESIGN_H

#include "SWInstance.h"
#include "ApiDependency.h"
#include "HierApiDependency.h"
#include "ComConnection.h"
#include "HierComConnection.h"
#include "ColumnDesc.h"
#include "ComponentInstance.h"

#include "librarycomponent.h"
#include <library/LibraryManager/vlnv.h>

#include <designEditors/common/ColumnTypes.h>
#include <common/Global.h>
#include <common/KactusAttribute.h>

#include <QSharedPointer>
#include <QList>
#include <QString>
#include <QMap>
#include <QDomDocument>
#include <QDomNode>
#include <QStringList>
#include <QFile>
#include <QPointF>
#include <QVector2D>


/*! \brief Describes an IP-XACT design
 *
 * Describes the spirit:design element in an IP-XACT document
 */
class KACTUS2_API Design: public LibraryComponent {

public:
    /*! \brief Describes the spirit:interface type in an IP-XACT
	* document
	*
	* Specifies the bus interface of a design component instance that
	* is part of an interconnection.
	*/
	struct KACTUS2_API Interface {
		/*! \brief MANDATORY spirit:componentRef
		* 
		* References the instance name of a component.
		*/
		QString componentRef;

		/*! \brief MANDATORY spirit:busRef
		*
		* References one of the component bus interfaces.
		*/
		QString busRef;

		/*! \brief The constructor
		*
		* \param interfaceNode A reference to a QDomNode where the
		* information should be parsed from.
		*/
		Interface(QDomNode &interfaceNode);

		/*! \brief Convenience constructor for initializing fields
		*/
		Interface(QString componentRef_ = QString(""), QString busRef_ = QString(""));

		//! \brief Copy constructor
		Interface(const Interface& other);

		//! \brief Assignment operator
		Interface& operator=(const Interface& other);

		/*! \brief The operator ==
		 *
		 * Method: 		operator==
		 * Full name:	Design::Interface::operator==
		 * Access:		public 
		 *
		 * \param other Reference to the Interface to compare to.
		 *
		 * \return True if the interfaces refer to same instance and bus interface.
		*/
		bool operator==(const Interface& other);

		/*! \brief The operator !=
		 *
		 * Method: 		operator!=
		 * Full name:	Design::Interface::operator!=
		 * Access:		public 
		 *
		 * \param other Reference to the Interface to compare to.
		 *
		 * \return True if the interfaces refer to different instance or at least different bus interface.
		*/
		bool operator!=(const Interface& other);

		/*! \brief The operator <
		 *
		 * Method: 		operator<
		 * Full name:	Design::Interface::operator<
		 * Access:		public 
		 *
		 * \param other Reference to the Interface to compare to.
		 *
		 * \return The operator uses the < operator of QString to compare.
		*/
		bool operator<(const Interface& other);

		/*! \brief Check if the interface is in a valid state.
		* 
		* \param instanceNames Contains the names of component instances in containing design.
		* \param errorList The list to add the possible error messages to.
		* \param parentIdentifier String from parent to help to identify the location of the error.
		*
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(const QStringList& instanceNames,
			QStringList& errorList, 
			const QString& parentIdentifier) const;

		/*! \brief Check if the interface is in a valid state.
		* 
		* \param instanceNames Contains the names of component instances in containing design.
		* 
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(const QStringList& instanceNames) const;
	};


	/*! \brief Describes the spirit:interconnection element in an
	* IP-XACT document
	*
	* Specifies a connection between one bus interface of a component
	* and another bus interface of a component
	*/
	struct Interconnection {

		/*! \brief MANDATORY spirit:name
		*
		* Unique name of the interconnection
		*/
		QString name;

		/*! \brief OPTIONAL spirit:displayName
		*
		* Allows a short descriptive text to be associated with the
		* interconnection.
		*/
		QString displayName;

		/*! \brief OPTIONAL spirit:description
		*
		* Allows a textual description of the interconnection.
		*/
		QString description;

		/*! \brief First of the two interfaces that form this
		* interconnection
		*/
		Interface interface1;

		/*! \brief Second of the two interfaces that form this
		* interconnection
		*/
		Interface interface2;

        /*! \brief VENDOREXT kactus2:route
         *
         *  The diagram route for the interconnection.
         */
        QList<QPointF> route;

        /*! \brief VENDOREXT kactus2:offPage
         *
         *  If true, the route is treated as an off-page connection.
         */
        bool offPage;

		/*! \brief The constructor
		*
		* \param interconnectionNode A reference to a QDomNode where
		* the information should be parsed from
		*/
		Interconnection(QDomNode &interconnectionNode);

		/*! \brief Convenience constructor for initializing fields
		*/
		Interconnection(QString name,
			Interface interface1,
			Interface interface2,
            QList<QPointF> const& route = QList<QPointF>(),
            bool offPage = false,
			QString displayName = QString(),
			QString description = QString());

		//! \brief Copy constructor
		Interconnection(const Interconnection& other);

		//! \brief Assignment operator
		Interconnection& operator=(const Interconnection& other);

		/*! \brief Check if the interconnection is in a valid state.
		* 
		* \param instanceNames List of component instance names contained in the design.
		* \param errorList The list to add the possible error messages to.
		* \param parentIdentifier String from parent to help to identify the location of the error.
		*
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(const QStringList& instanceNames, 
			QStringList& errorList, 
			const QString& parentIdentifier) const;

		/*! \brief Check if the interconnection is in a valid state.
		* 
		* \param instanceNames List of component instance names contained in the design.
		* 
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(const QStringList& instanceNames) const;
	};

	/*! \brief Describes the spirit:hierConnection element in an
	* IP-XACT document
	*
	* HierConnection represents a hierarchical interface connection
	* between a bus interface on the encompassing component and a bus
	* interface on a component instance of the design.
	*/
	struct HierConnection {

		/*! \brief MANDATORY spirit:interfaceRef
		*
		* Attribute that provides one end of the interconnection; it
		* is the name of the bus interface on the encompassing
		* component
		*/
		QString interfaceRef;

		/*!\brief Specifies the component instance bus interface for
		* connection to the encompassing component.
		*/
		Interface interface_;

        /*! \brief VENDOREXT kactus2:position
         *
         *  Specifies the position of the interface in the block diagram.
         */
        QPointF position;

        /*! \brief VENDOREXT kactus2:direction
         *
         *  Specifies the orientation of the interface in the block diagram.
         */
        QVector2D direction;

        /*! \brief VENDOREXT kactus2:route
         *
         *  The diagram route for the interconnection.
         */
        QList<QPointF> route;

        /*! \brief VENDOREXT kactus2:offPage
         *
         *  If true, the route is treated as an off-page connection.
         */
        bool offPage;

        /*!
	     * OPTIONAL (spirit: vendorExtensions)
	     * HierConnection vendor extensions.
	     */
        QList<QSharedPointer<VendorExtension> > vendorExtensions_;

		/*! \brief The constructor
		*
		* \param hierConnectionNode A reference to a QDomNode where
		* the information should be parsed from.
		*/
		HierConnection(QDomNode &hierConnectionNode);

		/*! \brief Convenience constructor for initializing fields
		*/
		HierConnection(QString interfaceRef_ = QString(""),
			Interface interface_ = Interface(),
            QPointF const& position = QPointF(),
            QVector2D const& direction = QVector2D(),
            QList<QPointF> const& route = QList<QPointF>(),
            bool offPage = false);

		//! \brief Copy constructor
		HierConnection(const HierConnection& other);

		//! \brief Assignment operator
		HierConnection& operator=(const HierConnection& other);

		/*! \brief Check if the hierarchical connection is in a valid state.
		* 
		* \param instanceNames List of component instance names contained in the design.
		* \param errorList The list to add the possible error messages to.
		* \param parentIdentifier String from parent to help to identify the location of the error.
		*
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(const QStringList& instanceNames,
			QStringList& errorList, 
			const QString& parentIdentifier) const;

		/*! \brief Check if the hierarchical connection is in a valid state.
		* 
		* \param instanceNames List of component instance names contained in the design.
		* 
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(const QStringList& instanceNames) const;
	};

	/*! \brief Describes a port reference in an ad-hoc connection
	*
	* PortRef a reference to a internal or external port. Internal
	* port (spirit:internalPortReference) is a port on a component
	* contained within the design and has a non-null
	* componentRef. External port (spirit:externalPortReference) is a
	* port on the component containing this design and has a null
	* componentRef.
	*/
	struct PortRef {

		/*! \brief MANDATORY spirit:portRef
		*
		* Name of the port referenced.
		*/
		QString portRef;

		/*! \brief MANDATORY spirit:componentRef
		* 
		* A reference to the instanceName of a component instance in
		* this design if this is a reference to an internal
		* port. Otherwise this field is set to a null string.
		*/
		QString componentRef;

		/*! \brief OPTIONAL spirit:left
		*
		* Left index of a vector
		*/
		int left;

		/*! \brief OPTIONAL spirit:right
		*
		* Right index of a vector
		*/
		int right;

		/*! \brief The constructor
		*
		* \param portReferenceNode A reference to a QDomNode where
		* the information should be parsed from.
		*/
		PortRef(QDomNode &portReferenceNode);

		/*! \brief Convenience constructor for initializing fields
		*/
		PortRef(QString portRef,
			QString componentRef = QString(),
			int left = -1,
			int right = -1);

		//! \brief Copy constructor
		PortRef(const PortRef& other);

		//! \brief Assignment operator
		PortRef& operator=(const PortRef& other);

		/*! \brief Check if the port ref is in a valid state.
		* 
		* \param externalRef Must be true for external refs so component reference 
		* is not checked.
		* \param instanceNames List containing the component instance names from
		* the containing design.
		* \param errorList The list to add the possible error messages to.
		* \param parentIdentifier String from parent to help to identify the location of the error.
		*
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(bool externalRef,
			const QStringList& instanceNames,
			QStringList& errorList, 
			const QString& parentIdentifier) const;

		/*! \brief Check if the port ref is in a valid state.
		* 
		* \param externalRef Must be true for external refs so component reference 
		* is not checked.
		* \param instanceNames List containing the component instance names from
		* the containing design.
		* 
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(bool externalRef,
			const QStringList& instanceNames) const;
	};

	/*! \brief Describes the spirit:adHocConnection element in an
	* IP-XACT document
	*
	* Specifies a connection that is made on a port-by-port basis and
	* not done through the higher-level bus interface. These
	* connections are between component instance ports or between
	* component instance ports and ports of the encompassing
	* component (in the case of a hierarchical component).
	*/
	struct AdHocConnection {

		/*! \brief MANDATORY spirit:name
		*
		* Unique name of the ad-hoc connection
		*/
		QString name;

		/*! \brief OPTIONAL spirit:displayName
		*
		* Allows a short descriptive text to be associated with the
		* ad-hoc connection
		*/
		QString displayName;

		/*! \brief OPTIONAL spirit:description
		*
		* Allows a textual description of the ad-hoc connection.
		*/
		QString description;

		/*! \brief OPTIONAL spirit:tiedValue
		*
		* The logic value of this ad-hoc connection
		*/
		QString tiedValue;

		/*! \brief MANDATORY spirit:internalPortReference
		*
		* List of PortRefs. Each references the port of a component
		* instance.
		*/
		QList<PortRef> internalPortReferences;

		/*! \brief OPTIONAL spirit:externalPortReference
		*
		* List of PortRefs. Each References a port of the
		* encompassing component where this design is referred (for
		* hierarchical ad-hoc connections).
		*/
		QList<PortRef> externalPortReferences;

        /*! \brief VENDOREXT kactus2:route
         *
         *  The diagram route for the interconnection.
         */
        QList<QPointF> route;

        /*! \brief VENDOREXT kactus2:offPage
         *
         *  If true, the route is treated as an off-page connection.
         */
        bool offPage;

		/*! \brief The constructor
		*
		* \param adHocConnectionNode a reference to a QDomNode where
		* the information should be parsed from.
		*/
		AdHocConnection(QDomNode &adHocConnectionNode);

		/*! \brief Convenience constructor for initializing fields
		*/
		AdHocConnection(
			QString name,
			QString displayName = QString(),
			QString description = QString(),
			QString tiedValue = QString(),
			QList<PortRef> internalPortReferences = QList<PortRef>(),
			QList<PortRef> externalPortReferences = QList<PortRef>(),
            QList<QPointF> const& route = QList<QPointF>(),
            bool offPage = false);

		//! \brief Copy constructor
		AdHocConnection(const AdHocConnection& other);

		//! \brief Assignment operator
		AdHocConnection& operator=(const AdHocConnection& other);

		/*! \brief Check if the ad hoc connection is in a valid state.
		* 
		* \param instanceNames List containing the component instance names from
		* the containing design.
		* \param errorList The list to add the possible error messages to.
		* \param parentIdentifier String from parent to help to identify the location of the error.
		*
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(const QStringList& instanceNames,
			QStringList& errorList, 
			const QString& parentIdentifier) const;

		/*! \brief Check if the ad hoc connection is in a valid state.
		* 
		* \param instanceNames List containing the component instance names from
		* the containing design.
		* 
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(const QStringList& instanceNames) const;
	};

	/*! \brief The constructor
	*
	* \param document a reference to a QDomDocument where the
	* information should be parsed from.
	*/
	Design(QDomDocument &document);

	Design(const VLNV &vlnv);

	//! \brief Copy constructor
	Design(const Design& other);

	//! \brief Assignment operator
	Design& operator=(const Design& other);

	/*! \brief The destructor
	*
	*/
	virtual ~Design();

	/*! \brief Clone this design and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit LibraryComponent.
	 *
	 * \return QSharedPointer<LibraryComponent> Pointer to the cloned design.
	*/
	virtual QSharedPointer<LibraryComponent> clone() const;

	/*! \brief Write XML IP-XACT document of this design
	*/
	virtual void write(QFile& file);

    /*! \brief Check the validity of the design.
	 * 
	 * This function should be used to check if the design is in valid state
	 * before writing the xml to the disk.
	 * 
	 * \param errorList The error messages of the detected errors are appended to
	 * this list.
	 *
	 * \return bool True if the design was valid.
	*/
	virtual bool isValid(QStringList& errorList) const;

	/*! \brief Check the validity of the design.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid() const;

	/*! \brief Set the vlnv
	 *
	 * \param vlnv Reference to the vlnv to set
	 *
	*/
	virtual void setVlnv(const VLNV& vlnv);

	/*! \brief Get list of the component instances in this design
	*
	* \return QList containing the component instances
	*/
	QList<ComponentInstance> const& getComponentInstances() const;

	/*! \brief Get the description of the specified HW instance.
	 *
	 * Method: 		getInstanceDescription
	 * Full name:	Design::getInstanceDescription
	 * Access:		public 
	 *
	 * \param instanceName Identifies the instance to search for.
	 *
	 * \return QString containing the description, empty if instance not found.
	*/
	QString getHWInstanceDescription(const QString& instanceName) const;

    /*!
     *  Returns a list of SW instances in the design.
     */
    QList<SWInstance> const& getSWInstances() const;

	/*! \brief Get list of the interconnections
	*
	* \return QList containing the interconnections
	*/
	const QList<Interconnection> &getInterconnections();

	/*! \brief Check if the specified interface is connected to some other interface within the design.
	 *
	 * Method: 		hasInterconnection
	 * Full name:	Design::hasInterconnection
	 * Access:		public 
	 *
	 * \param instanceName The name identifying the the instance.
	 * \param interfaceName The name identifying the interface within the containing instance.
	 *
	 * \return True if at least one interconnection is found.
	*/
	bool hasInterconnection(const QString& instanceName, const QString& interfaceName) const;

	/*! \brief Get list of the interfaces that are connected to the specified interface via interconnection.
	 *
	 * Method: 		getConnectedInterfaces
	 * Full name:	Design::getConnectedInterfaces
	 * Access:		public 
	 *
	 * \param instanceName The name identifying the instance.
	 * \param interfaceName The name of the interface within the instance.
	 *
	 * \return QList containing the interfaces connected to the specified interface.
	*/
	QList<Design::Interface> getConnectedInterfaces(const QString& instanceName,
		const QString& interfaceName) const;

	/*! \brief Get list of the interfaces that are connected to the specified interface via interconnection.
	 *
	 * Method: 		getConnectedInterfaces
	 * Full name:	Design::getConnectedInterfaces
	 * Access:		public 
	 *
	 * \param sourceInterface Identifies the source instance and bus interface.
	 *
	 * \return QList containing the interfaces connected to the specified interface.
	*/
	QList<Design::Interface> getConnectedInterfaces(const Design::Interface& sourceInterface) const;

	/*! \brief Get list of the hierarchical connections
	*
	* \return QList containing the hierarchical connections
	*/
	const QList<HierConnection> &getHierarchicalConnections();

	/*! \brief Get list of the ad-hoc connections
	*
	* \return QList containing the ad-hoc connections
	*/
	const QList<AdHocConnection> &getAdHocConnections();

    /*!
     *  Returns the list of API dependencies.
     */
    QList<ApiDependency> const& getApiDependencies() const;

    /*!
     *  Returns the list of hierarchical API dependencies.
     */
    QList<HierApiDependency> const& getHierApiDependencies() const;

    /*!
     *  Returns the list of COM connections.
     */
    QList<ComConnection> const& getComConnections() const;

    /*!
     *  Returns the list of hierarchical COM connections.
     */
    QList<HierComConnection> const& getHierComConnections() const;

    /*!
     *  Returns the port ad-hoc visibilities for the top-level component in this design.
     */
    QMap<QString, bool> const& getPortAdHocVisibilities() const;

    /*!
     *  Returns the ad-hoc port positions.
     */
    QMap<QString, QPointF> const& getAdHocPortPositions() const;

    /*! 
     *  Returns the list of columns.
     */
    QList<ColumnDesc> const& getColumns() const;

	/*! \brief Set the component instances of this design
	*
	* \param componentInstances QList containing the component
	* instances.
	*/
	void setComponentInstances(QList<ComponentInstance> const& componentInstances);

    /*!
     *  Sets the SW instances for the design.
     *
     *      @param [in] swInstances A list of SW instances.
     */
    void setSWInstances(QList<SWInstance> const& swInstances);

	/*! \brief Set the interconnections of this design
	*
	* \param interconnections QList containing the interconnections.
	*/
	void setInterconnections(QList<Interconnection> const& interconnections);

	/*! \brief Set the hierarchical connections of this design
	*
	* \param hierConnections QList containing the hierarchical
	* connections.
	*/
	void setHierarchicalConnections(QList<HierConnection> const& hierConnections);

	/*! \brief Set the ad-hoc connections of this design
	*
	* \param adHocConnections QList containing the ad-hoc connections.
	*/
	void setAdHocConnections(QList<AdHocConnection> const& adHocConnections);

    /*!
     *  Sets the API dependencies for the design.
     *
     *      @param [in] apiDependencies A list of API dependencies.
     */
    void setApiDependencies(QList<ApiDependency> const& apiDependencies);

    /*! Sets the hierarchical API dependencies for the design.
     *
     *      @param [in] hierApiDependencies A list of hierarchical API dependencies.
     */
    void setHierApiDependencies(QList<HierApiDependency> const& hierApiDependencies);

    /*!
     *  Sets the COM connections for the design.
     *
     *      @param [in] comConnections A list of COM connections.
     */
    void setComConnections(QList<ComConnection> const& comConnections);

    /*!
     *  Sets the hierarchical COM connections for the design.
     *
     *      @param [in] hierComConnections A list of hierarchical COM connections.
     */
    void setHierComConnections(QList<HierComConnection> const& hierComConnections);

    /*!
     *  Sets the port ad-hoc visibilities for the top-level component in this design.
     */
    void setPortAdHocVisibilities(QMap<QString, bool> const& portAdHocVisibilities);

    /*!
     *  Sets the ad-hoc port positions.
     */
    void setAdHocPortPositions(QMap<QString, QPointF> const& val);

    /*!
     *  Sets the columns of this design.
     */
    void setColumns(QList<ColumnDesc> const& columns);

	/*! \brief Reimplemented from base class
	*/
	virtual const QStringList getDependentFiles() const;

	/*! \brief Reimplemented from base class
	*/
	virtual const QList<VLNV> getDependentVLNVs() const;

    /*! \brief Get the VLNVs of the components referenced by this design
	 *
	 * \return QList<VLNV> contains the references to the components referenced
	 * within this design.
	*/
	QList<VLNV> getComponents() const;

	/*! \brief Get the VLNV of the HW component instantiated with given name.
	 *
	 * Method: 		getComponentVLNV
	 * Full name:	Design::getComponentVLNV
	 * Access:		public 
	 *
	 * \param instanceName The name identifying the component instance.
	 *
	 * \return VLNV of the instantiated component. If the instance does not exist then invalid VLNV is returned.
	*/
	VLNV getHWComponentVLNV(const QString& instanceName) const;

	/*! \brief Check if the design contains a HW instance with given name.
	 *
	 * Method: 		containsInstance
	 * Full name:	Design::containsInstance
	 * Access:		public 
	 *
	 * \param instanceName The instance name searched.
	 *
	 * \return True if the instance is found.
	*/
	bool containsHWInstance(const QString& instanceName) const;

	/*! \brief Check if there is a configurable element value set for the given HW instance.
	 *
	 * Method: 		hasConfElementValue
	 * Full name:	Design::hasConfElementValue
	 * Access:		public 
	 *
	 * \param instanceName Identifies the HW instance to search for.
	 * \param confElementName Identifies the configurable element value to check.
	 *
	 * \return True if the value has been set, otherwise false.
	*/
	bool hasConfElementValue(const QString& instanceName, const QString& confElementName) const;

	/*! \brief Get the configurable element value set for the given HW instance.
	 *
	 * Method: 		getConfElementValue
	 * Full name:	Design::getConfElementValue
	 * Access:		public 
	 *
	 * \param instanceName Identifies the HW instance.
	 * \param confElementName Identifies the configurable element value to get.
	 *
	 * \return QString containing the set configurable element value, empty string if value is not set.
	*/
	QString getConfElementValue(const QString& instanceName, const QString& confElementName) const;

    /*! \brief Set the implementation type of the design.
     *
     * \param implementation The implementation type.
     *
     */
	void setDesignImplementation(KactusAttribute::Implementation implementation);

    /*! \brief Set the implementation type of the design.
     *
     * \param implementation The implementation type.
     *
     */
	KactusAttribute::Implementation getDesignImplementation() const;

private:

    /*!
     *  Parses the vendor extensions of the design from a QDomNode.
     *
     *      @param [in] node The QDomNode from where to parse the information.
     */
    void parseVendorExtensions(QDomNode& node);

    QList<ColumnDesc> columns_;

    //! The HW component instances.
    QList<ComponentInstance> componentInstances_;

    //! The SW component instances (extension).
    QList<SWInstance> swInstances_;


    QList<Interconnection> interconnections_;
    QList<HierConnection> hierConnections_;
    QList<AdHocConnection> adHocConnections_;

    QMap<QString, bool> portAdHocVisibilities_;

    //! The port positions for ad-hoc ports.
    QMap<QString, QPointF> adHocPortPositions_;
	QMap<QString, QString> attributes_;

    //! The API dependencies (extension).
    QList<ApiDependency> apiDependencies_;

    //! The hierarchical API dependencies (extension).
    QList<HierApiDependency> hierApiDependencies_;

    //! The COM connections (extension).
    QList<ComConnection> comConnections_;

    //! The hierarchical COM connections (extension).
    QList<HierComConnection> hierComConnections_;
};

#endif

