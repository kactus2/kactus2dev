/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapselector.h
 *		Project: Kactus 2
 */

#ifndef REFERENCESELECTOR_H
#define REFERENCESELECTOR_H

#include <QColor>
#include <QComboBox>

//-----------------------------------------------------------------------------
//! Combo box to select a name reference.
//-----------------------------------------------------------------------------
class ReferenceSelector : public QComboBox
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 * \param component Pointer to the component that's memory maps are being selected.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	ReferenceSelector(QWidget *parent);
	
	//! The destructor.
	virtual ~ReferenceSelector();

public slots:

	//! Refresh the items to be selected in the combo box.
    void refresh(QStringList const& itemNames);

	/*! Set the specified memory map as selected.
	 *
	 * \param memoryMapName The name of the memory map to select.
	 *
	*/
    void selectItem(QString const& itemName);

protected:

    /*!
     *  Handles the change events and changes the text colour depengin on the enabled state of the component.
     *
     *      @param [in] currentEvent    The current change event.
     */
    virtual void changeEvent(QEvent *currentEvent);

signals:

	//! Emitted when memory map was selected by user.
	void itemSelected(QString const& memoryMapName);

private slots:

	//! Handler for selected index changes.
	void onIndexChange(int newIndex);

private:
	
	//! No copying
	ReferenceSelector(const ReferenceSelector& other);

	//! No assignment
	ReferenceSelector& operator=(const ReferenceSelector& other);

    /*!
     *  Sets the color of the text of the current text.
     *
     *      @param [in] color   The color to set.
     */
    void setTextColor(QColor const& color);
};

#endif // REFERENCESELECTOR_H
