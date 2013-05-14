#ifndef VHDLTOIPXACT_H
#define VHDLTOIPXACT_H

#include "textedit.h"
#include "vhdlanalysis.h"
//#include <models/component.h>
#include <QWidget>
#include <QtWidgets>

class VHDLtoIPXACT : public QWidget
{
    Q_OBJECT
    
public:
    VHDLtoIPXACT(/*QSharedPointer<Component> comp, const QString xmlFilePath
             */QWidget *parent = 0);
    ~VHDLtoIPXACT();

private slots:

    //customised closeevent for closing the tool
    //works as generic close event, but gives Qmessagebox
    //if some of the collected data has not been saved yet.
    //allowing user to cancel closing the tool
    void closeEvent(QCloseEvent* event);

    //for updating table on click
    void updateTable( const QString &worduc );

    //void TextEditDoubleClicked(int row,int column);


    //clicking open file push button opens file open dialog
    void on_pushButtonOpenFile_released();

    //on dependency analysy tool button opens file dependency
    //analysis tool
    void on_pushButtonDependency_clicked();

    //on save file button clicked opens VLNV dialog to ask
    //VLNV data, and when cliked ok saves collected vhdl data
    //into xml file using FileWriter module
    void on_pushButtonSaveFile_released();

    //Test function, not to be included in final product
    //void on_tableWidgetPorts_doubleClicked(const QModelIndex &index);

   // void on_tableWidgetPorts_cellDoubleClicked(int row, int column);


    //void on_tableWidgetModelParams_itemDoubleClicked(QTableWidgetItem *item);

    void removeRow();

    void addRow();

    void removeRowG();

    void addRowG();

    void on_tableWidgetPorts_customContextMenuRequested();

    void on_tableWidgetModelParams_customContextMenuRequested();


private:

    bool VHDLOpen; //to see if file is open
    bool saveState;  //if xml has been saved true=saved, false=not saved
    VHDLanalysis *VHDdata;
    int entityEndRow;
    QString xmlFile;
    //Component comp;

    QWidget *centralWidget;
    QGridLayout *gridLayout;
    textEdit *VHDLdisplay;
    QVBoxLayout *verticalLayout;
    QTableWidget *tableWidgetPorts;
    QLabel *label_3;
    QTableWidget *tableWidgetModelParams;
    QLabel *label_4;
    QTableWidget *tableWidgetFileSets;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButtonOpenFile;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButtonDependency;
    QPushButton *pushButtonSaveFile;
    QLabel *label;
    QLabel *label_2;

};

#endif // VHDLTOIPXACT_H
