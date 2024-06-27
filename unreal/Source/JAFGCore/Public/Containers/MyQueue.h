// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreTypes.h"
#include "Templates/UnrealTemplate.h"
#include "HAL/PlatformAtomics.h"
#include "HAL/PlatformMisc.h"

/**
 * Shamelessly copied from the unreal engine source code definition for queues.
 * Basically the same as TQueue but with a different name and added functionality that should never be
 * part of a queue implementation (but are here because of some bad design decisions).
 */
template <typename T, EQueueMode Mode = EQueueMode::Spsc>
class TMyQueue final
{

public:

    using FElementType = T;

    TMyQueue()
    {
        this->Head = this->Tail = new TNode();
    }

    ~TMyQueue(void)
    {
        while (this->Tail != nullptr)
        {
            const TNode* Node = this->Tail;
            this->Tail = this->Tail->NextNode;

            delete Node;
        }
    }

    /**
     * Removes and returns the item from the tail of the queue.
     * @note To be called only from consumer thread.
     */
    FORCEINLINE bool Dequeue(FElementType& OutItem)
    {
        TNode* Popped = this->Tail->NextNode;

        if (Popped == nullptr)
        {
            return false;
        }

        TSAN_AFTER(&Tail->NextNode);
        OutItem = MoveTemp(Popped->Item);

        const TNode* OldTail = this->Tail;
        this->Tail = Popped;
        this->Tail->Item = FElementType();
        delete OldTail;

        return true;
    }

    /**
     * Empty the queue, discarding all items.
     * @note To be called only from consumer thread.
     */
    FORCEINLINE void Empty(void)
    {
        while (Pop());
    }

    /**
     * Adds an item to the head of the queue.
     * @note To be called only from producer thread(s).
     */
    FORCEINLINE bool Enqueue(const FElementType& Item)
    {
        TNode* NewNode = new TNode(Item);

        if (NewNode == nullptr)
        {
            return false;
        }

        TNode* OldHead;

        if (Mode == EQueueMode::Mpsc)
        {
            OldHead = (TNode*)FPlatformAtomics::InterlockedExchangePtr((void**)&this->Head, NewNode);
            TSAN_BEFORE(&OldHead->NextNode);
            FPlatformAtomics::InterlockedExchangePtr((void**)&OldHead->NextNode, NewNode);
        }
        else
        {
            OldHead = this->Head;
            this->Head = NewNode;
            TSAN_BEFORE(&OldHead->NextNode);
            FPlatformMisc::MemoryBarrier();
            OldHead->NextNode = NewNode;
        }

        return true;
    }

    /**
     * Adds an item to the head of the queue.
     * @note To be called only from producer thread(s).
     */
    bool Enqueue(FElementType&& Item)
    {
        TNode* NewNode = new TNode(MoveTemp(Item));

        if (NewNode == nullptr)
        {
            return false;
        }

        TNode* OldHead;

        if (Mode == EQueueMode::Mpsc)
        {
            OldHead = (TNode*)FPlatformAtomics::InterlockedExchangePtr((void**)&this->Head, NewNode);
            TSAN_BEFORE(&OldHead->NextNode);
            FPlatformAtomics::InterlockedExchangePtr((void**)&OldHead->NextNode, NewNode);
        }
        else
        {
            OldHead = this->Head;
            this->Head = NewNode;
            TSAN_BEFORE(&OldHead->NextNode);
            FPlatformMisc::MemoryBarrier();
            OldHead->NextNode = NewNode;
        }

        return true;
    }

    /**
     * @return True if the queue is empty, false otherwise.
     * @note   To be called only from consumer thread.
     */
    FORCEINLINE bool IsEmpty(void) const
    {
        return this->Tail->NextNode == nullptr;
    }

    /**
     * Peeks at the queue's tail item without removing it.
     * @note To be called only from consumer thread.
     */
    FORCEINLINE bool Peek(FElementType& OutItem) const
    {
        if (this->Tail->NextNode == nullptr)
        {
            return false;
        }

        OutItem = this->Tail->NextNode->Item;

        return true;
    }

    /**
     * Peek at the queue's tail item without removing it.
     *
     * This version of Peek allows peeking at a queue of items that do not allow
     * copying, such as TUniquePtr.
     *
     * @return Pointer to the item, or nullptr if queue is empty
     */
    FORCEINLINE FElementType* Peek(void)
    {
        if (this->Tail->NextNode == nullptr)
        {
            return nullptr;
        }

        return &this->Tail->NextNode->Item;
    }

    FORCEINLINE const FElementType* Peek(void) const
    {
        return const_cast<TMyQueue*>(this)->Peek();
    }

    /**
     * @note To be called by the consumer thread or producer *thread* (if the queue is SPSC). If the queue is MPSC,
     *       the producer might run into some unique race conditions. But technically, this should be fine in this
     *       kind of application. [Insert sweating meme here]
     */
    FORCEINLINE bool Contains(const FElementType& Item) const
    {
        for (TNode* Node = this->Tail->NextNode; Node != nullptr; Node = Node->NextNode)
        {
            if (Node->Item == Item)
            {
                return true;
            }
        }

        return false;
    }

    /**
     * Removes the item from the tail of the queue.
     * @note To be called only from consumer thread.
     */
    FORCEINLINE bool Pop(void)
    {
        TNode* Popped = this->Tail->NextNode;

        if (Popped == nullptr)
        {
            return false;
        }

        TSAN_AFTER(&Tail->NextNode);

        const TNode* OldTail = this->Tail;
        this->Tail = Popped;
        this->Tail->Item = FElementType();
        delete OldTail;

        return true;
    }

private:

    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    struct TNode
    {
        TNode* volatile NextNode;

        FElementType Item;

        TNode() : NextNode(nullptr)
        {
        }

        explicit TNode(const FElementType& InItem) : NextNode(nullptr), Item(InItem)
        {
        }

        explicit TNode(FElementType&& InItem) : NextNode(nullptr), Item(MoveTemp(InItem))
        {
        }
    };

    MS_ALIGN(16) TNode* volatile Head GCC_ALIGN(16);
    TNode* Tail;

    TMyQueue(const TMyQueue&) = delete;
    TMyQueue& operator=(const TMyQueue&) = delete;
};
