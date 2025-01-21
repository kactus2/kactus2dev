//-----------------------------------------------------------------------------
// File: DesignParameterReferenceTree.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 28.09.2018
//
// Description:
// Tree widget for displaying references to a parameter within a design.
//-----------------------------------------------------------------------------

#ifndef DESIGNPARAMETERREFERENCETREE_H
#define DESIGNPARAMETERREFERENCETREE_H

#include <editors/ComponentEditor/parameterReferenceTree/ParameterReferenceTree.h>

#include <QSharedPointer>

class Design;
class ComponentInstance;
class DesignParameterReferenceCounter;
class AdHocConnection;
class PortReference;

//-----------------------------------------------------------------------------
//! Tree widget for displaying references to a parameter within a design.
//-----------------------------------------------------------------------------
class DesignParameterReferenceTree : public ParameterReferenceTree
{
    Q_OBJECT

public:

    /*!
     *   The constructor.
     *
     *    @param [in] formatter           Formatter for expressions.
     *    @param [in] referenceCounter    Calculates parameter references.
     *    @param [in] parent              The parent widget.
     */
    DesignParameterReferenceTree(QSharedPointer<ExpressionFormatter> formatter,
        QSharedPointer<DesignParameterReferenceCounter> referenceCounter, QWidget* parent = 0);

	/*!
     *  The destructor.
     */
    ~DesignParameterReferenceTree();

    /*!
     *  Set a new design containing the referable parameters.
     *
     *    @param [in] newDesign   The selected design.
     */
    void setDesign(QSharedPointer<Design> newDesign);

private:
    
	// Disable copying.
    DesignParameterReferenceTree(DesignParameterReferenceTree const& rhs);
    DesignParameterReferenceTree& operator=(DesignParameterReferenceTree const& rhs);

    /*!
     *  Setup the reference tree.
     */
    void setupTree();

    /*!
     *  Get the used parameter reference counter.
     *
     *    @return The design parameter reference calculator.
     */
    QSharedPointer<ParameterReferenceCounter> getReferenceCounter() const;

    /*!
     *  Create reference items for the component instances.
     */
    void createReferencesForComponentInstances();

    /*!
     *  Create reference items for a single component instance.
     *
     *    @param [in] instance        The selected component instance.
     *    @param [in] instancesItem   The parent item for the component instance item.
     */
    void createReferencesForSingleComponentInstance(QSharedPointer<ComponentInstance> instance,
        QTreeWidgetItem* instancesItem);

    /*!
     *  Create reference items for ad hoc connections.
     */
    void createReferencesForAdHocConnections();

    /*!
     *  Get the ad hoc connections in order of their referenced components.
     *
     *    @param [in] topComponentName    Identifier for the top component.
     *
     *    @return Map containing the name of the referenced component and ad hoc connections.
     */
    QMultiMap<QString, QSharedPointer<AdHocConnection> > getAdHocConnectionsInComponentOrder(
        QString const& topComponentName) const;

    /*!
     *  Create ad hoc connection reference items for a single component instance.
     *
     *    @param [in] componentName       Name of the selected component instance
     *    @param [in] connections         List of all the ad hoc connections with ports referencing the selected
     *                                      component.
     *    @param [in] connectionsItem     The parent item for the component item.
     */
    void createAdHocItemsForComponent(QString const& componentName,
        QSharedPointer<QList<QSharedPointer<AdHocConnection> > > connections, QTreeWidgetItem* connectionsItem);

    /*!
     *  Create reference items for a single ad hoc connection.
     *
     *    @param [in] connection      The selected ad hoc connection.
     *    @param [in] componentItem   The parent item for the ad hoc connection item.
     */
    void createReferencesForSingleAdHocConnection(QSharedPointer<AdHocConnection> connection,
        QTreeWidgetItem* componentItem);

    /*!
     *  Get the port for the tied value bounds.
     *
     *    @param [in] connection  Ad hoc connection containing the tied value.
     *
     *    @return The ad hoc port containing the tied value bounds.
     */
    QSharedPointer<PortReference> getTiedValuePort(QSharedPointer<AdHocConnection> connection) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component in which this id is situated.
    QSharedPointer<Design> design_;

    //! The design parameter reference calculator.
    QSharedPointer<DesignParameterReferenceCounter> referenceCounter_;
};

#endif // COMPONENTPARAMETERREFERENCETREE_H
