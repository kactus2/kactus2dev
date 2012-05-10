//-----------------------------------------------------------------------------
// File: SoftwareMappingsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.10.2011
//
// Description:
// Editor for editing software mappings for a specific HW component.
//-----------------------------------------------------------------------------

#ifndef SOFTWAREMAPPINGSEDITOR_H
#define SOFTWAREMAPPINGSEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QString>
#include <QMap>
#include <QList>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! SoftwareMappingsEditor class.
//-----------------------------------------------------------------------------
class SoftwareMappingsEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  Constructor.
     *
	 *      @param [in] component    Pointer to the component being edited.
	 *      @param [in] parent       Pointer to the owner of this widget.
	 * 
	 */
	SoftwareMappingsEditor(LibraryInterface* libHandler, 
		QSharedPointer<Component> component,
						  QWidget* parent,
						  QWidget* parentWnd);

	/*!
     *  Destructor.
     */
	~SoftwareMappingsEditor();

	/*!
     *  Checks for the validity of the edited mappings.
     *
	 *      @return True if all mappings are in valid state.
	 */
	bool isValid() const;

	/*!
     *  Applies the changes from the widgets editors to the IPXact model.
     */
	void makeChanges();

private:
    // Disable copying.
    SoftwareMappingsEditor(SoftwareMappingsEditor const& rhs);
    SoftwareMappingsEditor& operator=(SoftwareMappingsEditor const& rhs);

    //-----------------------------------------------------------------------------
    //! EndpointsMappingData structure.
    //-----------------------------------------------------------------------------
    struct EndpointsMappingData
    {
        VLNV endpointsVLNV; //!< The VLNV of the endpoints.
        VLNV linkedAppVLNV; //!< The VLNV of the linked application.
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library handler.
    LibraryInterface* libInterface_;

    //! The edited component.
    QSharedPointer<Component> component_;

    //! VLNV editor for the mapped SW design.
    VLNVEditor swDesignMapping_;
};

//-----------------------------------------------------------------------------

#endif // SOFTWAREMAPPINGSEDITOR_H
