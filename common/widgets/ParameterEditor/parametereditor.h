/* 
 *  	Created on: 22.2.2012
 *      Author: Antti Kamppi
 * 		filename: parametereditor.h
 *		Project: Kactus 2
 */

#ifndef PARAMETEREDITOR_H
#define PARAMETEREDITOR_H

#include <IPXACTmodels/parameter.h>

#include <editors/ComponentEditor/parameters/parametersmodel.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <QWidget>
#include <QList>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class Component;

/*! \brief An editor to edit parameters in a component.
 *
*/
class ParameterEditor : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
     * \param component     The component whose parameters to edit.
	 * \param parent        Pointer to the owner of this editor.
	 *
	*/
	ParameterEditor(QSharedPointer<Component> component, QWidget *parent);
	
	//! \brief The destructor
	virtual ~ParameterEditor();

	/*! \brief Check for the validity of the edited parameters
	*
	* \return True if all parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Restore the changes made in the editor back to ones in parameter models.
	*
	*/
	virtual void refresh();

signals:

	//! \brief Emitted when contents of the editor change.
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private slots:

	//! \brief Handler for adding parameters.
	void onAddItem(const QModelIndex& index);

	//! \brief Handler for parameter removes.
	void onRemoveItem(const QModelIndex& index);

private:
	//! \brief No copying
	ParameterEditor(const ParameterEditor& other);

	//! \brief No assignment
	ParameterEditor& operator=(const ParameterEditor& other);

	//! \brief The view that displays the parameters.
	EditableTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	ParametersModel model_;

	//! \brief The proxy that is used to do the sorting of parameters.
	QSortFilterProxyModel proxy_;
};

#endif // PARAMETEREDITOR_H
