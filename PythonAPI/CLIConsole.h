#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <QObject>

#include <windows.h>
#include <QWinEventNotifier>


#include <QSocketNotifier>
#include <iostream>
#include <QProcess>

class CLIConsole : public QObject
{
    Q_OBJECT

public:
    CLIConsole(QObject* parent);

    ~CLIConsole();

    bool initialize(char* argv);

signals:
    void quit();

    void write(std::string text);

private:
#ifdef Q_OS_WIN
    QWinEventNotifier *m_notifier;
#else
    QSocketNotifier *m_notifier;
#endif
  


private slots:
    void readCommand();

    void writeCommand();
};