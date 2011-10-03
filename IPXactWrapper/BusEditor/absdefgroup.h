/* 
 *  	Created on: 15.8.2011
 *      Author: Antti Kamppi
 * 		filename: absdefgroup.h
 *		Project: Kactus 2
 */

#ifndef ABSDEFGROUP_H
#define ABSDEFGROUP_H

#include "busportsview.h"
#include "busportsmodel.h"

#include <models/abstractiondefinition.h>

#include <QGroupBox>
#include <QPushButton>
#include <QSharedPointer>

class AbsDefGroup : public QGroupBox {
	Q_OBJECT

public:

	AbsDefGroup(QWidget *parent);
	
	//! \brief The destructor
	virtual ~AbsDefGroup();

	void save();

	/*! \brief Set the abstraction definition for the editor.
	 *
	 * \param absDef Pointer to the Abstraction definition
	 *
	*/
	void setAbsDef(QSharedPointer<AbstractionDefinition> absDef);

signals:

	//! \brief Emitted when contents of this editor changes.
	void contentChanged();

	//! \brief Emitted when an error should be printed to user.
	void errorMessage(const QString& message);

	//! \brief Emitted when a notification should be printed to user.
	void noticeMessage(const QString& message);

private slots:

	//! \brief Handler for the import button clicks.
	void onImport();

	//! \brief Handler for the export button clicks.
	void onExport();

	//! \brief Handler for add new signal options button clicks
	void onAddSignalOptions();

private:
	//! \brief No copying
	AbsDefGroup(const AbsDefGroup& other);

	//! \brief No assignment
	AbsDefGroup& operator=(const AbsDefGroup& other);

	void setupLayout();

	//! \brief Button to add new signals
	QPushButton newSignalButton_;

	//! \brief Button to add new signal with options
	QPushButton newSignalOptionsButton_;

	//! \brief Button to import a csv-file
	QPushButton importButton_;

	//! \brief Button to export a csv-file
	QPushButton exportButton_;

	//! \brief The table view to display the logical signals
	BusPortsView portView_;

	//! \brief The model that contains the logical signals of Abstraction Definition.
	BusPortsModel portModel_;
};

#endif // ABSDEFGROUP_H
