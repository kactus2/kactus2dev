/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditor.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITOR_H
#define COMPONENTEDITOR_H

#include <common/widgets/tabDocument/TabDocument.h>
#include <models/component.h>
#include "componenttreeview.h"
#include "componenteditortreemodel.h"

#include <QSharedPointer>
#include <QSplitter>

class LibraryInterface;

/*! \brief The editor to edit/packet IP-Xact components.
 *
 */
class ComponentEditor : public TabDocument {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Parent widget of the editor.
	 *
	*/
	ComponentEditor(LibraryInterface* libHandler, 
		QSharedPointer<Component> component, 
		QWidget *parent);

	//! \brief The destructor
	~ComponentEditor();

private:
	//! \brief No copying
	ComponentEditor(const ComponentEditor& other);

	//! \brief No assignment
	ComponentEditor& operator=(const ComponentEditor& other);

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! \brief The component being edited.
	QSharedPointer<Component> component_;

	//! \brief The tree view to navigate in the editor.
	ComponentTreeView navigationView_;

	//! \brief The model to control the navigation view.
	ComponentEditorTreeModel navigationModel_;
};

#endif // COMPONENTEDITOR_H
