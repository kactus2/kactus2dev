//-----------------------------------------------------------------------------
// File: SingleIndrectInterfaceEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2017
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef SINGLEINDRECTINTERFACEEDITOR_H
#define SINGLEINDRECTINTERFACEEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>


#include <QComboBox>
#include <QWidget>

class BridgesEditor;
class Component;
class LibraryInterface;
class IndirectInterface;
class ReferenceSelector;

class SingleIndirectInterfaceEditor : public ItemEditor
{
    Q_OBJECT
public:

	//! The constructor.
    SingleIndirectInterfaceEditor(QSharedPointer<IndirectInterface> indirectInterface,
        QSharedPointer<Component> component, LibraryInterface* library, QWidget* parent);

	//! The destructor.
	virtual ~SingleIndirectInterfaceEditor();

    virtual void refresh();
    
protected:

    void showEvent(QShowEvent* event);

private slots:

    void onIndirectAddressChanged();

    void onIndirectDataChanged();

    void onBitsInLauChanged();

    void onEndiannessChanged();

    void onMemoryMapChanged();

private:

	// Disable copying.
	SingleIndirectInterfaceEditor(SingleIndirectInterfaceEditor const& rhs);    
    SingleIndirectInterfaceEditor& operator=(SingleIndirectInterfaceEditor const& rhs);

   
    QStringList findAvailableReferences() const;

    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    QSharedPointer<IndirectInterface> indirectInterface_;
    
    QSharedPointer<Component> component_;

    NameGroupEditor* nameEditor_;

    ReferenceSelector* addressSelector_;
    
    ReferenceSelector* dataSelector_;

    QLineEdit*  bitsInLauEditor_;

    QComboBox* endiannessSelector_;

    ReferenceSelector* memoryMapSelector_;

    BridgesEditor* transparentBridgesEditor_;
};

#endif // SINGLEINDRECTINTERFACEEDITOR_H
