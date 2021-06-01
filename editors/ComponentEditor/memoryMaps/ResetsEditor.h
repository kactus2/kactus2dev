//-----------------------------------------------------------------------------
// File: ResetsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.01.2019
//
// Description:
// Editor for editing the resets of a field.
//-----------------------------------------------------------------------------

#ifndef RESETSEDITOR_H
#define RESETSEDITOR_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QGroupBox>

class ExpressionParser;
class ParameterFinder;
class ExpressionFormatter;

class EditableTableView;
class ResetsModel;

class FieldReset;
class FieldValidator;
class ResetType;

class ResetInterface;

class Field;

//-----------------------------------------------------------------------------
//! Editor for editing the resets of a field.
//-----------------------------------------------------------------------------
class ResetsEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] resetInterface      Interface for resets.
     *      @param [in] resetTypes          List of reset types within the containing component.
     *      @param [in] expressionParser    The expression parser.
     *      @param [in] parameterFinder     Component parameter finder.
     *      @param [in] containingField     Field containing the resets.
	 *      @param [in] parent              Pointer to the parent of this editor.
	 */
    ResetsEditor(ResetInterface* resetInterface, QSharedPointer<QList<QSharedPointer<ResetType> > > resetTypes,
        QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<Field> containingField, QWidget* parent = 0);

	/*!
	 *  The destructor.
	 */
    virtual ~ResetsEditor();

	/*!
	 *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:

    /*!
     *  Informs of changes to the component editor tree.
     */
    void contentChanged();

    /*!
     *  Increase the number of references made to the selected parameter.
     *
     *      @param [in] id  ID of the selected parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the number of references made to the selected parameter.
     *
     *      @param [in] id  ID of the selected parameter.
     */
    void decreaseReferences(QString const& id);

private:
	
	//! No copying. No assignment.
	ResetsEditor(const ResetsEditor& other);
    ResetsEditor& operator=(const ResetsEditor& other);

	//! The view to display the resets.
	EditableTableView* resetsView_;

	//! The proxy that does the sorting of resets.
	QSortFilterProxyModel* resetsProxy_;

	//! The model that manages the resets.
    ResetsModel* resetsModel_;

    //! Field containing the resets.
    QSharedPointer<Field> containingField_;

    //! Interface for accessing resets.
    ResetInterface* interface_;
};

#endif // RESETSEDITOR_H
