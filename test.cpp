#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <algorithm>
#include <initializer_list>
#include <numeric>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) : node_(node) {
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            try {
                this->node_ = other.node_;
            }
            catch (const std::bad_alloc&) {
                //throw;
            }
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return this->node_ != rhs.node_;
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return this->node_ != rhs.node_;
        }

        BasicIterator& operator++() noexcept {
            this->node_ = this->node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            SingleLinkedList<Type>::BasicIterator<ValueType>* x = new SingleLinkedList<Type>::BasicIterator<ValueType>;
            x->node_ = this->node_;
            this->node_ = this->node_->next_node;
            return *x;
        }

        [[nodiscard]] reference operator*() const noexcept {
            return reference(*this->node_);
        }

        [[nodiscard]] pointer operator->() const noexcept {
            return pointer(this->node_);
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    explicit SingleLinkedList()
        : size_(0) {
    }

    SingleLinkedList(const Type& value)
        : head_(value, new Node(head_))
        , size_(size_ != 0 ? ++size_ : 0) {
    }
    ~SingleLinkedList() {
        Clear();
    }

    SingleLinkedList(std::initializer_list<Type> values) {
        assert(size_ == 0 && head_.next_node == nullptr);
        SingleLinkedList copy;
        try {
            copy.CopyRight(values);
            //CopyRight(*copy);
            swap(copy);
        }
        catch (const std::bad_alloc&) {
            throw;
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        assert(size_ == 0 && head_.next_node == nullptr);
        SingleLinkedList copy;
        try {
            copy.CopyRight(other);
            //CopyRight(*copy);
            swap(copy);
        }
        catch (const std::bad_alloc&) {
            throw;
        }
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        SingleLinkedList copy(rhs);
        swap(copy);
        return *this;
    }

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{ head_.next_node };
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{ head_.next_node };
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{ head_.next_node };
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        Node* before = new Node;
        before->next_node = this->head_.next_node;
        return Iterator(before);
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        // Реализуйте самостоятельно
        return {};
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        // Реализуйте самостоятельно
        return {};
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }


    void swap(SingleLinkedList& other) noexcept {
        std::swap(this->head_.next_node, other.head_.next_node);
        std::swap(this->size_, other.size_);
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* insert = new Node(value, pos.node_->next_node);
        try {
            pos.node_->next_node = insert;
            ++this->size_;
        }
        catch (const std::bad_alloc&) {
            delete insert;
            throw;
        }
        return Iterator(pos.node_->next_node);
    }

    void PopFront() noexcept {
        before_begin().node_->next_node = this->begin().node_->next_node;
        --this->size_;
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        auto buffer = pos.node_->next_node;
        try {
            pos.node_->next_node = pos.node_->next_node->next_node;
            delete buffer;
        }
        catch (std::bad_alloc) {
            delete buffer;
            throw;
        }
        return Iterator(pos.node_->next_node);
    }

    void Clear() noexcept {
        while (this->head_.next_node != nullptr) {
            auto copy_head = this->head_.next_node->next_node;
            delete this->head_.next_node;
            this->head_.next_node = copy_head;
        }
        size_ = 0;
    }
private:
    Node head_;
    size_t size_ = 0;

    template <typename Move>
    void CopyRight(const Move& values) {
        Node* last = &head_;
        for (const auto& value : values) {
            last->next_node = new Node(value, nullptr);
            last = last->next_node;
            ++size_;
        }
    }
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    for (auto value : lhs) {
        auto x = std::find(rhs.begin(), rhs.end(), value);
        if (x == rhs.end()) {
            return false;
        }
    }
    return true;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {

    return std::accumulate(lhs.begin(), lhs.end(), 0) < std::accumulate(rhs.begin(), rhs.end(), 0);
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    // Заглушка. Реализуйте сравнение самостоятельно
    return std::accumulate(lhs.begin(), lhs.end(), 0) < std::accumulate(rhs.begin(), rhs.end(), 0) || lhs == rhs;
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    // Заглушка. Реализуйте сравнение самостоятельно
    return std::accumulate(lhs.begin(), lhs.end(), 0) > std::accumulate(rhs.begin(), rhs.end(), 0);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    // Заглушка. Реализуйте сравнение самостоятельно
    return std::accumulate(lhs.begin(), lhs.end(), 0) > std::accumulate(rhs.begin(), rhs.end(), 0) || lhs.GetSize() == rhs.GetSize();
}
