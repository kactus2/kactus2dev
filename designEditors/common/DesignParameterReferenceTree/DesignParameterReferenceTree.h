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
     *      @param [in] formatter   Formatter for expressions.
     *      @param [in] parent      The parent widget.
     */
    DesignParameterReferenceTree(QSharedPointer<ExpressionFormatter> formatter, QWidget* parent = 0);

	/*!
     *  The destructor.
     */
    ~DesignParameterReferenceTree();

    /*!
     *  Set a new design containing the referable parameters.
     *
     *      @param [in] newDesign   The selected design.
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
     *  Check if a reference exists in the contained component instances.
     *
     *      @return True, if a reference exists in the contained component instances, false otherwise.
     */
    bool referenceExistsInComponentInstances() const;

    /*!
     *  Check if a reference exists in the selected component instance.
     *
     *      @param [in] instance    The selected component instance.
     *
     *      @return True, if a reference exists in the selected component instance, false otherwise.
     */
    bool referenceExistsInSingleComponentInstance(QSharedPointer<ComponentInstance> instance) const;

    /*!
     *  Create reference items for the component instances.
     */
    void createReferencesForComponentInstances();

    /*!
     *  Create reference items for a single component instance.
     *
     *      @param [in] instance        The selected component instance.
     *      @param [in] instancesItem   The parent item for the component instance item.
     */
    void createReferencesForSingleComponentInstance(QSharedPointer<ComponentInstance> instance,
        QTreeWidgetItem* instancesItem);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component in which this id is situated.
    QSharedPointer<Design> design_;
};

#endif // COMPONENTPARAMETERREFERENCETREE_H
