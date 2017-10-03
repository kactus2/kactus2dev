//-----------------------------------------------------------------------------
// File: ParameterReferenceTreeWindow.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.09.2018
//
// Description:
// The window containing the parameter reference tree.
//-----------------------------------------------------------------------------

#ifndef PARAMETERREFERENCETREEWINDOW_H
#define PARAMETERREFERENCETREEWINDOW_H

#include <QDialog>
#include <QPushButton>

class ExpressionFormatter;
class ParameterReferenceTree;

//-----------------------------------------------------------------------------
//! The window containing the parameter reference tree.
//-----------------------------------------------------------------------------
class ParameterReferenceTreeWindow : public QDialog
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parameterReferenceTree  Tree containing the references made to the selected parameter.
     *      @param [in] parent                  Parent widget.
     */
    ParameterReferenceTreeWindow(ParameterReferenceTree* parameterReferenceTree, QWidget* parent = 0);

	/*!
     *  The destructor.
     */
    ~ParameterReferenceTreeWindow();

public slots:

    /*!
     *  Handles the creation and opening of the parameter reference tree.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] parameterName   Name of the selected parameter.
     */
    void openReferenceTree(QString const& parameterID, QString const& parameterName);

private slots:

    /*!
     *  Handles the expansion of all the items contained within the reference tree.
     */
    void onExpandAll();

    /*!
     *  Handles the collapsing of all the items contained within the reference tree.
     */
    void onCollapseAll();

private:
    
	// Disable copying.
    ParameterReferenceTreeWindow(ParameterReferenceTreeWindow const& rhs);
    ParameterReferenceTreeWindow& operator=(ParameterReferenceTreeWindow const& rhs);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    /*!
     *  Connect the necessary signals to slots.
     */
    void connectItems();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The tree containing the references to the selected parameter.
    ParameterReferenceTree* referenceTree_;

    //! Button for expanding all the items in the tree.
    QPushButton* expandAllButton_;

    //! Button for collapsing all the items in the tree.
    QPushButton* collapseAllButton_;

    //! Button for closing the tree window.
    QPushButton* closeButton_;
};

#endif // PARAMETERREFERENCETREEWINDOW_H
