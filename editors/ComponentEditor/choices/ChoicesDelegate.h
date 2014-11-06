//-----------------------------------------------------------------------------
// File: ChoicesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// Delegate class for a view and a ChoicesModel.
//-----------------------------------------------------------------------------

#ifndef CHOICESDELEGATE_H
#define CHOICESDELEGATE_H

#include <QStyledItemDelegate>

#include <QSharedPointer>
#include <QList>

class Choice;

//-----------------------------------------------------------------------------
//! Delegate class for a view and a ChoicesModel.
//-----------------------------------------------------------------------------
class ChoicesDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:

	//! The constructor.
	ChoicesDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, QObject* parent);

	//! The destructor.
	virtual ~ChoicesDelegate();

    /*!
     *  Creates an editor widget for editing cell contents.
     *
     *      @param [in] parent  The parent widget of the editor object.
     *      @param [in] option  Options used to create the editor.
     *      @param [in] index   The model index for which the editor is created.
     *
     *      @return The editor widget.
     */
    QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index) const;
    
    /*!
     *  Sets the data that is displayed and edited by the editor.
     *
     *      @param [in] editor  The editor widget being modified.
     *      @param [in] index   The model index being edited.
     */
    void setEditorData(QWidget* editor, QModelIndex const& index) const;
    
    /*!
     *  Stores the data from the editor in the model.
     *
     *      @param [in] editor  The editor that has the data.
     *      @param [in] model   The model being updated.
     *      @param [in] index   The model index being updated.
     */
    void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;
    
    /*!
     *  Updates the editor geometry.
     *
     *      @param [in] editor  The editor being updated.
     *      @param [in] option  The options used to update the editor.
     *      @param [in] index   The model index being edited.
     */
    void updateEditorGeometry(QWidget* editor, QStyleOptionViewItem const& option, QModelIndex const& index) const;

signals:

    //! Emitted when the enumerations of the choice change.
    void contentChanged();

private:

	// Disable copying.
	ChoicesDelegate(ChoicesDelegate const& rhs);
	ChoicesDelegate& operator=(ChoicesDelegate const& rhs);

    /*!
     *  Repositions the editor if there is not enough space under the default position. The editor
     *  is then resized to use the available space.
     *
     *      @param [in] editor   The editor to reposition.
     *      @param [in] option   The style options for the editor.
     */
    void repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The choices edited using the ChoiceModel.
    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;
};

#endif // CHOICESDELEGATE_H
