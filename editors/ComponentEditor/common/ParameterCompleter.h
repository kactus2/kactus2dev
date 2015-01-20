//-----------------------------------------------------------------------------
// File: ParameterCompleter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.01.2015
//
// Description:
// Completer for selecting parameter references.
//-----------------------------------------------------------------------------

#ifndef PARAMETERCOMPLETER_H
#define PARAMETERCOMPLETER_H

#include <QCompleter>
#include <QSharedPointer>
#include <QObject>

//-----------------------------------------------------------------------------
//! Completer for selecting parameter references.
//-----------------------------------------------------------------------------
class ParameterCompleter : public QCompleter
{
    Q_OBJECT
public:

	//! The constructor.
	ParameterCompleter(QObject* parent);

	//! The destructor.
	virtual ~ParameterCompleter();

    /*!
     *  Sets the model for the completer and initializes the popup view.
     *
     *      @param [in] model   The model to set.
     */
    void setModel(QAbstractItemModel* model); 


private:

	// Disable copying.
	ParameterCompleter(ParameterCompleter const& rhs);
	ParameterCompleter& operator=(ParameterCompleter const& rhs);

    //! Sets up the pop up view.
    void setupPopup();
};

#endif // PARAMETERCOMPLETER_H
