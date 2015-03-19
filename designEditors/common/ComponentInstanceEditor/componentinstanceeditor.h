/* 
 *  	Created on: 11.8.2011
 *      Author: Antti Kamppi
 * 		filename: componentinstanceeditor.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTINSTANCEEDITOR_H
#define COMPONENTINSTANCEEDITOR_H

#include <editors/ComponentEditor/software/PropertyValueEditor.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>
#include <common/widgets/nameGroupBox/namegroupbox.h>
#include <common/GenericEditProvider.h>
#include "configurableelementeditor.h"

#include <QWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QSharedPointer>

class ComponentItem;
class ParameterFinder;
class TopComponentParameterFinder;
/*! Editor to edit the details of a component instance within a design.
 *
 */
class ComponentInstanceEditor : public QWidget {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent Pointer to the owner of this widget.
	 *
	*/
	ComponentInstanceEditor(QWidget *parent);
	
	//! The destructor
	virtual ~ComponentInstanceEditor();

	/*! Set the component to be edited.
	 *
	 *      @param [in] component Pointer to the component instance being edited.
	*/
    void setComponentInstance(ComponentItem* component);

    /*!
     *  Sets the top component for the instances.
     *
     *      @param [in] topComponent   The top component.
     */
    void setContext(QSharedPointer<Component> topComponent, GenericEditProvider* editProvider);

    /*!
     *  Sets the protection state for the instance editor.
     *
     *      @param [in] locked   Locks/unlock the editor.
     */
    void setProtection(bool locked);

public slots:
        
    /*!
     *  Sets the active view to use for the top component.
     *
     *      @param [in] activeView   The name of the active view.
     */
    void setTopComponentActiveView(QString const& activeView);

	/*! Clear the editor so no instance details are shown
	 *
	*/
	void clear();

signals:

	//! Emitted when contents of the editor changes.
	void contentChanged();

private slots:

	//! Handler when user changes the name of the component instance.
	void onNameChanged(QString const& newName);

	//! Handler when user changes the display name of the component instance.
	void onDisplayNameChanged(QString const& newDisplayName);

	//! Handler when user changes the description of the component instance.
	void onDescriptionChanged(QString const& newDescription);

    //! Handler when the user changes any property values.
    void onPropertyValuesChanged();

    //! Handler when the user changes the file set reference.
    void onFileSetRefChanged(QString const& fileSetRef);

    //! Updates the file set reference when it is changed from the component.
    void updateFileSetRef(QString const& fileSetRef);

private:
	//! No copying
	ComponentInstanceEditor(const ComponentInstanceEditor& other);

	//! No assignment
	ComponentInstanceEditor& operator=(const ComponentInstanceEditor& other);
    
	//! Pointer to the component instance being edited.
	ComponentItem* component_;

	//! The widget to display the vlnv of the component instance
	VLNVDisplayer* vlnvDisplayer_;

	//! The widget to set the instance name, display name and description.
	NameGroupBox* nameGroup_;

	//! The widget to set the configurable elements of a component instance.
	ConfigurableElementEditor* configurableElements_;

    //! SW group.
    QGroupBox* swGroup_;

    //! File set reference combo box.
    QComboBox* fileSetRefCombo_;

    //! Property value editor for SW properties.
    PropertyValueEditor* propertyValueEditor_;

	//! Pointer to the generic edit provider that manages the undo/redo stack.
	GenericEditProvider* editProvider_;

    //! The parameter finder for component instances.
    QSharedPointer<ComponentParameterFinder> instanceFinder_;

    //! The parameter finder for component instances.
    QSharedPointer<TopComponentParameterFinder> topFinder_;

    //! The current top component.
    QSharedPointer<Component> topComponent_;
};

#endif // COMPONENTINSTANCEEDITOR_H
