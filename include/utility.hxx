#ifndef UTILITY_HXX
#define UTILITY_HXX

#include <stdexcept>
#include <numeric>
#include <ndarray.hxx>

#include <SFML/System.hpp>

#include <ni/XnOS.h>
#include <ni/XnCppWrapper.h>



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
        for (size_t i = 0; i < histo.size(); ++i)
        {
            histo[i] = (unsigned int) (256 * (1.0f - histo[i] / num_points));
        }
    }

    // Convert the depth data to RGBA.
    for (size_t y = 0; y < depth_data.height(); ++y)
    {
        for (size_t x = 0; x < depth_data.width(); ++x)
        {
            double const v = histo[depth_data(x, y)];
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

    std::vector<RGBA> colors;
    colors.push_back(RGBA(0, 0, 0, 0));
    colors.push_back(RGBA(0, 0, 255, 255));
    colors.push_back(RGBA(0, 255, 0, 255));
    colors.push_back(RGBA(255, 0, 0, 255));
    colors.push_back(RGBA(255, 255, 0, 255));
    colors.push_back(RGBA(255, 0, 255, 255));
    colors.push_back(RGBA(0, 255, 255, 255));

    for (size_t y = 0; y < user_data.height(); ++y)
        for (size_t x = 0; x < user_data.width(); ++x)
            user_rgba(x, y) = colors[user_data(x, y)];
}



#endif
