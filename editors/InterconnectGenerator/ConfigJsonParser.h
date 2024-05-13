#ifndef CONFIGJSONPARSER_H
#define CONFIGJSONPARSER_H

#include <QtCore>


class ConfigJsonParser
{
public:

    struct AddressPair {

        QString Start;
        QString End;

    };

    struct TargetStruct {

        int Index;
        QString Name;
        int DataWidth;
        QList<AddressPair> AddressRegions;

    };

    struct ConfigStruct {

        QMap<QString,QString> VLNVs;
        int AddressWidth;
        int IDWidth;
        int UserWidth;
        QList<TargetStruct> TargetList;

    };

    //! The constructor
    ConfigJsonParser();

    //! The destructor
    ~ConfigJsonParser() = default;

    ConfigStruct* readFile();

    //! No copying. No assignment.
    ConfigJsonParser(const ConfigJsonParser& other) = delete;
    ConfigJsonParser& operator=(const ConfigJsonParser& other) = delete;

private:

    ConfigStruct config_;
    QString path_ = "./config.json";
};

#endif // CONFIGJSONPARSER_H
