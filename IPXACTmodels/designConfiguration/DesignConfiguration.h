//-----------------------------------------------------------------------------
// File: DesignConfiguration.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.08.2015
//
// Description:
// Implementation for ipxact:designConfiguration element.
//-----------------------------------------------------------------------------

#ifndef DESIGNCONFIGURATION_H
#define DESIGNCONFIGURATION_H

#include <IPXACTmodels/common/Assertion.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/designConfiguration/InterconnectionConfiguration.h>
#include <IPXACTmodels/designConfiguration/ViewConfiguration.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QList>
#include <QString>
#include <QMap>
#include <QDomDocument>
#include <QDomNode>
#include <QStringList>
#include <QFile>
#include <QMap>

class VLNV;

//-----------------------------------------------------------------------------
//! Implementation for the ipxact:designConfiguration element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignConfiguration : public Document
{

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] vlnv    VLNV to be set for the design configuration.
	 */
	DesignConfiguration(VLNV const& vlnv);

    /*!
	 *  The constructor.
	 */
	DesignConfiguration();

    /*!
	 *  Copy constructor.
	 */
	DesignConfiguration(const DesignConfiguration& other);

    /*!
	 *  Assignment operator.
	 */
	DesignConfiguration& operator=(const DesignConfiguration& other);

    /*!
	 *  The destructor.
	 */
	virtual ~DesignConfiguration();

    /*!
	 *  Clone this design configuration.
	 */
	virtual QSharedPointer<Document> clone() const;

    /*!
	 *  Set the vlnv.
	 *
	 *      @param [in] vlnv    Reference to the vlnv to be set.
	 */
	virtual void setVlnv(const VLNV& vlnv);

    /*!
     *  Get the design reference.
     *
     *      @return VLNV element representing the design.
     */
    VLNV getDesignRef() const;

    /*!
     *  Get the list of generator chain configurations.
     *
     *      @return The list containing pointers to the generator chain configurations.
     */
    QSharedPointer<QList<QSharedPointer<ConfigurableVLNVReference> > > getGeneratorChainConfs();

    /*!
     *  Get the list of interconnection configurations.
     *
     *      @return A list containing pointers to the interconnection configurations.
     */
    QSharedPointer<QList<QSharedPointer<InterconnectionConfiguration> > > getInterconnectionConfs();

    /*!
     *  Get the list of view configurations.
     *
     *      @return A list containing pointers to the view configurations.
     */
    QSharedPointer<QList<QSharedPointer<ViewConfiguration> > > getViewConfigurations();

    /*!
     *  Set the design reference.
     *
     *      @param [in] designRef   A VLNV tag that identifies the design.
     */
    void setDesignRef(const VLNV& designRef);

    /*!
     *  Set the generator chain configurations.
     *
     *      @param [in] generatorChainConfs     A list containing pointers to the generator chain configurations.
     */
    void setGeneratorChainConfs(
        QSharedPointer<QList<QSharedPointer<ConfigurableVLNVReference> > > generatorChainConfs);

    /*!
     *  Set the interconnection configurations.
     *
     *      @param [in] interconnectionConfs    A list containing pointers to the interconnection configurations.
     */
    void setInterconnectionConfs(
        QSharedPointer<QList<QSharedPointer<InterconnectionConfiguration> > > interconnectionConfs);

    /*!
     *  Set the view configurations.
     *
     *      @param [in] newViewConfigurations   A list containing pointers to the view configurations.
     */
    void setViewConfigurations(QSharedPointer<QList<QSharedPointer<ViewConfiguration> > > newViewConfigurations);

    /*!
     *  Get the list of the files needed by this design configuration.
     *
     *      @return Empty QStringlist (contains VLNV dependencies).
     */
    virtual QStringList getDependentFiles() const;

    /*!
     *  Get the list of VLNVs needed by this design configuration.
     *
     *      @return A list of pointers to VLNVs that are needed in this design configuration.
     */
    virtual QList<VLNV> getDependentVLNVs() const;

    /*!
	 *  Add a new view configuration to the design configuration.
	 *
	 *      @param [in] instanceName    The name of the component instance that's view is set.
	 *      @param [in] viewName        The name of the component's view that is set as active.
	 */
	virtual void addViewConfiguration(QString const& instanceName, QString const& viewName);

    /*!
	 *  Remove the view configuration.
	 *
	 *      @param [in] instanceName    The instance name of the view configuration to be removed.
	 */
	virtual void removeViewConfiguration(QString const& instanceName);

    /*!
	 *  Get the active view name for the given component instance.
	 *
	 *      @param [in] instanceName   Identifies the component instance.
     *
     *      @return The name of the active view.
	 */
	QString getActiveView(QString const& instanceName) const;

    /*!
	 *  Check if an active view has been defined for the given instance.
	 *
	 *      @param [in] instanceName    The name of the component instance that's active view is searched for.
     *
     *      @return True if an active view is found, false otherwise.
	 */
	bool hasActiveView(QString const& instanceName) const;

    /*!
	 *  Set the implementation type of the design configuration.
	 *
	 *      @param [in] implementation  The implementation type.
	 */
	void setDesignConfigImplementation(KactusAttribute::Implementation implementation);

	/*!
	 *  Get the implementation type.
	 *
     *      @return The implementation type.
	 */
	KactusAttribute::Implementation getDesignConfigImplementation() const;
    
    /*!
     *  Sets the override configurable elemnt values for a given component instance.
     *
     *      @param [in] instanceUUID                The id for instance to get the values.
     *      @param [in] configurableElementValues   The values to be saved in the design configuration.
     */
    void setConfigurableElementValues(QString const& instanceUUID, 
        QMap<QString, QString> const& configurableElementValues);

    /*!
     *  Finds and creates instance extensions.
     *
     *      @param [in] instanceUUID    The id of the instance whose extension is being searched.
     *
     *      @return The instance extension.
     */
    QSharedPointer<VendorExtension> findOrCreateInstanceExtension(QString const& instanceUUID);

    /*!
     *  Gets the override configurable element values for a given component instance identified by UUID.
     *
     *      @param [in] instanceUUID   The uuid for the instance to get the values.
     *
     *      @return Configurable element values for the instance.
     */
    QMap<QString, QString> getConfigurableElementValues(QString const& instanceUUID) const;

    /*!
     *  Gets the override view configuration.
     *
     *      @return Pairs of instance id and their overridden views.
     */
    QMap<QString, QString> getKactus2ViewOverrides() const;

    /*!
     *  Sets the override view configuration.
     *
     *      @param [in] kactus2ViewOverrides    The new view overrides.
     */
    void setKactus2ViewOverrides(QMap<QString, QString> kactus2ViewOverrides);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design.
    VLNV designRef_;

    //! A list containing pointers to the generator chain configurations.
    QSharedPointer<QList<QSharedPointer<ConfigurableVLNVReference> > > generatorChainConfigurations_;

    //! A list containing pointers to the interconnection configurations.
    QSharedPointer<QList<QSharedPointer<InterconnectionConfiguration> > > interconnectionConfigurations_;

    //! A list containing pointers to the view configurations.
    QSharedPointer<QList<QSharedPointer<ViewConfiguration> > > viewConfigurations_;
};

#endif // DESIGNCONFIGURATION_H
