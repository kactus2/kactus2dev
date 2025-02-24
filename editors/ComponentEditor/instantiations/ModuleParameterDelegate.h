//-----------------------------------------------------------------------------
// File: ModuleParameterDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 31.3.2011
//
// Description:
// Delegate that provides widgets for editing model parameters.
//-----------------------------------------------------------------------------

#ifndef ModuleParameterDelegate_H
#define ModuleParameterDelegate_H

#include <IPXACTmodels/common/Document.h>

#include <QStyledItemDelegate>

#include <editors/ComponentEditor/parameters/ParameterDelegate.h>

class Choice;

//-----------------------------------------------------------------------------
//! Delegate that provides widgets for editing model parameters.
//-----------------------------------------------------------------------------
class ModuleParameterDelegate : public ParameterDelegate
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *    @param [in] choices              The choices available for model parameter value.
     *    @param [in] completionModel     Model containing the completions used in expression editor.
     *    @param [in] parameterFinder      The parameter finder.
     *    @param [in] expressionFormatter  The expression formatter.
     *    @param [in] docRevision          The IP-XACT standard revision in use.
	 *    @param [in] parent               The parent object
	*/
	ModuleParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices,
        QAbstractItemModel* completionModel,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        Document::Revision docRevision,
        QObject *parent = 0);

	//! The destructor.
	virtual ~ModuleParameterDelegate();

	/*!
     *  Create a new editor for the given item
	 *
	 *    @param [in] parent   Owner for the editor.
	 *    @param [in] option   Contains options for the editor.
	 *    @param [in] index    Model index identifying the item.
	 *
	 *    @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

protected:

    /*!
     *  Gets the column for name.
     *
     *    @return The column index for editing name.
     */
    virtual int nameColumn() const;

    /*!
     *  Gets the column for choices.
     *
     *    @return The column for editing choice selection.
     */
    virtual int choiceColumn() const;
    
    /*!
     *  Gets the column for value format.
     *
     *    @return The column for editing format selection.
     */
    virtual int formatColumn() const;

    /*!
    *  Gets the column for the left value of bit width vector.
    *
    *    @return The column for editing the left value of the bit width vector.
    */
    virtual int bitWidthLeftColumn() const;

    /*!
    *  Gets the column for the right value of bit width vector.
    *
    *    @return The column for editing the right value of the bit width vector.
    */
    virtual int bitWidthRightColumn() const;

    /*!
     *  Gets the column for minimum value.
     *
     *    @return The column for editing the minimum value.
     */
    virtual int minimumColumn() const;
        
    /*!
     *  Gets the column for maximum value.
     *
     *    @return The column for editing the maximum value.
     */
    virtual int maximumColumn() const;

    /*!
     *  Gets the column for values.
     *
     *    @return The column for editing value selection.
     */
    virtual int valueColumn() const;
    
    /*!
     *  Gets the column for resolve.
     *
     *    @return The column for resolve selection.
     */
    virtual int resolveColumn() const;

    /*!
     *  Gets the column index for description.
     *
     *    @return     The column index for description section.
     */
    virtual int descriptionColumn() const;
    
    /*!
     *  Gets the column index for array left.
     *
     *    @return     The column index for array left side.
     */
    virtual int arrayLeftColumn() const;

    /*!
     *  Gets the column index for array right.
     *
     *    @return     The column index for array right side.
     */
    virtual int arrayRightColumn() const;

    /*!
     *  Gets the column index for usage count.
     *
     *    @return     The column index for usage count.
     */
    virtual int usageCountColumn() const;

    /*!
     *  Gets the column index for the id of the model parameter.
     *
     *    @return     The column index for model parameter id.
     */
    virtual int idColumn() const;

private:

	//! No copying
	ModuleParameterDelegate(const ModuleParameterDelegate& other);

	//! No assignment
	ModuleParameterDelegate& operator=(const ModuleParameterDelegate& other);

    //! The IP-XACT standard revision in use.
    Document::Revision docRevision_;
};

#endif // ModuleParameterDelegate_H
