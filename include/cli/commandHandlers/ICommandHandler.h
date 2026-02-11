#pragma once
#include "cli/data/ParsedCommand.h"
#include <iostream>

namespace cli {

    class ICommandHandler {
    public:
        /// <summary>
        /// Deconstructor for full derived class cleanup.
        /// </summary>
        virtual ~ICommandHandler() = default;  // ensures proper cleanup of derived classes

        /// <summary>
        /// Executes command.
        /// </summary>
        virtual void execute() = 0; // pure virtual -> must be implemented

        /// <summary>
        /// Sets command arguments.
        /// </summary>
        /// <param name="cmd :">`ParsedCommand` struct of parsed args and flags.</param>
        virtual void set(const ParsedCommand& cmd);

    protected:
        cli::ParsedCommand commandData;
    };

} // namespace cli
