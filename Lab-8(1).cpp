#include <iostream>
#include <exception>
#include <new>

template <typename T>
class UniquePtr
{
public:
    UniquePtr()
        : ptr(nullptr)
    {}
    explicit UniquePtr(T* p)
        : ptr(p)
    {}


    UniquePtr(const UniquePtr& other) = delete;
    UniquePtr& operator=(const UniquePtr& other) = delete;

    UniquePtr(UniquePtr&& other) noexcept
        : ptr(other.ptr)
    {
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept
    {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    ~UniquePtr()
    {
        delete ptr;
    }

    T& operator*() const
    {
        if (ptr == nullptr) {
            throw std::runtime_error("nullptr dereferencing");
        }
        return *ptr;
    }

    T* operator->() const
    {
        if (ptr == nullptr)
        {
            throw std::runtime_error("nullptr dereferencing");
        }
        return ptr;
    }

    explicit operator bool() const
    {
        return ptr != nullptr;
    }

    T* get() const
    {
        return ptr;
    }

    void reset(T* newPtr = nullptr)
    {
        delete ptr;
        ptr = newPtr;
    }

    T* release()
    {
        T* tmp = ptr;
        ptr = nullptr;

        return tmp;
    }

    T& operator[](size_t index) const
    {
        if (ptr == nullptr) {
            throw std::runtime_error("nullptr dereferencing"); //разыменование nullptr
        }
        return ptr[index];
    }

private:
    T* ptr;
};


template <typename T, typename... Args>
UniquePtr<T> Make_Unique(Args&&... args)
{
    try
    {
        return UniquePtr<T>(new T(std::forward<Args>(args)...));
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occured: " << e.what() << "\n";
        throw;
    }
}


class MyPoint
{
public:
    MyPoint(int x, int y)
        : x(x)
        , y(y)
    {}

    int getX() const { return x; }
    int getY() const { return y; }

private:
    int x, y;
};

int main()
{
    // TEST UniquePtrPtr
    try
    {
        UniquePtr<int> p(new int(5));

        std::cout << *p << "\n";   // Доступ к данным

        *p = 10;  // переопределенный оператор *
        std::cout << *p << "\n";

        int* rawPtr = p.release(); // отдали владение
        std::cout << *rawPtr << "\n";


        UniquePtr<int> p2;         // попытка разыменования nullptr
        std::cout << *p2 << "\n"; // -> исключение
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occured: " << e.what() << "\n";
    }
    // TEST Make_Unique
    try
    {
        UniquePtr<int> intPtr = Make_Unique<int>(5);
        std::cout << "intPtr: " << *intPtr << "\n";

        UniquePtr<MyPoint> pointPtr = Make_Unique<MyPoint>(3, 4);
        std::cout << "pointPtr: ("
            << pointPtr->getX() << ", "
            << pointPtr->getY() << ")" << "\n";

        UniquePtr<MyPoint> pointPtr2 = Make_Unique<MyPoint>(1, 2);
        std::cout << "pointPtr2: ("
            << pointPtr2->getX() << ", "
            << pointPtr2->getY() << ")" << "\n";

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occured: " << e.what() << "\n";
    }

    return 0;
}