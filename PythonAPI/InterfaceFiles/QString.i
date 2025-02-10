/* QString interface file for Python */
%module QString

%{
#include <QString>
%}

class QString
{
public:
    static QString fromStdString(const std::string &s);
    std::string toStdString() const;
    
    int size() const;
    int count() const;
    int length() const;
    bool isEmpty() const;
};
