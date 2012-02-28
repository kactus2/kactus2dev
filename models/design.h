#ifndef DESIGN_H
#define DESIGN_H

#include "librarycomponent.h"
#include <LibraryManager/vlnv.h>
#include <common/ColumnTypes.h>
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
class Design: public LibraryComponent {

public:
    //-----------------------------------------------------------------------------
    //! ColumnDesc structure which describes the kactus2:column element
    //! in an IP-XACT document.
    //-----------------------------------------------------------------------------
    struct ColumnDesc
    {
        QString name;                       //!< The name of the column.
        ColumnContentType contentType;      //!< The content type for the column.
        unsigned int allowedItems;          //!< The allowed items for the column.

        /*!
         *  Constructor which parses the column description from an XML node.
         *
         *      @param [in] node A QDomNode from where the information is parsed.
         */
        ColumnDesc(const QDomNode& node);

        /*!
         *  Convenience constructor.
         */
        ColumnDesc(QString const& name, ColumnContentType contentType,
                   unsigned int allowedItems);

		//! \brief Copy constructor
		ColumnDesc(const ColumnDesc& other);

		//! \brief Assignment operator
		ColumnDesc& operator=(const ColumnDesc& other);
    };

	/*! \brief Describes the spirit:componentInstance element in an
	* IP-XACT document
	*
	* ComponentInstance holds component instance's configuration
	*/
	struct ComponentInstance {

		/*! \brief MANDATORY spirit:instanceName
		*
		* A unique name for this instance of a component.
		*/
		QString instanceName;

		/*! \brief OPTIONAL spirit:displayName
		*
		* Allows a short descriptive text to be associated with the
		* instance.
		*/
		QString displayName;

		/*! \brief OPTIONAL spirit:description
		*
		* Allows a textual description of the instance
		*/
		QString description;

		/*! \brief MANDATORY spirit:componentRef
		*
		* A reference to a component description.
		*/
		VLNV componentRef;

		/*! \brief OPTIONAL spirit:configurableElementValues
		*
		* Specifies the configuration for a specific component
		* instance by providing the value of a specific component
		* parameter. Key of the map is a reference to the id
		* attribute of an element in the component instance and the
		* value is the value assigned.
		*/
		QMap<QString, QString> configurableElementValues;

        /*! \brief VENDOREXT kactus2:position
         *
         *  Specifies the component instance's global position in the block diagram.
         */
        QPointF position;

        /*! \brief VENDOREXT kactus2:portPositions
         *
         *  Specifies the port positioning for the component instance's bus interfaces.
         */
        QMap<QString, QPointF> portPositions;

        /*!
         *  Specifies the positioning for the visible ad-hoc ports.
         */
        QMap<QString, QPointF> adHocPortPositions;

        /*! \brief VENDOREXT kactus2:adHocVisibilities
         *
         *  Specifies ad-hoc visibility of each port.
         */
        QMap<QString, bool> portAdHocVisibilities;

        /*! \brief VENDOREXT kactus2:mcapiNodeId
         *
         *  Specifies the MCAPI node id.
         */
        int mcapiNodeID;

        /*! \brief VENDOREXT kactus2:endpointsExpanded
         *
         *  Specifies whether the endpoints in an endpoint component are expanded.
         */
        bool endpointsExpanded;

        /*! \brief VENDOREXT kactus2:imported
         *
         *  Specifies whether the component is imported from a software mapping design.
         */
        bool imported;
        
        /*! \brief The constructor
		*
		* \param componentInstanceNode A reference to a QDomNode
		* where the information should be parsed from.
		*/
		ComponentInstance(QDomNode &componentInstanceNode);

		/*! \brief Convenience constructor for initializing fields
		*/
		ComponentInstance(QString instanceName, QString displayName,
			              QString description, VLNV const& componentRef,
                          QPointF const& position);

		//! \brief Copy constructor
		ComponentInstance(const ComponentInstance& other);

		//! \brief Assignment operator
		ComponentInstance& operator=(const ComponentInstance& other);

        void parsePortPositions(QDomNode& node);
        void parseAdHocVisibilities(QDomNode& node);

		/*! \brief Check if the component instance is in a valid state.
		*
		* \param errorList The list to add the possible error messages to.
		* \param parentIdentifier String from parent to help to identify the location of the error.
		*
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(QStringList& errorList, 
			const QString& parentIdentifier) const;

		/*! \brief Check if the component instance is in a valid state.
		*
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid() const;
    };

	/*! \brief Describes the spirit:interface type in an IP-XACT
	* document
	*
	* Specifies the bus interface of a design component instance that
	* is part of an interconnection.
	*/
	struct Interface {
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
	const QList<ComponentInstance> &getComponentInstances();

	/*! \brief Get list of the interconnections
	*
	* \return QList containing the interconnections
	*/
	const QList<Interconnection> &getInterconnections();

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
     *  Returns the port ad-hoc visibilities for the top-level component in this design.
     */
    QMap<QString, bool> const& getPortAdHocVisibilities() const;

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
     *  Sets the port ad-hoc visibilities for the top-level component in this design.
     */
    void setPortAdHocVisibilities(QMap<QString, bool> const& portAdHocVisibilities);

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

    /*!
     *  Writes a kactus2:position to the XML stream.
     */
    void writePosition(QXmlStreamWriter& xmlWriter, QPointF const& pos);

    /*!
     *  Writes a kactus2:direction to the XML stream.
     */
    void writeDirection(QXmlStreamWriter& xmlWriter, QVector2D const& dir);

    /*!
     *  Writes the given port ad-hoc visibilities along with the port positions to
     *  a specific XML format.
     */
    void writeAdHocVisibilities(QXmlStreamWriter& xmlWriter,
                                QMap<QString, bool> const& adHocVisibilities,
                                QMap<QString, QPointF> const& adHocPortPositions);

	/*! \brief Get the VLNVs of the components referenced by this design
	 *
	 * \return QList<VLNV> contains the references to the components referenced
	 * within this design.
	*/
	QList<VLNV> getComponents() const;

private:

    /*!
     *  Parses the vendor extensions of the design from a QDomNode.
     *
     *      @param [in] node The QDomNode from where to parse the information.
     */
    void parseVendorExtensions(QDomNode& node);

    QList<ColumnDesc> columns_;
    QList<ComponentInstance> componentInstances_;
    QList<Interconnection> interconnections_;
    QList<HierConnection> hierConnections_;
    QList<AdHocConnection> adHocConnections_;

    QMap<QString, bool> portAdHocVisibilities_;

	QMap<QString, QString> attributes_;
};

#endif

