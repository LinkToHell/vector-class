#include <algorithm>

template<typename T>
class Vector {
private:
    void *data;
    size_t v_size, v_capacity;

    void safe_copy(T *begin, T *end, T *where, void *pointer = nullptr) {
        auto b = begin, w = where;
        try {
            for (; b != end; ++b, ++w)
                new(w) T(*b);
        } catch (...) {
            delete_range(where, where + (b - begin));
            operator delete(pointer);
            throw;
        }
    }

    void safe_init(T *begin, T *end, void *pointer = nullptr) {
        auto b = begin;
        try {
            for (; b != end; ++b)
                new(b) T;
        } catch (...) {
            delete_range(begin, b);
            operator delete(pointer);
            throw;
        }
    }

    void delete_range(T *begin, T *end, void *pointer = nullptr) {
        for (; begin != end; ++begin)
            begin->~T();
        operator delete(pointer);
    }

    void *allocate(size_t n) const {
        return operator new(sizeof(T) * n);
    }

public:
    Vector() : data(nullptr), v_size(0), v_capacity(0) {}

    explicit Vector(const size_t n) : data(allocate(n)), v_size(n), v_capacity(n) {
        safe_init(begin(), end(), data);
    }

    Vector(const Vector<T> &v) : data(allocate(v.v_size)), v_size(v.v_size),
                                 v_capacity(v.v_size) {
        safe_copy(v.begin(), v.end(), begin(), data);
    }

    Vector<T> &operator=(const Vector<T> &v) {
        void *new_data = allocate(v.v_size);
        safe_copy(v.begin(), v.end(), static_cast<T *>(new_data), new_data);
        delete_range(begin(), end(), data);
        data = new_data;
        v_capacity = v_size = v.v_size;
        return *this;
    }

    size_t size() const {
        return v_size;
    }

    size_t capacity() const {
        return v_capacity;
    }

    void clear() {
        delete_range(begin(), end());
        v_size = 0;
    }

    void swap(Vector<T> &other) {
        std::swap(v_size, other.v_size);
        std::swap(v_capacity, other.v_capacity);
        std::swap(data, other.data);
    }

    void reserve(size_t n) {
        if (v_capacity < n) {
            void *new_data = allocate(n);
            safe_copy(begin(), end(), static_cast<T *>(new_data), new_data);
            delete_range(begin(), end(), data);
            data = new_data;
            v_capacity = n;
        }
    }

    void resize(size_t n) {
        if (v_capacity < n) {
            void *new_data = allocate(n);
            T *new_begin = static_cast<T *>(new_data);
            safe_copy(begin(), end(), new_begin, new_data);
            try {
                safe_init(new_begin + v_size, new_begin + n);
            } catch (...) {
                delete_range(new_begin, new_begin + v_size, new_data);
                throw;
            }
            delete_range(begin(), end(), data);
            data = new_data;
            v_capacity = n;
        } else if (v_size < n) {
            safe_init(end(), begin() + n);
        } else {
            delete_range(begin() + n, end());
        }
        v_size = n;
    }

    void push_back(const T &n) {
        if (v_capacity == 0)
            reserve(1);
        if (v_size == v_capacity)
            reserve(v_capacity * 2);
        try {
            new(end()) T(n);
        } catch (...) {
            throw;
        }
        ++v_size;
    }

    void push_back(T &&n) {
        if (v_capacity == 0)
            reserve(1);
        if (v_size == v_capacity)
            reserve(v_capacity * 2);
        try {
            new(end()) T(std::move(n));
        } catch (...) {
            throw;
        }
        ++v_size;
    }

    void pop_back() {
        (end() - 1)->~T();
        --v_size;
    }

    T &operator[](size_t n) {
        return *(begin() + n);
    }

    const T &operator[](size_t n) const {
        return *(begin() + n);
    }

    T *begin() {
        return static_cast<T *>(data);
    }

    const T *begin() const {
        return static_cast<T *>(data);
    }

    T *end() {
        return begin() + v_size;
    }

    const T *end() const {
        return begin() + v_size;
    }

    void shrink_to_fit() {
        void *new_data = allocate(v_size);
        safe_copy(begin(), end(), static_cast<T *>(new_data), new_data);
        delete_range(begin(), end(), data);
        data = new_data;
        v_capacity = v_size;
    }

    ~Vector() {
        delete_range(begin(), end(), data);
    }
};
