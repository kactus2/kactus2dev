/* 
 *
 *  Created on: 18.4.2011
 *      Author: Antti Kamppi
 * 		filename: filebuilderseditor.h
 */

#ifndef FILEBUILDERSEDITOR_H
#define FILEBUILDERSEDITOR_H

#include "filebuildersmodel.h"

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QPushButton>

/*! \brief Editor to edit file builders.
 *
 */
class FileBuildersEditor : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param dataPointer Pointer to the QList containing the file builders.
	 * \param parent Pointer to the owner of the editor.
	 *
	*/
	FileBuildersEditor(void* dataPointer, QWidget *parent);

	//! \brief the destructor
	virtual ~FileBuildersEditor();

	/*! \brief Check for the validity of the edited file builders.
	*
	* \return True if all file builders are in valid state.
	*/
	virtual bool isValid() const;

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

signals:

	//! \brief Emit an error message to user.
	void errorMessage(const QString& msg);

	//! \brief Emit a notice to user.
	void noticeMessage(const QString& msg);

	//! \brief Inform that contents of this editor have changed.
	void contentChanged();

private slots:

	//! \brief Remove the selected row from the model
	void onRemove();

private:

	//! \brief No copying
	FileBuildersEditor(const FileBuildersEditor& other);

	//! No assignment
	FileBuildersEditor& operator=(const FileBuildersEditor& other);
	
	//! \brief The button to add a new port
	QPushButton addRowButton_;

	//! \brief The button to remove the selected port
	QPushButton removeRowButton_;

	//! \brief The view that displays the parameters.
	QTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	FileBuildersModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel proxy_;
};

#endif // FILEBUILDERSEDITOR_H
