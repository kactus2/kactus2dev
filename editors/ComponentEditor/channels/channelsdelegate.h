//-----------------------------------------------------------------------------
// File: channelsdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// The delegate that provides editors to edit the channels of a component.
//-----------------------------------------------------------------------------

#ifndef CHANNELSDELEGATE_H
#define CHANNELSDELEGATE_H

#include <QStyledItemDelegate>
#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit the channels of a component.
//-----------------------------------------------------------------------------
class ChannelsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	//! The minimum height for editor editing list of interfaces
	static const int LIST_EDITOR_MIN_HEIGHT = 100;

	/*! The constructor
	 *
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      The owner of this delegate.
	*/
	ChannelsDelegate(QSharedPointer<Component> component, QObject *parent);
	
	//! The destructor
	virtual ~ChannelsDelegate();

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
	 *      @param [in] editor  The editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 *      @param [in] editor  The editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		QModelIndex const& index) const;

private slots:

	//! Commit the data from the sending editor and close the editor.
	void commitAndCloseEditor();

private:
	
	//! No copying
	ChannelsDelegate(const ChannelsDelegate& other);
	ChannelsDelegate& operator=(const ChannelsDelegate& other);

	//! The component being edited.
	QSharedPointer<Component> component_;
};

#endif // CHANNELSDELEGATE_H
