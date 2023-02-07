//-----------------------------------------------------------------------------
// File: channelsdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// The delegate that provides editors to edit the channels of a component.
//-----------------------------------------------------------------------------

#ifndef CHANNELSDELEGATE_H
#define CHANNELSDELEGATE_H

#include <editors/ComponentEditor/common/EnumerationEditorConstructorDelegate.h>

#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit the channels of a component.
//-----------------------------------------------------------------------------
class ChannelsDelegate : public EnumerationEditorConstructorDelegate
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

private:
	
	//! No copying
	ChannelsDelegate(const ChannelsDelegate& other);
	ChannelsDelegate& operator=(const ChannelsDelegate& other);

    /*!
     *  Check if the column is used for enumerations.
     *
     *      @param [in] index   The selected index.
     *
     *      @return True, if the column is used for editing enumerations, false otherwise.
     */
    virtual bool isEnumerationEditorColumn(QModelIndex const& index) const override final;

    /*!
     *  The list of currently selected enumerations in the selected item.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return List of currently selected enumerations.
     */
    virtual QStringList getCurrentSelection(QModelIndex const& index) const override final;

    /*!
     *  Get the list of the available enumerations.
     *
     *      @return List of the available enumerations.
     */
    virtual QStringList getAvailableItems() const override final;

    /*!
     *  Set the selected enumerations to the selected item.
     *
     *      @param [in] index           Model index identifying the item that's data is to be saved.
     *      @param [in] model           Model that contains the data structure where data is to be saved to.
     *      @param [in] selectedItems   List of the selected enumerations.
     */
    virtual void setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model, QStringList const& selectedItems) const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The component being edited.
	QSharedPointer<Component> component_;
};

#endif // CHANNELSDELEGATE_H
