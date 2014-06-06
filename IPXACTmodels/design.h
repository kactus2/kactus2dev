//-----------------------------------------------------------------------------
// File: design.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// Describes the spirit:design element in an IP-XACT document
//-----------------------------------------------------------------------------

#ifndef DESIGN_H
#define DESIGN_H

#include "SWInstance.h"
#include "ApiDependency.h"
#include "HierApiDependency.h"
#include "ComConnection.h"
#include "HierComConnection.h"
#include "ColumnDesc.h"
#include "ComponentInstance.h"
#include "AdHocConnection.h"
#include "Interconnection.h"
#include "HierConnection.h"

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
	QList<Interface> getConnectedInterfaces(const QString& instanceName,
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
	QList<Interface> getConnectedInterfaces(const Interface& sourceInterface) const;

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

    /*!
     *  Parses the column layout from kactus2:columnLayout.
     *
     *      @param [in] layoutNode   The DOM node containing the layout.
     */
    void parseColumnLayout(QDomNode& layoutNode);

    /*!
     *  Parses the routes from kactus2:routes.
     *
     *      @param [in] routesNode   The DOM node containing the routes.
     */
    void parseRoutes(QDomNode& routesNode);

    /*!
     *  Parses a route from kactus2:route.
     *
     *      @param [in] routeNode   The DOM node containing the route.
     */
    void parseRoute(QDomNode& routeNode);

    /*!
     *  Parses the ad-hoc port visibilities from kactus2:adHocVisibilities.
     *
     *      @param [in] visibilitiesNode   The DOM node containing the visibilities.
     */
    void parseAdHocVisibilities(QDomNode& visibilitiesNode);

    /*!
     *  Parses the SW instances from kactus2:swInstances.
     *
     *      @param [in] swInstancesNode   The DOM node containing the SW instances.
     */
    void parseSWInstances(QDomNode& swInstancesNode);

    /*!
     *  Parses the API dependencies from kactus2:apiDependencies.
     *
     *      @param [in] dependenciesNode   The DOM node containing the API dependencies.
     */
    void parseApiDependencies(QDomNode& dependenciesNode);

    /*!
     *  Parses the hierarchical API dependencies from kactus2:hierApiDependencies.
     *
     *      @param [in] hierDependenciesNode   The DOM node containing the hierarchical API dependencies.
     */
    void parseHierApiDependencies(QDomNode& hierDependenciesNode);

    /*!
     *  Parses the COM connections from kactus2:comConnections.
     *
     *      @param [in] comConnectionsNode   The DOM node containing the hierarchical COM connections.
     */
    void parseComConnections(QDomNode& comConnectionsNode);

    /*!
     *  Parses the hierarchical COM connections from kactus2:hierComConnections.
     *
     *      @param [in] hierComConnectionsNode   The DOM node containing the 
     *                                           hierarchical hierarchical COM connections.
     */
    void parseHierComConnections(QDomNode& hierComConnectionsNode);

    /*!
     *  Parses the sticky note from kactus2:note.
     *
     *      @param [in] noteNode   The DOM node containing the sticky note.
     */
    void parseStickyNote(QDomNode& noteNode);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

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

