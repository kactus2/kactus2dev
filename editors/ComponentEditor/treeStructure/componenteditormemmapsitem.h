//-----------------------------------------------------------------------------
// File: componenteditormemmapsitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Memory maps-item in the component navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORMEMMAPSITEM_H
#define COMPONENTEDITORMEMMAPSITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class ComponentEditorTreeModel;
class MemoryMapsVisualizer;
class ExpressionParser;
class MemoryMap;
class MemoryMapValidator;
class MemoryMapInterface;
class BusInterfaceInterface;
class ParameterValidator;
class MemoryMapsValidator;

//-----------------------------------------------------------------------------
//! The Memory maps-item in the component navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorMemMapsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] model                   The model that owns the items.
	 *    @param [in] libHandler              The instance that manages the library.
	 *    @param [in] component               The component being edited.
     *    @param [in] referenceCounter        The instance for counting references to parameters.
	 *    @param [in] parameterFinder         The parameter finder.
	 *    @param [in] expressionFormatter     The expression formatter.
     *    @param [in] expressionParser        The expression formatter.
	 *    @param [in] parent                  The parent item.
	 */
	ComponentEditorMemMapsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
		ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorMemMapsItem() = default;

	/*!
     *  Get the font to be used for text of this item.
	 *
	 *    @return QFont instance that defines the font to be used.
	 */
	virtual QFont getFont() const;

	/*!
     *  Get the tool tip for the item.
	 * 
	 *    @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *    @return QString Contains the text to display.
	 */
	virtual QString text() const;

	/*!
     *  Get The editor of this item.
	 *
	 *    @return The editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
     *  Add a new child to the item.
	 * 
	 *    @param [in] index The index to add the child into.
	 */
	virtual void createChild(int index);

	/*!
     *  Get The visualizer of memory maps.
	 * 
	 *    @return The visualizer to use for memory maps.
	 */
	virtual ItemVisualizer* visualizer();

public slots:

    /*!
     *  Give the selected memory map order to pass its address unit bits forward.
     *
     *    @param [in] memoryMapIndex   The index of the selected memory map.
     */
    void addressUnitBitsChangedOnMemoryMap(int memoryMapIndex);

signals:

    /*!
     *  Informs that a new memory remap item should be added.
     *
     *    @param [in] memoryRemapIndex        The index of the new memory remap item.
     *    @param [in] parentMemoryMapName     Name of the parent memory map of the new memory remap.
     */
    void memoryRemapAdded(int memoryRemapIndex, QString const& parentMemoryMapName);

    /*!
     *  Informs that a memory remap item should be removed.
     *
     *    @param [in] memoryRemapIndex        The index of the removed memory remap item.
     *    @param [in] parentMemoryMapName     Name of the parent memory map of the removed memory remap.
     */
    void memoryRemapRemoved(int memoryRemapIndex, QString const& parentMemoryMapName);

    /*
     *  Informs of memory map name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void memoryMapNameChanged(QString const& oldName, QString const& newName);

    /*
     *  Informs of memory remap name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void memoryRemapNameChanged(QString const& parentName, QString const& oldName, QString const& newName);

private:
	//! No copying.
	ComponentEditorMemMapsItem(const ComponentEditorMemMapsItem& other);

	//! No assignment.
	ComponentEditorMemMapsItem& operator=(const ComponentEditorMemMapsItem& other);

    /*!
     *  Check the validity of this item.
     *
     *    @return bool True if item is in valid state.
     */
    virtual bool isValid() const override;

    /*!
     *  Create the necessary validators for memory maps.
     */
    void createValidators();

    /*!
     *  Create the interfaces for memory maps.
     */
    void createMemoryMapInterface();

    /*!
     *  Create the interface for accessing bus interfaces.
     *
     *    @param [in] parameterValidator  Validator for parameters.
     *
     *    @return The interface for accessing bus interfaces.
     */
    BusInterfaceInterface* createInterfaceForBus(QSharedPointer<ParameterValidator> parameterValidator);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Contains the memory maps being edited.
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps_;

	//! The visualizer to display the memory maps
	MemoryMapsVisualizer* visualizer_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The current memory maps validator
    QSharedPointer<MemoryMapsValidator> memoryMapsValidator_;
    
    //! The current memory map validator.
    QSharedPointer<MemoryMapValidator> memoryMapValidator_;

    //! Interface for accessing memory maps.
    MemoryMapInterface* mapInterface_;
};

#endif // COMPONENTEDITORMEMMAPSITEM_H
