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

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/kactusExtensions/ColumnDesc.h>
#include <IPXACTmodels/kactusExtensions/SWInstance.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/ApiInterconnection.h>
#include <IPXACTmodels/kactusExtensions/ComInterconnection.h>

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

class ConnectionRoute;
class InterfaceGraphicsData;

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
	Design(Design const& other);

	/*!
	 *  Assignment operator.
	 */
	Design& operator=(Design const& other);

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

	/*!
	 *  Set the VLNV.
	 *
	 *      @param [in] vlnv    The new VLNV.
	 */
	virtual void setVlnv(VLNV const& vlnv);

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
	QString getHWInstanceDescription(QString const& instanceName) const;

    /*!
     *  Get a list of SW instances.
     *
     *      @return A list of sw instances.
     */
    QList <QSharedPointer<SWInstance> > getSWInstances() const;

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
	bool hasInterconnection(QString const& instanceName, QString const& interfaceName) const;

    /*!
     *  Get a list of ad-hoc connections.
     *
     *      @return Pointer to a list of ad-hoc connections.
     */
    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > getAdHocConnections() const;

    /*!
     *  Returns the list of API dependencies.
     */
    QList<QSharedPointer<ApiInterconnection> > getApiConnections() const;

    /*!
     *  Returns the list of COM connections.
     */
    QList<QSharedPointer<ComInterconnection> > getComConnections() const;

    /*!
     *  Returns the port ad-hoc visibilities for the top-level component in this design.
     */
    QMap<QString, bool> getPortAdHocVisibilities() const;

    /*!
     *  Returns the ad-hoc port positions.
     */
    QSharedPointer<VendorExtension> getAdHocPortPositions() const;

    /*! 
     *  Returns the list of columns.
     */
    QList<QSharedPointer<ColumnDesc> > getColumns() const;

    /*! 
     *  Returns the list of routes.
     */
    QList<QSharedPointer<ConnectionRoute> > getRoutes() const;

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
    void setSWInstances(QList<QSharedPointer<SWInstance> > newSWInstances);

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
    void setApiConnections(QList<QSharedPointer<ApiInterconnection> > newApiConnections);

    /*!
     *  Sets the COM connections for the design.
     *
     *      @param [in] comConnections A list of COM connections.
     */
    void setComConnections(QList<QSharedPointer<ComInterconnection> > newComConnections);

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
    //void setColumns(QList<QSharedPointer<ColumnDesc> > newColumns);

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
	 *  Get the VLNV of the HW component instantiated with the given name.
	 *
	 *      @param [in] instanceName    The name identifying the component instance.
     *
     *      @return VLNV of the instantiated component.
	 */
	VLNV getHWComponentVLNV(QString const& instanceName) const;

	/*!
	 *  Check if the design contains a given HW instance.
	 *
	 *      @param [in] instanceName    The name of the HW instance being searched.
     *
     *      @return True if the instance is found, false otherwise.
	 */
	bool containsHWInstance(QString const& instanceName) const;

	/*!
	 *  Check if there is a configurable element value set for the given HW instance.
	 *
	 *      @param [in] instanceName        The name of the HW instance.
	 *      @param [in] confElementName     The identifier of the configurable element value.
     *
     *      @return Ture if the value has been set, otherwise false.
	 */
	bool hasConfElementValue(QString const& instanceName, QString const& confElementName) const;

	/*!
	 *  Get the configurable element value set for the given HW instance.
	 *
	 *      @param [in] instanceName        Identifies the HW instance.
	 *      @param [in] confElementName     Identifies the configurable element value.
     *
     *      @return QString containing the configurable element value.
	 */
	QString getConfElementValue(QString const& instanceName, QString const& confElementName) const;

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

    /*!
     *  Adds a column to the design vendor extensions.
     *
     *      @param [in] column   The column to add.
     */
    void addColumn(QSharedPointer<ColumnDesc> column);

    /*!
     *  Removes a column from design vendor extensions.
     *
     *      @param [in] column   The column to remove
     */
    void removeColumn(QSharedPointer<ColumnDesc> column);    

    void addRoute(QSharedPointer<ConnectionRoute> route);
    
    void removeRoute(QSharedPointer<ConnectionRoute> route);

    /*!
     *  Get the interface graphics data.
     *
     *      @return A list containing all the interface graphics data.
     */
    QList<QSharedPointer<InterfaceGraphicsData> > getInterfaceGraphicsData() const;

    /*!
     *  Remove the selected interface graphics data.
     *
     *      @param [in] name    Name of the selected graphics data.
     */
    void removeInterfaceGraphicsData(QString const& name);

private:
    /*!
     *  Copy the shared lists of another design.
     *
     *      @param [in] other   The design being copied.
     */
    void copySharedLists(Design const& other);

    /*!
     *  Gets the vendor extension containing the column layout.
     *
     *      @return The extensions for the column layout.
     */
    QSharedPointer<Kactus2Group> getLayoutExtension() const;
  
    /*!
     *  Gets the vendor extension containing the interconnection routes.
     *
     *      @return The extensions for the interconnection routes.
     */
    QSharedPointer<Kactus2Group> getRoutesExtension() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The HW component instances.
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > componentInstances_;

    //! The interconnections.
    QSharedPointer<QList<QSharedPointer<Interconnection> > > interconnections_;
    
    //! The monitor interconnections.
    QSharedPointer<QList<QSharedPointer<MonitorInterconnection> > > monitorInterconnections_;

    //! Ad-hoc connections
    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > adHocConnections_;

};

#endif // DESIGN_H
