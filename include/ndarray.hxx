#ifndef NDARRAY_HXX
#define NDARRAY_HXX

#include <vector>



template <typename T>
class Array2D
{
public:

    typedef T value_type;
    typedef value_type & reference;
    typedef value_type const & const_reference;
    typedef typename std::vector<value_type>::iterator iterator;
    typedef typename std::vector<value_type>::const_iterator const_iterator;

    explicit Array2D(size_t width = 0, size_t height = 0, value_type const & val = value_type())
        :
          data_(width*height, val),
          width_(width),
          height_(height)
    {}

    void resize(size_t width, size_t height, value_type const & val = value_type())
    {
        data_.resize(width*height, val);
        width_ = width;
        height_ = height;
    }

    size_t width() const
    {
        return width_;
    }

    size_t height() const
    {
        return height_;
    }

    reference operator()(size_t x, size_t y)
    {
        return data_[y*width_+x];
    }

    const_reference operator()(size_t x, size_t y) const
    {
        return data_[y*width_+x];
    }

    reference front()
    {
        return data_.front();
    }

    const_reference front() const
    {
        return data_.front();
    }

    reference back()
    {
        return data_.back();
    }

    const_reference back() const
    {
        return data_.back();
    }

    iterator begin()
    {
        return data_.begin();
    }

    const_iterator begin() const
    {
        return data_.begin();
    }

    const_iterator cbegin() const
    {
        return data_.cbegin();
    }

    iterator end()
    {
        return data_.end();
    }

    const_iterator end() const
    {
        return data_.end();
    }

    const_iterator cend() const
    {
        return data_.cend();
    }

private:

    std::vector<value_type> data_;
    size_t width_;
    size_t height_;

};



#endif
