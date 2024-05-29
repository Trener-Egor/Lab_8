#include <iostream>
#include <exception>
#include <new>
#include <atomic>

template <typename T>
class MyShared
{
public:
    MyShared()
        : ptr(nullptr)
        , refCount(nullptr)
    {}

    explicit MyShared(T* p)
        : ptr(p)
        , refCount(new std::atomic<int>(1))
    {}

    MyShared(const MyShared& other)
        : ptr(other.ptr)
        , refCount(other.refCount)
    {
        (*refCount)++;
    }

    MyShared& operator=(const MyShared& other)
    {
        if (this != &other)
        {
            if (--(*refCount) == 0)
            {
                delete ptr;
                delete refCount;
            }
            ptr = other.ptr;
            refCount = other.refCount;
            (*refCount)++;
        }
        return *this;
    }

    MyShared(MyShared&& other) noexcept
        : ptr(other.ptr)
        , refCount(other.refCount)
    {
        other.ptr = nullptr;
        other.refCount = nullptr;
    }

    MyShared& operator=(MyShared&& other) noexcept
    {
        if (this != &other)
        {
            if (--(*refCount) == 0)
            {
                delete ptr;
                delete refCount;
            }
            ptr = other.ptr;
            refCount = other.refCount;
        }
        return *this;
    }

    ~MyShared()
    {
        if (--(*refCount) == 0)
        {
            delete ptr;
            delete refCount;
        }
    }

    T& operator*() const
    {
        if (ptr == nullptr)
        {
            throw std::runtime_error("Рnullptr dereferencing");
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
        if (--(*refCount) == 0)
        {
            delete ptr;
            delete refCount;
        }
        ptr = newPtr;
        refCount = newPtr ? new std::atomic<int>(1) : nullptr;
    }

    T& operator[](size_t index) const
    {
        if (ptr == nullptr)
        {
            throw std::runtime_error("nullptr dereferencing");
        }
        return ptr[index];
    }

private:
    T* ptr;
    std::atomic<int>* refCount; // Счетчик ссылок обычно явл атомарной переменной
};

template <typename T, typename... Args>
MyShared<T> Make_MyShared(Args&&... args)
{
    try
    {
        return MyShared<T>(new T(std::forward<Args>(args)...));
    }
    catch (const std::exception& e)
    {
        std::cerr << "error occured: " << e.what() << "\n";
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
    try
    {
        MyShared<int> intPtr = Make_MyShared<int>(5);
        std::cout << "intPtr: " << *intPtr << "\n";

        MyShared<int> intPtrCopy = intPtr;
        std::cout << "intPtrCopy: " << *intPtrCopy << "\n";

        MyShared<MyPoint> pointPtr = Make_MyShared<MyPoint>(3, 4);
        std::cout << "pointPtr: ("
            << pointPtr->getX() << ", "
            << pointPtr->getY() << ")" << "\n";

        MyShared<MyPoint> pointPtrCopy = pointPtr;
        std::cout << "pointPtrCopy: ("
            << pointPtrCopy->getX() << ", "
            << pointPtrCopy->getY() << ")" << "\n";

        MyShared<MyPoint> pointPtr2 = Make_MyShared<MyPoint>(1, 2);
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