//-----------------------------------------------------------------------------
// File: cpusdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// The delegate to provide editors to add/remove/edit the cpus of a component.
//-----------------------------------------------------------------------------

#ifndef CPUSDELEGATE_H
#define CPUSDELEGATE_H

#include <QStyledItemDelegate>
#include <QSharedPointer>

class Component;
//-----------------------------------------------------------------------------
//! The delegate to provide editors to add/remove/edit the cpus of a component.
//-----------------------------------------------------------------------------
class CpusDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	//! The minimum height for list editor for address space references.
	static const int LIST_EDITOR_MIN_HEIGHT = 100;

	/*! The constructor
	 *
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      The owner of the delegate.
	*/
	CpusDelegate(QSharedPointer<Component> component, QObject *parent);
	
	//! The destructor.
	virtual ~CpusDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
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
	 *      @param [in] editor  The editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

private slots:

	//! Commit the data from the sending editor and close the editor.	 
	void commitAndCloseEditor();

private:
	
	//! No copying
	CpusDelegate(const CpusDelegate& other);
	CpusDelegate& operator=(const CpusDelegate& other);

	//! The component being edited.
	QSharedPointer<Component> component_;
};

#endif // CPUSDELEGATE_H
