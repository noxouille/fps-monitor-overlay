#pragma once

#include <string>
#include <fstream>
#include <mutex>

namespace fps_monitor {

/**
 * @brief Simple file-based logging for debugging
 * 
 * Provides thread-safe logging with timestamps and log levels.
 * Only active in debug builds.
 */
class Logger {
public:
    /**
     * @brief Log level enumeration
     */
    enum class Level {
        Debug,
        Info,
        Warning,
        Error
    };

    /**
     * @brief Get the singleton logger instance
     * 
     * @return Logger& Reference to the singleton
     */
    static Logger& getInstance();

    /**
     * @brief Initialize the logger
     * 
     * @param filename Log file path (default: "fps_monitor.log")
     * @return true if initialized successfully
     * @return false otherwise
     */
    bool initialize(const std::string& filename = "fps_monitor.log");

    /**
     * @brief Shutdown the logger
     */
    void shutdown();

    /**
     * @brief Log a message with specified level
     * 
     * @param level Log level
     * @param message Message to log
     */
    void log(Level level, const std::string& message);

    /**
     * @brief Log a debug message
     * 
     * Only logs in debug builds.
     * 
     * @param message Message to log
     */
    void debug(const std::string& message);

    /**
     * @brief Log an info message
     * 
     * @param message Message to log
     */
    void info(const std::string& message);

    /**
     * @brief Log a warning message
     * 
     * @param message Message to log
     */
    void warning(const std::string& message);

    /**
     * @brief Log an error message
     * 
     * @param message Message to log
     */
    void error(const std::string& message);

private:
    Logger() = default;
    ~Logger();

    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    /**
     * @brief Get current timestamp string
     * 
     * @return std::string Formatted timestamp
     */
    std::string getTimestamp() const;

    /**
     * @brief Convert log level to string
     * 
     * @param level Log level
     * @return std::string Level string
     */
    std::string levelToString(Level level) const;

    /**
     * @brief Check if log file needs rotation
     */
    void checkRotation();

    std::ofstream m_file;           ///< Log file stream
    std::string m_filename;         ///< Current log file path
    std::mutex m_mutex;             ///< Thread safety mutex
    bool m_initialized;             ///< Initialization flag
    static constexpr size_t MAX_FILE_SIZE = 5 * 1024 * 1024; ///< 5MB max log size
};

// Convenience macros
#ifdef _DEBUG
    #define LOG_DEBUG(msg) fps_monitor::Logger::getInstance().debug(msg)
    #define LOG_INFO(msg) fps_monitor::Logger::getInstance().info(msg)
    #define LOG_WARNING(msg) fps_monitor::Logger::getInstance().warning(msg)
    #define LOG_ERROR(msg) fps_monitor::Logger::getInstance().error(msg)
#else
    #define LOG_DEBUG(msg)
    #define LOG_INFO(msg)
    #define LOG_WARNING(msg)
    #define LOG_ERROR(msg)
#endif

} // namespace fps_monitor
