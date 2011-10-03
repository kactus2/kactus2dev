/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: envidentifiereditor.h
 */

#ifndef ENVIDENTIFIEREDITOR_H
#define ENVIDENTIFIEREDITOR_H

#include "envidentifiersmodel.h"

#include <QGroupBox>
#include <QString>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QPushButton>

class View;

/*! \brief Widget to edit the spirit:envIdentifiers element within view.
 *
 */
class EnvIdentifierEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param view Pointer to the view being edited
	 * \param parent Pointer to the owner of this widget.
	 * \param title The title to set for the group box
	 *
	*/
	EnvIdentifierEditor(View* view, QWidget *parent, 
		const QString title = QString("Environment identifiers"));
	
	//! \brief The destructor
	virtual ~EnvIdentifierEditor();

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void restoreChanges();

	/*! \brief Applies the changes made with the editor to the model.
	*
	* After calling this function it is no longer possible to automatically 
	* restore the previous state of the model.
	* 
	* Note: if the editor is not in valid state nothing is changed.
	*/
	virtual void applyChanges();

	/*! \brief Check for the validity of the edited parameters.
	*
	* \return True if all parameters are in valid state.
	*/
	virtual bool isValid() const;

signals:

	//! \brief Signal to inform user that one element has been modified.
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private slots:

	//! \brief Remove a row from the model
	void onRemove();

private:

	//! \brief No copying
	EnvIdentifierEditor(const EnvIdentifierEditor& other);

	//! No assignment
	EnvIdentifierEditor& operator=(const EnvIdentifierEditor& other);

	//! \brief The button to add a new port
	QPushButton addRowButton_;

	//! \brief The button to remove the selected port
	QPushButton removeRowButton_;

	//! \brief The view that displays the parameters.
	QTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	EnvIdentifiersModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel proxy_;
};

#endif // ENVIDENTIFIEREDITOR_H
