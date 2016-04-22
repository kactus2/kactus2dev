//-----------------------------------------------------------------------------
// File: ValueOrIndexedValueEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.04.2016
//
// Description:
// Editor for changing values in items that can be given as a single value or array.
//-----------------------------------------------------------------------------

#ifndef VALUEORINDEXEDVALUEEDITOR_H
#define VALUEORINDEXEDVALUEEDITOR_H

#include <QTableView>
#include <QCompleter>

class ArrayView;
class ExpressionEditor;
class ParameterFinder;
class ExpressionParser;
class ExpressionFormatter;
class ComponentParameterModel;

//-----------------------------------------------------------------------------
//! Editor for changing values in items that can be given as a single value or array.
//-----------------------------------------------------------------------------
class ValueOrIndexedValueEditor : public QTableView
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] finder      The used parameter finder.
     *      @param [in] parser      The used expression parser.
     *      @param [in] formatter   The used expression formatter.
     *      @param [in] parent      The parent widget of this table.
     */
    ValueOrIndexedValueEditor(QSharedPointer<ParameterFinder> finder, QSharedPointer<ExpressionParser> parser,
        QSharedPointer<ExpressionFormatter> formatter, QWidget* parent);

    /*!
     *  The destructor.
     */
    ~ValueOrIndexedValueEditor();

    /*!
     *  Setup the array editor.
     *
     *      @param [in] value       The current value of the remap condition.
     *      @param [in] leftBound   The left bound of the array.
     *      @param [in] rightBound  The right bound of the array.
     */
    void setupArrayEditor(QString const& value, int const& leftBound, int const& rightBound);

    /*!
     *  Get the value of the editor.
     *
     *      @return A single value, if the single editor was edited. An array if the array was edited.
     */
    QString getValueData() const;
    
    /*!
     *  The event filter for this widget.
     *
     *      @param [in] filterObject    The object to be filtered.
     *      @param [in] filterEvent     The event to be filtered.
     */
    virtual bool eventFilter(QObject* filterObject, QEvent* filterEvent);

signals:
    
    /*!
     *  Increase the amount of references to a parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to a parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void decreaseReferences(QString id);

    /*!
     *  Content has been changed in the editor.
     */
    void contentChanged();

private slots:

    /*!
     *  Handler for editing the single value of a remap condition.
     */
    void onSingleValueEdited();

    /*!
     *  Handler for editing the array values of a remap condition.
     */
    void onDataChangeInArrayView();

private:
    //! Disable copying and assignment.
    ValueOrIndexedValueEditor(const ValueOrIndexedValueEditor& other);
    ValueOrIndexedValueEditor& operator=(const ValueOrIndexedValueEditor& other);

    /*!
     *  Setup the single value editor.
     */
    void setupSingleValueEditor();

    /*!
     *  Create a parameter name completer.
     *
     *      @param [in] completerParent     The parent for the completer.
     *
     *      @return The created parameter name completer.
     */
    QCompleter* createParameterNameCompleter(QObject* completerParent) const;

    /*!
     *  Setup the layout for this widget.
     */
    void setupLayout();

    /*!
     *  Calculate the formatted value for the selected expression.
     *
     *      @param [in] expression  The selected expression.
     *
     *      @return The formatted value of the selected expression.
     */
    QString formattedValueFor(QString const& expression) const;

    /*!
     *  Set the value for the single value editor.
     *
     *      @param [in] value   The new value.
     */
    void setValueToSingleEditor(QString const& value);

    /*!
     *  Check if the array contains values.
     *
     *      @param [in] arrayContents   The contents of the array.
     *
     *      @return True, if the array contains any values, false otherwise.
     */
    bool arrayContainsValues(QString const& arrayContents) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The single remap condition value editor.
    ExpressionEditor* singleValueEditor_;

    //! The array editor for bit-by-bit remap condition.
    ArrayView* arrayView_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The used parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! The used expression formatter.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! The parameter model for the expression editors.
    ComponentParameterModel* parameterModel_;
};

#endif // VALUEORINDEXEDVALUEEDITOR_H