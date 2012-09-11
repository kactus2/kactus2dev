/* 
 *
 *  Created on: 14.4.2011
 *      Author: Antti Kamppi
 * 		filename: vieweditor.h
 */

#ifndef VIEWEDITOR_H
#define VIEWEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>
#include "envidentifiereditor.h"
#include "flatviewgeneraltab.h"
#include <IPXactWrapper/ComponentEditor/fileBuilders/filebuilderseditor.h>
#include "hierarchyrefwidget.h"
#include <models/view.h>

#include <QComboBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QSharedPointer>

class Component;
class LibraryInterface;

/*! \brief Editor to edit a view within a component.
 *
 */
class ViewEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component that contains the view.
	 * \param view Pointer to the view being edited.
	 * \param libHandler Pointer to the instance that manages the library
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	ViewEditor(QSharedPointer<Component> component, 
		QSharedPointer<View> view,
		LibraryInterface* libHandler,
		QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~ViewEditor();

	/*! \brief Check for the validity of the edited view.
	*
	* \return True if all model parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! \brief Handler for changes on the hierarchical/flat selector.
	void onStackChange(int index);

private:

	//! \brief No copying
	ViewEditor(const ViewEditor& other);

	//! No assignment
	ViewEditor& operator=(const ViewEditor& other);

	//! \brief Set up the layout for the editor.
	void setupLayout();

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! \brief Pointer to the view being edited.
	QSharedPointer<View> view_;
	
	//! \brief Editor to set the name, display name and description of the view.
	NameGroupEditor nameEditor_;

	//! \brief Combo box to select between hierarchical/flat views.
	QComboBox viewTypeSelector_;

	//! \brief The editor to edit the envIdentifier element.
	EnvIdentifierEditor envIdentifier_;

	//! \brief Widget that contains the flat and hierarchical editors
	QStackedWidget stack_;

	//! \brief Tab widget contains the editor to edit a flat view.
	QWidget flatElements_;

	//! \brief Editor to set general settings of flat view.
	FlatViewGeneralTab generalTab_;

	//! \brief Editor to set the parameters of flat view.
	ParameterGroupBox parameters_;

	//! \brief Editor to set the default file builders of flat view.
	FileBuildersEditor fileBuilders_;

	//! \brief The widget to edit the hierarchical reference
	HierarchyRefWidget hierarchyRef_;
};

#endif // VIEWEDITOR_H
