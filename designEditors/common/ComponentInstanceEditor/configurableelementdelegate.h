/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementdelegate.h
 *		Project: Kactus 2
 */

#ifndef CONFIGURABLEELEMENTDELEGATE_H
#define CONFIGURABLEELEMENTDELEGATE_H

#include <IPXACTmodels/component.h>

#include <QStyledItemDelegate>
#include <QSharedPointer>
#include <QWidget>

/*! \brief Delegate that provides editors to edit the configurable element values.
 *
 */
class ConfigurableElementDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component that defines the component instance type.
	 * \param parent Pointer to the owner of this delegate.
	 *
	*/
	ConfigurableElementDelegate(QSharedPointer<Component> component, QObject *parent);
	
	//! \brief The destructor
	virtual ~ConfigurableElementDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index identifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, 
		const QStyleOptionViewItem& option, const QModelIndex& index ) const;

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

	/*! \brief Set the component that identifies the component instance type.
	 *
	 * \param component Pointer to the component that defines the component instance type.
	 *
	*/
	void setComponent(QSharedPointer<Component> component);

private slots:

	/*! \brief Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	//! \brief No copying
	ConfigurableElementDelegate(const ConfigurableElementDelegate& other);

	//! \brief No assignment
	ConfigurableElementDelegate& operator=(const ConfigurableElementDelegate& other);

	//! \brief Pointer to the component that's instance is being edited.
	QSharedPointer<Component> component_;
};

#endif // CONFIGURABLEELEMENTDELEGATE_H
