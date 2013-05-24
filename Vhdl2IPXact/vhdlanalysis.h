//-----------------------------------------------------------------------------
// File: VHDLanalysis.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Sillanpää
// Date: 24.05.2013
//
// Description:
// header file introducing VHDLanalysis class
//-----------------------------------------------------------------------------
#ifndef VHDLANALYSIS_H
#define VHDLANALYSIS_H

#include <QTableWidget>
#include <QString>
#include <QStringList>
#include <QTableWidgetItem>

//-----------------------------------------------------------------------------
//! Class for analyzing entity of vhdl-file.
//-----------------------------------------------------------------------------
class VHDLanalysis
{
public:
    VHDLanalysis();

    ~VHDLanalysis();

    /*!
     *  Analyzes given one line of vhdl-file. Supports ports and generics in entity, multiple ports can be
     *  declared in one line, generic values can be used as range for vectors or integers.
     *
     *      @param [in] gene_t  table for model parameters.
     *      @param [in] port_t  table for ports.
     *      @param [in] line    line of vhdl-file containing data for analyzer.
     *      @param [in] clicked if true, user has clicked line and line will be added or removed.
     *      @return returns current section, before entity = 0, in entity = 1, in generic list = 2,
                        in port list = 3, beyond end of entity = 4.
     */
    int entityConverter(QTableWidget& gene_t,QTableWidget& port_t, QString line, bool clicked);

private:

    /*!
     *  Converts QString values to integer, converts generic values known to integers.
     *
     *      @param [in] generics    names and values for converting generics.
     *      @param [in] number      number or generic to convert.
     *      @return 0 if unknown generic or number variable as integer.
     */
    int Qstring2int(QTableWidget const& generics,QString number);

    /*!
     *  Analyzes left and right bounds of port(s) recognices basic -,+,*,/
     *  operations and identifies known generics.
     *
     *      @param [out] generics_t  names and values for converting generics.
     *      @param [in] value           value to analyze, may contain: generics,+,-,/,*.
     *      @return converted value after generic replacements and arithmetic operations.
     */
    int analyzeValues(QTableWidget const& generics_t,QString value);

    /*!
     * splits generics and ports into parts, handles each part properly and stores parts into table
     * for generics:
     * data should be "genericName : type := value"
     *              or  "genericName : type"
     *
     * for ports: () = optional
     * data should be "portName : direction type (:= value)"
     *              or  "portName,...,portName : direction type (:= value)"
     *
     *      @param [in] generics_t  table to store model parameters.
     *      @param [in] ports_t     table to store ports.
     *      @param [in] data        contains relevant data from single line.
     *      @param [in] generic     if true, generic analyze is performed.
     *      @param [in] addremove   if true, removes generic/port if found or adds it.
     *      @param [in] comment     contains comment part of line to be stored into table.
     */
    void analyzeData(QTableWidget& generics_t,QTableWidget& ports_t, QString data,
                     bool generic,bool addremove,QString comment);

    //! controls execution of diffrent parts of code
    int section_;

};

#endif // VHDLANALYSIS_H
