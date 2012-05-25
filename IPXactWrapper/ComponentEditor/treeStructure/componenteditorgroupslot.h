/* 
 *  	Created on: 21.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorgroupslot.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORGROUPSLOT_H
#define COMPONENTEDITORGROUPSLOT_H

#include <QWidget>
#include <QHBoxLayout>

class ComponentEditorGroupSlot : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 * 
	 * \param parent Pointer to the owner of the group slot.
	 * \param title The title for the group box.
	 *
	*/
	ComponentEditorGroupSlot(QWidget *parent);
	
	//! \brief The destructor
	~ComponentEditorGroupSlot();

	/*! \brief Set the widget to be displayed inside the group slot.
	 *
	 * \param widget Pointer to the widget to display.
	 *
	*/
	void setWidget(QWidget* widget);

private:
	
	//! \brief No copying
	ComponentEditorGroupSlot(const ComponentEditorGroupSlot& other);

	//! \brief No assignment
	ComponentEditorGroupSlot& operator=(const ComponentEditorGroupSlot& other);

	//! \brief The layout for the group slot.
	QHBoxLayout layout_;

	//! \brief The currently displayed widget.
	QWidget* currentWidget_;
};

#endif // COMPONENTEDITORGROUPSLOT_H
