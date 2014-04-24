//-----------------------------------------------------------------------------
// File: SystemDetailsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.7.2012
//
// Description:
// Docking editor for system details.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDETAILSEDITOR_H
#define SYSTEMDETAILSEDITOR_H

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <IPXACTmodels/component.h>

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSharedPointer>
#include <QList>

class DesignWidget;
class SystemView;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Docking editor to edit system details.
//-----------------------------------------------------------------------------
class SystemDetailsEditor : public QWidget
{
	Q_OBJECT

public:
	/*!
     *  Constructor.
	 *
	 *      @param [in] handler The library interface.
	 *      @param [in] parent  The parent widget.
	 */
	SystemDetailsEditor(LibraryInterface* handler, QWidget *parent);
	
	/*!
     *  Destructor.
     */
	virtual ~SystemDetailsEditor();

	/*!
     *  Set the system to be displayed.
	 *
	 *      @param [in] designWidget The system design widget containing the opened system.
	 *      @param [in] locked       If true, the system details are locked and cannot be modified.
	 */
	void setSystem(DesignWidget* designWidget, bool locked);

	/*!
     *  Clear the contents of this editor.
	 */
	void clear();

public slots:
	//! Handler for design widget's refreshed signal.
	void onRefresh();

	//! Set the editor to locked/unlocked mode.
	void setLocked(bool locked);

signals:
	//! Emitted when the editor changes some value.
	void contentChanged();

private slots:
    //! Called when the HW component reference has been changed.
    void onHWRefChanged();

    //! Called when the view reference has been changed from the combo box.
    void onViewRefChanged(QString const& viewRef);

    //! Applies the selected HW to the system.
    void applyHW();

    //! Reverts the changes.
    void revert();

    //! Removes the HW mapping altogether.
    void removeMapping();

    //! Exports SW to a new system component.
    void exportSW();

private:
    // Disable copying.
    SystemDetailsEditor(SystemDetailsEditor const& rhs);
    SystemDetailsEditor& operator=(SystemDetailsEditor const& rhs);

	/*! 
     *  Sets up the layout of this widget.
     */
	void setupLayout();

	/*!
     *  Set up the signals between widgets.
     */
	void setupConnections();
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;

    //! HW reference editor.
    VLNVEditor* hwRefEditor_;

	//! Combo box to select the used configuration.
	QComboBox* viewSelector_;

    //! Button to apply new HW mapping.
    QPushButton* applyButton_;

    //! Button to revert to the previously applied HW mapping.
    QPushButton* revertButton_;

    //! Button to remove mappings.
    QPushButton* removeMappingButton_;

    //! Button to export SW.
    QPushButton* exportButton_;

	//! Pointer to the top component being edited.
	QSharedPointer<Component> component_;

	//! Pointer to the design widget that contains the design.
	DesignWidget* designWidget_;

    //! The system view containing the edited system.
    SystemView* systemView_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDETAILSEDITOR_H
