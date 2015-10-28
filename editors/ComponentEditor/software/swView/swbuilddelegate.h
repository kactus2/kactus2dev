//-----------------------------------------------------------------------------
// File: swbuilddelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 02.04.2013
//
// Description:
// The delegate class to edit SW build commands.
//-----------------------------------------------------------------------------

#ifndef SWBUILDDELEGATE_H
#define SWBUILDDELEGATE_H

#include <QStyledItemDelegate>

/*! The delegate class to edit SW build commands.
 *
 */
class SWBuildDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] parent The owner of the delegate.
	 *
	*/
	SWBuildDelegate(QObject *parent);
	
	//! The destructor.
	virtual ~SWBuildDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index idetifying the item.
	 *
	 *      @return The editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 *      @param [in] editor The editor where the data is to be set.
	 *      @param [in] index Model index identifying the item that's data is to be set.
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 *      @param [in] editor The editor that contains the data to store.
	 *      @param [in] model Model that contains the data structure where data is to be saved to.
     *      @param [in] index Model index identifying the item that's data is to be saved.
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:

	virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option,
        QModelIndex const& index);

private slots:

	/*! Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	
	//! No copying
	SWBuildDelegate(const SWBuildDelegate& other);

	//! No assignment
	SWBuildDelegate& operator=(const SWBuildDelegate& other);

};

#endif // SWBUILDDELEGATE_H
