//-----------------------------------------------------------------------------
// File: componenteditorregisterfileitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// Item representing a register file in the component editor browser tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORREGISTERFILEITEM_H
#define COMPONENTEDITORREGISTERFILEITEM_H

#include "componenteditoritem.h"

#include <IPXACTmodels/Component/RegisterFile.h>
#include <QSharedPointer>

class MemoryVisualizationItem;
class MemoryMapsVisualizer;
class RegisterFileGraphItem;
class RegisterFileValidator;

//-----------------------------------------------------------------------------
//! Item representing a register file in the component editor browser tree.
//-----------------------------------------------------------------------------
class ComponentEditorRegisterFileItem : public ComponentEditorItem
{
    Q_OBJECT

public:
    /*!
     *  The constructor.
     *
     *      @param [in] regFile                 The register file being edited.
     *      @param [in] model                   The model that owns the items.
     *      @param [in] libHandler              The instance that manages the library.
     *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The instance for counting references to parameters.
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] expressionParser        The expression formatter.
     *      @param [in] addressBlockValidator   Validator used for address blocks.
     *      @param [in] parent                  The parent item.
     */
    ComponentEditorRegisterFileItem(
        QSharedPointer<RegisterFile> regFile,
        ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<RegisterFileValidator> registerFileValidator,
        ComponentEditorItem* parent);

    //! The destructor.
    virtual ~ComponentEditorRegisterFileItem() = default;

    /*!
    *  Get the tool tip for the item.
    *
    *      @return The text for the tool tip to print to user.
    */
    virtual QString getTooltip() const;

    /*!
    *  Get the text to be displayed to user in the tree for this item.
    *
    *      @return QString Contains the text to display.
    */
    virtual QString text() const;

    /*!
    *  Check the validity of this item and sub items.
    *
    *      @return bool True if item is in valid state.
    */
    virtual bool isValid() const;

    /*!
    *  Get pointer to the editor of this item.
    *
    *      @return The editor to use for this item.
    */
    virtual ItemEditor* editor();

    /*!
    *  Add a new child to the item.
    *
    *      @param [in] index The index to add the child into.
    */
    virtual void createChild(int index);

    /*!
    *  Get the visualizer graphics item for the memory map.
    *
    *      @return QGraphicsItem* The graphics item.
    */
    virtual QGraphicsItem* getGraphicsItem();

    /*!
    *  Remove the graphics item of the memory map.
    */
    virtual void removeGraphicsItem();

    /*!
    *  Get pointer to the visualizer of this item.
    *
    *      @return The visualizer to use for this item.
    */
    virtual ItemVisualizer* visualizer();


    virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

protected slots:


    /*!
    *  Update the graphics item of the memory map.
    */
    virtual void updateGraphics();

    //!  Handler for editor's contentChanged signal.
    virtual void onGraphicsChanged();

private:

    //! Updates the graphical items to match the current register file dimension.
    void resizeGraphicsToCurrentDimensionSize();

    /*!
    *  Creates a graphical item for representing a register file dimension.
    *
    *      @param [in] dimensionIndex      The index of the represented dimension.
    *      @param [in] parentItem          The parent graphical item for the dimension item.
    */
    void createDimensionGraphicsItem(int dimensionIndex, MemoryVisualizationItem* parentItem);

    /*!
    *  Removes a graphical item representing a register file dimension.
    *
    *      @param [in] dimensionIndex      The index of the dimension to remove.
    *      @param [in] parentItem          The parent graphical item of the dimension item.
    */
    void removeDimensionGraphicsItem(int dimensionIndex, MemoryVisualizationItem* parentItem);

    //! The register file being edited.
    QSharedPointer<RegisterFile> registerFile_;

    //! The visualizer for memory maps.
    MemoryMapsVisualizer* visualizer_;

    //! The graph items that visualizes the register file dimensions.
    QList<RegisterFileGraphItem*> registerFileDimensions_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The used address block validator.
    QSharedPointer<RegisterFileValidator> registerFileValidator_;
};

#endif // COMPONENTEDITORREGISTERFILEITEM_H
