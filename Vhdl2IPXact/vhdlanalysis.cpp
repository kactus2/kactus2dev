//-----------------------------------------------------------------------------
// File: VHDLanalysis.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Sillanpää
// Date: 26.05.2013
//
// Description:
// Analyses entity of vhdl-file and returns model parameters and ports of
// entity in tables.
//-----------------------------------------------------------------------------
#include <QTableWidget>
#include <QString>
#include <QTableWidgetItem>
#include <cmath>

#include "vhdlanalysis.h"

VHDLanalysis::VHDLanalysis()
{
    section_ = 0;
}

VHDLanalysis::~VHDLanalysis()
{
}

//-----------------------------------------------------------------------------
// Function: VHDLanalysis::entityConverter()
//-----------------------------------------------------------------------------
int VHDLanalysis::entityConverter(QTableWidget& gene_t,QTableWidget& port_t, QString line, bool clicked)
{
    QString comment; // Stores comment part of the line
    int return_value;

    line = line.simplified();

    //if user clicks to modify this if desides right section parameter
    if (line.contains(':') && clicked)
    {
        //no need to analyze comment line with ":"
        if (line.startsWith("--"))
        {
            section_ = 4;
        }
        else if(line.contains("in ",Qt::CaseInsensitive) ||
                line.contains("out ",Qt::CaseInsensitive) ||
                line.contains("inout ",Qt::CaseInsensitive) ||
                line.contains("buffer ",Qt::CaseInsensitive) ||
                line.contains("linkage ",Qt::CaseInsensitive))
        {
            //port(s)
            section_ = 3;

        }
        else
        {
            //generic
            section_ = 2;
        }
    }

    //Analyzes entity section of code
    //-----------------------------------------------------------------------------
    if (section_ != 4)
    {
        //entity starts
        if (line.startsWith("entity",Qt::CaseInsensitive))
        {
            section_ = 1;
        }
        //generic list starts
        else if (line.startsWith("generic",Qt::CaseInsensitive) && section_ == 1)
        {
            section_ = 2;
        }
        //port list starts
        else if (line.startsWith("port",Qt::CaseInsensitive))
        {
            section_ = 3;
        }
        //entity ends
        else if (line.startsWith("end",Qt::CaseInsensitive))
        {
            section_ = 4;
        }

        //-----------------------------------------------------------------------------
        // Generic section of entity
        //-----------------------------------------------------------------------------
        if (line.startsWith("--"))
        {

        }
        else if (section_ == 2 && line.contains(':'))
        {
            comment = line.section("--",1);
            comment = comment.trimmed();
            line = line.section("--",0,0);

            if (line.startsWith("generic(",Qt::CaseInsensitive))
            {
                line = line.section("generic(",1,-1,QString::SectionCaseInsensitiveSeps);
            }
            else if (line.startsWith("generic (",Qt::CaseInsensitive))
            {
                line = line.section("generic (",1,-1,QString::SectionCaseInsensitiveSeps);
            }

            if (line.endsWith(");"))
            {

                line = line.section(");",0,0);
                analyzeData(gene_t,port_t,line,1,clicked,comment);
                section_ = 1;
            }
            else
            {
                line = line.section(';',0,0);
                analyzeData(gene_t,port_t,line,1,clicked,comment);
            }
            //line should be "genericName : type := value"
            //          or  "genericName : type"
        }

        //-----------------------------------------------------------------------------
        // Port section of entity
        //-----------------------------------------------------------------------------
        else if (section_ == 3 && line.contains(':'))
        {
            comment = line.section("--",1);
            comment = comment.trimmed();
            line = line.section("--",0,0);

            if (line.startsWith("port(",Qt::CaseInsensitive))
            {
                line = line.section("port(",1,-1,QString::SectionCaseInsensitiveSeps);
            }
            else if (line.startsWith("port (",Qt::CaseInsensitive))
            {
                line = line.section("port (",1,-1,QString::SectionCaseInsensitiveSeps);
            }

            //if entity contains only one line of ports
            if (line.endsWith(");"))
            {

                line = line.section(");",0,0);
                analyzeData(gene_t,port_t,line,0,clicked,comment);
            }
            else
            {
                line = line.section(';',0,0);
                analyzeData(gene_t,port_t,line,0,clicked,comment);
            }
            //line should be "portName : direction type"
            //          or  "portName,...,portName : direction type"
        }
      }

    //returns info what was done, 2 = generic 3 = port, 4 = end->
    return_value = section_;
    if (clicked)
    {
       section_ = 4;
    }
    return return_value;
}

//-----------------------------------------------------------------------------
// Function: VHDLanalysis::Qstring2int()
//-----------------------------------------------------------------------------
int VHDLanalysis::Qstring2int(QTableWidget const& generics,QString number)
{
    bool ok = true; // Boolean flag for succesfull operation
    int result = 0; // converted QString value
    QString generic_v; // value of searched generic

    number = number.trimmed();
    result = number.toInt(&ok);

    if (!ok)
    {
        //if number is generic or unknown
        for (int i=0; i< generics.rowCount()-1; i++)
        {
            QTableWidgetItem *itemData = generics.item(i,0);
            if (itemData->text() == number)
            {
                generic_v = generics.item(i,3)->text();
                result = generic_v.toInt(&ok);
                break;
            }
        }
    }
    return result;
}

//-----------------------------------------------------------------------------
// Function: VHDLanalysis::analyzeValues()
//-----------------------------------------------------------------------------
int VHDLanalysis::analyzeValues(QTableWidget const& generics_t,QString value)
{
    QString left; // left part of value
    QString right; // right part of value
    QString left2; // left part of left or right
    QString right2; // right part of left or right
    int result; // result of value after calculations
    int number1; // variable for performing calculations
    int number2; // variable for performing calculations
    int left_i; // QSting left after calculations
    int right_i; // QSting right after calculations
    int status = 0;  // variable to control calculations

    // "left/right"
    //-----------------------------------------------------------------------------
    if (value.contains('/'))
    {
        left = value.section('/',0,0);
        right = value.section('/',1);
        // "left2-right2/right"
        if (left.contains('-'))
        {
            left2 = left.section('-',0,0);
            right2 = left.section('-',1);
            status = 1;
        }
        // "left2+right2/right"
        else if (left.contains('+'))
        {
            left2 = left.section('+',0,0);
            right2 = left.section('+',1);
            status = 2;
        }
        // "left/right"
        else
        {
            status = 0;
        }

        //analyze left
        if (status > 0)
        {

            number1 = Qstring2int(generics_t,left2);
            number2 = Qstring2int(generics_t,right2);
            if(status == 1)
            {
                left_i = number1 - number2;
            }
            else if (status == 1)
            {
                left_i = number1 + number2;
            }
        }
        else
        {
            left_i = Qstring2int(generics_t,left);
        }

        // "left/left2-right2"
        if (right.contains('-'))
        {
            left2 = right.section('-',0,0);
            right2 = right.section('-',1);
            status = 1;
        }
        // "left/left2+right2"
        else if (right.contains('+'))
        {
            left2 = right.section('+',0,0);
            right2 = right.section('+',1);
            status = 2;
        }
        // "left/right"
        else
        {
            status = 0;
        }

        //analyze right
        if (status > 0)
        {

            number1 = Qstring2int(generics_t,left2);
            number2 = Qstring2int(generics_t,right2);
            if (status == 1)
            {
                right_i = number1 - number2;
            }
            else if (status == 2)
            {
                right_i = number1 + number2;
            }
        }
        else
        {
            right_i = Qstring2int(generics_t,right);
        }

        if (right_i != 0)
        {
            result = left_i/right_i;
        }
    } //end of /

    // "left-right"
    //-----------------------------------------------------------------------------
    else if (value.contains('-'))
    {
        left = value.section('-',0,0);
        right = value.section('-',1);
        // "left2 * right2 - right"
        if (left.contains('*'))
        {
            left2 = left.section('*',0,0);
            right2 = left.section('*',1);
            status = 1;
        }
        // "left - left2 * right2"
        else if (right.contains('*'))
        {
            left2 = right.section('*',0,0);
            right2 = right.section('*',1);
            status = 2;
        }
        else
        {
            status = 0;
        }

        if (status > 0)
        {
            number1 = Qstring2int(generics_t,left2);
            number2 = Qstring2int(generics_t,right2);
            // "left2 * right2 - right"
            if(status == 1)
            {
                left_i = number1 * number2;
                right_i = Qstring2int(generics_t,right);

            }
            // "left - left2 * right2"
            else if (status == 2)
            {
                right_i = number1 * number2;
                left_i = Qstring2int(generics_t,left);
            }
        }
        else
        {
            left_i = Qstring2int(generics_t,left);
            right_i = Qstring2int(generics_t,right);

        }
        result = left_i - right_i;
    } //end of "-"

    // "left+right"
    //-----------------------------------------------------------------------------
    else if (value.contains('+'))
    {
        left = value.section('-',0,0);
        right = value.section('-',1);
        // "left2 * right2 + right"
        if (left.contains('*'))
        {
            left2 = left.section('*',0,0);
            right2 = left.section('*',1);
            status = 1;
        }
        // "left + left2 * right2"
        else if (right.contains('*'))
        {
            left2 = right.section('*',0,0);
            right2 = right.section('*',1);
            status = 2;
        }
        else
        {
            status = 0;
        }

        if (status > 0)
        {
            number1 = Qstring2int(generics_t,left2);
            number2 = Qstring2int(generics_t,right2);
            // "left2 * right2 + right"
            if (status == 1)
            {
                left_i = number1 * number2;
                right_i = Qstring2int(generics_t,right);

            }
            // "left + left2 * right2"
            else if (status == 2)
            {
                right_i = number1 * number2;
                left_i = Qstring2int(generics_t,left);
            }
        }
        else
        {
            left_i = Qstring2int(generics_t,left);
            right_i = Qstring2int(generics_t,right);

        }
        result = left_i + right_i;
    } //end of "+"


    // "left*right"
    //-----------------------------------------------------------------------------
    else if (value.contains('*'))
    {
        left = value.section('*',0,0);
        right = value.section('*',1);
        right_i = Qstring2int(generics_t,right);
        left_i = Qstring2int(generics_t,left);
        result = left_i * right_i;
    } //end of "*"

    // "value"
    //-----------------------------------------------------------------------------
    else
    {
        result = Qstring2int(generics_t,value);
    }
    return result;
}

//-----------------------------------------------------------------------------
// Function: VHDLanalysis::analyzeData()
//-----------------------------------------------------------------------------
void VHDLanalysis::analyzeData(QTableWidget& generics_t, QTableWidget& ports_t, QString data,
                               bool generic, bool addremove, QString comment)
{
    QTableWidgetItem* itemData; // variable to interact with Qtablewidgert
    QString type; //type of port
    QString temp; // used for slicing generics for table
    int rows; // number of rows in table
    bool found = false; // true, if searched name of generic/port was found in table

    //-----------------------------------------------------------------------------
    // Data is Generic
    //-----------------------------------------------------------------------------
    if (generic)
    {
        temp = data.section(':',0,0);
        temp = temp.trimmed();

        //searches generic table and removes generic if found
        //-----------------------------------------------------------------------------
        if (addremove)
        {
            itemData = new QTableWidgetItem;
            itemData->setText(temp);
            for (int i=0; i< generics_t.rowCount()-1; i++)
            {
                if (generics_t.item(i,0)->text() == temp)
                {
                    rows = i;
                    generics_t.removeRow(rows);
                    found = true;
                    break;
                }
            }
        }
        if (found)
        {

        }
        //else adds generic to table
        //-----------------------------------------------------------------------------
        else
        {
            rows = generics_t.rowCount() - 1;

            //sets generic name to table
            generics_t.setItem(rows,0,new QTableWidgetItem);
            generics_t.item(rows,0)->setText(temp.trimmed());

            //sets generic type to table
            temp = data.section(':',1,1);
            generics_t.setItem(rows,1,new QTableWidgetItem);
            generics_t.item(rows,1)->setText(temp.trimmed());

            //sets comment
            generics_t.setItem(rows,4,new QTableWidgetItem);
            generics_t.item(rows,4)->setText(comment);

            //sets value, if present
            if (data.contains(":="))
            {
                temp = data.section(":=",1);
                generics_t.setItem(rows,3,new QTableWidgetItem);
                generics_t.item(rows,3)->setText(temp.trimmed());
            }
            else
            {
                generics_t.setItem(rows,3,new QTableWidgetItem);
                generics_t.item(rows,3)->setText("0");
            }
            generics_t.insertRow(rows+1);
            rows++;
        }
    }

    //-----------------------------------------------------------------------------
    // Data is Port
    //-----------------------------------------------------------------------------
    else
    {
        int max = 0; // left bound
        int min = 0; // right bound
        QString max_qs; // left bound as qs
        QString min_qs; // right bound as qs
        QString direction; // direction of port
        QString port; // name of port
        QString initvalue = ""; // port default value
        QStringList ports; // names of multiple ports in same line
        int width = 1; // width of port
        bool morethan1 = false; // true, if there are more than one port in same line

        port = data.section(':',0,0);
        data = data.section(':',1);
        port = port.simplified();

        //line contains more than one port of same kind
        //-----------------------------------------------------------------------------
        if (port.contains(','))
        {
            ports = port.split(',');
            morethan1 = true;
            //ports now contains all name of the ports in one red line

        }

        //removes multiple ports from table if found
        //-----------------------------------------------------------------------------
        if (addremove && morethan1)
        {
            int nroOfPorts = ports.size();
            for (int x = 0; x < nroOfPorts; x++)
            {
                itemData = new QTableWidgetItem;
                itemData->setText(ports.at(x));
                for (int y=0; y< ports_t.rowCount()-1; y++)
                {
                    if (ports_t.item(y,0)->text() == ports.at(x).simplified())
                    {
                        ports_t.removeRow(y);
                        found = true;
                        break;
                    }
                }
            }
        }
        //removes one port from table if found
        //-----------------------------------------------------------------------------
        else if (addremove)
        {
            itemData = new QTableWidgetItem;
            itemData->setText(port);
            for (int i=0; i< ports_t.rowCount()-1; i++)
            {
                if (ports_t.item(i,0)->text() == port)
                {
                    ports_t.removeRow(i);
                    found = true;
                    break;
                }
            }
        }
        //doesnt add new if remove was performed
        if (found)
        {

        }
        //adds new port(s) to port table
        //-----------------------------------------------------------------------------
        else
        {
            //takes direction and removes it from data
            data = data.trimmed();
            direction = data.section(' ',0,0);
            direction = direction.trimmed();

            data = data.section(' ',1);
            data = data.toLower();

            //takes initial value from data
            if (data.contains(":="))
            {
                initvalue = data.section(":=",1);
                data = data.section(":=",0,0);
                initvalue = initvalue.trimmed();
            }
            //data now contains type and range(if vector or integer)

            //vector
            //-----------------------------------------------------------------------------
            if (data.contains("vector"))
            {
                //type = "std_logic_vector";
                type = data.section('(',0,0);
                type = type.trimmed();

                data = data.section('(',1);
                data = data.section(')',0,0);

                if (data.contains("downto"))
                {
                    max_qs = data.section("downto",0,0);
                    min_qs = data.section("downto",1);
                }
                else
                {
                    max_qs = data.section("to",0,0);
                    min_qs = data.section("to",1);
                }
            }

            //integer
            //-----------------------------------------------------------------------------
            else if (data.contains("integer"))
            {
                type = "integer";

                if (data.contains("range",Qt::CaseInsensitive))
                {
                    data = data.section("range",1);
                    min_qs = data.section("to",0,0);
                    max_qs = data.section("to",1);
                }
                //full range if not specified
                else
                {
                    width = 32;
                    max = 2147483647;
                    min = 0; // or -2147483648;
                }
            }

            //data type is std_logic or other 1 wide type
            //-----------------------------------------------------------------------------
            else
            {
                type = data.trimmed();
            }

            if (width == 1)
            {
                max = analyzeValues(generics_t,max_qs);
                min = analyzeValues(generics_t,min_qs);
                //determine width of integer
                if (type == "integer")
                {
                    width = (int)ceil(log((double)max)/log((double)2));
                }
                //width in other types
                else if (type == "std_logic_vector")
                {
                    width += max - min;
                }
            }

            //-----------------------------------------------------------------------------
            // Adding port(s) to table.
            //-----------------------------------------------------------------------------
            rows = ports_t.rowCount() - 1;

            // if more than one port of same kind
            if (morethan1)
            {
                int nroOfPorts = ports.size();
                for (int i = 0; i < nroOfPorts; i++)
                {
                    //Port name
                    ports_t.setItem(rows,0,new QTableWidgetItem);
                    ports_t.item(rows,0)->setText(ports.at(i).simplified());

                    //Direction
                    ports_t.setItem(rows,1,new QTableWidgetItem);
                    ports_t.item(rows,1)->setText(direction);

                    //Width
                    ports_t.setItem(rows,2,new QTableWidgetItem);
                    ports_t.item(rows,2)->setText(QString::number(width));

                    //Left bound
                    ports_t.setItem(rows,3,new QTableWidgetItem);
                    ports_t.item(rows,3)->setText(QString::number(max));

                    //Right bound
                    ports_t.setItem(rows,4,new QTableWidgetItem);
                    ports_t.item(rows,4)->setText(QString::number(min));

                    //Type
                    ports_t.setItem(rows,5,new QTableWidgetItem);
                    ports_t.item(rows,5)->setText(type);

                    //Initial value
                    ports_t.setItem(rows,7,new QTableWidgetItem);
                    ports_t.item(rows,7)->setText(initvalue);

                    //Comment
                    ports_t.setItem(rows,8,new QTableWidgetItem);
                    ports_t.item(rows,8)->setText(comment);

                    ports_t.insertRow(rows+1);
                    rows++;
                }
            }
            // One port
            else
            {
                //Port name
                ports_t.setItem(rows,0,new QTableWidgetItem);
                ports_t.item(rows,0)->setText(port.simplified());

                //Direction
                ports_t.setItem(rows,1,new QTableWidgetItem);
                ports_t.item(rows,1)->setText(direction);

                //Width
                ports_t.setItem(rows,2,new QTableWidgetItem);
                ports_t.item(rows,2)->setText(QString::number(width));

                //Left bound
                ports_t.setItem(rows,3,new QTableWidgetItem);
                ports_t.item(rows,3)->setText(QString::number(max));

                //Right bound
                ports_t.setItem(rows,4,new QTableWidgetItem);
                ports_t.item(rows,4)->setText(QString::number(min));

                //Type
                ports_t.setItem(rows,5,new QTableWidgetItem);
                ports_t.item(rows,5)->setText(type);

                //Initial value
                ports_t.setItem(rows,7,new QTableWidgetItem);
                ports_t.item(rows,7)->setText(initvalue);

                //Comment
                ports_t.setItem(rows,8,new QTableWidgetItem);
                ports_t.item(rows,8)->setText(comment);

                ports_t.insertRow(rows+1);
                rows++;
            }
        }
    }
    return;
}
