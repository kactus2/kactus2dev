/* 
 *  	Created on: 14.6.2012
 *      Author: Antti Kamppi
 * 		filename: channelsdelegate.h
 *		Project: Kactus 2
 */

#ifndef CHANNELSDELEGATE_H
#define CHANNELSDELEGATE_H

#include <IPXACTmodels/component.h>

#include <QStyledItemDelegate>
#include <QSharedPointer>

/*! \brief The delegate that provides editors to edit the channels of a component.
 *
 */
class ChannelsDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief The minimum height for editor editing list of interfaces
	static const int LIST_EDITOR_MIN_HEIGHT = 100;

	//! \brief The column numbers
	enum Column {
		NAME_COLUMN = 0,
		DISPLAY_NAME_COLUMN,
		INTERFACE_COLUMN,
		DESCRIPTION_COLUMN,
		COLUMN_COUNT
	};

	//! \brief The roles to display/edit interface column with QStringList.
	enum Roles {
		USER_DISPLAY_ROLE = Qt::UserRole,
		USER_EDIT_ROLE
	};

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this delegate.
	 *
	*/
	ChannelsDelegate(QSharedPointer<Component> component, 
		QObject *parent);
	
	//! \brief The destructor
	virtual ~ChannelsDelegate();

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

private slots:

	/*! \brief Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	
	//! \brief No copying
	ChannelsDelegate(const ChannelsDelegate& other);

	//! \brief No assignment
	ChannelsDelegate& operator=(const ChannelsDelegate& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // CHANNELSDELEGATE_H
