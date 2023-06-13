//-----------------------------------------------------------------------------
// File: SingleIndrectInterfaceEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2017
//
// Description:
// Editor for a single indirect interface within a component.
//-----------------------------------------------------------------------------

#ifndef SINGLEINDRECTINTERFACEEDITOR_H
#define SINGLEINDRECTINTERFACEEDITOR_H

#include <editors/ComponentEditor/ParameterItemEditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <QCheckBox>
#include <QComboBox>
#include <QWidget>

class BridgesEditor;
class Component;
class ExpressionFormatter;
class LibraryInterface;
class IndirectInterface;
class IndirectInterfaceValidator;
class ReferenceSelector;
class ParameterGroupBox;
class ParameterFinder;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Editor for a single indirect interface within a component.
//-----------------------------------------------------------------------------
class SingleIndirectInterfaceEditor : public ParameterItemEditor
{
    Q_OBJECT
public:

    /*!
     *  The constructor..
     *
     *      @param [in] indirectInterface   The indirect interface to edit.
     *      @param [in] validator           Validator for the indirect interface.
     *      @param [in] component           The component containing the indirect interface.
     *      @param [in] library             The available IP-XACT library.
     *      @param [in] finder              Finder for available parameters for the indirect interface.
     *      @param [in] formatter           Formatter for expressions.
     *      @param [in] parent              The parent widget.
     */
    SingleIndirectInterfaceEditor(QSharedPointer<IndirectInterface> indirectInterface,
        QSharedPointer<IndirectInterfaceValidator> validator,
        QSharedPointer<Component> component,
        LibraryInterface* library,
        QSharedPointer<ParameterFinder> finder,
        QSharedPointer<ExpressionFormatter> formatter,
        BusInterfaceInterface* busInterface,
        QWidget* parent);

	/*!
     *  The destructor.
     */
	virtual ~SingleIndirectInterfaceEditor();

    /*!
     *  Refreshes the information in the editor.
     */
    virtual void refresh();
    
protected:

    /*!
     *  Called when the editor is shown.
     */
    void showEvent(QShowEvent* event);

private slots:

    /*!
     *  Called when the indirect address reference has changed.
     */
    void onIndirectAddressChanged();

    /*!
     *  Called when the indirect ddatareference has changed.
     */
    void onIndirectDataChanged();

    /*!
     *  Called when the bits in lau has changed.
     */
    void onBitsInLauChanged();

    /*!
     *  Called when the endianness has changed.
     */
    void onEndiannessChanged();

    /*!
     *  Called when the indirect memory map has changed.
     */
    void onMemoryMapChanged();

    /*!
     *  Handle the selection of memory map.
     *
     *      @param [in] checked     Flag for enabling memory map.
     */
    void onMemoryMapSelected(bool checked);

    /*!
     *  Handle the selection of transparent bridges.
     *
     *      @param [in] checked     Flag for enabling transparent bridges.
     */
    void onTransparentBridgeSelected(bool checked);
private:

	// Disable copying.
	SingleIndirectInterfaceEditor(SingleIndirectInterfaceEditor const& rhs);    
    SingleIndirectInterfaceEditor& operator=(SingleIndirectInterfaceEditor const& rhs);

    //! Changes the address reference editor text color to red when invalid and black when valid.
    void setAddressReferenceColorByValidity();

    //! Changes the data reference editor text color to red when invalid and black when valid.
    void setDataReferenceColorByValidity();

    //! Changes the memory map reference editor text color to red when invalid and black when valid.
    void setMemoryMapColorByValidity();

    /*!
     *  Finds all the available field references.
     *
     *      @return The available field references in the current component.
     */
    QStringList findAvailableReferences() const;

    //! Sets the editor layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The indirect interface being edited.
    QSharedPointer<IndirectInterface> indirectInterface_;
    
    //! The validator for the indirect interface.
    QSharedPointer<IndirectInterfaceValidator> validator_;

    //! The component containing the indirect interface.
    QSharedPointer<Component> component_;

    //! Editor for indirect interface name group.
    NameGroupEditor* nameEditor_;

    //! Selection editor for address reference.
    ReferenceSelector* addressSelector_;

    //! Selection editor for data reference.
    ReferenceSelector* dataSelector_;

    //! Editor for indirect interface bits in lau.
    QLineEdit*  bitsInLauEditor_;

    //! Selection editor for endianness.
    QComboBox* endiannessSelector_;

    QCheckBox* memoryMapSelection_;

    QCheckBox* bridgeSelection_;

    //! Selection editor for memory map reference.
    ReferenceSelector* memoryMapSelector_;

    //! Editor for transparent bridges.
    BridgesEditor* transparentBridgesEditor_;

    //! Editor for indirect interface parameters.
    ParameterGroupBox* parametersEditor_;
};

#endif // SINGLEINDRECTINTERFACEEDITOR_H
