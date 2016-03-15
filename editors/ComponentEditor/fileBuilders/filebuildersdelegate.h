//-----------------------------------------------------------------------------
// File: filebuildersdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 18.04.2011
//
// Description:
// Delegate to provide editors to edit file builders.
//-----------------------------------------------------------------------------

#ifndef FILEBUILDERSDELEGATE_H
#define FILEBUILDERSDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <QStyledItemDelegate>
#include <QObject>

//-----------------------------------------------------------------------------
//! Delegate to provide editors to edit file builders.
//-----------------------------------------------------------------------------
class FileBuildersDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] parameterNameCompleter  The completer to use for parameter names in expression editor.
     *      @param [in] parameterFinder         The parameter finder to use for for expression editor.
	 *      @param [in] parent                  Pointer to the owner of the delegate.
	 */
    FileBuildersDelegate(QCompleter* parameterNameCompleter, QSharedPointer<ParameterFinder> parameterFinder,
        QObject *parent);
	
	//! The destructor.
	virtual ~FileBuildersDelegate();

	/*!
     *  Create a new editor for the given item.
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index idetifying the item.
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

	/*!
	 *  Paints the delegate.
	 *
	 *      @param [in] painter     The used painter.
	 *      @param [in] option      The style options.
	 *      @param [in] index       Index used for painting.
	 */
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:

	/*!
	 *  Editor event.
	 *
	 *      @param [in] event   The editor event.
	 *      @param [in] model   The item model.
	 *      @param [in] option  Style option.
	 *      @param [in] index   Model index.
	 */
	virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
        const QModelIndex &index);

    /*!
     *  Check if a given column accepts expressions.
     *
     *      @param [in] column  The selected column.
     *
     *      @return True, if the column accepts expressions, false otherwise.
     */
    virtual bool columnAcceptsExpression(int column) const;

    /*!
     *  Gets the column number for the description column.
     *
     *      @return File builders do not have description.
     */
    virtual int descriptionColumn() const;

private slots:

	/*!
     *  Commit the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:
	
	//! No copying. No assignment.
	FileBuildersDelegate(const FileBuildersDelegate& other);
	FileBuildersDelegate& operator=(const FileBuildersDelegate& other);
};

#endif // FILEBUILDERSDELEGATE_H
