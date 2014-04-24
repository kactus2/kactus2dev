/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgesdelegate.h
 *		Project: Kactus 2
 */

#ifndef BRIDGESDELEGATE_H
#define BRIDGESDELEGATE_H

#include <IPXACTmodels/component.h>

#include <QStyledItemDelegate>
#include <QSharedPointer>

class BridgesDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief The column numbers for columns.
	enum Column {
		MASTER_COLUMN = 0,
		OPAQUE_COLUMN,
		COLUMN_COUNT
	};

	/*! \brief The constructor.
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this delegate.
	 *
	*/
	BridgesDelegate(QSharedPointer<Component> component,
		QObject *parent);
	
	//! \brief The destructor.
	virtual ~BridgesDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index identifying the item.
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

private:
	
	//! \brief No copying
	BridgesDelegate(const BridgesDelegate& other);

	//! \brief No assignment
	BridgesDelegate& operator=(const BridgesDelegate& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! Boolean for ad-hoc group modify.
	bool opaqueGroupModify_;

	//! The new state for the group modify.
	Qt::CheckState opaque;
};

#endif // BRIDGESDELEGATE_H
