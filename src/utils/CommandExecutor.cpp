#include "CommandExecutor.h"
#include <cstdlib> // Für system()

CommandExecutor::CommandExecutor() {}

bool CommandExecutor::executeCommand(const std::string& command) {
    int result = std::system(command.c_str());
    return result == 0; // Gibt true zurück, wenn der Befehl erfolgreich war
}