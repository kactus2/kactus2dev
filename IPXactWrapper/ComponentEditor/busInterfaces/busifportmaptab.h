/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifportmaptab.h
 */

#ifndef BUSIFPORTMAPTAB_H
#define BUSIFPORTMAPTAB_H

#include "portmaps/physlistview.h"
#include "portmaps/logicallistview.h"
#include "portmaps/physlistmodel.h"
#include "portmaps/portmapsmodel.h"
#include "portmaps/logicallistmodel.h"
#include "portmaps/portmapsview.h"
#include <models/businterface.h>
#include <models/generaldeclarations.h>
#include <models/component.h>

#include <QWidget>
#include <QPushButton>
#include <QTableView>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QKeyEvent>
#include <QSharedPointer>

class LibraryInterface;
class PortMap;
class PortMapsPhysModel;

/*! \brief Editor to set the port maps of a bus interface.
 *
 */
class BusIfPortmapTab : public QWidget {
	Q_OBJECT

public:

	//! \brief Connection mode specifies how multiple selections are handled
	enum ConnectionMode {
		ONE2ONE = 0, 
		ONE2MANY};

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param busif Pointer to the bus interface being edited.
	 * \param parent Pointer to the owner of the editor.
	 *
	*/
	BusIfPortmapTab(LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		BusInterface* busif,
		QWidget* parent);
	
	//! \brief The destructor
	virtual ~BusIfPortmapTab();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

	/*! \brief Set the abstraction type that defines the logical signals to use.
	 *
	 * \param vlnv
	 *
	 * \return void
	*/
	void setAbsType(const VLNV& vlnv, General::InterfaceMode mode);

public slots:

	//! \brief Handler for remove button clicks.
	void onRemove();
	
protected:

	virtual void keyPressEvent(QKeyEvent* event);

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private slots:

	//! \brief The handler for refresh button clicks
	void onRefresh();

	//! \brief The handler for connect button clicks
	void onConnect();

	//! \brief Handler for when user changes the connection mode.
	void onConnectionModeChange();

	/*! \brief Make connections between physical signals and logical signals.
	 *
	 * \param physicals List of physical signal names.
	 * \param logicals List of logical signal names.
	 *
	*/
	void onMakeConnections(const QStringList& physicals, const QStringList& logicals);

private:
	
	//! \brief No copying
	BusIfPortmapTab(const BusIfPortmapTab& other);

	//! No assignment
	BusIfPortmapTab& operator=(const BusIfPortmapTab& other);

	//! \brief Set up the layout of the GUI items
	void setupLayout();

	//! \brief Specifies the connection mode
	ConnectionMode mode_;

	//! \brief Pointer to the currently selected port map.
	QSharedPointer<General::PortMap> portMap_;

	//! \brief Pointer to the bus interface being edited.
	BusInterface* busif_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! \brief The proxy filter that does sorting for port maps.
	QSortFilterProxyModel mapProxy_;

	//! \brief The model to manage the port maps.
	PortMapsModel model_;

	//! \brief The view to display the port maps.
	PortMapsView view_;

	//! \brief The view to display the logical ports
	LogicalListView logicalView_;

	//! \brief The model to present the logical ports
	LogicalListModel logicalModel_;

	//! \brief The view to display the physical ports
	PhysListView physicalView_;

	//! \brief The model to present the physical ports
	PhysListModel physModel_; 

	//! \brief The button to clean the lists of physical and logical ports.
	QPushButton cleanButton_;

	//! \brief The button to connect the selected logical and physical ports.
	QPushButton connectButton_;

	//! \brief The button to select the one to one connection mode.
	QPushButton one2OneButton_;

	//! \brief The button to select the one to many connection mode.
	QPushButton one2ManyButton_;
};

#endif // BUSIFPORTMAPTAB_H
