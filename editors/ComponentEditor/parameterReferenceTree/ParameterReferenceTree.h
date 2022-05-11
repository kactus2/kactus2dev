//-----------------------------------------------------------------------------
// File: ParameterReferenceTree.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.01.2015
//
// Description:
// Tree widget for displaying references to a parameter.
//-----------------------------------------------------------------------------

#ifndef PARAMETERREFERENCETREE_H
#define PARAMETERREFERENCETREE_H

#include <KactusAPI/include/ExpressionFormatter.h>

#include <QTreeWidget>
#include <QSharedPointer>

class Parameter;
class ConfigurableElementValue;
class ParameterReferenceCounter;

//-----------------------------------------------------------------------------
//! Tree widget for displaying references to a parameter.
//-----------------------------------------------------------------------------
class ParameterReferenceTree : public QTreeWidget 
{
    Q_OBJECT
public:

    //! Enumeration of the columns.
    enum COLUMNS
    {
        ITEM_NAME = 0,
        ITEM_EXPRESSION,
        COLUMN_COUNT
    };
    
    /*!
     *   The constructor.
     *
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] parent                  The parent widget.
     */
    ParameterReferenceTree(QSharedPointer<ExpressionFormatter> formatter, QWidget* parent = 0);

	/*!
     *  The destructor.
     */
    ~ParameterReferenceTree();

    /*!
     *  Setup the parameter reference tree for the selected parameter.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     */
    void openReferenceTree(QString const& parameterID);

protected:

    /*!
     *  Get the ID of the selected parameter.
     *
     *      @return ID of the selected parameter.
     */
    QString getTargetID() const;

    /*!
     *  Create the references for parameters.
     *
     *      @param [in] parameters      List of parameters containing a reference.
     *      @param [in] parentItem      The parent item for references.
     */
    void createParameterReferences(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
        QTreeWidgetItem* parentItem);

    /*!
     *  Create references for configurable element values.
     *
     *      @param [in] elements    List of the configurable element values.
     *      @param [in] parent      The parent tree item.
     */
    void createReferencesForConfigurableElementValues(
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > elements, QTreeWidgetItem* parent);

    /*!
     *  Create a tree widget item to the top of the tree.
     *
     *      @param [in] itemName    The name of the upcoming item.
     *
     *      @return A pointer to the new tree item.
     */
    QTreeWidgetItem* createTopItem(QString const& itemName);

    /*!
     *  Create a tree item between two items.
     *
     *      @param [in] itemnName   The name of the upcoming item.
     *      @param [in] parent      The parent of the upcoming item.
     *
     *      @return A pointer to the new tree item.
     */
    QTreeWidgetItem* createMiddleItem(QString const& itemnName, QTreeWidgetItem* parent);

    /*!
     *  Create tree items for a parameter that is referencing this parameter.
     *
     *      @param [in] parameter   The referencing parameter.
     *      @param [in] parent      The parent of the upcoming item.
     */
    void createItemsForParameter(QSharedPointer<Parameter> parameter, QTreeWidgetItem* parent);

    /*!
     *  Create a referencing tree item.
     *
     *      @param [in] itemName        The name of the upcoming tree item.
     *      @param [in] expression      The expression where the upcoming tree item references this parameter.
     *      @param [in] parent          The parent of the upcoming item.
     */
    void createItem(QString const& itemName, QString const& expression, QTreeWidgetItem* parent);

    /*!
     *  Colour the background of the selected item grey.
     *
     *      @param [in] item    The selected item.
     */
    void colourItemGrey(QTreeWidgetItem* item);

private:
    
	// Disable copying.
    ParameterReferenceTree(ParameterReferenceTree const& rhs);
    ParameterReferenceTree& operator=(ParameterReferenceTree const& rhs);

    /*!
     *  Setup the reference tree.
     */
    virtual void setupTree() = 0;

    /*!
     *  Get the parameter reference calculator.
     */
    virtual QSharedPointer<ParameterReferenceCounter> getReferenceCounter() const = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parameter whose references are being searched for.
    QString targetID_;

    //! The formatter for the expressions.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // PARAMETERREFERENCETREE_H
