//-----------------------------------------------------------------------------
// File: PowerDomainLinkDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.10.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef POWERDOMAINLINKDELEGATE_H
#define POWERDOMAINLINKDELEGATE_H

#include <editors/ComponentEditor/common/EnumerationEditorConstructorDelegate.h>

#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit the channels of a component.
//-----------------------------------------------------------------------------
class PowerDomainLinkDelegate : public EnumerationEditorConstructorDelegate
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      The owner of this delegate.
	*/
	explicit PowerDomainLinkDelegate(QObject *parent = nullptr);
	
	//! The destructor
	~PowerDomainLinkDelegate() final = default;

    //! No copying
    PowerDomainLinkDelegate(const PowerDomainLinkDelegate& other) = delete;
    PowerDomainLinkDelegate& operator=(const PowerDomainLinkDelegate& other) = delete;

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
	*/
	QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option,
        QModelIndex const& index) const final;

	/*! Set the data for the editor.
	 *
	 *      @param [in] editor  The editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 *
	*/
	void setEditorData(QWidget* editor, QModelIndex const& index) const final;

	/*! Save the data from the editor to the model.
	 *
	 *      @param [in] editor  The editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 *
	*/
	void setModelData(QWidget* editor, QAbstractItemModel* model, 
		QModelIndex const& index) const final;

	void setComponents(QSharedPointer<Component> externalComponent, QSharedPointer<Component> internalComponent) noexcept;

private:
	

    /*!
     *  Check if the column is used for enumerations.
     *
     *      @param [in] index   The selected index.
     *
     *      @return True, if the column is used for editing enumerations, false otherwise.
     */
    bool isEnumerationEditorColumn(QModelIndex const& index) const final;

    /*!
     *  The list of currently selected enumerations in the selected item.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return List of currently selected enumerations.
     */
    QStringList getCurrentSelection(QModelIndex const& index) const final;

    /*!
     *  Get the list of the available enumerations.
     *
     *      @return List of the available enumerations.
     */
    QStringList getAvailableItems() const final;

    /*!
     *  Set the selected enumerations to the selected item.
     *
     *      @param [in] index           Model index identifying the item that's data is to be saved.
     *      @param [in] model           Model that contains the data structure where data is to be saved to.
     *      @param [in] selectedItems   List of the selected enumerations.
     */
    void setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model, QStringList const& selectedItems) const final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    QSharedPointer<Component> externalComponent_ = nullptr;

	QSharedPointer<Component> internalComponent_ = nullptr;
};

#endif // POWERDOMAINLINKDELEGATE_H
