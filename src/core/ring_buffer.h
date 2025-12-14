#pragma once

#include <vector>
#include <mutex>
#include <stdexcept>

namespace fps_monitor {

/**
 * @brief Thread-safe circular buffer for storing fixed-size samples
 * 
 * Template-based ring buffer that provides efficient storage for FPS samples
 * with no dynamic allocation during runtime. Supports thread-safe operations
 * and fast random access for graph rendering.
 * 
 * @tparam T The type of data to store
 * @tparam N The fixed capacity of the buffer (must be > 0)
 */
template<typename T, size_t N>
class RingBuffer {
    static_assert(N > 0, "RingBuffer capacity must be greater than 0");
    
public:
    /**
     * @brief Construct a new Ring Buffer object
     */
    RingBuffer() : m_head(0), m_size(0) {
        m_buffer.resize(N);
    }

    /**
     * @brief Push a new element into the buffer
     * 
     * If the buffer is full, overwrites the oldest element.
     * Thread-safe operation.
     * 
     * @param value The value to push
     */
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_buffer[m_head] = value;
        m_head = (m_head + 1) % N;
        if (m_size < N) {
            ++m_size;
        }
    }

    /**
     * @brief Get element at specified index
     * 
     * Index 0 is the oldest element, size()-1 is the newest.
     * Thread-safe operation.
     * 
     * @param index The index to access
     * @return T The value at the specified index
     * @throws std::out_of_range if index >= size()
     */
    T get(size_t index) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (index >= m_size) {
            throw std::out_of_range("RingBuffer index out of range");
        }
        size_t actualIndex = (m_head + N - m_size + index) % N;
        return m_buffer[actualIndex];
    }

    /**
     * @brief Get the most recently pushed element
     * 
     * @return T The latest value
     * @throws std::runtime_error if buffer is empty
     */
    T latest() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_size == 0) {
            throw std::runtime_error("RingBuffer is empty");
        }
        size_t lastIndex = (m_head + N - 1) % N;
        return m_buffer[lastIndex];
    }

    /**
     * @brief Get all elements in order (oldest to newest)
     * 
     * Thread-safe operation that copies all current elements.
     * 
     * @return std::vector<T> Vector containing all elements
     */
    std::vector<T> getAll() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<T> result;
        result.reserve(m_size);
        for (size_t i = 0; i < m_size; ++i) {
            size_t actualIndex = (m_head + N - m_size + i) % N;
            result.push_back(m_buffer[actualIndex]);
        }
        return result;
    }

    /**
     * @brief Get the current number of elements in the buffer
     * 
     * @return size_t Number of elements currently stored
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_size;
    }

    /**
     * @brief Get the maximum capacity of the buffer
     * 
     * @return size_t Maximum number of elements the buffer can hold
     */
    constexpr size_t capacity() const {
        return N;
    }

    /**
     * @brief Check if the buffer is full
     * 
     * @return true if size() == capacity()
     * @return false otherwise
     */
    bool isFull() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_size == N;
    }

    /**
     * @brief Check if the buffer is empty
     * 
     * @return true if size() == 0
     * @return false otherwise
     */
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_size == 0;
    }

    /**
     * @brief Clear all elements from the buffer
     * 
     * Thread-safe operation that resets the buffer to empty state.
     */
    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_head = 0;
        m_size = 0;
    }

private:
    std::vector<T> m_buffer;      ///< Internal storage
    size_t m_head;                 ///< Index of next write position
    size_t m_size;                 ///< Current number of elements
    mutable std::mutex m_mutex;    ///< Mutex for thread safety
};

} // namespace fps_monitor
