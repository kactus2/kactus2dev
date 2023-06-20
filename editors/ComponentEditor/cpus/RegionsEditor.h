
#ifndef REGIONSEDITOR_H
#define REGIONSEDITOR_H

#include "RegionsModel.h"

#include <common/views/EditableTableView/editabletableview.h>
#include <editors/common/ExpressionSet.h>

#include <QSharedPointer>
#include <QGroupBox>

class LibraryInterface;
class ExpressionParser;
class ExpressionFormatter;
class Component;
class AddressSpace;
class Region;
//-----------------------------------------------------------------------------
//! The editor to edit the regions of an address space.
//-----------------------------------------------------------------------------
class RegionsEditor : public QGroupBox
{
	Q_OBJECT

public:

    /*!
     *  The constructor
     *
     *      @param [in] regions                 The edited regions.
     *      @param [in] componentPath           The path to component xml file.
     *      @param [in] parameterFinder         Finder for available parameter names.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] parent                  Pointer to the owner of this editor.
	 */
	RegionsEditor(QSharedPointer<QList<QSharedPointer<Region> > > regions,
        QString const& componentPath,
		ExpressionSet expressions,
		QWidget *parent);
	
	//! The destructor.
	virtual ~RegionsEditor() = default;


    //! No copying. No assignment.
    RegionsEditor(const RegionsEditor& other) = delete;
    RegionsEditor& operator=(const RegionsEditor& other) = delete;

	/*!
     *  Read the settings from the address space to the editor.
	 */
	void refresh();

signals:

	//! Emitted when the contents of the editor change.
	void contentChanged();

	//! Print an error message to the user.
	void errorMessage(const QString& msg);

	//! Print a notification to the user.
	void noticeMessage(const QString& msg);

	//! Emitted when a new region is added to the address space.
	void regionAdded(QSharedPointer<Region> region);

	//! Emitted when a region is removed from the address space.
	void regionRemoved(const QString& regionName);

	//! Emitted when a region is renamed.
	void regionRenamed(const QString& oldName, const QString& newName);

	//! Emitted when the range or offset of a region has changed.
	void regionChanged(QSharedPointer<Region> region);

    /*!
     *  Increase the amount of references to a parameter with a matching id.
     *
     *      @param [in] id      Id of the parameter, whose references are being increased.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the amount of references to a parameter with a matching id.
     *
     *      @param [in] id      Id of the parameter, whose references are being increased.
     */
    void decreaseReferences(QString const& id);

private:
	//! The view to display the regions.
	EditableTableView view_;

	//! The model that contains the regions.
	RegionsModel model_;
};

#endif // REGIONSEDITOR_H
