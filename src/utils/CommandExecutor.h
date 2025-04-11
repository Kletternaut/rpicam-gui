#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include <string> // Für std::string

class CommandExecutor {
public:
    CommandExecutor(); // Konstruktor deklarieren
    bool executeCommand(const std::string& command);
};

#endif // COMMANDEXECUTOR_H