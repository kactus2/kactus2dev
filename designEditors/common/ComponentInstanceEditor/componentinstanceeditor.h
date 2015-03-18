/* 
 *  	Created on: 11.8.2011
 *      Author: Antti Kamppi
 * 		filename: componentinstanceeditor.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTINSTANCEEDITOR_H
#define COMPONENTINSTANCEEDITOR_H

#include <editors/ComponentEditor/software/PropertyValueEditor.h>

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
/*! \brief Editor to edit the details of a component instance within a design.
 *
 */
class ComponentInstanceEditor : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	ComponentInstanceEditor(QWidget *parent);
	
	//! \brief The destructor
	virtual ~ComponentInstanceEditor();

	/*! \brief Set the component to be edited.
	 *
	 * \param component Pointer to the component instance being edited.
	 *
	*/
    void setComponentInstance(ComponentItem* component);

    void setTopComponent(QSharedPointer<Component> topComponent);

public slots:

	/*! \brief Clear the editor so no instance details are shown
	 *
	*/
	void clear();

signals:

	//! \brief Emitted when contents of the editor changes.
	void contentChanged();

private slots:

	//! \brief Handler when user changes the name of the component instance.
	void onNameChanged(QString const& newName);

	//! \brief Handler when user changes the display name of the component instance.
	void onDisplayNameChanged(QString const& newDisplayName);

	//! \brief Handler when user changes the description of the component instance.
	void onDescriptionChanged(QString const& newDescription);

    //! Handler when the user changes any property values.
    void onPropertyValuesChanged();

    //! Handler when the user changes the file set reference.
    void onFileSetRefChanged(QString const& fileSetRef);

    //! Updates the file set reference when it is changed from the component.
    void updateFileSetRef(QString const& fileSetRef);

private:
	//! \brief No copying
	ComponentInstanceEditor(const ComponentInstanceEditor& other);

	//! \brief No assignment
	ComponentInstanceEditor& operator=(const ComponentInstanceEditor& other);

	//! \brief Pointer to the component instance being edited.
	ComponentItem* component_;

	//! \brief The widget to display the vlnv of the component instance
	VLNVDisplayer* vlnvDisplayer_;

	//! \brief The widget to set the instance name, display name and description.
	NameGroupBox* nameGroup_;

	//! \brief The widget to set the configurable elements of a component instance.
	ConfigurableElementEditor* configurableElements_;

    //! SW group.
    QGroupBox* swGroup_;

    //! File set reference combo box.
    QComboBox* fileSetRefCombo_;

    //! Property value editor for SW properties.
    PropertyValueEditor* propertyValueEditor_;

	//! \brief Pointer to the generic edit provider that manages the undo/redo stack.
	GenericEditProvider* editProvider_;

    //! The parameter finder for component instances.
    QSharedPointer<ParameterFinder> instanceFinder_;

    //! The parameter finder for component instances.
    QSharedPointer<ParameterFinder> topFinder_;
};

#endif // COMPONENTINSTANCEEDITOR_H
