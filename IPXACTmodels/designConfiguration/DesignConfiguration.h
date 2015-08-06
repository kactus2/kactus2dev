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
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/designConfiguration/InterconnectionConfiguration.h>
#include <IPXACTmodels/designConfiguration/ViewConfiguration.h>
#include <IPXACTmodels/librarycomponent.h>
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
#include <QMap>

class VLNV;

//-----------------------------------------------------------------------------
//! Implementation for the ipxact:designConfiguration element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignConfiguration : public LibraryComponent
{

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] doc     A reference to a QDomDocument that represents the information for parsing.
	 */
	DesignConfiguration(QDomDocument& doc);

    /*!
	 *  The constructor.
	 *
	 *      @param [in] vlnv    VLNV to be set for the design configuration.
	 */
	DesignConfiguration(const VLNV& vlnv);

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
	 *  Clone this design configuration and return pointer to the copy.
	 */
	virtual QSharedPointer<LibraryComponent> clone() const;

    /*!
	 *  Library component contains write function as purely virtual. This should be deleted
	 */
	virtual void write(QFile& file);

    /*!
	 *  Check the validity of the design configuration.
	 *
	 *      @param [in] errorList   A list of error messages of the detected errors.
     *
     *      @return True if the design configuration is valid, otherwise false.
	 */
	virtual bool isValid(QStringList& errorList) const;


    /*!
	 *  Check the validity of the design configuration.
	 *
     *      @return True if the state is valid, otherwise false.
	 */
	virtual bool isValid() const;

    /*!
	 *  Set the vlnv.
	 *
	 *      @param [in] vlnv    Reference to the vlnv to be set.
	 */
	virtual void setVlnv(const VLNV& vlnv);

    /*!
     *  Get the description.
     *
     *      @return The description.
     */
    QString getDescription() const;

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
    const QList<QSharedPointer<ConfigurableVLNVReference > > getGeneratorChainConfs();

    /*!
     *  Get the list of interconnection configurations.
     *
     *      @return A list containing pointers to the interconnection configurations.
     */
    const QList<QSharedPointer<InterconnectionConfiguration> >& getInterconnectionConfs();

    /*!
     *  Get the list of view configurations.
     *
     *      @return A list containing pointers to the view configurations.
     */
    QList<QSharedPointer<ViewConfiguration> > getViewConfigurations();

    /*!
     *  Get the attributes.
     *
     *      @return A map containing the attribute key-value pairs.
     */
    const QMap<QString, QString>& getAttributes();

    /*!
     *  Set the attributes.
     *
     *      @param [in] attributes  A map of key-value pairs.
     */
    void setAttributes(const QMap<QString, QString>& attributes);

    /*!
     *  Set the description.
     *
     *      @param [in] description     The new description.
     */
    void setDescription(const QString& description);

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
    void setGeneratorChainConfs(QList<QSharedPointer<ConfigurableVLNVReference> > generatorChainConfs);

    /*!
     *  Set the interconnection configurations.
     *
     *      @param [in] interconnectionConfs    A list containing pointers to the interconnection configurations.
     */
    void setInterconnectionConfs(QList<QSharedPointer<InterconnectionConfiguration> >& interconnectionConfs);

    /*!
     *  Set the view configurations.
     *
     *      @param [in] newViewConfigurations   A list containing pointers to the view configurations.
     */
    void setViewConfigurations(QList<QSharedPointer<ViewConfiguration> > newViewConfigurations);

    /*!
     *  Get the list of the files needed by this design configuration.
     *
     *      @return Empty QStringlist (contains VLNV dependencies).
     */
    virtual const QStringList getDependentFiles() const;

    /*!
     *  Get the list of VLNVs needed by this design configuration.
     *
     *      @return A list of pointers to VLNVs that are needed in this design configuration.
     */
    virtual const QList<VLNV> getDependentVLNVs() const;

    /*!
	 *  Add a new view configuration to the design configuration.
	 *
	 *      @param [in] instanceName    The name of the component instnace that's view is set.
	 *      @param [in] viewName        The name of the component's view that is set as active.
	 */
	virtual void addViewConfiguration(const QString& instanceName, const QString& viewName);

    /*!
	 *  Remove the view configuration.
	 *
	 *      @param [in] instanceName    The instance name of the view configuration to be removed.
	 */
	virtual void removeViewConfiguration(const QString& instanceName);

    /*!
	 *  Get the active view name for the given component instance.
	 *
	 *      @param [in] instanceName   Identifies the component instance.
     *
     *      @return The name of the active view.
	 */
	QString getActiveView(const QString& instanceName) const;

    /*!
	 *  Check if an active view has been defined for the given instance.
	 *
	 *      @param [in] instanceName    The name of the component instance that's active view is searched for.
     *
     *      @return True if an active view is found, false otherwise.
	 */
	bool hasActiveView(const QString& instanceName) const;

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

    /*!
     *  Get the assertions.
     *
     *      @return A list of pointers to the assertions
     */
    QSharedPointer<QList<QSharedPointer<Assertion> > > getAssertions() const;

    /*!
     *  Set the assertions.
     *
     *      @param [in] newAssertions   A list of new assertions.
     */
    void setAssertions(QSharedPointer<QList<QSharedPointer<Assertion> > > newAssertions);

    /*!
     *  Get the parameters.
     *
     *      @return A list of pointers to the parameters.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

    /*!
     *  Set the parameters.
     *
     *      @param [in] newParameters   A list of new parameters.
     */
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters);

private:

    /*!
     *  Parses the vendor extensions of the design from a QDomNode.
     *
     *      @param [in] node The QDomNode from where to parse the information.
     */
    void parseVendorExtensions(QDomNode const& extensionsNode);

    /*!
     *  Parses the override configurable element values for component instances.
     *
     *      @param [in] extensionNode   The DOM node containing the extensions for configurable element values.
     *
     *      @remark This is for Kactus2 extensions.
     */
    void parseConfigurableElementValues(QDomNode const& extensionNode);

    /*!
     *  Parses the view configuration values.
     *
     *      @param [in] extensionNode   The DOM node containing the extensions for view configurations.
     */
    void parseViewConfigurationValues(QDomNode const& extensionNode);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design.
    VLNV designRef_;

    //! A list containing pointers to the generator chain configurations.
    QList<QSharedPointer<ConfigurableVLNVReference> > generatorChainConfigurations_;

    //! A list containing pointers to the interconnection configurations.
    QList<QSharedPointer<InterconnectionConfiguration> > interconnectionConfigurations_;

    //! A list containing pointers to the view configurations.
    QList<QSharedPointer<ViewConfiguration> > viewConfigurations_;

    //! The textual description.
    QString description_;

    //! Contains the attributes for the design configuration.
    QMap<QString, QString> attributes_;

    //! A list containing pointers to the assertions.
    QSharedPointer<QList<QSharedPointer<Assertion> > > assertions_;

    //! A list containing pointers to the parameters.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
};

#endif // DESIGNCONFIGURATION_H
