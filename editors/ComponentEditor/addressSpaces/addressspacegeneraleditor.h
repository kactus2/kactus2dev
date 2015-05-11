//-----------------------------------------------------------------------------
// File: addressspacegeneraleditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.02.2012
//
// Description:
// Editor to set the general settings of an address space.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEGENERALEDITOR_H
#define ADDRESSSPACEGENERALEDITOR_H

#include <IPXACTmodels/addressspace.h>

#include <common/widgets/NumberLineEdit/numberlineedit.h>

#include <QGroupBox>
#include <QSharedPointer>

class ExpressionEditor;
class ExpressionParser;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! Editor to set the general settings of an address space.
//-----------------------------------------------------------------------------
class AddressSpaceGeneralEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*! The constructor
	 *
     *       @param [in] addrSpace          The address space being edited.
     *       @param [in] parameterFinder    The finder for parameters available in expressions.
     *       @param [in] expressionParser   The expression parser.
	 *       @param [in] parent             The owner of the editor.
	*/
	AddressSpaceGeneralEditor(QSharedPointer<AddressSpace> addrSpace, 
        QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionParser> expressionParser,
        QWidget *parent);

	//! The destructor
	virtual ~AddressSpaceGeneralEditor();

	/*! Check if the editor is in valid state and can be saved.
	 *
	 *      @return True if everything is ok.
	*/
	bool isValid() const;

	/*! Read the settings from the address space model to the editor fields.
	 *
	*/
	void refresh();

signals:

	//! Emitted when the contents of one of the editor fields changes.
	void contentChanged();

    //! Emitted when the change in fields should be reflected in the visualization.
    void graphicsChanged();

	/*! Emitted when user changes the size of an addressable unit.
	 *
	 *       @param units The new addressable unit size.
	*/
	void addressableUnitsChanged(int units);

	/*! Emitted when user changes the width of a row in address space.
	 *
	 *       @param width The new width of a row.
	*/
	void widthChanged(int width);

	/*! Emitted when user changes the range of an address space.
	 *
	 *       @param range The new range as number of addressable units.
	*/
	void rangeChanged(QString const& range);
    
    /*!
     *  Increase the amount of references to a parameter with a matching id.
     *
     *      @param [in] id      Id of the parameter, whose references are being increased.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to a parameter with a matching id.
     *
     *      @param [in] id      Id of the parameter, whose references are being increased.
     */
    void decreaseReferences(QString id);

private slots:

	//! Handler for changes in addressable unit.
    void onAddrUnitChanged();

	//! Handler for changes in width.
    void onWidthChanged();

	//! Handler for changes in range.
    void onRangeChanged();

private:
	//! No copying
	AddressSpaceGeneralEditor(const AddressSpaceGeneralEditor& other);
	AddressSpaceGeneralEditor& operator=(const AddressSpaceGeneralEditor& other);
   
    
    /*!
     *  Formats a given expression to human-readable format.
     *
     *      @param [in] expression   The expression to format.
     *
     *      @return The formatted expression.
     */
    QString format(QString const& expression) const;
	
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The address space being edited.
    QSharedPointer<AddressSpace> addrSpace_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

	//! Editor to set the size of an addressable unit.
	NumberLineEdit addrUnitEditor_;

    //! Editor to set the range of the address space.
    ExpressionEditor* rangeEditor_;

	//! Editor to set the width of one row in address space.
	ExpressionEditor* widthEditor_;
};

#endif // ADDRESSSPACEGENERALEDITOR_H
