/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapdelegate.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPDELEGATE_H
#define MEMORYMAPDELEGATE_H

#include <QStyledItemDelegate>

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionDelegate.h>

/*! \brief The delegate that provides editors to add/remove/edit details of a single memory map.
 *
 */
class MemoryMapDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:
	
	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterNameCompleter  The completer to use for parameter names in expression editor.
	 *      @param [in] parameterFinder         The parameter finder for expression editor.
	 *      @param [in] parent                  Pointer to the owner of the delegate.
	 */
	MemoryMapDelegate(QCompleter* parameterNameCompleter, QSharedPointer<ParameterFinder> parameterFinder,
        QObject *parent);
	
	//! \brief The destructor.
	virtual ~MemoryMapDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index identifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, 
        QModelIndex const& index) const;


	/*! \brief Set the data for the editor.
	 *
	 * \param editor Pointer to the editor where the data is to be set.
	 * \param index Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! \brief Save the data from the editor to the model.
	 *
	 * \param editor Pointer to the editor that contains the data to store.
	 * \param model Model that contains the data structure where data is to be saved to.
	 * \param index Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		QModelIndex const& index) const;

protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column  The column to check.
     *
     *      @return True if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

private slots:

	/*! \brief Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	
	//! \brief No copying
	MemoryMapDelegate(const MemoryMapDelegate& other);

	//! \brief No assignment
	MemoryMapDelegate& operator=(const MemoryMapDelegate& other);

    /*!
     *  Creates an editor for name.
     *
     *      @param [in] parent   The parent widget for the editor.
     *      @param [in] option   The style options for the editor.
     *      @param [in] index    The index for which the editor is created.
     *
     *      @return Editor for editing name.
     */
    QWidget* createNameEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index) const;
};

#endif // MEMORYMAPDELEGATE_H
