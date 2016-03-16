//-----------------------------------------------------------------------------
// File: InstantiationsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 29.01.2016
//
// Description:
// Editor for component, design and design configuration instantiations.
//-----------------------------------------------------------------------------

#ifndef INSTANTIATIONSEDITOR_H
#define INSTANTIATIONSEDITOR_H

#include "ComponentInstantiationsEditor.h"

#include <editors/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>

class DesignConfigurationInstantiationsEditor;
class DesignInstantiationsEditor;
class LibraryInterface;
class InstantiationsValidator;
//-----------------------------------------------------------------------------
//! Editor for component, design and design configuration instantiations. 
//-----------------------------------------------------------------------------
class InstantiationsEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component           The component being edited.
     *      @param [in] parameterFinder     Finder used to identify parameters.
	 *      @param [in] handler             The instance managing the library.
     *      @param [in] viewValidator       The validator used for instantiations.
	 *      @param [in] parent              The parent of this editor.
	 */
	InstantiationsEditor(QSharedPointer<Component> component, QSharedPointer<ParameterFinder> parameterFinder,
        LibraryInterface* handler, QSharedPointer<InstantiationsValidator> validator, QWidget* parent = 0);
	
	//! The destructor.
	virtual ~InstantiationsEditor();

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:

    //! Emitted when a new component instantiation is added.
    void componentInstanceAdded(int);
    
    //! Emitted when a component instantiation is removed.
    void componentInstanceRemoved(int);

    //! Emitted when a new design configuration instantiation is added.
    void designConfigurationInstanceAdded(int);

    //! Emitted when a design configuration instantiation is removed.
    void designConfigurationInstanceRemoved(int);

    //! Emitted when a new design instantiation is added.
    void designInstanceAdded(int);

     //! Emitted when a new design instantiation is removed.   
    void designInstanceRemoved(int);

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying.
	InstantiationsEditor(const InstantiationsEditor& other);

	//! No assignment.
	InstantiationsEditor& operator=(const InstantiationsEditor& other);

    //! Editor for component instantiations.
	ComponentInstantiationsEditor* componentInstantiationsEditor_;

    //! Editor for design configuration instantiations.
    DesignConfigurationInstantiationsEditor* designConfigurationInstantiationsEditor_;

    //! Editor for design instantiations.
    DesignInstantiationsEditor* designInstantiationsEditor_;

};

#endif // INSTANTIATIONSEDITOR_H
