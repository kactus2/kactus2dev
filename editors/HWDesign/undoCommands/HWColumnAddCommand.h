//-----------------------------------------------------------------------------
// File: HWColumnAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.10.2017
//
// Description:
// Undo command for adding HW graphics columns.
//-----------------------------------------------------------------------------

#ifndef HWCOLUMNADDCOMMAND_H
#define HWCOLUMNADDCOMMAND_H

#include <common/graphicsItems/GraphicsColumnAddCommand.h>

#include <editors/HWDesign/HierarchicalPortItem.h>

class Kactus2Group;
class Kactus2Placeholder;

//-----------------------------------------------------------------------------
//! Undo command for adding HW graphics columns.
//-----------------------------------------------------------------------------
class HWColumnAddCommand : public GraphicsColumnAddCommand
{
public:

    /*!
     *  Constructor.
     *
     *    @param [in] layout      The column layout.
     *    @param [in] column      The column to add.
     *    @param [in] diagram     Diagram containing the column layout.
     *    @param [in] parent      The parent command.
     */
    HWColumnAddCommand(GraphicsColumnLayout* layout, GraphicsColumn* column, DesignDiagram* diagram,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~HWColumnAddCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
    HWColumnAddCommand(HWColumnAddCommand const& rhs);
    HWColumnAddCommand& operator=(HWColumnAddCommand const& rhs);

    /*!
     *  Get the mandatory port items that have not been inserted into any columns.
     *
     *    @return The mandatory port items that have not been inserted into any columns.
     */
    QVector<HierarchicalPortItem*> getMissingAdHocPortItems() const;

    /*!
     *  Get the mandatory ports that have not been inserted into any columns.
     *
     *    @return The mandatory ports that have not been inserted into any column
     */
    QVector<QSharedPointer<Port> > getMissingAdHocPorts() const;

    /*!
     *  Remove the mandatory ad hoc ports that have been inserted in the column.
     */
    void removeMissingPortsFromDesign();

    /*!
     *  Add the missing mandatory ad hoc ports to the column.
     */
    void addMissingPortsToNewColumn();

    /*!
     *  Check if the selected port exists in the design diagram.
     *
     *    @param [in] adHocPort   The selected ad hoc port.
     *
     *    @return True, if the port is found in the design diagram, otherwise false.
     */
    bool portExistsInDiagram(QSharedPointer<Port> adHocPort) const;

    /*!
     *  Get the position vendor extension for the selected ad hoc port.
     *
     *    @param [in] positionGroup   Extension group for ad hoc positions.
     *    @param [in] adhocPort       The selected ad hoc port.
     */
    QSharedPointer<Kactus2Placeholder> getAdHocPositionExtension(QSharedPointer<Kactus2Group> positionGroup,
        QSharedPointer<Port> adhocPort) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The missing mandatory ad hoc port items.
    QVector<HierarchicalPortItem*> missingAdHocPortItems_;
};

//-----------------------------------------------------------------------------

#endif // HWCOLUMNADDCOMMAND_H