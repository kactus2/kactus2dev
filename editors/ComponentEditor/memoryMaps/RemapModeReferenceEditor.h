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
#include <common/widgets/GroupBoxWithAlertSign/GroupBoxWithAlertSign.h>

#include <QGroupBox>

class ModeReferenceEditor;
class MemoryRemapModeReferenceModel;


class RemapModeReferenceEditor : public GroupBoxWithAlertSign
{
    Q_OBJECT

public:

    /*!
     *	The constructor.
     *  
     *    @param [in] modeRefInterface    The mode reference interface to use.
     *    @param [in] parent              The parent widget.
     */
    RemapModeReferenceEditor(ModeReferenceInterface* modeRefInterface, QWidget* parent, bool isRemap);

    virtual ~RemapModeReferenceEditor() = default;

    //! No copying.
    RemapModeReferenceEditor(RemapModeReferenceEditor& other) = delete;
    RemapModeReferenceEditor& operator=(RemapModeReferenceEditor& other) = delete;

signals:

    /*!
     *	Emitted whenever a mode reference has been edited.
     */
    void contentChanged();
private slots:

    /*!
     *	Checks if it is needed to display alert sign in GroupBoxWithAlertSign
     */
    void checkForAlert();

private:

    //! The mode reference table editor.
    ModeReferenceEditor* modeReferencesEditor_;

    //! The mode reference interface.
    ModeReferenceInterface* modeRefInterface_;
};

#endif // REMAPMODEREFERENCEEDITOR_H
