/* 
 *  	Created on: 11.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapseditor.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSEDITOR_H
#define MEMORYMAPSEDITOR_H

#include "memorymapsdelegate.h"

#include <editors/ComponentEditor/itemeditor.h>

#include <IPXACTmodels/component.h>

#include <QWidget>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class LibraryInterface;
class MemoryMapsView;
class MemoryMapsModel;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor for editing the memory maps of a component.
//-----------------------------------------------------------------------------
class MemoryMapsEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The default height and width of the editor.
	enum Sizes {
		HEIGHT = 300,
		WIDTH = 700
	};

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance managing the library.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	MemoryMapsEditor(QSharedPointer<Component> component,
		LibraryInterface* handler, 
		QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~MemoryMapsEditor();

	/*! \brief Check for the validity of the edited memory maps.
	*
	* \return True if all memory maps are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

	/*! \brief The size hint for the editor.
	 *
	 * \return QSize contains the size hint.
	*/
	virtual QSize sizeHint() const;

signals:

	//! \brief Emitted when a bus interface editor should be opened.
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

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! \brief Handler for view's doubleClicked-signal.
	virtual void onDoubleClick(const QModelIndex& index);

private:
	
	//! \brief No copying
	MemoryMapsEditor(const MemoryMapsEditor& other);

	//! \brief No assignment
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
