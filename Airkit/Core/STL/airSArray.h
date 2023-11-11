#ifndef __SAIRARRAY__H__
#define __SAIRARRAY__H__
#include <Airkit/Precompiled.h>

// 静态数组类
namespace airkit
{
    // 一维数组
    template <typename Type, const size_t col>
    class SArray
    {
    public:
        // 迭代器
        class Iterator
        {
        private:
            friend class SArray;
            size_t mIndex;
            explicit Iterator(const size_t index = -1) : mIndex(index) {}

        public:
            Iterator(const Iterator &rhs) : mIndex(rhs.mIndex) {}

            // 是否有效
            bool valid() const { return mIndex < col; }

            // 重载操作符
            // 前置 ++、--
            inline Iterator &operator++()
            {
                ++mIndex;
                return *this;
            }
            inline Iterator &operator--()
            {
                --mIndex;
                return *this;
            }
            // 后置 ++、--
            inline Iterator operator++(int32_t)
            {
                Iterator ret = *this;
                ++mIndex;
                return ret;
            }
            inline Iterator operator--(int32_t)
            {
                Iterator ret = *this;
                --mIndex;
                return ret;
            }
            inline Iterator &operator=(const Iterator &rhs)
            {
                mIndex = rhs.mIndex;
                return *this;
            }

            inline bool operator==(const Iterator &rhs) { return mIndex == rhs.mIndex; }
            inline bool operator!=(const Iterator &rhs) { return mIndex != rhs.mIndex; }

            inline size_t operator*() { return mIndex; }
        };

    protected:
        Type mArr[col];

    public:
        inline SArray()
        {
            for (size_t i = 0; i < col; ++i)
                constructor<Type>(&mArr[i]);
        }
        inline ~SArray()
        {
            for (size_t i = 0; i < col; ++i)
                destructor<Type>(&mArr[i]);
        }

        inline size_t getCol() const { return col; }

        inline Type &operator[](size_t index)
        {
            make_ensure(index < col);
            return mArr[index];
        }
        inline Iterator begin() { return Iterator(0); }
        inline Iterator end() { return Iterator(col); }
        inline Type &operator[](const Iterator &iter)
        {
            make_ensure(iter.mIndex < col);
            return mArr[iter.mIndex];
        }
    };
    // 二维数组
    template <typename Type, const size_t col, const size_t col2>
    class SArray2
    {
    public:
        // 迭代器
        class Iterator
        {
        private:
            friend class SArray2;
            size_t mI, mJ;
            explicit Iterator(const size_t i = col, const size_t j = col2) : mI(i), mJ(j) {}

        public:
            Iterator(const Iterator &rhs) : mI(rhs.mI), mJ(rhs.mJ) {}

            // 是否有效
            bool valid() const { return mI < col && mJ < col2; }

            const size_t i() const { return mI; }
            const size_t j() const { return mJ; }

            // 重载操作符
            // 前置 ++、--
            inline Iterator &operator++()
            {
                ++mJ;
                if (mJ == col2)
                {
                    mJ = 0;
                    ++mI;
                }
                return *this;
            }
            inline Iterator &operator--()
            {
                --mJ;
                if (mJ == 0)
                {
                    mJ = col2 - 1;
                    --mI;
                }
                return *this;
            }
            // 后置 ++、--
            inline Iterator operator++(int32_t)
            {
                Iterator ret = *this;
                ++mJ;
                if (mJ == col2)
                {
                    mJ = 0;
                    ++mI;
                }
                return ret;
            }
            inline Iterator operator--(int32_t)
            {
                Iterator ret = *this;
                --mJ;
                if (mJ == 0)
                {
                    mJ = col2 - 1;
                    --mI;
                }
                return ret;
            }
            inline Iterator &operator=(const Iterator &rhs)
            {
                mI = rhs.mI;
                mJ = rhs.mJ;
                return *this;
            }

            inline bool operator==(const Iterator &rhs) { return mI == rhs.mI && mJ == rhs.mJ; }
            inline bool operator!=(const Iterator &rhs) { return mI != rhs.mI || mJ != rhs.mJ; }
        };

    protected:
        SArray<Type, col> mArr[col2];

    public:
        inline SArray2()
        {
            for (size_t i = 0; i < col2; ++i)
                constructor<SArray<Type, col>>(&mArr[i]);
        }
        inline ~SArray2()
        {
            for (size_t i = 0; i < col2; ++i)
                destructor<SArray<Type, col>>(&mArr[i]);
        }
        inline size_t getCol() const { return col; }
        inline size_t getCol2() const { return col2; }

        inline SArray<Type, col> &operator[](size_t index)
        {
            make_ensure(index < col2);
            return mArr[index];
        }
        inline Iterator begin() { return Iterator(0, 0); }
        inline Iterator end() { return Iterator(col, 0); }
        inline Type &operator[](const Iterator &iter)
        {
            make_ensure(iter.valid());
            return mArr[iter.mI][iter.mJ];
        }
    };
    // 三维数组
    template <typename Type, const size_t col, const size_t col2, const size_t col3>
    class SArray3
    {
    public:
        // 迭代器
        class Iterator
        {
        private:
            friend class SArray3;
            size_t mI, mJ, mK;
            explicit Iterator(const size_t i = col, const size_t j = col2, const size_t k = col3) : mI(i), mJ(j), mK(k) {}

        public:
            Iterator(const Iterator &rhs) : mI(rhs.mI), mJ(rhs.mJ), mK(rhs.mK) {}

            // 是否有效
            bool valid() const { return mI < col && mJ < col2 && mK < col3; }

            const size_t i() const { return mI; }
            const size_t j() const { return mJ; }
            const size_t k() const { return mK; }

            // 重载操作符
            // 前置 ++、--
            inline Iterator &operator++()
            {
                ++mK;
                if (mK == col3)
                {
                    mK = 0;
                    ++mJ;
                    if (mJ == col2)
                    {
                        mJ = 0;
                        ++mI;
                    }
                }
                return *this;
            }
            inline Iterator &operator--()
            {
                -mK;
                if (mK == 0)
                {
                    mK = col3 - 1;
                    ++mJ;
                    if (mJ == 0)
                    {
                        mJ = col2 - 1;
                        ++mI;
                    }
                }
                return *this;
            }
            // 后置 ++、--
            inline Iterator operator++(int32_t)
            {
                Iterator ret = *this;
                ++mK;
                if (mK == col3)
                {
                    mK = 0;
                    ++mJ;
                    if (mJ == col2)
                    {
                        mJ = 0;
                        ++mI;
                    }
                }
                return ret;
            }
            inline Iterator operator--(int32_t)
            {
                Iterator ret = *this;
                -mK;
                if (mK == 0)
                {
                    mK = col3 - 1;
                    ++mJ;
                    if (mJ == 0)
                    {
                        mJ = col2 - 1;
                        ++mI;
                    }
                }
                return ret;
            }
            inline Iterator &operator=(const Iterator &rhs)
            {
                mI = rhs.mI;
                mJ = rhs.mJ;
                mK = rhs.mK;
                return *this;
            }

            inline bool operator==(const Iterator &rhs) { return mI == rhs.mI && mJ == rhs.mJ && mK == rhs.mK; }
            inline bool operator!=(const Iterator &rhs) { return mI != rhs.mI || mJ != rhs.mJ || mK != rhs.mK; }
        };

    protected:
        SArray2<Type, col, col2> mArr[col3];

    public:
        inline SArray3()
        {
            for (size_t i = 0; i < col3; ++i)
                constructor<SArray2<Type, col, col2>>(&mArr[i]);
        }
        inline ~SArray3()
        {
            for (size_t i = 0; i < col3; ++i)
                destructor<SArray2<Type, col, col2>>(&mArr[i]);
        }
        inline size_t getCol() const { return col; }
        inline size_t getCol2() const { return col2; }
        inline size_t getCol3() const { return col3; }

        inline SArray2<Type, col, col2> &operator[](size_t index)
        {
            make_ensure(index < col3);
            return mArr[index];
        }

        inline Iterator begin() { return Iterator(0, 0, 0); }
        inline Iterator end() { return Iterator(col, 0, 0); }
        inline Type &operator[](const Iterator &iter)
        {
            make_ensure(iter.valid());
            return mArr[iter.mI][iter.mJ][iter.mK];
        }
    };
}

#endif //!__AIRARRAY__H__