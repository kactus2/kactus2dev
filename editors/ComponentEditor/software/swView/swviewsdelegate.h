//-----------------------------------------------------------------------------
// File: swviewsdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.06.2012
//
// Description:
// The delegate class that provides editors for editing software views.
//-----------------------------------------------------------------------------
#ifndef SWVIEWSDELEGATE_H
#define SWVIEWSDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! The delegate class that provides editors for editing software views.
//-----------------------------------------------------------------------------
class SWViewsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	//! The column numbers for columns
	enum Column {
		NAME_COLUMN = 0,
		HIER_REF_COLUMN,
		DISPLAY_NAME_COLUMN,
		DESCRIPTION_COLUMN,
		COLUMN_COUNT
	};

	/*! The constructor.
	 *
	 *      @param [in] parent The owner of the delegate.
	*/
	SWViewsDelegate(QObject *parent);
	
	//! The destructor.
	virtual ~SWViewsDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent      Owner for the editor.
	 *      @param [in] option      Contains options for the editor.
	 *      @param [in] index       Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 *      @param [in] editor The editor where the data is to be set.
	 *      @param [in] index Model index identifying the item that's data is to be set.
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 *      @param [in] editor The editor that contains the data to store.
	 *      @param [in] model Model that contains the data structure where data is to be saved to.
	 *      @param [in] index Model index identifying the item that's data is to be saved.
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

private slots:

	//! Commit the data from the sending editor and close the editor.
	void commitAndCloseEditor();

private:
	
	//! No copying
	SWViewsDelegate(const SWViewsDelegate& other);

	//! No assignment
	SWViewsDelegate& operator=(const SWViewsDelegate& other);
};

#endif // SWVIEWSDELEGATE_H
