
#include <QTableWidget>
#include <QString>
#include <QTableWidgetItem>
#include <cmath>

#include "vhdlanalysis.h"

VHDLanalysis::VHDLanalysis()
{
    section = 0;
}
VHDLanalysis::~VHDLanalysis()
{
}



//Converts QString values to integer, converts generic values known to integers
int VHDLanalysis::Qstring2int(QTableWidget& generics,QString number)
{
    bool ok;
    int result;
    QString generic_v;
    number = number.trimmed();
    result = number.toInt(&ok);
    if(!ok)
    {
        //if number is generic or unknown
        for(int i=0; i< generics.rowCount()-1;i++)
        {
            QTableWidgetItem *itemData = generics.item(i,0);
            if(itemData->text() == number)
            {
                generic_v = generics.item(i,3)->text();
                result = generic_v.toInt(&ok);
                break;
            }
        }
    }
    return result;
}

    // Analyzes left and right bounds of port(s) recognices basic -,+,*,/
    // operations and identifies known generics.
int VHDLanalysis::analyzeGenericValues(QTableWidget& generics_t,QString value)
{
    QString left;
    QString right;
    QString left2;
    QString right2;
    int result;
    int number1;
    int number2;
    int left_i;
    int right_i;
    int status = 0;  // 0 = NOP, 1 = -, and 2 = +


    if(value.contains('/'))
    {
        // "left/right"
        left = value.section('/',0,0);
        right = value.section('/',1);
        // "left2-right2/right"
        if(left.contains('-'))
        {
            left2 = left.section('-',0,0);
            right2 = left.section('-',1);
            status = 1;
        }
        // "left2+right2/right"
        else if(left.contains('+'))
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
        if(status > 0)
        {

            number1 = Qstring2int(generics_t,left2);
            number2 = Qstring2int(generics_t,right2);
            if(status == 1)
            {
                left_i = number1 - number2;
            }
            else if(status == 1)
            {
                left_i = number1 + number2;
            }
        }
        else
        {
            left_i = Qstring2int(generics_t,left);
        }

        // "left/left2-right2"
        if(right.contains('-'))
        {
            left2 = right.section('-',0,0);
            right2 = right.section('-',1);
            status = 1;
        }
        // "left/left2+right2"
        else if(right.contains('+'))
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
        if(status > 0)
        {

            number1 = Qstring2int(generics_t,left2);
            number2 = Qstring2int(generics_t,right2);
            if(status == 1)
            {
                right_i = number1 - number2;
            }
            else if(status == 2)
            {
                right_i = number1 + number2;
            }
        }
        else
        {
            right_i = Qstring2int(generics_t,right);
        }

        if(right_i != 0)
        {
            result = left_i/right_i;
        }
    } //end of /

    // "left-right"
    else if(value.contains('-'))
    {
        left = value.section('-',0,0);
        right = value.section('-',1);
        // "left2 * right2 - right"
        if(left.contains('*'))
        {
            left2 = left.section('*',0,0);
            right2 = left.section('*',1);
            status = 1;
        }
        // "left - left2 * right2"
        else if(right.contains('*'))
        {
            left2 = right.section('*',0,0);
            right2 = right.section('*',1);
            status = 2;
        }
        else
        {
            status = 0;
        }

        if(status > 0)
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
            else if(status == 2)
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
    else if(value.contains('+'))
    {
        left = value.section('-',0,0);
        right = value.section('-',1);
        // "left2 * right2 + right"
        if(left.contains('*'))
        {
            left2 = left.section('*',0,0);
            right2 = left.section('*',1);
            status = 1;
        }
        // "left + left2 * right2"
        else if(right.contains('*'))
        {
            left2 = right.section('*',0,0);
            right2 = right.section('*',1);
            status = 2;
        }
        else
        {
            status = 0;
        }

        if(status > 0)
        {
            number1 = Qstring2int(generics_t,left2);
            number2 = Qstring2int(generics_t,right2);
            // "left2 * right2 + right"
            if(status == 1)
            {
                left_i = number1 * number2;
                right_i = Qstring2int(generics_t,right);

            }
            // "left + left2 * right2"
            else if(status == 2)
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
    }
    // "left*right"
    else if(value.contains('*'))
    {
        left = value.section('*',0,0);
        right = value.section('*',1);
        right_i = Qstring2int(generics_t,right);
        left_i = Qstring2int(generics_t,left);
        result = left_i * right_i;
    }
    // "value"
    else
    {
        result = Qstring2int(generics_t,value);
    }

    return result;
}

    // splits generics and ports into parts, handles each part properly and stores parts into table
void VHDLanalysis::analyzeData(QTableWidget& generics_t,QTableWidget& ports_t, QString data, bool generic, bool addremove,QString comment)
{
    QTableWidgetItem* itemData;
    QString type;
    QString temp;
    int rows;
    bool found = false;


    // Data is generic
    //
    if(generic)
    {
        temp = data.section(':',0,0);
        temp = temp.trimmed();
        //searches generic table and removes generic if found
        if(addremove)
        {

            itemData = new QTableWidgetItem;
            itemData->setText(temp);
            for(int i=0; i< generics_t.rowCount()-1;i++)
            {
                if(generics_t.item(i,0)->text()==temp)
                {
                    rows = i;
                    generics_t.removeRow(rows);
                    found = true;
                    break;
                }
            }
        }
        if(found)
        {

        }
        //else adds generic to table
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
            if(data.contains(":="))
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

    //Data is Port
    else
    {
        int max = 0;
        int min = 0;
        QString max_qs;
        QString min_qs;
        QString direction;
        QString port;
        QStringList ports;
        int width = 1;
        bool morethan1 = false;

        port = data.section(':',0,0);
        data = data.section(':',1);
        port = port.simplified();

        //line contains more than one port of same kind
        if(port.contains(','))
        {
            ports = port.split(',');
            morethan1 = true;
            //ports now contains all name of the ports in one red line

        }

        //removes multiple ports from table if found
        if(addremove && morethan1)
        {
            int nroOfPorts = ports.size();
            for(int x = 0; x < nroOfPorts; x++)
            {
                itemData = new QTableWidgetItem;
                itemData->setText(ports.at(x));
                for(int y=0; y< ports_t.rowCount()-1;y++)
                {
                    if(ports_t.item(y,0)->text()==ports.at(x).simplified())
                    {
                        ports_t.removeRow(y);
                        found = true;
                        break;
                    }
                }
            }
        }
        //removes one port from table if found
        else if(addremove)
        {
            itemData = new QTableWidgetItem;
            itemData->setText(port);
            for(int i=0; i< ports_t.rowCount()-1;i++)
            {
                if(ports_t.item(i,0)->text()==port)
                {
                    ports_t.removeRow(i);
                    found = true;
                    break;
                }
            }
        }
        //doesnt add new if remove was performed
        if(found)
        {

        }
        //adds new port(s) to port table
        else
        {
            //takes direction and removes it from data
            direction = data.section(' ',1,1);
            data = data.section(' ',2);
            data = data.toLower();

            //data now contains type and range(if vector or integer)
            //vector
            if(data.contains("vector"))
            {
                type = "std_logic_vector";
                data = data.section('(',1);
                data = data.section(')',0,0);

                if(data.contains("downto"))
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
            else if(data.contains("integer"))
            {
                type = "integer";

                if(data.contains("range",Qt::CaseInsensitive))
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
            else
            {
                type = data;
            }

            if(width == 1)
            {
                max = analyzeGenericValues(generics_t,max_qs);
                min = analyzeGenericValues(generics_t,min_qs);
                //determine width of integer
                if(type == "integer")
                {
                    width = (int)ceil(log((double)max)/log((double)2));
                }
                //width in other types
                else if(type == "std_logic_vector")
                {
                    width += max - min;
                }
            }

            // Adding port(s) to table
            rows = ports_t.rowCount() - 1;

            // if more than one port of same kind
            if(morethan1)
            {
                int nroOfPorts = ports.size();
                for(int i = 0; i < nroOfPorts; i++)
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


int VHDLanalysis::EntityConverter(QTableWidget& gene_t,QTableWidget& port_t, QString line, bool clicked)//QTextStream& vhdlFile)
{
    QString comment;
    line = line.simplified();
    int return_value;

    //if user clicks to modify this if desides right section parameter
    if(line.contains(':') && clicked)
    {
        //no need to analyze comment line with ":"
        if(line.startsWith("--"))
        {
            section = 4;
        }
        else if(line.contains("in ",Qt::CaseInsensitive)||line.contains("out ",Qt::CaseInsensitive)||line.contains("inout ",Qt::CaseInsensitive)
                ||line.contains("buffer ",Qt::CaseInsensitive)||line.contains("linkage ",Qt::CaseInsensitive))
        {
            //port(s)
            section = 3;

        }
        else
        {
            //generic
            section = 2;
        }
        //editMode = true;
    }

    //Analyzes entity section of code
    if(section != 4)
    {
        //entity starts
        if(line.startsWith("entity",Qt::CaseInsensitive))
        {
            section = 1;
        }
        //generic list starts
        else if(line.startsWith("generic",Qt::CaseInsensitive) && section == 1)
        {
            section = 2;
        }
        //port list starts
        else if(line.startsWith("port",Qt::CaseInsensitive))
        {
            section = 3;
        }
        //entity ends
        else if(line.startsWith("end",Qt::CaseInsensitive))
        {
            section = 4;
        }

        //generic section of entity
        if(line.startsWith("--"))
        {

        }
        else if(section == 2 && line.contains(':'))
        {
            comment = line.section("--",1);
            line = line.section("--",0,0);

            if(line.startsWith("generic",Qt::CaseInsensitive))
            {
                line = line.section("generic(",1,-1,QString::SectionCaseInsensitiveSeps);
            }
            else if(line.startsWith("generic (",Qt::CaseInsensitive))
            {
                line = line.section("generic (",1,-1,QString::SectionCaseInsensitiveSeps);
            }

            if(line.endsWith(");"))
            {

                line = line.section(");",0,0);
                analyzeData(gene_t,port_t,line,1,clicked,comment);
                section = 1;
            }
            else
            {
                line = line.section(';',0,0);
                analyzeData(gene_t,port_t,line,1,clicked,comment);
            }
            //line should be "genericName : type := value"
            //          or  "genericName : type"

        }

        //port section of entity
        else if(section == 3 && line.contains(':'))
        {
            comment = line.section("--",1);
            line = line.section("--",0,0);

            if(line.startsWith("port(",Qt::CaseInsensitive))
            {
                line = line.section("port(",1,-1,QString::SectionCaseInsensitiveSeps);
            }
            else if(line.startsWith("port (",Qt::CaseInsensitive))
            {
                line = line.section("port (",1,-1,QString::SectionCaseInsensitiveSeps);
            }

            //if entity contains only one line of ports
            if(line.endsWith(");"))
            {

                line = line.section(");",0,0);
                analyzeData(gene_t,port_t,line,0,clicked,comment);
                //section = 1;
                //trimmed?
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
    return_value = section;
    if(clicked)
    {
       section = 4;
    }
    return return_value;
}
