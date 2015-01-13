/* 
 *  	Created on: 11.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapseditor.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSEDITOR_H
#define MEMORYMAPSEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <IPXACTmodels/component.h>

#include <QWidget>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class LibraryInterface;
class EditableTableView;
class MemoryMapsModel;
class LibraryInterface;

/*! \brief Editor to edit the memory maps of a component.
 *
 */
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

	//! \brief The view to display the table.
	EditableTableView* view_;

	//! \brief The proxy that does the sorting of items.
	QSortFilterProxyModel* proxy_;

	//! \brief The model that manages the memory maps.
	MemoryMapsModel* model_;
};

#endif // MEMORYMAPSEDITOR_H
