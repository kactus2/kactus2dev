/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapselector.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSELECTOR_H
#define MEMORYMAPSELECTOR_H

#include <models/component.h>

#include <QComboBox>
#include <QSharedPointer>

/*! \brief Combo box to select a memory map from a component.
 *
 */
class MemoryMapSelector : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param component Pointer to the component that's memory maps are being selected.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	MemoryMapSelector(QSharedPointer<Component> component,
		QWidget *parent);
	
	//! \brief The destructor.
	virtual ~MemoryMapSelector();

public slots:

	//! \brief Refresh the items to be selected in the combo box.
	void refresh();

	/*! \brief Set the specified memory map as selected.
	 *
	 * \param memoryMapName The name of the memory map to select.
	 *
	*/
	void selectMemoryMap(const QString& memoryMapName);

signals:

	//! \brief Emitted when memory map was selected by user.
	void memoryMapSelected(const QString& memoryMapName);

private slots:

	//! \brief Handler for selected index changes.
	void onIndexChange(int newIndex);

private:
	
	//! \brief No copying
	MemoryMapSelector(const MemoryMapSelector& other);

	//! \brief No assignment
	MemoryMapSelector& operator=(const MemoryMapSelector& other);

	//! \brief Pointer to the component that's memory maps are being selected.
	QSharedPointer<Component> component_;
};

#endif // MEMORYMAPSELECTOR_H
