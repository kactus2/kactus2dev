//-----------------------------------------------------------------------------
// File: busifportmaptab.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 5.4.2011
//
// Description:
// Tab for editing and viewing component port maps.
//-----------------------------------------------------------------------------

#ifndef BUSIFPORTMAPTAB_H
#define BUSIFPORTMAPTAB_H


#include "portmaps/physlistview.h"
#include "portmaps/logicallistview.h"
#include "portmaps/physlistmodel.h"
#include "portmaps/PortMapsTreeModel.h"
#include "portmaps/logicallistmodel.h"
#include "portmaps/PortMapsTreeView.h"
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/component.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/BitMappingModel.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortListSortProxyModel.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/BitMappingView.h>

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QTableView>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QKeyEvent>
#include <QSharedPointer>

class LibraryInterface;
class PortMap;


/*! Editor to set the port maps of a bus interface.
 *
 */
class BusIfPortmapTab : public QWidget {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 * @param libHandler Pointer to the instance that manages the library.
	 * @param component Pointer to the component being edited.
	 * @param busif Pointer to the bus interface being edited.
	 * @param parent Pointer to the owner of the editor.
	 *
	*/
	BusIfPortmapTab(LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		BusInterface* busif,
		QWidget* parent);
	
	//! The destructor
	virtual ~BusIfPortmapTab();

	/*! Check for the validity of the edited item.
	*
	* @return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! Check for the validity of the edited item.
	*
    *      @param [inout] errorList   The list to add the possible error messages to.
    *
	*      @return True if item is valid.
	*/
	virtual bool isValid(QStringList& errorList) const;

	/*! Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

	/*! Set the abstraction type that defines the logical signals to use.
	 *
	 * @param vlnv
	 *
	 * @return void
	*/
	virtual void setAbsType(const VLNV& vlnv, General::InterfaceMode mode);

    /*!
     *  Sets a subset of component ports to be visible in the physical port list.
     *
     *      @param [in] ports   List of port names to show.
     */
    virtual void setPhysicalPorts(QStringList const& ports);

public slots:

	//! Handler for remove button clicks.
	void onRemove();
	
protected:

	//! Handler for key press events.
	virtual void keyPressEvent(QKeyEvent* event);

	//! Handler for widget's show event
    virtual void showEvent(QShowEvent* event);

signals:

	//! Emitted when contents of the model change
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

    //! Emitted when a help page should be changed in the context help window.
    void helpUrlRequested(QString const& url);

private slots:

	//! The handler for refresh button clicks
	void onRefresh();

	//! The handler for connect button clicks
	void onConnect();

    //! The handler for connect bits button clicks.
    void onBitConnect();

    //! The handler for connecting port to first unmapped logical bits.
    void onMapPortToLastBit();

	/*! Make connections between physical signals and logical signals.
	 *
	 * @param physicals List of physical signal names.
	 * @param logicals List of logical signal names.
	 *
	*/
    void onMakeConnections(QStringList const& physicalPorts, QStringList const& logicalPorts);

    /*!
     *  The handler for change in logical port selection.
     *
     *      @param [in] index   The index of the selected logical port.     
     */
    void onLogicalChanged(const QModelIndex& index);

    /*!
     *  The handler for changing bit field mapping visibility.
     *
     */
    void toggleMappingVisibility();

    //! Handler for show all button clicks.
    void onShowAll();

private:
	
	//! No copying
	BusIfPortmapTab(const BusIfPortmapTab& other);

	//! No assignment
	BusIfPortmapTab& operator=(const BusIfPortmapTab& other);

	//! Set up the layout of the GUI items
	void setupLayout();

    void setupFilterButton(QPushButton& button);

    /*!
     *  Maps each given physical port to one corresponding logical port.
     *
     *      @param [in] physicalPorts   The physical ports to map.
     *      @param [in] logicalPorts    The logical ports to map.
     */
    void mapOneToOne(QStringList const& physicalPorts, QStringList const& logicalPorts);

    /*!
     *  Maps all the given physical ports to all the given logical ports.
     *
     *      @param [in] physicalPorts   The physical ports to map.
     *      @param [in] logicalPorts    The logical ports to map.
     */
    void mapOneToMany(QStringList const& physicalPorts, QStringList const& logicalPorts);

    /*!
     *  Maps a single physical port to a single logical port.
     *
     *      @param [in] physicalPort   The physical port to map.
     *      @param [in] logicalPort    The logical port to map.
     */
    void mapPorts(QString const& physicalPort, QString const& logicalPort);

    /*!
     *  Gets the size of a logical port.
     *
     *      @param [in] logicalPort     The name of the logical port.
     *      @param [in] physicalPort    The name of the physical port to map the logical port to.
     *
     *      @return  The logical port size.
     */
    int getLogicalSize(QString const& logicalPort, QString const& physicalPort);

    /*!
     *  Gets the size of a physical port.
     *
     *      @param [in] physicalPort   The name of the physical port.
     *
     *      @return  The port size.
     */
    int getPhysicalSize(QString const& physicalPort);

	//! Pointer to the bus interface being edited.
	BusInterface* busif_;

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! The model to manage the port maps.
	PortMapsTreeModel model_;

	//! The view to display the port maps.
	PortMapsTreeView view_;

	//! The view to display the logical ports
	LogicalListView logicalView_;

	//! The model to present the logical ports.
	LogicalListModel logicalModel_;

    //! The label for bit-level mapping.
    QLabel mappingLabel_;

    //! The bit-level mapping view.
    BitMappingView mappingView_;

    //! The sorting proxy for bit-level mapping.
    QSortFilterProxyModel mappingProxy_;

    //! The model to present bit-level mapping.
    BitMappingModel mappingModel_;

	//! The view to display the physical ports.
	PhysListView physicalView_;

    //! The filter proxy for the physical ports.
    PortListSortProxyModel physProxy_;

	//! The model to present the physical ports.
	PhysListModel physModel_; 

	//! The button to clean the lists of physical and logical ports.
	QPushButton cleanButton_;

	//! The button to connect the selected logical and physical ports.
	QPushButton connectButton_;

    //! The button to clear port filter.
    QCheckBox showAllButton_;

    //! The button to show/hide bit-level mapping.
    QCheckBox showHideMappingButton_;

    //! Editor for filtering of physical ports by name.
    QLineEdit* nameFilterEditor_;

    //! Button for filtering ports by direction in.
    QPushButton inButton_;

    //! Button for filtering ports by direction out.
    QPushButton outButton_;

    //! Checkbox for showing/hiding connected physical ports.
    QCheckBox hideConnectedBox_;

    //! List of selected ports for physical port filter.
    QStringList portSet_;
};

#endif // BUSIFPORTMAPTAB_H
