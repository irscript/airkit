#ifndef __AIRILIST__H__
#define __AIRILIST__H__

#include <Airkit/Precompiled.h>

namespace airkit
{

    // 双向循环链表节点
    template <typename Node>
    struct IListNode
    {

        Node *mNext; // 后继节点
        Node *mPrev; // 前驱节点

        inline IListNode() { init(); }
        // 初始化节点
        inline void init() { mNext = nullptr, mPrev = nullptr; }
        // 获取后继节点
        inline Node *getNext() { return mNext; }
        // 获取前驱节点
        inline Node *getPrev() { return mPrev; };
        // 设置后继节点
        inline void setNext(Node *next) { mNext = next; }
        // 设置前驱节点
        inline void setPrev(Node *prev) { mPrev = prev; };
        // 设置节点
        inline void setNode(Node *next, Node *prev) { mNext = next, mPrev = prev; }
        // 移除节点
        inline void remove()
        {
            if (nullptr != mPrev && nullptr != mNext)
            {
                mPrev->setNext(mNext);
                mNext->setPrev(mPrev);
                init();
            }
        }
        // 插入前驱节点
        inline void insertEntry(Node *entry)
        {
            auto cthis = (Node *)this;
            entry->setNode(cthis, cthis->mPrev);
            cthis->mPrev->setNext(entry);
            cthis->setPrev(entry);
        }
        // 插入后继节点
        inline void insertTail(Node *entry)
        {
            auto cthis = (Node *)this;
            entry->setNode(cthis->mNext, cthis);
            cthis->mNext->setPrev(entry);
            cthis->setNext(entry);
        }
    };

    // 双向循环链表
    template <typename Node>
    struct IList
    {
    protected:
        IListNode<Node> mRoot; // 链表根节点
        size_t mCount;         // 节点计数
    public:
        IList() { init(); }
        // 初始化节点
        inline void init()
        {
            auto node = (Node *)&mRoot;
            mRoot.setNode(node, node);
            mCount = 0;
        }
        // 获取根节点
        inline Node *root() { return (Node *)&mRoot; }
        // 获取头结点
        inline Node *entry() { return mRoot.getNext(); }
        inline Node *front() { return mRoot.getNext(); }
        // 获取尾结点
        inline Node *tail() { return mRoot.getPrev(); }
        inline Node *back() { return mRoot.getPrev(); }

        // 插入节点
        inline void insertEntry(Node *node)
        {
            auto root = (Node *)&mRoot;
            if (node != nullptr && node != root)
            {
                mRoot.insertTail(node);
                ++mCount;
            }
        }
        inline void insertTail(Node *node)
        {
            auto root = (Node *)&mRoot;
            if (node != nullptr && node != root)
            {
                mRoot.insertEntry(node);
                ++mCount;
            }
        }
        // 移除节点
        inline void remove(Node *node)
        {
            auto root = (Node *)&mRoot;
            if (node != nullptr && node != root)
            {
                node->remove();
                --mCount;
            }
        }
    };
}

#endif