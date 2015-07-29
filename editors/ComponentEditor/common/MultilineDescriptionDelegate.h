//-----------------------------------------------------------------------------
// File: MultilineDescriptionDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.07.2015
//
// Description:
// A delegate that provides a multiline editor for descriptions.
//-----------------------------------------------------------------------------

#ifndef MULTILINEDESCRIPTIONDELEGATE_H
#define MULTILINEDESCRIPTIONDELEGATE_H

#include <QStyledItemDelegate>
#include <QEvent>

//-----------------------------------------------------------------------------
//! A delegate that provides a multiline editor for descriptions.
//-----------------------------------------------------------------------------
class MultilineDescriptionDelegate : public QStyledItemDelegate 
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent      The parent object.
     */
	MultilineDescriptionDelegate(QObject* parent);
	
	//! The destructor
	virtual ~MultilineDescriptionDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, 
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
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

protected:

    //! Filters events for editors.
    virtual bool eventFilter(QObject* editor, QEvent *event);

    virtual int descriptionColumn() const = 0;

private:
	//! No copying
	MultilineDescriptionDelegate(const MultilineDescriptionDelegate& other);
	MultilineDescriptionDelegate& operator=(const MultilineDescriptionDelegate& other);

};

#endif // MULTILINEDESCRIPTIONDELEGATE_H
