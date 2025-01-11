#pragma once

#include "common.h"

#include <vector>
#include <numeric>

namespace base {

// MENTION: this thing is mostly a copy from https://github.com/chrischristakis/seecs

template <class IndexType>
class BaseSparseArray {
public:
    virtual ~BaseSparseArray() = default;
    virtual size_t size() const = 0;
    virtual void clear() = 0;
    virtual void remove(IndexType realIndex) = 0;
    virtual b8 contains(IndexType realIndex) = 0;
    virtual std::vector<IndexType>& realIndexList() = 0;
    virtual const std::vector<IndexType>& realIndexList() const = 0;
};

template <class IndexType, class ValueType>
class SparseArray: public BaseSparseArray<IndexType> {
public:
    SparseArray(size_t reserve = 0) { dense_.reserve(reserve); }

    ValueType& set(IndexType realIndex, ValueType&& value) {
        const auto denseIndex = getDenseIndex(realIndex);
        if (denseIndex != INVALID_INDEX) {
            dense_[denseIndex] = std::move(value);
            denseToRealIndexMap_[denseIndex] = realIndex;
            return dense_[denseIndex];
        }
        setDenseIndex(realIndex, dense_.size());
        dense_.emplace_back(std::move(value));
        denseToRealIndexMap_.emplace_back(realIndex);
        return dense_.back();
    }

    ValueType& set(IndexType realIndex, const ValueType& value) {
        const auto denseIndex = getDenseIndex(realIndex);
        if (denseIndex != INVALID_INDEX) {
            dense_[denseIndex] = value;
            denseToRealIndexMap_[denseIndex] = realIndex;
            return dense_[denseIndex];
        }
        setDenseIndex(realIndex, dense_.size());
        dense_.emplace_back(std::move(value));
        denseToRealIndexMap_.emplace_back(realIndex);
        return dense_.back();
    }

    ValueType* get(IndexType realIndex) {
        const auto denseIndex = getDenseIndex(realIndex);
        return (denseIndex != INVALID_INDEX) ? &dense_[denseIndex] : nullptr;
    }

    ValueType& getRef(IndexType realIndex) {
        const auto denseIndex = getDenseIndex(realIndex);
        E_ASSERT(denseIndex != INVALID_INDEX);
        return dense_[denseIndex];
    }

    void remove(IndexType realIndex) override {
        const auto denseIndex = getDenseIndex(realIndex);
        if (dense_.empty() || denseIndex == INVALID_INDEX) { return; }
        setDenseIndex(denseToRealIndexMap_.back(), denseIndex);
        setDenseIndex(realIndex, INVALID_INDEX);

        std::swap(dense_.back(), dense_[denseIndex]);
        std::swap(denseToRealIndexMap_.back(), denseToRealIndexMap_[denseIndex]);

        dense_.pop_back();
        denseToRealIndexMap_.pop_back();
    }

    size_t size() const override {
        return dense_.size();
    }

    std::vector<IndexType>& realIndexList() override {
        return denseToRealIndexMap_;
    }

    const std::vector<IndexType>& realIndexList() const override {
        return denseToRealIndexMap_;
    }

    std::vector<ValueType>& data() {
        return dense_;
    }

    const std::vector<ValueType>& data() const {
        return dense_;
    }

    b8 contains(IndexType realIndex) override {
        return getDenseIndex(realIndex) != INVALID_INDEX;
    }

    void clear() override {
        dense_.clear();
        sparsePages_.clear();
        denseToRealIndexMap_.clear();
    }

private:
    static inline const size_t PAGE_SIZE{ 1024 };
    static inline constexpr IndexType INVALID_INDEX{ std::numeric_limits<IndexType>::max() };
    std::vector<std::vector<size_t>> sparsePages_;
    std::vector<IndexType> denseToRealIndexMap_;
    std::vector<ValueType> dense_;

    void setDenseIndex(IndexType realIndex, size_t denseIndex) {
        const auto page = realIndex / PAGE_SIZE;
        const auto sparseIndex = realIndex % PAGE_SIZE;
        if (page >= sparsePages_.size()) {
            sparsePages_.resize(page + 1);
        }
        auto& sparse = sparsePages_[page];
        if (sparseIndex >= sparse.size()) {
            sparse.resize(sparseIndex + 1, INVALID_INDEX);
        }
        sparse[sparseIndex] = denseIndex;
    }

    size_t getDenseIndex(IndexType realIndex) {
        const auto page = realIndex / PAGE_SIZE;
        const auto sparseIndex = realIndex % PAGE_SIZE;
        if (page < sparsePages_.size()) {
            const auto& sparse = sparsePages_[page];
            if (sparseIndex < sparse.size()) {
                return sparse[sparseIndex];
            }
        }
        return INVALID_INDEX;
    }
};

}