//-----------------------------------------------------------------------------
// File: BridgesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.6.2012
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef BRIDGESDELEGATE_H
#define BRIDGESDELEGATE_H

#include <QStyledItemDelegate>
#include <QSharedPointer>

#include <IPXACTmodels/common/Document.h>

class Component;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
// Data.
//-----------------------------------------------------------------------------
class BridgesDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] busInterface    Interface for accessing bus interfaces.
     *      @param [in] revision        Currently active IP-XACT revision.
	 *      @param [in] parent          The owner of this delegate.
     */
    BridgesDelegate(BusInterfaceInterface* busInterface, Document::Revision revision, QObject* parent);

	/*!
     *  The destructor.
     */
	virtual ~BridgesDelegate() = default;

    //! No copying. No assignment.
    BridgesDelegate(const BridgesDelegate& other) = delete;
    BridgesDelegate& operator=(const BridgesDelegate& other) = delete;


	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent Owner for the editor.
	 *      @param [in] option Contains options for the editor.
	 *      @param [in] index Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
     */
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, QModelIndex const& index)
        const final;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor The editor where the data is to be set.
	 *      @param [in] index Model index identifying the item that's data is to be set.
     */
	void setEditorData(QWidget* editor, QModelIndex const& index) const final;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor The editor that contains the data to store.
	 *      @param [in] model Model that contains the data structure where data is to be saved to.
	 *      @param [in] index Model index identifying the item that's data is to be saved.
     */
	void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const final;

private:
	
    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;

    //! The currently active IP-XACT revision.
    Document::Revision revision_;
};

#endif // BRIDGESDELEGATE_H
