/* 
 *	Created on:	9.4.2013
 *	Author:		Antti Kamppi
 *	File name:	localheadersavedelegate.h
 *	Project:		Kactus 2
*/

#ifndef LOCALHEADERSAVEDELEGATE_H
#define LOCALHEADERSAVEDELEGATE_H

#include <IPXACTmodels/component.h>

#include <QStyledItemDelegate>
#include <QSharedPointer>

/*! \brief The delegate class to select the local headers to be generated.
 *
 */
class LocalHeaderSaveDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		LocalHeaderSaveDelegate
	 * Full name:	LocalHeaderSaveDelegate::LocalHeaderSaveDelegate
	 * Access:		private 
	 *
	 * \param component Pointer to the component to generate the headers for.
	 * \param parent Pointer to the owner of the delegate.
	 *
	*/
	LocalHeaderSaveDelegate(QSharedPointer<Component> component, QObject *parent);
	
	//! \brief The destructor.
	virtual ~LocalHeaderSaveDelegate();

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

private:
	
	//! \brief No copying
	LocalHeaderSaveDelegate(const LocalHeaderSaveDelegate& other);

	//! \brief No assignment
	LocalHeaderSaveDelegate& operator=(const LocalHeaderSaveDelegate& other);

	//! \brief Pointer to the component the headers are generated for.
	QSharedPointer<Component> component_;
};

#endif // LOCALHEADERSAVEDELEGATE_H
