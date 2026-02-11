#pragma once

#include <string>
#include <vector>

namespace utils {

	/// <summary>
	/// Simple logger class for logging messages to the console.
	/// Can be extended in the future to support file logging, log levels, etc.
	/// </summary>
	class Logger {

	private:
		inline static std::vector<std::string> logHistory = {};  // Static vector to store log history

		inline static bool enabled = false;  // Flag to enable/disable logging (can be toggled with `-v / --verbose`)

	public:

		/// <summary>
		/// Adds a log entry to the log history list
		/// </summary>
		void Log(const std::string& logEntry) {
			logHistory.push_back(logEntry);
		}

		/// <summary>
		/// Displays the log history to the console. Can be used for debugging or informational purposes. (use `-v / --verbose`).
		/// </summary>
		void displayLog() {
			if (!enabled) { return; }

			for (const auto& entry : logHistory) {
				std::cout << entry << std::endl;
			}
		}

		void enable() {
			enabled = true;
		}
	};

	inline Logger gLogger;  // Global logger instance

	// Global access function for logging messages
	inline void log(const std::string& message) {
		gLogger.Log(message);
	}

	inline void displayLogs() {
		gLogger.displayLog();
	}

	inline void enable() {
		gLogger.enable();
	}

} 