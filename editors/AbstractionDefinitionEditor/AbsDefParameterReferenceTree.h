//-----------------------------------------------------------------------------
// File: AbsDefParameterReferenceTree.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.08.2024
//
// Description:
// Tree widget for displaying references to a parameter within an abstraction definition.
//-----------------------------------------------------------------------------

#ifndef ABSDEFPARAMETERREFERENCETREE_H
#define ABSDEFPARAMETERREFERENCETREE_H

#include <editors/ComponentEditor/parameterReferenceTree/ParameterReferenceTree.h>

class AbsDefParameterReferenceCounter;
class AbstractionDefinition;

class AbsDefParameterReferenceTree : public ParameterReferenceTree
{
    Q_OBJECT

public:
    
    /*!
     *	The constructor.
     *  
     *      @param [in] absDef                  The abstraction definition.
     *      @param [in] referenceCounter        The reference counter to use.
     *      @param [in] expressionFormatter     The expression formatter to use.
     */
    AbsDefParameterReferenceTree(QSharedPointer<AbstractionDefinition> absDef,
        QSharedPointer<AbsDefParameterReferenceCounter> referenceCounter,
        QSharedPointer<ExpressionFormatter> expressionFormatter);

    virtual ~AbsDefParameterReferenceTree() = default;

    AbsDefParameterReferenceTree(AbsDefParameterReferenceTree const& other) = delete;
    AbsDefParameterReferenceTree& operator=(AbsDefParameterReferenceTree const& other) = delete;

    /*!
     *	Set a new abstraction definition.
     *  
     *      @param [in] absDef     The abstraction definition to set.
     */
    void setAbsDef(QSharedPointer<AbstractionDefinition> absDef);

private:
    
    /*!
     *  Setup the reference tree.
     */
    void setupTree() final;

    /*!
     *  Get the parameter reference calculator.
     */
    QSharedPointer<ParameterReferenceCounter> getReferenceCounter() const final;

    //! The reference counter to use.
    QSharedPointer<AbsDefParameterReferenceCounter> referenceCounter_;

    //! The abstraction definition whose parameter references are counted.
    QSharedPointer<AbstractionDefinition> absDef_;
};

#endif // ABSDEFPARAMETERREFERENCETREE_H
