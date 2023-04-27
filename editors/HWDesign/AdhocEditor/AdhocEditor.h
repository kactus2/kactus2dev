//-----------------------------------------------------------------------------
// File: AdhocEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 27.04.2023
//
// Description:
// Editor for ad-hoc port visibilities and tie off values.
//-----------------------------------------------------------------------------

#ifndef ADHOC_EDITOR_H
#define ADHOC_EDITOR_H

#include "AdhocPortEditor.h"
#include "AdHocVisibilityEditor.h"

#include <KactusAPI/include/ParameterFinder.h>

#include <QWidget>
#include <QSharedPointer>

class AdHocItem;
class HWDesignDiagram;

//-----------------------------------------------------------------------------
//! Editor for ad-hoc port visibilities and tie off values.
//-----------------------------------------------------------------------------
class AdhocEditor : public QWidget
{
    Q_OBJECT
public:

    //! The constructor.
    AdhocEditor(QSharedPointer<ParameterFinder> designFinder, QWidget* parent = nullptr);


    //! The destructor.
    virtual ~AdhocEditor() = default;

    //! No copying.
    AdhocEditor(const AdhocEditor& other) = delete;

    //! No assignment.
    AdhocEditor& operator=(const AdhocEditor& other) = delete;

    /*!
     *  Sets the ad-hoc ports visibility data source.
     *
     *      @param [in] dataSource      The data source.
     *      @param [in] design          The design containing the data source.
     *      @param [in] editProvider    The edit provider to use for undo/redo.
     *      @param [in] lockEditor      If true, the contents cannot be edited.
     */
    void setVisibilityData(AdHocEnabled* dataSource, QSharedPointer<Design> design, 
        QSharedPointer<IEditProvider> editProvider, bool lockEditor);

    /*!
     *  Hides all information in the port visibility section.
     */
    void clearVisibilityData();

    /*!
     *  Set the ad hoc port data to display.
     *
     *      @param [in] endPoint            The selected ad hoc port.
     *      @param [in] containingDiagram   The design diagram containing the ad hoc port.
     *      @param [in] editProvider        The provider for undo commands.
     */
    void setPortData(AdHocItem* endPoint, HWDesignDiagram* containingDiagram, QSharedPointer<IEditProvider> editProvider);

    /*!
     *  Hides all information in the port section.
     */
    void clearPortData();

signals:

    /*!
     *  Informs of a change in the contents.
     */
    void contentChanged();

    /*!
     *  Informs of new references made to the parameter with the selected ID.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     */
    void increaseReferences(QString const& parameterID);

    /*!
     *  Informs of removing references from the parameter with the selected ID.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     */
    void decreaseReferences(QString const& parameterID);

private:

    //! Setup the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    AdHocVisibilityEditor visibilityEditor_;

    AdhocPortEditor portEditor_;

};


#endif // ADHOC_EDITOR_H
