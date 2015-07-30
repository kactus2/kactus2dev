//-----------------------------------------------------------------------------
// File: PortTagEditorDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.07.2015
//
// Description:
// Delegate for selecting and creating tags for ports.
//-----------------------------------------------------------------------------

#ifndef PORTTAGEDITORDELEGATE_H
#define PORTTAGEDITORDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! The delegate for selecting and creating port tags.
//-----------------------------------------------------------------------------
class PortTagEditorDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] existingPortTags    The port tags that have been given in other ports.
	 *      @param [in] parent              Pointer to the owner of this delegate.
	 */
	PortTagEditorDelegate(QStringList existingPortTags, QObject* parent);

    /*!
     *  The destructor.
     */
    virtual ~PortTagEditorDelegate();

    /*!
     *  Create a new editor for the given item.
     *
     *      @param [in] parent  Owner of the editor.
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
     *      @param [in] model   Model that contains the data structure where data is to be saved into.
     *      @param [in] index   Model index identifying the item that's data is to be saved.
     */
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

private:
	//! No copying
    PortTagEditorDelegate(const PortTagEditorDelegate& other);

	//! No assignment
    PortTagEditorDelegate& operator=(const PortTagEditorDelegate& other);

    //! A list of existing port tags.
    QStringList existingTags_;
};

#endif // PORTTAGEDITORDELEGATE_H
