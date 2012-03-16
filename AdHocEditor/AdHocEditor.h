//-----------------------------------------------------------------------------
// File: AdHocEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.2.2012
//
// Description:
// Ad-hoc visibility editor.
//-----------------------------------------------------------------------------

#ifndef ADHOCEDITOR_H
#define ADHOCEDITOR_H

#include "AdHocModel.h"

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>
#include <common/widgets/nameGroupBox/namegroupbox.h>
#include <common/GenericEditProvider.h>

#include <QWidget>
#include <QLabel>
#include <QSharedPointer>
#include <QTableWidget>

class ComponentItem;
class AdHocEnabled;

/*! \brief Editor to edit the details of a component instance within a design.
 *
 */
class AdHocEditor : public QWidget
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
	AdHocEditor(QWidget *parent);
	
	/*!
     *  Destructor.
     */
	virtual ~AdHocEditor();

	/*!
     *  Sets the ad-hoc ports visibility data source.
     *
     *      @param [in] component The data source.
     */
	void setDataSource(AdHocEnabled* dataSource);

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
	AdHocEditor(AdHocEditor const& rhs);
	AdHocEditor& operator=(AdHocEditor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The data source being edited.
	AdHocEnabled* dataSource_;

    //! The name label.
    QLabel nameLabel_;

	//! The port ad-hoc visibility table.
    QTableView portAdHocTable_;

    //! The ad-hoc model.
    AdHocModel adHocModel_;
};

#endif // ADHOCEDITOR_H
