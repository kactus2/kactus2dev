//-----------------------------------------------------------------------------
// File: memorymapseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QWidget>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class LibraryInterface;
class MemoryMapsView;
class MemoryMapsModel;
class Component;
class MemoryMap;

class MemoryMapValidator;
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
	 *      @param [in] component           Pointer to the component being edited.
	 *      @param [in] parameterFinder     Pointer to the parameter finder.
	 *      @param [in] handler             Pointer to the instance managing the library.
     *      @param [in] memoryMapValidator  Validator used for memory maps.
	 *      @param [in] parent              Pointer to the owner of this editor.
	 */
	MemoryMapsEditor(QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
		LibraryInterface* handler, 
        QSharedPointer<MemoryMapValidator> memoryMapValidator,
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
	 *      @return QSize contains the size hint.
	 */
	virtual QSize sizeHint() const;

signals:

	//! Emitted when a bus interface editor should be opened.
	void selectBusInterface(const QString& interfaceName);

    /*!
     *  Emitted when address unit bits are changed.
     *
     *      @param [in] memoryMapIndex   The index of the changed memory map.
     */
    void changeInAddressUnitBitsOnRow(int memoryMapIndex);

    /*!
     *  Inform of the added memory remap.
     *
     *      @param [in] memoryRemapIndex    The index of the new memory remap.
     *      @param [in] parentMemoryMap     The owner of the new memory remap.
     */
    void memoryRemapAdded(int memoryRemapIndex, QSharedPointer<MemoryMap> parentMemoryMap);

    /*!
     *  Inform of the removed memory remap.
     *
     *      @param [in] memoryRemapIndex    The index of the removed memory remap.
     *      @param [in] parentMemoryMap     The owner of the removed memory remap.
     */
    void memoryRemapRemoved(int memoryRemapIndex, QSharedPointer<MemoryMap> parentMemoryMap);

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
     *      @return A list containing all the names of the remap states.
     */
    QStringList getRemapStateNames() const;

	//! The view to display the table.
    MemoryMapsView* view_;

	//! The proxy that does the sorting of items.
	QSortFilterProxyModel* proxy_;

	//! The model that manages the memory maps.
	MemoryMapsModel* model_;

    //! The delegate of the view.
    MemoryMapsDelegate* delegate_;
};

#endif // MEMORYMAPSEDITOR_H
