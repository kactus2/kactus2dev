//-----------------------------------------------------------------------------
// File: BusDefinitionParameterReferenceTree.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 22.08.2024
//
// Description:
// Tree widget for displaying references to a parameter within a bus definition.
//-----------------------------------------------------------------------------

#ifndef BUSDEFINITIONPARAMETERREFERENCETREE_H
#define BUSDEFINITIONPARAMETERREFERENCETREE_H

#include <editors/ComponentEditor/parameterReferenceTree/ParameterReferenceTree.h>

class ParameterReferenceCounter;
class BusDefinition;

class BusDefinitionParameterReferenceTree : public ParameterReferenceTree
{
    Q_OBJECT

public:

    /*!
     *   The constructor.
     *
     *    @param [in] busDef                  The bus definition whose references to display.
     *    @param [in] expressionFormatter     Formatter for expressions.
     *    @param [in] referenceCounter        Calculator for parameter references.
     *    @param [in] parent                  The parent widget.
     */
    BusDefinitionParameterReferenceTree(QSharedPointer<BusDefinition> busDef, QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ParameterReferenceCounter> referenceCounter, QWidget* parent = nullptr);

    virtual ~BusDefinitionParameterReferenceTree() = default;
    
    /*
     *  No copying or assignment.
     */
    BusDefinitionParameterReferenceTree(BusDefinitionParameterReferenceTree& other) = delete;
    BusDefinitionParameterReferenceTree& operator=(BusDefinitionParameterReferenceTree& other) = delete;

private:

    /*!
     *  Setup the reference tree.
     */
    void setupTree();

    /*!
     *  Get the parameter reference calculator.
     */
    QSharedPointer<ParameterReferenceCounter> getReferenceCounter() const;

    //! The parameter reference counter to use.
    QSharedPointer<ParameterReferenceCounter> referenceCounter_;

    //! The bus definition containing the referenced parameter.
    QSharedPointer<BusDefinition> busDef_;

};

#endif // BUSDEFINITIONPARAMETERREFERENCETREE_H
