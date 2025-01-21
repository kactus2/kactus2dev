//-----------------------------------------------------------------------------
// File: memorymapseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 11.04.2012
//
// Description:
// Editor for editing the memory maps of a component.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPSEDITOR_H
#define MEMORYMAPSEDITOR_H

#include "memorymapsdelegate.h"

#include <editors/ComponentEditor/itemeditor.h>
#include <KactusAPI/include/ParameterFinder.h>

#include <QWidget>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class LibraryInterface;
class MemoryMapsView;
class MemoryMapsModel;
class Component;
class MemoryMap;
class ExpressionFormatter;
class ExpressionParser;
class MemoryMapValidator;
class EditableTreeSortFilter;
class MemoryMapInterface;

//-----------------------------------------------------------------------------
//! Editor for editing the memory maps of a component.
//-----------------------------------------------------------------------------
class MemoryMapsEditor : public ItemEditor
{
	Q_OBJECT

public:

	//! The default height and width of the editor.
	enum Sizes
    {
		HEIGHT = 300,
		WIDTH = 700
	};

	/*!
	 *  The constructor.
	 *
     *    @param [in] mapInterface        Interface for memory maps.
	 *    @param [in] component           The component being edited.
	 *    @param [in] parameterFinder     The parameter finder.
     *    @param [in] expressionParser    The used expression parser.
	 *    @param [in] handler             The instance managing the library.
	 *    @param [in] parent              The owner of this editor.
	 */
	MemoryMapsEditor(MemoryMapInterface* mapInterface,
        QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
		LibraryInterface* handler,
        QWidget *parent = 0);
	
	//! The destructor.
	virtual ~MemoryMapsEditor();

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

	/*!
     *  The size hint for the editor.
	 *
	 *    @return QSize contains the size hint.
	 */
	virtual QSize sizeHint() const;

signals:

	//! Emitted when a bus interface editor should be opened.
	void selectBusInterface(const QString& interfaceName);

    /*!
     *  Emitted when address unit bits are changed.
     *
     *    @param [in] memoryMapIndex   The index of the changed memory map.
     */
    void changeInAddressUnitBitsOnRow(int memoryMapIndex);

    /*!
     *  Inform of the added memory remap.
     *
     *    @param [in] memoryRemapIndex        The index of the new memory remap.
     *    @param [in] parentMemoryMapName     Name of the owner of the new memory remap.
     */
    void memoryRemapAdded(int memoryRemapIndex, QString const& parentMemoryMapName);

    /*!
     *  Inform of the removed memory remap.
     *
     *    @param [in] memoryRemapIndex        The index of the removed memory remap.
     *    @param [in] parentMemoryMapName     Name of the owner of the removed memory remap.
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
     *    @param [in] parentName  Name of the containing memory map.
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void memoryRemapNameChanged(QString const& parentName, QString const& oldName, QString const& newName);

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private slots:

	//! Handler for view's doubleClicked-signal.
	virtual void onDoubleClick(const QModelIndex& index);

private:
	
	//! No copying.
	MemoryMapsEditor(const MemoryMapsEditor& other);

	//! No assignment.
	MemoryMapsEditor& operator=(const MemoryMapsEditor& other);

    /*!
     *  Gets the names of the components remap states.
     *
     *    @return A list containing all the names of the remap states.
     */
    QStringList getRemapStateNames() const;

    /*!
     *  Connect signals.
     */
    void connectSignals();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The view to display the table.
    MemoryMapsView* view_;

	//! The proxy that does the sorting of items.
    EditableTreeSortFilter* proxy_;

	//! The model that manages the memory maps.
	MemoryMapsModel* model_;

    //! The delegate of the view.
    MemoryMapsDelegate* delegate_;

    //! Interface for memory maps.
    MemoryMapInterface* interface_;

    //! Component containing the memory maps.
    QSharedPointer<Component> component_;
};

#endif // MEMORYMAPSEDITOR_H
