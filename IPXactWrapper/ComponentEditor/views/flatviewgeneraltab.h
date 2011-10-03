/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: flatviewgeneraltab.h
 */

#ifndef FLATVIEWGENERALTAB_H
#define FLATVIEWGENERALTAB_H

#include "filesetrefeditor.h"

#include <QWidget>
#include <QLineEdit>
#include <QSharedPointer>
#include <QCheckBox>

class View;
class Component;

/*! \brief Contains the GUI items to edit the general settings of non-hierarchical view.
 * 
 */
class FlatViewGeneralTab : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component that contains the view.
	 * \param view Pointer to the view being edited.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FlatViewGeneralTab(QSharedPointer<Component> component, 
		View* view, 
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~FlatViewGeneralTab();

	/*! \brief Check for the validity of the edited view
	*
	* \return True if all model parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Restore the settings from the model to this editor.
	 *
	*/
	void restoreChanges();

	/*! \brief Save the settings from editor to the model
	 * 
	 * After calling this function the settings can not be restored back to 
	 * previous ones.
	 *
	*/
	void applyChanges();

signals:

	//! \brief Emit an error message to user.
	void errorMessage(const QString& msg);

	//! \brief Emit a notice to user.
	void noticeMessage(const QString& msg);

	//! \brief Emitted when contents of the editor change
	void contentChanged();

private:
	
	//! \brief No copying
	FlatViewGeneralTab(const FlatViewGeneralTab& other);

	//! No assignment
	FlatViewGeneralTab& operator=(const FlatViewGeneralTab& other);

	//! \brief Pointer to the view being edited.
	View* view_;

	//! \brief Editor to set the hardware description language.
	QLineEdit language_;

	//! \brief Set the strict definition to the language.
	QCheckBox languageStrict_;

	//! \brief Editor to set the model name element value.
	QLineEdit modelName_;

	//! \brief Editor to set the file set references for the flat view.
	FileSetRefEditor fileSetRefs_;
};

#endif // FLATVIEWGENERALTAB_H
