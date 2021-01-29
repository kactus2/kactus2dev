#include "CLIConsole.h"

CLIConsole::CLIConsole(QObject* parent) : QObject(parent)
{
   
}


CLIConsole::~CLIConsole()
{
    Py_Finalize();
}

bool CLIConsole::initialize(char* argv)
{
    wchar_t *program = Py_DecodeLocale(argv, NULL);
    if (program == NULL) {
        //fprintf(stderr, "Fatal error: cannot decode argv[0]\n");

    }

    Py_SetProgramName(program);  /* optional but recommended */
    Py_Initialize();

#ifdef Q_OS_WIN
    m_notifier = new QWinEventNotifier(GetStdHandle(STD_INPUT_HANDLE), this);
    connect(m_notifier, &QWinEventNotifier::activated, this, &CLIConsole::readCommand);

#else
    m_notifier = new QSocketNotifier(_fileno(stdin), QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, SLOT(readCommand()));
#endif

    //m_notifier->setEnabled(true);

    //connect(process_, SIGNAL(readyReadStandardOutput()), this, SLOT(writeCommand()));

    PyMem_RawFree(program);

    bool success = Py_IsInitialized();
    if (success)
    {
        std::cout << "Starting Python interpreter.\n" <<
            "Python " << Py_GetVersion() << "\n";        
        std::cout << ">";
    }

    return success;
}

void CLIConsole::readCommand()
{
    std::string line;

    std::getline(std::cin, line);

    if (std::cin.eof())
    {        
        emit quit();
    }
    else
    {
        line.push_back('\n');

        int error = PyRun_SimpleString(line.c_str());
        if (error != NULL)
        {
        }
       // process_->write(QByteArray::fromStdString(line));

        std::cout << ">";
        
    }


}

void CLIConsole::writeCommand()
{
   // std::cout << process_->readAllStandardOutput().toStdString();
}
