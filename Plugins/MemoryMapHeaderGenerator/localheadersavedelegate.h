//-----------------------------------------------------------------------------
// File: localheadersavedelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.04.2013
//
// Description:
// The delegate class to select the local headers to be generated.
//-----------------------------------------------------------------------------

#ifndef LOCALHEADERSAVEDELEGATE_H
#define LOCALHEADERSAVEDELEGATE_H

#include <IPXACTmodels/Component/Component.h>

#include <QStyledItemDelegate>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! The delegate class to select the local headers to be generated.
//-----------------------------------------------------------------------------
class LocalHeaderSaveDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component   Pointer to the component to generate the headers for.
	 *      @param [in] parent      Pointer to the owner of the delegate.
	 */
	LocalHeaderSaveDelegate(QSharedPointer<Component> component, QObject *parent);
	
	//! The destructor.
	virtual ~LocalHeaderSaveDelegate();

	/*!
     *  Create a new editor for the given item.
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

private:
	
	//! No copying.
	LocalHeaderSaveDelegate(const LocalHeaderSaveDelegate& other);

	//! No assignment.
	LocalHeaderSaveDelegate& operator=(const LocalHeaderSaveDelegate& other);

	//! Pointer to the component the headers are generated for.
	QSharedPointer<Component> component_;
};

#endif // LOCALHEADERSAVEDELEGATE_H
