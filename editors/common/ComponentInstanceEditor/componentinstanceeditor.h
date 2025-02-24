//-----------------------------------------------------------------------------
// File: componentinstanceeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 11.08.2011
//
// Description:
// Editor to edit the details of a component instance within a design.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCEEDITOR_H
#define COMPONENTINSTANCEEDITOR_H

#include "PowerDomainLinkEditor.h"

#include <common/IEditProvider.h>
#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>
#include <common/widgets/nameGroupBox/namegroupbox.h>

#include <KactusAPI/include/ComponentParameterFinder.h>
#include <KactusAPI/include/ListParameterFinder.h>

#include <editors/ComponentEditor/software/PropertyValueEditor.h>
#include <editors/ComponentEditor/common/ConfigurableElementFinder.h>

#include <editors/common/ComponentInstanceEditor/ComponentInstanceConfigurableElementsEditor.h>
#include <editors/common/TopComponentParameterFinder.h>

#include <QWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QSharedPointer>

class ComponentItem;
class ParameterFinder;
class TopComponentParameterFinder;
class Design;
class DesignCompletionModel;
class View;
class ListParameterFinder;

//-----------------------------------------------------------------------------
//! Editor to edit the details of a component instance within a design.
//-----------------------------------------------------------------------------
class ComponentInstanceEditor : public QWidget
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *    @param [in] parent Pointer to the owner of this widget.
	 */
	ComponentInstanceEditor(QWidget *parent);
	
	//! The destructor.
	virtual ~ComponentInstanceEditor() = default;

    //! No copying.
    ComponentInstanceEditor(const ComponentInstanceEditor& other) = delete;

    //! No assignment.
    ComponentInstanceEditor& operator=(const ComponentInstanceEditor& other) = delete;

	/*!
     *  Set the component to be edited.
	 *
	 *    @param [in] component       Pointer to the component instance being edited.
     *    @param [in] editProvider    Interface for providing editing capabilities (undo & redo).
     *    @param [in] design          Design containing the component instance.
	 */
    void setComponentInstance(ComponentItem* component, QSharedPointer<IEditProvider> editProvider,
        QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConfiguration, QString const& activeViewName);

    /*!
     *  Sets the top component for the instances.
     *
     *    @param [in] topComponent            The top component.
     *    @param [in] editProvider            The generic edit provider.
     */
    void setContext(QSharedPointer<Component> topComponent, QSharedPointer<IEditProvider> editProvider);

    /*!
     *  Sets the protection state for the instance editor.
     *
     *    @param [in] locked   Locks/unlock the editor.
     */
    void setProtection(bool locked);

public slots:
        
    /*!
     *  Sets the active view to use for the top component.
     *
     *    @param [in] activeView   The name of the active view.
     */
    void setTopComponentActiveView(QSharedPointer<View> activeView);

	/*!
     *  Clear the editor so no instance details are shown
	 */
	void clear();

signals:

	/*!
     *  Emitted when contents of the editor changes.
     */
	void contentChanged();
    
    /*!
     *  Increase the amount of references to a parameter with the matching ID.
     *
     *    @param [in] id  ID of the parameter, whose references are being increased.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the amount of references to a parameter with a matching ID.
     *
     *    @param [in] id  ID of the parameter, whose references are being decreased.
     */
    void decreaseReferences(QString const& id);

private slots:

	//! Handler when user changes the name of the component instance.
    void onNameChanged();

	//! Handler when user changes the display name of the component instance.
    void onDisplayNameChanged();

	//! Handler when user changes the description of the component instance.
    void onDescriptionChanged();

    //! Handler when the user changes any property values.
    void onPropertyValuesChanged();

    //! Handler when the user changes the file set reference.
    void onFileSetRefChanged(QString const& fileSetRef);

    //! Updates the file set reference when it is changed from the component.
    void updateFileSetRef(QString const& fileSetRef);

private:
    void setupLayout();

	//! Pointer to the component instance being edited.
	ComponentItem* component_ = nullptr;

	//! The widget to display the vlnv of the component instance
	VLNVDisplayer* vlnvDisplayer_ = new VLNVDisplayer(this, VLNV(), true);

	//! The widget to set the instance name, display name and description.
	NameGroupBox* nameGroup_ = new NameGroupBox(this, tr("Component instance name"));

    QLabel* activeViewLabel_ = new QLabel(this);

	//! The widget to set the configurable elements of a component instance.
    ComponentInstanceConfigurableElementsEditor* configurableElements_ = nullptr;

    //! The widget for setting power domain links.
    PowerDomainLinkEditor* powerDomainEditor_ = new PowerDomainLinkEditor(this);

    //! SW group.
    QGroupBox* swGroup_ = new QGroupBox(tr("Software"), this);

    //! File set reference combo box.
    QComboBox* fileSetRefCombo_ = new QComboBox(this);

    //! Property value editor for SW properties.
    PropertyValueEditor* propertyValueEditor_ = new PropertyValueEditor(this);

	//! Pointer to the generic edit provider that manages the undo/redo stack.
	QSharedPointer<IEditProvider> editProvider_ = nullptr;

    //! The parameter finder for component instances.
    QSharedPointer<ComponentParameterFinder> instanceFinder_ =
        QSharedPointer<ComponentParameterFinder>(new ComponentParameterFinder(nullptr));

    //! The parameter finder for component instances.
    QSharedPointer<TopComponentParameterFinder> topFinder_ =
        QSharedPointer<TopComponentParameterFinder>(new TopComponentParameterFinder(nullptr));

    //! The parameter finder for design.
    QSharedPointer<ListParameterFinder> designParameterFinder_ =
        QSharedPointer<ListParameterFinder>(new ListParameterFinder());

    //! The current top component.
    QSharedPointer<Component> topComponent_ = nullptr;

    //! The design containing the component instance.
    QSharedPointer<Design> containingDesign_ = nullptr;
};

#endif // COMPONENTINSTANCEEDITOR_H
