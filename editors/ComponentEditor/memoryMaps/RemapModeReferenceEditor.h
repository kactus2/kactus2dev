//-----------------------------------------------------------------------------
// File: RemapModeReferenceEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 17.10.2023
//
// Description:
// Editor for remap mode references.
//-----------------------------------------------------------------------------

#ifndef REMAPMODEREFERENCEEDITOR_H
#define REMAPMODEREFERENCEEDITOR_H

#include <KactusAPI/include/MemoryMapInterface.h>

#include <QGroupBox>

class ModeReferenceEditor;
class MemoryRemapModeReferenceModel;

class RemapModeReferenceEditor : public QGroupBox
{
    Q_OBJECT

public:

    /*!
     *	The constructor.
     *  
     *      @param [in] model      The mode reference model to use.
     *      @param [in] parent     The parent widget.
     */
    RemapModeReferenceEditor(MemoryRemapModeReferenceModel* model, QWidget* parent);

    virtual ~RemapModeReferenceEditor() = default;

    //! No copying.
    RemapModeReferenceEditor(RemapModeReferenceEditor& other) = delete;
    RemapModeReferenceEditor& operator=(RemapModeReferenceEditor& other) = delete;

signals:

    /*!
     *	Emitted whenever a mode reference has been edited.
     */
    void contentChanged();

private:

    //! The mode reference table editor.
    ModeReferenceEditor* modeReferencesEditor_;

};

#endif // REMAPMODEREFERENCEEDITOR_H
