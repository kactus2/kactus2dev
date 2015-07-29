//-----------------------------------------------------------------------------
// File: lineeditdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 9.12.2011
//
// Description:
// The base class to provide QLineEditor for a widget.
//-----------------------------------------------------------------------------

#ifndef LINEEDITDELEGATE_H
#define LINEEDITDELEGATE_H

#include <QStyledItemDelegate>

/*! The base class to provide QLineEditor for a widget.
 * 
 * LineEditDelegate can be sub classed to provide more specific editors.
 * In subclasses the createEditor() can be reimplemented to create more specific
 * line editors.
 */ 
class LineEditDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	//! \brief The minimum height for the editors provided.
	static const int MINIMUM_EDITOR_HEIGHT = 20;

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this delegate.
	 *
	*/
	LineEditDelegate(QObject *parent);
	
	//! \brief The destructor
	virtual ~LineEditDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index identifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*! \brief Set the data for the editor.
	 *
	 * \param editor Pointer to the editor where the data is to be set.
	 * \param index Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! \brief Save the data from the editor to the model.
	 *
	 * \param editor Pointer to the editor that contains the data to store.
	 * \param model Model that contains the data structure where data is to be saved to.
	 * \param index Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

private slots:

	/*! \brief Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	//! \brief No copying
	LineEditDelegate(const LineEditDelegate& other);

	//! \brief No assignment
	LineEditDelegate& operator=(const LineEditDelegate& other);
};

#endif // LINEEDITDELEGATE_H
