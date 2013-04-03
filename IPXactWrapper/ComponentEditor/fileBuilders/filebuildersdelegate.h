/* 
 *
 *  Created on: 18.4.2011
 *      Author: Antti Kamppi
 * 		filename: filebuildersdelegate.h
 */

#ifndef FILEBUILDERSDELEGATE_H
#define FILEBUILDERSDELEGATE_H

#include <QStyledItemDelegate>

/*! \brief Delegate to provide editors to edit file builders.
 *
 */
class FileBuildersDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief Specifies the columns and their numbers.
	enum Columns {
		FILETYPE_COLUMN = 0,
		COMMAND_COLUMN,
		FLAGS_COLUMN,
		REPLACE_DEFAULT_COLUMN,
		COLUMN_COUNT
	};

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the delegate.
	 *
	*/
	FileBuildersDelegate(QObject *parent);
	
	//! \brief The destructor
	virtual ~FileBuildersDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index idetifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index) const;

	/*! \brief Set the data for the editor.
	 *
	 * \param editor Pointer to the editor where the data is to be set.
	 * \param index Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*! \brief Save the data from the editor to the model.
	 *
	 * \param editor Pointer to the editor that contains the data to store.
	 * \param model Model that contains the data structure where data is to be saved to.
	 * \param index Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		const QModelIndex& index) const;

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:

	virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private slots:

	/*! \brief Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	
	//! \brief No copying
	FileBuildersDelegate(const FileBuildersDelegate& other);

	//! No assignment
	FileBuildersDelegate& operator=(const FileBuildersDelegate& other);
};

#endif // FILEBUILDERSDELEGATE_H
