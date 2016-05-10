//-----------------------------------------------------------------------------
// File: AdHocVisibilityEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.2.2012
//
// Description:
// Ad-hoc visibility editor.
//-----------------------------------------------------------------------------

#ifndef ADHOCVISIBILITYEDITOR_H
#define ADHOCVISIBILITYEDITOR_H

#include "AdHocVisibilityModel.h"

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>
#include <common/widgets/nameGroupBox/namegroupbox.h>
#include <common/GenericEditProvider.h>

#include <QWidget>
#include <QLabel>
#include <QSharedPointer>
#include <QTableWidget>

class AdHocEnabled;
class ComponentItem;
class IEditProvider;
class AdHocVisibilityPolicy;
class Design;

//-----------------------------------------------------------------------------
//! Editor to edit the details of a component instance within a design.
//-----------------------------------------------------------------------------
class AdHocVisibilityEditor : public QWidget
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
	AdHocVisibilityEditor(QWidget *parent);
	
	/*!
     *  Destructor.
     */
	virtual ~AdHocVisibilityEditor();

	/*!
	 *  Sets the ad-hoc ports visibility data source.
	 *
	 *      @param [in] dataSource      The data source.
     *      @param [in] design          The design containing the data source.
	 *      @param [in] editProvider    The edit provider to use for undo/redo.
	 *      @param [in] lockEditor      If true, the contents cannot be edited.
	 */
	void setDataSource(AdHocEnabled* dataSource, QSharedPointer<Design> design,
        QSharedPointer<IEditProvider> editProvider, bool lockEditor);

public slots:
    /*!
     *  Hides all information in the editor.
     */
	void clear();

    /*!
     *  Called when the data source contents have changed.
     */
    void onContentChanged();

private slots:

private:
	// Disable copying.
	AdHocVisibilityEditor(AdHocVisibilityEditor const& rhs);
	AdHocVisibilityEditor& operator=(AdHocVisibilityEditor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The data source being edited.
	AdHocEnabled* dataSource_;

    //! The handler for ad hoc port visibilities.
    QSharedPointer<AdHocVisibilityPolicy> visibilityPolicy_;

    //! The name label.
    QLabel nameLabel_;

	//! The port ad-hoc visibility table.
    QTableView portAdHocTable_;

    //! The ad-hoc model.
    AdHocVisibilityModel adHocModel_;
};

#endif // ADHOCVISIBILITYEDITOR_H
