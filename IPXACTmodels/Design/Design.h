//-----------------------------------------------------------------------------
// File: Design.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.08.2015
//
// Description:
// Describes the ipxact:design element in an IP-XACT document
//-----------------------------------------------------------------------------

#ifndef DESIGN_H
#define DESIGN_H

#include "ComponentInstance.h"
#include "Interconnection.h"
#include "MonitorInterconnection.h"
#include "AdHocConnection.h"

#include <IPXACTmodels/SWInstance.h>
#include <IPXACTmodels/ApiConnection.h>
#include <IPXACTmodels/HierApiDependency.h>
#include <IPXACTmodels/ComConnection.h>
#include <IPXACTmodels/HierComConnection.h>
#include <IPXACTmodels/ColumnDesc.h>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

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

//-----------------------------------------------------------------------------
//! Implementation for the ipxact:design element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Design : public Document
{

public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] vlnv    The VLNV for the design.
	 */
	Design(const VLNV &vlnv);

    /*!
     *  The constructor.
     */
    Design();

	/*!
	 *  Copy constructor.
	 */
	Design(const Design& other);

	/*!
	 *  Assignment operator.
	 */
	Design& operator=(const Design& other);

	/*!
	 *  The destructor.
	 */
	virtual ~Design();

	/*!
	 *  Clone this design.
	 *
     *      @return A pointer to the cloned copy.
	 */
	virtual QSharedPointer<Document> clone()  const;

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
	/*!
	//virtual bool isValid(QStringList& errorList) const;

	/*! \brief Check the validity of the design.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	//virtual bool isValid() const;

	/*!
	 *  Set the VLNV.
	 *
	 *      @param [in] vlnv    The new VLNV.
	 */
	virtual void setVlnv(const VLNV& vlnv);

    /*!
     *  Get the component instances.
     *
     *      @return A pointer to a list containing the component instances
     */
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > getComponentInstances() const;

    /*!
	 *  Get the description of the specified HW instance.
	 *
	 *      @param [in] instanceName    The name of the instance to be searched for.
     *
     *      @return The description of the found HW instance.
	 */
	QString getHWInstanceDescription(const QString& instanceName) const;

    /*!
     *  Get a list of SW instances.
     *
     *      @return A list of sw instances.
     */
    QList<SWInstance> const& getSWInstances() const;
    QList<SWInstance>& getSWInstances();

    /*!
     *  Get a list of the interconnections.
     *
     *      @return Pointer to a list of interconnections.
     */
    QSharedPointer<QList<QSharedPointer<Interconnection> > > getInterconnections() const;

	/*!
	 *  Check if the specified interface is connected to some other interface within the design.
	 *
	 *      @param [in] instanceName    The name of the instance.
	 *      @param [in] interfaceName   The name of the interface.
     *
     *      @return True if at least one interconnection is found, false otherwise.
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
	//QList<Interface> getConnectedInterfaces(const QString& instanceName,
	//	const QString& interfaceName) const;

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
	//QList<Interface> getConnectedInterfaces(const Interface& sourceInterface) const;


    /*!
     *  Get a list of ad-hoc connections.
     *
     *      @return Pointer to a list of ad-hoc connections.
     */
    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > getAdHocConnections() const;

    /*!
     *  Returns the list of API dependencies.
     */
    QList<ApiConnection> const& getApiDependencies() const;

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

    /*!
     *  Set the component instances.
     *
     *      @param [in] newComponentInstances   A list of new component instances.
     */
    void setComponentInstances(QSharedPointer<QList<QSharedPointer<ComponentInstance> > > newComponentInstances);

    /*!
     *  Sets the SW instances for the design.
     *
     *      @param [in] swInstances A list of SW instances.
     */
    void setSWInstances(QList<SWInstance> const& swInstances);

    /*!
     *  Set the interconnections.
     *
     *      @param [in] newInterconnections     A list of new interconnections.
     */
    void setInterconnections(QSharedPointer<QList<QSharedPointer<Interconnection> > > newInterconnections);

    /*!
     *  Set the ad-hoc connections.
     *
     *      @param [in] newAdHocConnections     A list of new ad-hoc connections.
     */
    void setAdHocConnections(QSharedPointer<QList<QSharedPointer<AdHocConnection> > > newAdHocConnections);

    /*!
     *  Sets the API dependencies for the design.
     *
     *      @param [in] apiDependencies A list of API dependencies.
     */
    void setApiDependencies(QList<ApiConnection> const& apiDependencies);

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
     *
     *      @param [in] portAdHocVisibilities   A list of ad-hoc port visibilities.
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

	/*!
	 *  Get the dependent files.
     *
     *      @return A list of the dependent file names.
	 */
	virtual QStringList getDependentFiles() const;

	/*!
	 *  Get dependend VLNVs.
	 *
     *      @return A list of the dependend VLNVs.
	 */
	virtual QList<VLNV> getDependentVLNVs() const;

	/*!
	 *  Get the VLNVs of the components referenced in this design.
	 *
     *      @return A list of component VLNVs.
	 */
	QList<VLNV> getComponents() const;

	/*!
	 *  Get the VLNV of the HW component instantiated with the given name.
	 *
	 *      @param [in] instanceName    The name identifying the component instance.
     *
     *      @return VLNV of the instantiated component.
	 */
	VLNV getHWComponentVLNV(const QString& instanceName) const;

	/*!
	 *  Check if the design contains a given HW instance.
	 *
	 *      @param [in] instanceName    The name of the HW instance being searched.
     *
     *      @return True if the instance is found, false otherwise.
	 */
	bool containsHWInstance(const QString& instanceName) const;

	/*!
	 *  Check if there is a configurable element value set for the given HW instance.
	 *
	 *      @param [in] instanceName        The name of the HW instance.
	 *      @param [in] confElementName     The identifier of the configurable element value.
     *
     *      @return Ture if the value has been set, otherwise false.
	 */
	bool hasConfElementValue(const QString& instanceName, const QString& confElementName) const;

	/*!
	 *  Get the configurable element value set for the given HW instance.
	 *
	 *      @param [in] instanceName        Identifies the HW instance.
	 *      @param [in] confElementName     Identifies the configurable element value.
     *
     *      @return QString containing the configurable element value.
	 */
	QString getConfElementValue(const QString& instanceName, const QString& confElementName) const;

	/*!
	 *  Set the implementation type of the design.
	 *
	 *      @param [in] implementation  The new implementation type.
	 */
	void setDesignImplementation(KactusAttribute::Implementation implementation);

	/*!
	 *  Get the implementation type.
	 *
     *      @return The implementation type.
	 */
	KactusAttribute::Implementation getDesignImplementation() const;

    /*!
     *  Get the monitor interconnections.
     *
     *      @return A pointer to the monitor interconnections.
     */
    QSharedPointer<QList<QSharedPointer<MonitorInterconnection> > > getMonitorInterconnecions() const;

    /*!
     *  Set the monitor interconnections.
     *
     *      @param [in] newMonitorInterconnections  The new monitor interconnections.
     */
    void setMonitorInterconnections(
        QSharedPointer<QList<QSharedPointer<MonitorInterconnection> > > newMonitorInterconnections);

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

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! A list of columns in the design.
    QList<ColumnDesc> columns_;

    //! The HW component instances.
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > componentInstances_;

    //! The SW component instances (extension).
    QList<SWInstance> swInstances_;

    //! The interconnections.
    QSharedPointer<QList<QSharedPointer<Interconnection> > > interconnections_;
    
    //! The monitor interconnections.
    QSharedPointer<QList<QSharedPointer<MonitorInterconnection> > > monitorInterconnections_;

    //! Ad-hoc connections
    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > adHocConnections_;

    //! Port ad-hoc visibilities.
    QMap<QString, bool> portAdHocVisibilities_;

    //! The port positions for ad-hoc ports.
    QMap<QString, QPointF> adHocPortPositions_;

    //! The attributes?
	QMap<QString, QString> attributes_;

    //! The API dependencies (extension).
    QList<ApiConnection> apiDependencies_;

    //! The hierarchical API dependencies (extension).
    QList<HierApiDependency> hierApiDependencies_;

    //! The COM connections (extension).
    QList<ComConnection> comConnections_;

    //! The hierarchical COM connections (extension).
    QList<HierComConnection> hierComConnections_;
};

#endif // DESIGN_H
