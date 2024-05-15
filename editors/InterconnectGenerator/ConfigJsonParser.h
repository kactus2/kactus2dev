#ifndef CONFIGJSONPARSER_H
#define CONFIGJSONPARSER_H

#include <QtCore>
#include <QDir>


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

        QString InterconVLNV;
        QString DesignVLNV;
        QString BusVLNV;
        QString ClkVLNV;
        QString RstVLNV;
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
    QString path_ = QDir::currentPath() + "/editors/InterconnectGenerator/config.json";
};

#endif // CONFIGJSONPARSER_H
