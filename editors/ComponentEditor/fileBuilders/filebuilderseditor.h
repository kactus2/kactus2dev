//-----------------------------------------------------------------------------
// File: filebuilderseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.05.2012
//
// Description:
// Editor to edit file builders.
//-----------------------------------------------------------------------------

#ifndef FILEBUILDERSEDITOR_H
#define FILEBUILDERSEDITOR_H

#include "filebuildersmodel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QObject>

class FileBuilder;
class ParameterFinder;
class ExpressionFormatter;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! Editor to edit file builders.
//-----------------------------------------------------------------------------
class FileBuildersEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] fileBuilders            QList that contains the file builders to edit.
     *      @param [in] parameterFinder         Finder used to identify parameters.
     *      @param [in] expressionParser        Parser used to calculate expressions.
     *      @param [in] expressionFormatter     Formatter used to format expressions.
	 *      @param [in] parent                  Pointer to the owner of this editor.
	 */
    FileBuildersEditor(QSharedPointer<QList<QSharedPointer<FileBuilder> > > fileBuilders,
        QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget* parent);

	//! The destructor.
	virtual ~FileBuildersEditor();

	/*!
     *  Check for the validity of the edited file builders.
	 *
	 *      @return True if all file builders are in valid state.
	 */
	virtual bool isValid() const;

public slots:

	//! Refresh the contents to display.
	void refresh();

signals:

	//! Emit an error message to user.
	void errorMessage(const QString& msg);

	//! Emit a notice to user.
	void noticeMessage(const QString& msg);

	//! Inform that contents of this editor have changed.
	void contentChanged();

    /*!
     *  Increase the number of references in the selected parameter.
     *
     *      @param [in] id  Id of the selected parameter.
     */
    void increaseReferences(QString id) const;
    
    /*!
     *  Decrease the number of references in the selected parameter.
     *
     *      @param [in] id  Id of the selected parameter.
     */
    void decreaseReferences(QString id) const;

private:

	//! No copying. No assignment.
	FileBuildersEditor(const FileBuildersEditor& other);
	FileBuildersEditor& operator=(const FileBuildersEditor& other);

	//! The view that displays the parameters.
	EditableTableView view_;

	//! The model that holds the data to be displayed to the user.
    FileBuildersModel* model_;

	//! \brief Pointer to the proxy that is used to sort the view.
	QSortFilterProxyModel proxy_;
};

#endif // FILEBUILDERSEDITOR_H
