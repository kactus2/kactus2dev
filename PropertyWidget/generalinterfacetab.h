/* 
 *
 *  Created on: 11.3.2011
 *      Author: Antti Kamppi
 * 		filename: generalinterfacetab.h
 */

#ifndef GENERALINTERFACETAB_H
#define GENERALINTERFACETAB_H

#include <designwidget/DiagramConnectionEndpoint.h>
#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QGraphicsItem>

/*! \brief Widget to display the generals settings of an interface.
 *
 */
class GeneralInterfaceTab : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param interface Pointer to the DiagramPort that is to be shown
	 * \param parent Pointer to the owner of this widget
	 *
	*/
	GeneralInterfaceTab(DiagramConnectionEndPoint* interface, QWidget *parent);
	
	//! \brief The destructor
	virtual ~GeneralInterfaceTab();

signals:

	//! \brief Emitted when user changes the settings in interface
	void contentChanged();

public slots:
    /*! \brief Save the editor contents to the bus interface
     *
     */
    void onSave();

    //! Updates the editor contents.
    void onContentChanged();

private:

	//! \brief No copying
	GeneralInterfaceTab(const GeneralInterfaceTab& other);

	//! No assignment
	GeneralInterfaceTab& operator=(const GeneralInterfaceTab& other);

	//! \brief The interface to be displayed.
	DiagramConnectionEndPoint* interface_;
	
	//! \brief Displays the Bus Definition for this bus interface.
	VLNVDisplayer busType_;

	//! \brief Displays the Abstraction Definition for this bus interface.
	VLNVDisplayer absType_;

	//! \brief The name of this bus interface.
	QLineEdit instanceName_;

	//! \brief The mode of this interface.
	QComboBox interfaceMode_;
};

#endif // GENERALINTERFACETAB_H
