#include "logger.h"
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace fps_monitor {

Logger::~Logger() {
    shutdown();
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

bool Logger::initialize(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_initialized) {
        return true;
    }

    m_filename = filename;
    m_file.open(m_filename, std::ios::out | std::ios::app);
    
    if (!m_file.is_open()) {
        return false;
    }

    m_initialized = true;
    log(Level::Info, "Logger initialized");
    
    return true;
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_initialized && m_file.is_open()) {
        // Write shutdown message before closing
        m_file << getTimestamp() << " [INFO ] Logger shutting down" << std::endl;
        m_file.close();
        m_initialized = false;
    }
}

void Logger::log(Level level, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_initialized) {
        return;
    }

    checkRotation();

    m_file << getTimestamp() << " [" << levelToString(level) << "] " << message << std::endl;
    m_file.flush();
}

void Logger::debug(const std::string& message) {
#ifdef _DEBUG
    log(Level::Debug, message);
#endif
}

void Logger::info(const std::string& message) {
    log(Level::Info, message);
}

void Logger::warning(const std::string& message) {
    log(Level::Warning, message);
}

void Logger::error(const std::string& message) {
    log(Level::Error, message);
}

std::string Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm tm;
    localtime_s(&tm, &nowTime);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << nowMs.count();

    return oss.str();
}

std::string Logger::levelToString(Level level) const {
    switch (level) {
        case Level::Debug:   return "DEBUG";
        case Level::Info:    return "INFO ";
        case Level::Warning: return "WARN ";
        case Level::Error:   return "ERROR";
        default:             return "?????";
    }
}

void Logger::checkRotation() {
    if (!m_file.is_open()) {
        return;
    }

    // Check file size
    m_file.seekp(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(m_file.tellp());

    if (fileSize >= MAX_FILE_SIZE) {
        // Rotate log file
        m_file.close();

        // Rename old log with error handling
        std::string backupName = m_filename + ".old";
        DeleteFileA(backupName.c_str()); // Remove old backup if exists
        
        if (!MoveFileA(m_filename.c_str(), backupName.c_str())) {
            // If move fails, try to delete and create new
            DeleteFileA(m_filename.c_str());
        }

        // Open new log
        m_file.open(m_filename, std::ios::out | std::ios::app);
    }
}

} // namespace fps_monitor
