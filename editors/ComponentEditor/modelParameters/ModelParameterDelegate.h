//-----------------------------------------------------------------------------
// File: ModelParameterDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.3.2011
//
// Description:
// Delegate that provides widgets for editing model parameters.
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETERDELEGATE_H
#define MODELPARAMETERDELEGATE_H

#include <QStyledItemDelegate>

#include <editors/ComponentEditor/parameters/ParameterDelegate.h>

class Choice;

//-----------------------------------------------------------------------------
//! Delegate that provides widgets for editing model parameters.
//-----------------------------------------------------------------------------
class ModelParameterDelegate : public ParameterDelegate
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *     @param [in] choices              The choices available for model parameter value.
     *     @param [in] parameterCompleter   The completer to use for expression editors.
     *     @param [in] parameterFinder      The parameter finder.
     *     @param [in] expressionFormatter  The expression formatter.
	 *     @param [in] parent               The parent object
	*/
	ModelParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices,
        QCompleter* parameterCompleter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QObject *parent = 0);

	//! The destructor
	virtual ~ModelParameterDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent   Owner for the editor.
	 *      @param [in] option   Contains options for the editor.
	 *      @param [in] index    Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

protected:

    /*!
     *  Gets the column for name.
     *
     *      @return The column index for editing name.
     */
    virtual int nameColumn() const;

    /*!
     *  Gets the column for choices.
     *
     *      @return The column for editing choice selection.
     */
    virtual int choiceColumn() const;
    
    /*!
     *  Gets the column for value format.
     *
     *      @return The column for editing format selection.
     */
    virtual int formatColumn() const;
        
    /*!
     *  Gets the column for value bit string length.
     *
     *      @return The column for editing bit string length.
     */
    virtual int bitwidthColumn() const;

    /*!
     *  Gets the column for minimum value.
     *
     *      @return The column for editing the minimum value.
     */
    virtual int minimumColumn() const;
        
    /*!
     *  Gets the column for maximum value.
     *
     *      @return The column for editing the maximum value.
     */
    virtual int maximumColumn() const;

    /*!
     *  Gets the column for values.
     *
     *      @return The column for editing value selection.
     */
    virtual int valueColumn() const;
    
    /*!
     *  Gets the column for resolve.
     *
     *      @return The column for resolve selection.
     */
    virtual int resolveColumn() const;

    /*!
     *  Gets the column index for description.
     *
     *      @return     The column index for description section.
     */
    virtual int descriptionColumn() const;
    
    /*!
     *  Gets the column index for array size.
     *
     *      @return     The column index for array size.
     */
    virtual int arraySizeColumn() const;

    /*!
     *  Gets the column index for array offset.
     *
     *      @return     The column index for array offset.
     */
    virtual int arrayOffsetColumn() const;

    /*!
     *  Gets the column index for usage count.
     *
     *      @return     The column index for usage count.
     */
    virtual int usageCountColumn() const;

    /*!
     *  Gets the column index for the unique id.
     *
     *      @return     The column index for id.
     */
    virtual int valueIdColumn() const;

private:

	//! No copying
	ModelParameterDelegate(const ModelParameterDelegate& other);

	//! No assignment
	ModelParameterDelegate& operator=(const ModelParameterDelegate& other);


};

#endif // MODELPARAMETERDELEGATE_H
