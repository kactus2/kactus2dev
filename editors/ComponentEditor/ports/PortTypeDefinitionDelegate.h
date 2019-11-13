//-----------------------------------------------------------------------------
// File: PortTypeDefinitionDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.11.2017
//
// Description:
// The delegate providing editors for port type definition.
//-----------------------------------------------------------------------------

#ifndef PORTTYPEDEFINITIONDELEGATE_H
#define PORTTYPEDEFINITIONDELEGATE_H

#include <QStyledItemDelegate>

class BasicLineContentMatcher;

//-----------------------------------------------------------------------------
//! The delegate providing editors for port type definition.
//-----------------------------------------------------------------------------
class PortTypeDefinitionDelegate : public QStyledItemDelegate
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent object.
     */
    explicit PortTypeDefinitionDelegate(QObject* parent = 0);
	
	/*!
     *  The destructor.
     */
    virtual ~PortTypeDefinitionDelegate();

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner of the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor  The editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor  The editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

private:

    // No copying. No assignment.
    PortTypeDefinitionDelegate(PortTypeDefinitionDelegate const& rhs);
    PortTypeDefinitionDelegate& operator=(PortTypeDefinitionDelegate const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The content matcher for type definitions.
    BasicLineContentMatcher* matcher_;
};

#endif // PORTTYPEDEFINITIONDELEGATE_H
