/* 
 *	Created on:	2.4.2013
 *	Author:		Antti Kamppi
 *	File name:	swbuilddelegate.h
 *	Project:		Kactus 2
*/

#ifndef SWBUILDDELEGATE_H
#define SWBUILDDELEGATE_H

#include <QStyledItemDelegate>

/*! \brief The delegate class to edit SW build commands.
 *
 */
class SWBuildDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief Defines the columns for the model.
	enum Columns {
		FILETYPE_COLUMN = 0,
		COMMAND_COLUMN,
		FLAGS_COLUMN,
		REPLACE_DEF_COLUMN,
		COLUMN_COUNT
	};

	/*! \brief The constructor.
	 *
	 * Method: 		SWBuildDelegate
	 * Full name:	SWBuildDelegate::SWBuildDelegate
	 * Access:		private 
	 *
	 * \param parent Pointer to the owner of the delegate.
	 *
	*/
	SWBuildDelegate(QObject *parent);
	
	//! \brief The destructor.
	virtual ~SWBuildDelegate();

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
	SWBuildDelegate(const SWBuildDelegate& other);

	//! \brief No assignment
	SWBuildDelegate& operator=(const SWBuildDelegate& other);

};

#endif // SWBUILDDELEGATE_H
