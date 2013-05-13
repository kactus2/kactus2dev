#ifndef VHDLANALYSIS_H
#define VHDLANALYSIS_H

#include <QTableWidget>
#include <QString>
#include <QStringList>
#include <QTableWidgetItem>

class VHDLanalysis
{
public:
    VHDLanalysis();

    ~VHDLanalysis();

    //Analyzes given one line of vhdl-file. Supports ports and generics in entity, multiple ports can be
    //declared in one line, generic values can be used as range for vectors or integers.
    int EntityConverter(QTableWidget& gene_t,QTableWidget& port_t, QString line, bool clicked);//QTextStream& vhdlFile);

private:

    //Converts QString values to integer, converts generic values known to integers
    int Qstring2int(QTableWidget& generics,QString number);

    //Analyzes values of integers or vectors(range/downto/to)
    int analyzeGenericValues(QTableWidget& generics_t,QString value);

    //Stores generic and port values to Qtablewidget using above funktions
    //for generics:
    //input should be "genericName : type := value"
    //          or  "genericName : type"

    //for ports:
    //input should be "portName : direction type"
    //          or  "portName,...,portName : direction type"
    void analyzeData(QTableWidget& generics_t,QTableWidget& ports_t, QString data, bool generic,bool addremove,QString comment);

    int section = 0;
};

#endif // VHDLANALYSIS_H
