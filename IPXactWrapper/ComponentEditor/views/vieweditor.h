/* 
 *
 *  Created on: 14.4.2011
 *      Author: Antti Kamppi
 * 		filename: vieweditor.h
 */

#ifndef VIEWEDITOR_H
#define VIEWEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include <common/widgets/nameGroupBox/namegroupbox.h>
#include "envidentifiereditor.h"
#include "flatviewgeneraltab.h"
#include "flatviewparameterstab.h"
#include <IPXactWrapper/ComponentEditor/fileBuilders/filebuilderseditor.h>
#include "hierarchyrefwidget.h"

#include <QComboBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QSharedPointer>

class Component;
class View;
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
	 * \param dataPointer Pointer to the view being edited.
	 * \param libHandler Pointer to the instance that manages the library
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	ViewEditor(QSharedPointer<Component> component, 
		void* dataPointer,
		LibraryInterface* libHandler,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~ViewEditor();

	/*! \brief Check for the validity of the edited view.
	*
	* \return True if all model parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Remove the edited view from the model.
	 *
	 * This function should be used when user wants to remove a view from 
	 * the component.
	 *
	*/
	virtual void removeModel();

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	*/
	virtual void makeChanges();

private:

	//! \brief No copying
	ViewEditor(const ViewEditor& other);

	//! No assignment
	ViewEditor& operator=(const ViewEditor& other);

	//! \brief Set up the layout for the editor.
	void setupLayout();

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the view being edited.
	View* view_;
	
	//! \brief Editor to set the name, display name and description of the view.
	NameGroupBox nameGroup_;

	//! \brief Combo box to select between hierarchical/flat views.
	QComboBox viewTypeSelector_;

	//! \brief The editor to edit the envIdentifier element.
	EnvIdentifierEditor envIdentifier_;

	//! \brief Widget that contains the flat and hierarchical editors
	QStackedWidget stack_;

	//! \brief Tab widget contains the editor to edit a flat view.
	QTabWidget flatTabs_;

	//! \brief Editor to set general settings of flat view.
	FlatViewGeneralTab generalTab_;

	//! \brief Editor to set the parameters of flat view.
	FlatViewParametersTab parametersTab_;

	//! \brief Editor to set the default file builders of flat view.
	FileBuildersEditor fileBuildersTab_;

	//! \brief The widget to edit the hierarchical reference
	HierarchyRefWidget hierarchyRef_;
};

#endif // VIEWEDITOR_H
