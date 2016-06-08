//-----------------------------------------------------------------------------
// File: Model.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.10.2015
//
// Description:
// Describes the ipxact:model element in an IP-XACT document
//-----------------------------------------------------------------------------

#ifndef MODEL_H
#define MODEL_H

#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QList>
#include <QSharedPointer>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Describes the ipxact:model element in an IP-XACT document
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Model
{

public:

	/*!
     *  The default constructor.
	 */
	Model();

	//! Copy constructor.
	Model(const Model &other);

	//! Assignment operator.
	Model& operator=(const Model& other);

	/*!
     *  The destructor.
	 */
	~Model();

	/*!
     *  Get the views of this model.
	 *
	 *      @return Pointer to a lis containing pointers to the views of this model.
	 */
    QSharedPointer<QList<QSharedPointer<View> > > getViews() const;

	/*!
     *  Set the views for this model.
	 *
	 *      @param [in] newViews    Pointer to a list containing pointers to the views.
	 */
	void setViews(QSharedPointer<QList<QSharedPointer<View> > > newViews);
	
	/*
     *  Get a component instantiation by name.
	 *
	 *      @return The component instantiation if found, else null.
	 */
	QSharedPointer<ComponentInstantiation> findComponentInstantiation(QString const& name) const;

    /*!
     *  Get the component instantiations.
     *
     *      @return Pointer to a list containing the component instantiations.
     */
    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > getComponentInstantiations() const;

    /*!
     *  Set the component instantiations.
     *
     *      @param [in] instantiations  Pointer to a list containing the new component instantiations.
     */
    void setComponentInstantiations(QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations);
	
	/*
     *  Get a design instantiation by name.
	 *
	 *      @return The design instantiation if found, else null.
	 */
	QSharedPointer<DesignInstantiation> findDesignInstantiation(QString const& name) const;

    /*!
     *  Get the design instantiations.
     *
     *      @return Pointer to a list containing the design instantiations.
     */
    QSharedPointer<QList<QSharedPointer<DesignInstantiation> > > getDesignInstantiations() const;

    /*!
     *  Set the design instantiations.
     *
     *      @param [in] instantiations  Pointer to a list containing the new design instantiations.
     */
    void setDesignInstantiations(QSharedPointer<QList<QSharedPointer<DesignInstantiation> >  > newInstantiations);
	
	/*
     *  Get a design configuration instantiation by name.
	 *
	 *      @return The design configuration instantiation if found, else null.
	 */
	QSharedPointer<DesignConfigurationInstantiation> findDesignConfigurationInstantiation(QString const& name) const;

    /*!
     *  Get the design configuration instantiations.
     *
     *      @return Pointer to a list containing the design configuration instantiations.
     */
    QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > >
        getDesignConfigurationInstantiations() const;

    /*!
     *  Set the design conifguration instantiations.
     *
     *      @param [in] instantiations  Pointer to a list containing the new design configuration instantiations.
     */
    void setDesignConfigurationInstantiations(
        QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > newInstantiations);
    
	/*!
     *  Get the ports of this model.
	 *
	 *      @return Pointer to a list containing the ports.
	 */
    QSharedPointer<QList<QSharedPointer<Port> > > getPorts() const;

    /*!
     *  Set the ports.
     *
     *      @param [in] newPorts    Pointer to a list containing the new ports.
     */
    void setPorts(QSharedPointer<QList<QSharedPointer<Port> > > newPorts);

	/*!
     *  Checks if the model has views.
	 *
	 *      @return bool True if views exist.
	 */
	bool hasViews() const;
    
	/*!
     *  Get list of the view names in this model.
	 *
	 *      @return QStringList containing the view names.
	 */
	QStringList getViewNames() const;

	/*!
     *  Get the hierarchical views of the model.
	 *
	 *      @return QStringList containing the names of the hierarchical views.
	 */
	QStringList getHierViews() const;

	/*
     *  Get the non-hierarchical views of the model.
	 *
	 *      @return QStringList containing the names of the non-hierarchical views.
	 */
	QStringList getFlatViews() const;

	/*
     *  Get a view by name.
	 *
	 *      @return The view if found, else null.
	 */
	QSharedPointer<View> findView(const QString name) const;
    
	/*!
     *  Get the VLNV of a design in model's view.
	 *
	 *      @return A vlnv of a design that is used within this component. Null pointer if design is not found.
	 */
	VLNV getHierRef(QString const& viewName = QString()) const;

	/*!
     *  Get list of the design VLNVs that are referenced within model.
	 *
	 *      @return QList containing pointers to the VLNVs
	 */
	QList<VLNV> getHierarchyRefs() const;
    
	/*!
     *  Get the specific port of the model.
     *
	 *      @param [in] name    Name of the port.
	 *
	 *      @return Pointer to the specified port.
	 */
	QSharedPointer<Port> getPort(const QString& name) const;

    /*!
     *  Check if the model has a specified port.
     *
     *      @param [in] name    Name of the port being searched for.
     *
     *      @return True, if the port is found, false otherwise.
     */
    bool hasPort(QString const& name) const;

	/*!
     *  Get names of the ports in this model.
	 *
	 *      @return QStringList containing the port names.
	 */
	QStringList getPortNames() const;

	/*!
     *  Checks if the model has ports.
	 *
	 *      @return bool True if ports exist.
	 */
	bool hasPorts() const;

	/*!
     *  Does this model contains a hierarchical view.
     *
	 *      @return True if a hierarchyRef is found in one of the views, otherwise false.
	 */
	bool hasHierView() const;
    
    /*!
     *  Get the references of the views.
     *
     *      @return A list of VLNVs referenced by the views.
     */
    QList<VLNV> getViewReferences() const;
    
    /*!
     *  Check if the model has any contents.
     *
     *      @return True, if contents are found, false otherwise.
     */
    bool hasContents() const;

private:

    /*!
     *  Copy the views.
     *
     *      @param [in] other   The model being copied.
     */
    void copyViews(const Model& other) const;

    /*!
     *  Copy the component instantiations.
     *
     *      @param [in] other   The model being copied.
     */
    void copyComponentInstantiations(const Model& other) const;

    /*!
     *  Copy the design instantiations.
     *
     *      @param [in] other   The model being copied.
     */
    void copyDesignInstantiations(const Model& other) const;

    /*!
     *  Copy the design configurations.
     *
     *      @param [in] other   The model being copied.
     */
    void copyDesignConfigurationInstantiations(const Model& other) const;

    /*!
     *  Copy the ports.
     *
     *      @param [in] other   The model being copied.
     */
    void copyPorts(const Model& other) const;

	//! Contains the views for this model.
    QSharedPointer<QList<QSharedPointer<View> > > views_;

    //! Contains the component instantiations.
    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > componentInstantiations_;

    //! Contains the design instantiations.
    QSharedPointer<QList<QSharedPointer<DesignInstantiation> > > designInstantiations_;

    //! Contains the design configuration instantiations.
    QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > designConfigurationInstantiations_;

	//! Contains the ports for this model.
    QSharedPointer<QList<QSharedPointer<Port> > > ports_;
};

#endif // MODEL_H
