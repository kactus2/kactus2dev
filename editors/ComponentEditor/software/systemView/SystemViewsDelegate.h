//-----------------------------------------------------------------------------
// File: SystemViewsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The delegate class that provides editors for editing system views.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVIEWSDELEGATE_H
#define SYSTEMVIEWSDELEGATE_H

#include <QStyledItemDelegate>

/*! \brief The delegate class that provides editors for editing software views.
 *
 */
class SystemViewsDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief The column numbers for columns
	enum Column {
		NAME_COLUMN = 0,
		HIER_REF_COLUMN,
		DISPLAY_NAME_COLUMN,
		DESCRIPTION_COLUMN,
		COLUMN_COUNT
	};

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of the delegate.
	 *
	*/
	SystemViewsDelegate(QObject *parent);
	
	//! \brief The destructor.
	virtual ~SystemViewsDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index identifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index) const;

	/*! \brief Set the data for the editor.
	 *
	 * \param editor Pointer to the editor where the data is to be set.
	 * \param index Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*! \brief Save the data from the editor to the model.
	 *
	 * \param editor Pointer to the editor that contains the data to store.
	 * \param model Model that contains the data structure where data is to be saved to.
	 * \param index Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		const QModelIndex& index) const;

private slots:

	/*! \brief Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	
	//! \brief No copying
	SystemViewsDelegate(const SystemViewsDelegate& other);

	//! \brief No assignment
	SystemViewsDelegate& operator=(const SystemViewsDelegate& other);
};

#endif // SYSTEMVIEWSDELEGATE_H
