//-----------------------------------------------------------------------------
// File: parametergroupbox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 23.2.2012
//
// Description:
// Group box that can be used to edit list of parameters.
//-----------------------------------------------------------------------------

#ifndef PARAMETERGROUPBOX_H
#define PARAMETERGROUPBOX_H

#include <IPXACTmodels/parameter.h>

#include <QGroupBox>
#include <QList>
#include <QSharedPointer>

class ColumnFreezableTable;
class Component;
class Choice;
class ParameterFinder;
class ParametersModel;
class ExpressionFormatter;

//-----------------------------------------------------------------------------
//! Group box that can be used to edit list of parameters.
//-----------------------------------------------------------------------------
class ParameterGroupBox : public QGroupBox 
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in]  parameters              The list containing the parameters.
	 *      @param [in]  component               The containing component whose parameters are being edited.
	 *      @param [in]  parameterFinder         The parameter finder used to search the components parameters.
	 *      @param [in]  expressionFormatter     The expression formatter.
	 *      @param [in]  parent                  Pointer to the owner of this editor.
	 */
	ParameterGroupBox(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters, 
        QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QWidget *parent);
	
	//! The destructor
	virtual ~ParameterGroupBox();

	/*! Check for the validity of the edited parameters
	*
	* \return True if all parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! Check for the validity of the edited parameters.
	*
	*       @param [inout] errorList          The list to add the possible error messages to.
	*       @param [in]    parentIdentifier   String from parent to help to identify the location of the error.
	*
	*       @return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, const QString& parentIdentifier) const;

	//! Restore the changes made in the editor back to ones in parameter models.
	virtual void refresh();

signals:

	//! Emitted when contents of the editor change.
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

    /*!
     *  Increase the amount of references to the parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to the parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void decreaseReferences(QString id);

    /*!
     *  Open the reference tree of the parameter with the id.
     *
     *      @param [in] id      The id of the parameter.
     */
    void openReferenceTree(QString const& id) const;

private:
	//! No copying
	ParameterGroupBox(const ParameterGroupBox& other);

	//! No assignment
	ParameterGroupBox& operator=(const ParameterGroupBox& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The view that displays the parameters.
    ColumnFreezableTable* view_;

	//! The model that holds the data to be displayed to the user
	ParametersModel* model_;
};

#endif // PARAMETERGROUPBOX_H
