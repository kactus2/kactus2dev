/* 
 *
 *  Created on: 14.3.2011
 *      Author: Antti Kamppi
 * 		filename: portmapinterfacetab.h
 */

#ifndef PORTMAPINTERFACETAB_H
#define PORTMAPINTERFACETAB_H

#include <designwidget/DiagramConnectionEndpoint.h>

#include <QWidget>
#include <QTableWidget>

/*! \brief PortmapInterfaceTab is a widget to edit/view port's signal mappings.
 *
 */
class PortmapInterfaceTab : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param interface Pointer to the interface instance being edited
	 * \param parent Pointer to the owner of this widget
	 *
	*/
	PortmapInterfaceTab(DiagramConnectionEndPoint* interface, QWidget *parent);

	//! \brief The destructor
	virtual ~PortmapInterfaceTab();

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
	PortmapInterfaceTab(const PortmapInterfaceTab& other);

	//! No assignment
	PortmapInterfaceTab& operator=(const PortmapInterfaceTab& other);
	
	//! \brief Contains the mapping of ports and logical signals
	QTableWidget mappings_;

	//! \brief The interface being edited/viewed.
	DiagramConnectionEndPoint* interface_;
};

#endif // PORTMAPINTERFACETAB_H
