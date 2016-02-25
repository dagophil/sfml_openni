#ifndef UTILITY_HXX
#define UTILITY_HXX

#include <stdexcept>
#include <numeric>
#include <ndarray.hxx>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "platform_support.hxx"
#include <XnCppWrapper.h>



/**
 * @brief Compute the average of the last N elements.
 */
template <typename T, unsigned int N>
class Averager
{
public:

    typedef T value_type;
    
    Averager(T const & zero)
        :
          zero_(zero)
    {}

    /**
     * @brief Add a new element to the queue and compute the new mean.
     */
    void push(value_type const & v)
    {
        values_.push_back(v);
        if (values_.size() > N)
            values_.erase(values_.begin());
        if (values_.size() > 0)
            mean_ = std::accumulate(values_.begin(), values_.end(), zero_) / static_cast<double>(values_.size());
    }
    
    /**
     * @brief Return the mean.
     */
    T mean() const
    {
        return mean_;
    }
    
    bool empty() const
    {
        return values_.empty();
    }

private:

    std::vector<T> values_;
    T mean_;
    T zero_;
    
};

/**
 * @brief The FPS class can be used to compute the mean FPS.
 */
class FPS
{
public:

    /**
     * @brief Measure the FPS (compute mean over the given time span).
     * @param span the time span
     */
    explicit FPS(float span = 1.0f)
        :
          span_(span),
          current_span_(0.0f),
          elapsed_time_(0.0f),
          last_mean_(0.0f)
    {
        if (span <= 0.0f)
            throw std::domain_error("FPS::FPS(): Time span must be greater than zero.");
    }

    /**
     * @brief Update the FPS and return the mean.
     * @return mean FPS
     */
    float update()
    {
        elapsed_time_ = clock_.GetElapsedTime();
        clock_.Reset();
        fps_.push_back(1.0f / elapsed_time_);

        current_span_ += elapsed_time_;
        if (current_span_ >= span_)
        {
            last_mean_ = std::accumulate(fps_.begin(), fps_.end(), 0.0f) / fps_.size();
            fps_.clear();
            current_span_ = 0.0f;
        }
        return last_mean_;
    }

    /**
     * @brief Return the elapsed time since the last frame.
     * @return the elapsed time
     */
    float elapsed_time() const
    {
        return elapsed_time_;
    }

private:

    sf::Clock clock_;
    std::vector<float> fps_;
    float const span_;
    float current_span_;
    float elapsed_time_;
    float last_mean_;
};

/**
 * @brief Convert the depth data to RGBA using a depth histogram.
 */
template <typename DEPTHARRAY, typename RGBAARRAY>
void depth_to_rgba(
        DEPTHARRAY const & depth_data,
        XnUInt32 z_res,
        RGBAARRAY & depth_rgba
){
    if (depth_data.width() != depth_rgba.width() || depth_data.height() != depth_rgba.height())
        throw std::runtime_error("depth_to_rgba(): Shape mismatch.");

    // Create the accumulative depth histogram.
    std::vector<double> histo(z_res, 0.0);
    size_t num_points = 0;
    for (size_t y = 0; y < depth_data.height(); ++y)
    {
        for (size_t x = 0; x < depth_data.width(); ++x)
        {
            if (depth_data(x, y) != 0)
            {
                ++histo[depth_data(x, y)];
                ++num_points;
            }
        }
    }
    for (size_t i = 1; i < histo.size(); ++i)
    {
        histo[i] += histo[i-1];
    }
    if (num_points > 0)
    {
        for (auto & h : histo)
        {
            h = (unsigned int) (256 * (1.0f - h / num_points));
        }
    }

    // Convert the depth data to RGBA.
    for (size_t y = 0; y < depth_data.height(); ++y)
    {
        for (size_t x = 0; x < depth_data.width(); ++x)
        {
            auto const v = histo[depth_data(x, y)];
            depth_rgba(x, y).r = v;
            depth_rgba(x, y).g = v;
            depth_rgba(x, y).b = 0;
            depth_rgba(x, y).a = 255;
        }
    }
}

/**
 * @brief Convert the user labels to RGBA using a (hardcoded) colormap.
 */
template <typename USERARRAY, typename RGBAARRAY>
void user_to_rgba(
        USERARRAY const & user_data,
        RGBAARRAY & user_rgba
){
    if (user_data.width() != user_rgba.width() || user_data.height() != user_rgba.height())
        throw std::runtime_error("user_to_rgba(): Shape mismatch.");

    typedef typename RGBAARRAY::value_type RGBA;

    std::vector<RGBA> colors = {
        {0, 0, 0, 0},
        {0, 0, 255, 255},
        {0, 255, 0, 255},
        {255, 0, 0, 255},
        {255, 255, 0, 255},
        {255, 0, 255, 255},
        {0, 255, 255, 255}
    };

    for (size_t y = 0; y < user_data.height(); ++y)
        for (size_t x = 0; x < user_data.width(); ++x)
            user_rgba(x, y) = colors[user_data(x, y)];
}

/**
 * Return the pointer to the first uint8 value in the given color array.
 */
template <typename ARR>
sf::Uint8 const * uint8_ptr(
        ARR const & a
){
    typedef typename ARR::value_type value_type;
    static_assert(std::is_same<value_type, sf::Color>::value, "uint8_ptr(): Only valid for sf::Color arrays.");
    return &a.front().r;
}

/**
 * Return the pointer to the first uint8 value in the given color array.
 */
template <typename ARR>
sf::Uint8 * uint8_ptr(
        ARR & a
){
    typedef typename ARR::value_type value_type;
    static_assert(std::is_same<value_type, sf::Color>::value, "uint8_ptr(): Only valid for sf::Color arrays.");
    return &a.front().r;
}

/**
 * Split the given string at the given character.
 */
std::vector<std::string> split_string(std::string const & s)
{
    std::vector<std::string> v;
    std::string word = "";
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (s[i] == ' ' || s[i] == '\n')
        {
            v.push_back(word);
            word = "";
        }
        else
        {
            word += s[i];
        }
    }
    if (word.size() > 0)
        v.push_back(word);
    return v;
}

/**
 * Inserts line breaks into the sf::String so it does not exceed max_width.
 */
void insert_line_breaks(sf::String & str, double max_width)
{
    auto const words = split_string(str.GetText());
    if (words.size() == 0)
        return;
    str.SetText(words[0]);
    for (size_t i = 1; i < words.size(); ++i)
    {
        auto const & word = words[i];
        std::string s = str.GetText();
        str.SetText(s + " " + word);
        if (str.GetCharacterPos(s.size()+word.size()).x > max_width)
            str.SetText(s + "\n" + word);
    }
}

/**
 * @brief operator- for vectors.
 */
XnVector3D operator-(XnVector3D const & a, XnVector3D const & b)
{
    XnVector3D r;
    r.X = a.X - b.X;
    r.Y = a.Y - b.Y;
    r.Z = a.Z - b.Z;
    return r;
}

/**
 * @brief Return the length of the vector.
 */
double length(XnVector3D const & a)
{
    return std::sqrt(a.X*a.X + a.Y*a.Y + a.Z*a.Z);
}

XnVector3D & operator+=(XnVector3D & a, XnVector3D const & b)
{
    a.X=a.X+b.X;
    a.Y=a.Y+b.Y;
    a.Z=a.Z+b.Z;
    return a;
}

XnVector3D operator+(XnVector3D a, XnVector3D const & b)
{
    return a += b;
}

XnVector3D operator/(XnVector3D const & a, float b)
{
    XnVector3D r;
    r.X=a.X/b;
    r.Y=a.Y/b;
    r.Z=a.Z/b;
    return r;
}

#endif
