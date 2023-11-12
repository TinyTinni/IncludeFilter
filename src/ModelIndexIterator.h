#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>
#include <functional>
#include <iterator>

class ModelIndexIterator
{
    std::reference_wrapper<const QAbstractItemModel>
        mModel; // todo: model needed? use index' model?
    QModelIndex mCurrentIndex;

    QModelIndex depthFirst(QModelIndex index) const noexcept
    {
        if (auto r = mModel.get().index(0, 0, index); r.isValid())
            return r;
        // index has no child, try sibling
        do
        {
            int lastVisitedChildRow = index.row();
            index = index.parent();
            if (auto r = mModel.get().index(lastVisitedChildRow + 1, 0, index);
                r.isValid())
            {
                return r;
            }
            // if no such child exist, check if the parent has a next child
        } while (index.isValid());
        return index;
    }

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = QModelIndex;
    using pointer = QModelIndex *;
    using reference = QModelIndex &;
    using iterator_category = std::forward_iterator_tag;

    ModelIndexIterator(const QAbstractItemModel &model, QModelIndex index)
        : mModel{model}, mCurrentIndex{index}
    {
    }

    const QModelIndex &operator*() const noexcept { return mCurrentIndex; }
    QModelIndex &operator*() noexcept { return mCurrentIndex; }
    QModelIndex *operator->() noexcept { return &mCurrentIndex; }

    const ModelIndexIterator &operator++()
    {
        mCurrentIndex = depthFirst(mCurrentIndex);
        return *this;
    }

    ModelIndexIterator operator++(int)
    {
        ModelIndexIterator r(*this);
        ++(*this);
        return r;
    }

    bool operator==(const ModelIndexIterator &rhs) const noexcept
    {
        return mCurrentIndex == rhs.mCurrentIndex;
    }
    bool operator!=(const ModelIndexIterator &rhs) const noexcept
    {
        return mCurrentIndex != rhs.mCurrentIndex;
    }
};

inline ModelIndexIterator begin(const QAbstractItemModel &model)
{
    return ModelIndexIterator(model, model.index(0, 0));
}

inline ModelIndexIterator end(const QAbstractItemModel &model)
{
    return ModelIndexIterator(model, QModelIndex());
}