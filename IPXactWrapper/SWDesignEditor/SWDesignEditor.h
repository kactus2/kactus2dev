//-----------------------------------------------------------------------------
// File: SWDesignEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 15.11.2011
//
// Description:
// Editor for editing SW mapping designs.
//-----------------------------------------------------------------------------

#ifndef SWDESIGNEDITOR_H
#define SWDESIGNEDITOR_H

#include <common/widgets/tabDocument/TabDocument.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>
#include <models/component.h>

#include <IPXactWrapper/ComponentEditor/software/SoftwareMappingsEditor.h>
#include <LibraryManager/libraryinterface.h>

#include <QSharedPointer>
#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QString>
#include <QMap>
#include <QList>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
//! Editor for editing SW mapping designs.
//-----------------------------------------------------------------------------
class SWDesignEditor : public TabDocument
{
	Q_OBJECT

public:
	/*!
     *  Constructor.
	 *
	 *      @param [in] parent      The parent widget.
     *      @param [in] parentWnd   The parent window.
	 *      @param [in] libHandler  The library interface.
     *      @param [in] component   The component representing the SW design.
	 */
	SWDesignEditor(QWidget* parent, QWidget* parentWnd, LibraryInterface* libHandler,
                   QSharedPointer<Component> component);
	
	/*!
     *  Destructor.
     */
	virtual ~SWDesignEditor();

    /*!
     *  Sets the protection state of the document.
     *
     *      @param [in] locked True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

	/*!
     *  Returns the VLNV of the SW design component.
     */
	virtual VLNV getComponentVLNV() const;

public slots:
	//! Saves the document and resets the modified state.
	virtual bool save();

	//! \brief Saves the document as new object and resets modifies state
	virtual bool saveAs();

    /*!
     *  Adds a new empty endpoints mapping to the software mappings.
     */
    void addEndpointsMapping();

    /*!
     *  Removes the selected endpoints mapping from the software mappings.
     */
    void removeEndpointsMapping();

    /*!
     *  Updates the name of the selected list item based on the endpoints VLNV.
     */
    void updateListItem();

    /*!
     *  Updates the data entry related to the given list item.
     */
    void updateData();

    /*!
     *  Called when the selected item in the list widget has changed.
     */
    void onListItemChanged(QListWidgetItem* cur, QListWidgetItem* prev);

private:
    // Disable copying.
    SWDesignEditor(SWDesignEditor const& rhs);
    SWDesignEditor& operator=(SWDesignEditor const& rhs);

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

	//! Pointer to the instance that handles the library.
	LibraryInterface* libHandler_;

	//! The SW design component.
	QSharedPointer<Component> component_;

    //! VLNV editor for the API mapping.
    VLNVEditor platformMapping_;

    //! Group box for the endpoint stuff.
    QGroupBox endpointGroup_;

    //! List view of the mapped endpoints.
    QListWidget endpointList_;

    //! The button to add a new endpoints mapping.
    QPushButton addButton_;

    //! The button to remove the selected endpoints mapping.
    QPushButton removeButton_;

    //! Endpoint type VLNV editor.
    VLNVEditor endpointType_;

    //! VLNV editor for the linked application.
    VLNVEditor linkedAppType_;

    //! Component preview for the SW design component.
    ComponentPreviewBox previewBox_;

    //! Mapping data for the endpoints.
    QVector<EndpointsMappingData> endpointsMappingDatas_;
};

//-----------------------------------------------------------------------------

#endif // SWDESIGNEDITOR_H
