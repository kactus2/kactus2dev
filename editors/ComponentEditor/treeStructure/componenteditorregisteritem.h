//-----------------------------------------------------------------------------
// File: componenteditorregisteritem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// The item for single register in component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORREGISTERITEM_H
#define COMPONENTEDITORREGISTERITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/field.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/referenceCounter/ReferenceCounter.h>

#include <QSharedPointer>
#include <QList>

class RegisterEditor;
class MemoryMapsVisualizer;
class MemoryVisualizationItem;
class RegisterGraphItem;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! The item for single register in component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorRegisterItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] reg                     The register being edited.
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
	 *      @param [in] referenceCounter        The instance counting references made to parameters.
	 *      @param [in] expressionParser        The expression parser to use.
	 *      @param [in] parent                  The parent item.
	 */
	ComponentEditorRegisterItem(QSharedPointer<Register> reg, 
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ExpressionParser> expressionParser,
		ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorRegisterItem();

	/*! Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! Get pointer to the editor of this item.
	 *
	 *      @return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Add a new child to the item.
	 * 
	 *      @param [in] index The index to add the child into.
	*/
	virtual void createChild(int index);

	/*! Get pointer to the visualizer of this item.
	 * 
	 *      @return Pointer to the visualizer to use for this item.
	*/
	virtual ItemVisualizer* visualizer();

	/*! Set the visualizer for this item.
	 *
	 *      @param [in] visualizer Pointer to the visualizer.
	*/
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*! Get the visualizer graphics item for the register.
	 *
	 *      @return QGraphicsItem* Pointer to the graphics item.
	*/
	virtual QGraphicsItem* getGraphicsItem();


	//! Remove the graphics item of the register.
	virtual void removeGraphicsItem();

protected slots:

	//! Handler for editor's contentChanged signal.
	virtual void onEditorChanged();

    //! Update the graphics item of the register.
    virtual void updateGraphics();

    virtual void onGraphicsChanged();
    


private:
	
	//! No copying
	ComponentEditorRegisterItem(const ComponentEditorRegisterItem& other);
	ComponentEditorRegisterItem& operator=(const ComponentEditorRegisterItem& other);

    //! Updates the graphical items to match the current register dimension.
    void resizeGraphicsToCurrentDimensionSize();

    /*!
     *  Creates a graphical item for representing a register dimension.
     *
     *      @param [in] dimensionIndex      The index of the represented dimension.
     *      @param [in] parentItem          The parent graphical item for the dimension item.
     */
    void createDimensionGraphicsItem(int dimensionIndex, MemoryVisualizationItem* parentItem);
    
    /*!
     *  Removes a graphical item representing a register dimension.
     *
     *      @param [in] dimensionIndex      The index of the dimension to remove.
     *      @param [in] parentItem          The parent graphical item of the dimension item.
     */
    void removeDimensionGraphicsItem(int dimensionIndex, MemoryVisualizationItem* parentItem);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The register being edited.
	QSharedPointer<Register> reg_;

	//! The visualizer of memory maps.
	MemoryMapsVisualizer* visualizer_;

	//! The graph items that visualizes the register dimensions.
	QList<RegisterGraphItem*> registerDimensions_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // COMPONENTEDITORREGISTERITEM_H
