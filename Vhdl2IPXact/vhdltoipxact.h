//-----------------------------------------------------------------------------
// File: VHDLtoIPXACT.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Timo Korpela
// Date: 26.05.2013
//
// Description:
// header file introducing VHDLtoIPXACT class
//-----------------------------------------------------------------------------

#ifndef VHDLTOIPXACT_H
#define VHDLTOIPXACT_H
#include "textedit.h"
#include "vhdlanalysis.h"
#include <models/component.h>
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QDialog>

//-----------------------------------------------------------------------------
//! Class to display vhdl files port and parameter analysis.
//-----------------------------------------------------------------------------

class VHDLtoIPXACT : public QDialog
{
    Q_OBJECT
    
public:
    VHDLtoIPXACT(QSharedPointer<Component> comp, const QString xmlFilePath,
             QWidget *parent = 0);
    ~VHDLtoIPXACT();

    /*!
     *  Display window with vhdl file display and tables for ports and parameters
     *  from vhdl file.
     *
     */

private slots:

    /*!
     *  Custom close event for display window. Gives warning if closing before saving
     *  IP-XACT data into QSharedPointer<Component> comp_
     *
     * \param event type of close event.
     */
    void closeEvent(QCloseEvent* event);

    /*!
     *  Updates table content for parameters or ports
     *
     * \param worduc content of line clicked
     */
    void updateTable( const QString &worduc );

    /*!
     *  Open file- push button handler
     *  Opens fileOpen dialog for vhdl file selection. Displays selected file on
     *  VHDLdisplay, and calls VHDLanalyse class to analyse file.
     *
     */
    void on_pushButtonOpenFile_released();

    /*!
     * Analyses file dependencies for vhdl file
     */
    //void on_pushButtonDependency_clicked();

    /*!
     *  Save IP-XACT push button function
     *  Saves port and parameter data from tables into comp_
     */
    void on_pushButtonSaveFile_released();


    /*!
     * Removes row from tableWidgetPorts from position of cursor
     */
    void removeRow();

    /*!
     * Adds row to tableWidgetPorts in position of cursor
     */
    void addRow();

    /*!
     * Removes row from tableWidgetModelParams from position of cursor
     */
    void removeRowG();

    /*!
     * Adds row to tableWidgetModelParams in position of cursor
     */
    void addRowG();

    /*!
     *  Custom context menu function for tableWidgetPorts
     *  Opens menu with add row and Delete row options
     */
    void on_tableWidgetPorts_customContextMenuRequested();

    /*!
     *  Custom context menu function for tableWidgetModelParams
     *  Opens menu with add row and Delete row options
     */
    void on_tableWidgetModelParams_customContextMenuRequested();


private:


    /*!
     * Prevent copy-construction
     */
    VHDLtoIPXACT(const VHDLtoIPXACT&);


    /*!
     * Prevent assignment
     */
    VHDLtoIPXACT& operator=(const VHDLtoIPXACT&);


    //! State of the VHDL file (open/close)
    bool VHDLOpen;

    //! State of the metadata saving to comp_
    bool saveState;

    //! VHDLanalysis class. needed to keep up with scan status
    VHDLanalysis *VHDdata;

    //! changed from vhdl analysis return value. marks the end of vhdl entity line number
    int entityEndRow;

    //! xml file name and path
    QString xmlFile;

    //! vhdl file name and path
    QString vhdlFile;

    //! component class, metadata container
    QSharedPointer<Component> comp_;

    //! UI display elements

    //! grid layout for all ui elements
    QGridLayout *gridLayout;
    //! shows vhdl file content
    textEdit *VHDLdisplay;
    //! layout for tables and their labels
    QVBoxLayout *verticalLayout;
    //! shows and stores port data
    QTableWidget *tableWidgetPorts;

    //! shows and stores parameter data
    QTableWidget *tableWidgetModelParams;

    //! show and store fileset

    QHBoxLayout *horizontalLayout;

    //! open vhdl file push button
    QPushButton *pushButtonOpenFile;
    //! spaces push buttons
    QSpacerItem *horizontalSpacer;
    //! save ip-xact file push button
    QPushButton *pushButtonSaveFile;
    //! labels for table titles
    //! vhdl display
    QLabel *label;
    //! port table
    QLabel *label_2;
    //! parameters table
    QLabel *label_3;

};

#endif // VHDLTOIPXACT_H
