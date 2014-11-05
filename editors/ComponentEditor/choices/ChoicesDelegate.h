//-----------------------------------------------------------------------------
// File: ChoicesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef CHOICESDELEGATE_H
#define CHOICESDELEGATE_H

#include <QStyledItemDelegate>

#include <QSharedPointer>
#include <QList>

class Choice;

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

signals:

    void contentChanged();

private:

	// Disable copying.
	ChoicesDelegate(ChoicesDelegate const& rhs);
	ChoicesDelegate& operator=(ChoicesDelegate const& rhs);

    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;
};

#endif // CHOICESDELEGATE_H
