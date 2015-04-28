/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfielditem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORFIELDITEM_H
#define COMPONENTEDITORFIELDITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/field.h>
#include <IPXACTmodels/register.h>

#include <QFont>
#include <QSharedPointer>

class FieldEditor;
class MemoryMapsVisualizer;
class FieldGraphItem;

/*! \brief The item for single field in component editor's navigation tree.
 *
 */
class ComponentEditorFieldItem : public ComponentEditorItem {
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] reg                 Pointer to the register which contains this field.
	 *      @param [in] field               Pointer to the field being edited.
	 *      @param [in] model               Pointer to the model that owns the items.
	 *      @param [in] libHandler          Pointer to the instance that manages the library.
	 *      @param [in] component           Pointer to the component being edited.
	 *      @param [in] parameterFinder     Pointer to the parameter finder.
	 *      @param [in] referenceCounter    Pointer to the instance for counting references made to the parameters.
	 *      @param [in] parent              Pointer to the parent item.
	 */
	ComponentEditorFieldItem(QSharedPointer<Register> reg,
		QSharedPointer<Field> field,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ReferenceCounter> referenceCounter,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorFieldItem();

	/*! \brief Get the tool tip for the item.
	 * 
	 * \return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Check the validity of this item and sub items.
	 *
	 * \return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Get pointer to the visualizer of this item.
	 * 
	 * 
	 * \return Pointer to the visualizer to use for this item.
	*/
	virtual ItemVisualizer* visualizer();

	/*! \brief Set the visualizer for this item.
	 *
	 * \param visualizer Pointer to the visualizer.
	 *
	*/
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*! \brief Get the visualizer graphics item for the field.
	 *
	 * \return QGraphicsItem* Pointer to the graphics item.
	*/
	virtual QGraphicsItem* getGraphicsItem();

	/*! \brief Update the graphics item of the field.
	 *
	*/
	virtual void updateGraphics();

	/*! \brief Remove the graphics item of the field.
	 *
	*/
	virtual void removeGraphicsItem();

protected slots:

	/*! \brief Handler for editor's contentChanged signal.
	 *
	*/
	virtual void onEditorChanged();

private:
	
	//! \brief No copying
	ComponentEditorFieldItem(const ComponentEditorFieldItem& other);

	//! \brief No assignment
	ComponentEditorFieldItem& operator=(const ComponentEditorFieldItem& other);

	//! \brief Pointer to the register which contains this field.
	QSharedPointer<Register> reg_;

	//! \brief Pointer to the field being edited.
	QSharedPointer<Field> field_;

	//! \brief Pointer to the visualizer of memory maps.
	MemoryMapsVisualizer* visualizer_;

	//! \brief Pointer to the graph item that visualizes the field.
	FieldGraphItem* graphItem_;
};

#endif // COMPONENTEDITORFIELDITEM_H
