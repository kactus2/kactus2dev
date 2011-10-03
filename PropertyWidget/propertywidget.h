/* 
 *
 *  Created on: 22.2.2011
 *      Author: Antti Kamppi
 * 		filename: propertywidget.h
 */

#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H

#include "messageconsole.h"

#include <QTabWidget>

class DiagramComponent;
class DiagramPort;
class DiagramInterface;

/*! \brief Widget used to display messages to user and details of items.
 *
 */
class PropertyWidget : public QTabWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 *
	*/
	PropertyWidget(QWidget *parent);

	//! \brief The destructor
	virtual ~PropertyWidget();

signals:

	//! \brief The signal to emit error messages for user.
	void errorMessage(const QString& message);

	//! \brief The signal to emit notifications to user.
	void noticeMessage(const QString& message);

	//! \brief The signal to inform that an object has changed.
	void contentChanged();

public slots:

	/*! \brief Open the component editor for the component.
	 *
	 * \param component pointer to the DiagramComponent instance that user has 
	 * selected.
	 *
	*/
	void onComponentSelected(DiagramComponent* component);

	/*! \brief Open the interface editor for the port.
	 *
	 * \param port Pointer to the DiagramPort instance that user has selected.
	 *
	*/
	void onPortSelected(DiagramPort* port);

    /*!
     *  Opens the interface editor for the given interface.
     *
     *      @param [in] interface The interface that the user has selected.
     */
    void onInterfaceSelected(DiagramInterface* interface);

    
	/*! \brief Clear the editors from the propertyWidget because nothing is selected.
	 *
	*/
	void onClearItemSelection();

private:

	//! \brief No copying
	PropertyWidget(const PropertyWidget& other);

	//! No assignment
	PropertyWidget& operator=(const PropertyWidget& other);
	
	//! \brief The text edit to print notices and error messages to user.
	MessageConsole console_;
};

#endif // PROPERTYWIDGET_H
