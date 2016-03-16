//-----------------------------------------------------------------------------
// File: ComponentInstantiationDisplayer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.01.2016
//
// Description:
// Display widget for component instantiations.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANTIATIONDISPLAYER_H
#define COMPONENTINSTANTIATIONDISPLAYER_H

#include <QLabel>
#include <QGroupBox>

class ComponentInstantiation;

//-----------------------------------------------------------------------------
//! Display widget for component instantiations.
//-----------------------------------------------------------------------------
class ComponentInstantiationDisplayer : public QGroupBox
{

public:

    /*!
     *  Constructor.
     *
     *      @param [in] parent          The parent widget.
     */
	ComponentInstantiationDisplayer(QWidget* parent);

    //! Destructor.
    virtual ~ComponentInstantiationDisplayer();

	//! Reload the information from the model to the editor.
	virtual void refresh();

    //! Sets the instantiation whose information to display.
    void setInstantiation(QSharedPointer<ComponentInstantiation> instantiation);

private:
    // Disable copying.
    ComponentInstantiationDisplayer(ComponentInstantiationDisplayer const& rhs);
    ComponentInstantiationDisplayer& operator=(ComponentInstantiationDisplayer const& rhs);

    //! Setup the layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design instantiation being edited.
    QSharedPointer<ComponentInstantiation> instantiation_;

    //! Text field for instantiation language.
    QLabel* languageLabel_;

    //! Text field for instantiation library.
    QLabel* libraryLabel_;

    //! Text field for instantiation package.
    QLabel* packageLabel_;

    //! Text field for instantiation module name.
    QLabel* moduleNameLabel_;

    //! Text field for instantiation architecture name.
    QLabel* architectureLabel_;

    //! Text field for instantiation configuration name.
    QLabel* configurationLabel_;

    //! Text field for instantiation file set references.
    QLabel* filesetsLabel_;
};

//-----------------------------------------------------------------------------

#endif // COMPONENTINSTANTIATIONDISPLAYER_H
