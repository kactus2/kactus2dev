//-----------------------------------------------------------------------------
// File: componenteditorfielditem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 28.08.2012
//
// Description:
// The item for single field in component editor's navigation tree
//-----------------------------------------------------------------------------

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
class ExpressionParser;

//-----------------------------------------------------------------------------
//! The item for single field in component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorFieldItem : public ComponentEditorItem
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] reg                 The register which contains this field.
	 *      @param [in] field               The field being edited.
	 *      @param [in] model               The model that owns the items.
	 *      @param [in] libHandler          The instance that manages the library.
	 *      @param [in] component           The component being edited.
	 *      @param [in] parameterFinder     The parameter finder.
	 *      @param [in] referenceCounter    The instance for counting references made to the parameters.
     *      @param [in] expressionParser    Expression parser to use.
     *      @param [in] parent              The parent item.
	 */
	ComponentEditorFieldItem(QSharedPointer<Register> reg,
		QSharedPointer<Field> field,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ExpressionParser> expressionParser,
		ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorFieldItem();

	/*! Get the tool tip for the item.
	 * 
	 *       @return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! Get the text to be displayed to user in the tree for this item.
	 *
	 *       @return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! Check the validity of this item and sub items.
	 *
	 *       @return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! Get the editor of this item.
	 *
	 *       @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Get the visualizer of this item.
	 * 
	 *       @return The visualizer to use for this item.
	*/
	virtual ItemVisualizer* visualizer();

	/*! Set the visualizer for this item.
	 *
	 *      @param [in] visualizer The visualizer to set.
	*/
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*! Get the visualizer graphics item for the field.
	 *
	 *       @return The graphics item.
	*/
	virtual QGraphicsItem* getGraphicsItem();

	/*! Update the graphics item of the field.
	 *
	*/
	virtual void updateGraphics();

	/*! Remove the graphics item of the field.
	 *
	*/
	virtual void removeGraphicsItem();

signals:

    /*!
     *  Inform of the need to redraw the visualization.
     */
    void graphicsChanged();

protected slots:

	/*! Handler for editor's contentChanged signal.
	 *
	*/
	virtual void onEditorChanged();

private:
	
	//! No copying
	ComponentEditorFieldItem(const ComponentEditorFieldItem& other);

	//! No assignment
	ComponentEditorFieldItem& operator=(const ComponentEditorFieldItem& other);

	//! The register which contains this field.
	QSharedPointer<Register> reg_;

	//! The field being edited.
	QSharedPointer<Field> field_;

	//! The visualizer of memory maps.
	MemoryMapsVisualizer* visualizer_;

	//! The graph item that visualizes the field.
    FieldGraphItem* graphItem_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // COMPONENTEDITORFIELDITEM_H
