//-----------------------------------------------------------------------------
// File: TopAdHocVisibilityChangeCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.10.2017
//
// Description:
// Undo command for changing port adhoc visibility in top component of design diagram.
//-----------------------------------------------------------------------------

#ifndef TOPADHOCVISIBILITYCHANGECOMMAND_H
#define TOPADHOCVISIBILITYCHANGECOMMAND_H

#include <designEditors/HWDesign/undoCommands/AdHocVisibilityChangeCommand.h>

class HWDesignDiagram;
class GraphicsColumnLayout;

//-----------------------------------------------------------------------------
//! Undo command for changing port adhoc visibility in top component of design diagram.
//-----------------------------------------------------------------------------
class TopAdHocVisibilityChangeCommand : public AdHocVisibilityChangeCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] dataSource    The ad-hoc enabled data source.
     *      @param [in] portName      The name of the port.
     *      @param [in] newVisiblity  The new ad-hoc visibility of the port.
     *      @param [in] parent        The parent undo command.
     */
    TopAdHocVisibilityChangeCommand(HWDesignDiagram* dataSource, QString const& portName, bool newVisibility,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~TopAdHocVisibilityChangeCommand();

private:

    // Disable copying.
    TopAdHocVisibilityChangeCommand(TopAdHocVisibilityChangeCommand const& rhs);
    TopAdHocVisibilityChangeCommand& operator=(TopAdHocVisibilityChangeCommand const& rhs);

    /*!
     *  Setup a command for the deleting of the tie off value connection of the selected port item.
     *
     *      @param [in] diagram     Design diagram containing the ad hoc port.
     *      @param [in] portName    Name of the selected port item.
     */
    void setupTieOffConnectionForDeletion(HWDesignDiagram* diagram, QString const& portName);

    /*!
     *  Setup the addition of an ad hoc port column.
     *
     *      @param [in] diagram     Design diagram containing the ad hoc port.
     */
    void setupColumnAddition(HWDesignDiagram* diagram);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};

#endif // TOPADHOCVISIBILITYCHANGECOMMAND_H
