#ifndef WIDGETS_HXX
#define WIDGETS_HXX

#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <string>
#include <stdexcept>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "utility.hxx"

namespace kin
{

namespace detail
{
    void do_nothing0() {} // default function for widget callbacks

    template <typename A>
    void do_nothing1(A const &) {} // default function for widget callbacks

    template <typename A, typename B>
    void do_nothing2(A const &, B const &) {} // default function for widget callbacks
}

/**
 * @brief Parent class for all widgets.
 *
 * In each frame, the following methods should be called:
 * hover(x, y)
 * update(elapsed_time)
 * render(target)
 */
class Widget
{
public:

    typedef std::shared_ptr<Widget> WidgetPointer;
    typedef int DiffType;

    Widget(
            DiffType x,
            DiffType y,
            DiffType width,
            DiffType height,
            int z_index
    )
        :
          rect_(x, y, x+width, y+height),
          z_index_(z_index),
          handle_mouse_enter_(detail::do_nothing0),
          handle_mouse_leave_(detail::do_nothing0),
          handle_hover_(detail::do_nothing2<DiffType, DiffType>),
          hovered_(false),
          hoverable_(true),
          visible_(true)
    {}

    /**
     * @brief Store a new widget.
     */
    void add_widget(WidgetPointer w)
    {
        widgets_.push_back(w);
    }

    /**
     * @brief Remove a widget.
     */
    void remove_widget(WidgetPointer w)
    {
        auto it = std::find(widgets_.begin(), widgets_.end(), w);
        if (it != widgets_.end())
            widgets_.erase(it);
    }

    /**
     * @brief Return a const reference to the sub widgets.
     */
    std::vector<WidgetPointer> const & widgets() const
    {
        return widgets_;
    }

    /**
     * @brief Set the hovered state.
     */
    void hover(DiffType x, DiffType y, bool just_unhover = false)
    {
        auto previously_hovered = hovered_;

        if (just_unhover)
        {
            // Just unhover this and all sub widgets.
            hovered_ = false;
            x -= rect_.Left;
            y -= rect_.Top;
            for (auto w : widgets_)
                w->hover(x, y, true);
        }
        else
        {
            // Check if the mouse is inside and set the hover state accordingly.
            // If a sub widget is hovered, all other sub widgets cannot be hovered.
            hovered_ = rect_.Contains(x, y);
            x -= rect_.Left;
            y -= rect_.Top;
            sort_widgets();
            for (auto it = widgets_.rbegin(); it != widgets_.rend(); ++it)
            {
                auto & w = *it;
                w->hover(x, y, just_unhover);
                if (w->hovered() && w->hoverable_ && w->visible_)
                    just_unhover = true;
            }
        }

        // Raise the hover events.
        if (visible_)
        {
            if (previously_hovered && !hovered_)
                handle_mouse_leave_();
            if (!previously_hovered && hovered_)
                handle_mouse_enter_();
            if (hovered_)
                handle_hover_(x, y);
        }
    }

    /**
     * @brief Whether the widget contains (x, y).
     */
    bool contains(DiffType x, DiffType y) const
    {
        return rect_.Contains(x, y);
    }

    /**
     * @brief Whether the widget is currently hovered.
     */
    bool hovered() const
    {
        return hovered_;
    }

    /**
     * @brief Update the widgets.
     */
    virtual void update(float elapsed_time)
    {
        update_impl(elapsed_time);
        for (auto w : widgets_)
            w->update(elapsed_time);
    }

    /**
     * @brief Render the widget.
     */
    virtual void render(sf::RenderTarget & target)
    {
        if (visible_)
        {
            render_impl(target);

            // For correct rendering, widgets must be rendered with ascending z index.
            sort_widgets();
            for (auto w : widgets_)
            {
                w->rect_.Offset(rect_.Left, rect_.Top);
                w->render(target);
                w->rect_.Offset(-rect_.Left, -rect_.Top);
            }
        }
    }

    sf::Rect<DiffType> rect_; // position and size
    int z_index_; // the z index
    std::function<void()> handle_mouse_enter_; // callback for mouse enter events
    std::function<void()> handle_mouse_leave_; // callback for mouse leave events
    std::function<void(DiffType, DiffType)> handle_hover_; // callback for hover events
    bool hoverable_; // whether the widget reacts for hovers
    bool visible_; // whether the widget is visible

protected:

    virtual void update_impl(float elapsed_time)
    {}

    virtual void render_impl(sf::RenderTarget & target)
    {}

private:

    /**
     * @brief Sort the widgets by ascending z index.
     *
     * This will be called very often. Since the widgets and their z index
     * change only seldom, they will probably already be sorted. Therefore
     * calling both is_sorted and sort should be more efficient than always
     * calling sort.
     */
    void sort_widgets()
    {
        auto comp = [](WidgetPointer a, WidgetPointer b)
        {
            return a->z_index_ < b->z_index_;
        };
        if (!std::is_sorted(widgets_.begin(), widgets_.end(), comp))
            std::sort(widgets_.begin(), widgets_.end(), comp);
    }

    std::vector<WidgetPointer> widgets_; // contained widgets
    bool hovered_; // whether the widget is hovered

};

/**
 * @brief Mixin to enable hoverclick on other widget classes.
 *
 * Example:
 * Use HoverclickWidget<ImageWidget> instead of ImageWidget.
 */
template <typename T>
class HoverclickWidget : public T
{
public:

    typedef typename T::DiffType DiffType;

    template <typename... Args>
    HoverclickWidget(
            Args... args
    )
        :
          T(args...),
          handle_click_(detail::do_nothing2<DiffType, DiffType>),
          click_delay_(3.0),
          hover_time_(0.0),
          clicked_(false)
    {}

    std::function<void(DiffType, DiffType)> handle_click_; // callback for click events

    float click_delay_; // the time hover phase until a click event is raised

protected:

    /**
     * @brief Update the hover time and eventually raise the click event.
     */
    void update_impl(float elapsed_time)
    {
        if (T::hovered())
        {
            hover_time_ += elapsed_time;
        }
        else
        {
            hover_time_ = 0;
            clicked_ = false;
        }

        if (hover_time_ > click_delay_ && !clicked_)
        {
            handle_click_(T::rect_.GetWidth()/2, T::rect_.GetHeight()/2);
            clicked_ = true;
        }

        T::update_impl(elapsed_time);
    }

private:

    float hover_time_; // the current hover time
    bool clicked_; // whether a click event has been raised in the current hover phase

};

/**
 * @brief A widget that is filled with the given color.
 */
class ColorWidget : public Widget
{
public:

    template <typename... Args>
    ColorWidget(
            sf::Color const & color,
            Args... args
    )
        :
          Widget(args...),
          color_(color)
    {}

    /**
     * @brief Render the rectangle.
     */
    void render_impl(sf::RenderTarget & target)
    {
        auto bg = sf::Shape::Rectangle(rect_.Left, rect_.Top, rect_.Right, rect_.Bottom, color_);
        target.Draw(bg);
    }

private:

    sf::Color color_;

};

/**
 * @brief A widget that displays a single image.
 */
class ImageWidget : public Widget
{
public:

    enum ScaleStyle
    {
        Stretch,
        Fit,
        FitX,
        FitY
    };

    enum AlignX
    {
        Left,
        Right,
        CenterX
    };

    enum AlignY
    {
        Top,
        Bottom,
        CenterY
    };

    template <typename... Args>
    ImageWidget(
            std::string const & filename,
            Args... args
    )
        :
          Widget(args...),
          scale_style_(Stretch),
          align_x_(Left),
          align_y_(Top)
    {
        if (!image_.LoadFromFile(filename))
            throw std::runtime_error("Could not load image " + filename);
    }

    ScaleStyle scale_style_; // the stretch style
    AlignX align_x_; // the horizontal align
    AlignY align_y_; // the vertical align

protected:

    /**
     * @brief Render the stored image.
     */
    void render_impl(sf::RenderTarget & target)
    {
        auto const scale = compute_scale_factor();
        auto const pos = compute_image_position(scale.x, scale.y);
        sf::Sprite spr(image_, pos, scale);
        target.Draw(spr);
    }

private:

    /**
     * @brief Compute the scale in x and y direction.
     */
    sf::Vector2f compute_scale_factor() const
    {
        float width = rect_.GetWidth();
        float height = rect_.GetHeight();
        float image_width = image_.GetWidth();
        float image_height = image_.GetHeight();
        if (scale_style_ == Stretch)
        {
            return {width / image_width, height / image_height};
        }
        else if (scale_style_ == Fit)
        {
            float factor = height / image_height;
            if (factor*image_width > rect_.GetWidth())
                factor = rect_.GetWidth() / image_width;
            return {factor, factor};
        }
        else if (scale_style_ == FitX)
        {
            float factor = width / image_width;
            return {factor, factor};
        }
        else
        {
            float factor = height / image_height;
            return {factor, factor};
        }
    }

    /**
     * @brief Compute the image position.
     */
    sf::Vector2f compute_image_position(float scale_x, float scale_y) const
    {
        float x = rect_.Left;
        if (align_x_ == Left)
            x += 0;
        else if (align_x_ == Right)
            x += rect_.GetWidth() - scale_x * image_.GetWidth();
        else
            x += (rect_.GetWidth() - scale_x * image_.GetWidth()) / 2.0;

        float y = rect_.Top;
        if (align_y_ == Top)
            y += 0;
        else if (align_y_ == Bottom)
            y += rect_.GetHeight() - scale_y * image_.GetHeight();
        else
            y += (rect_.GetHeight() - scale_y * image_.GetHeight()) / 2.0;

        return {x, y};
    }

    sf::Image image_; // the image

};

/**
 * @brief A widget for displaying text.
 */
class TextWidget : public Widget
{
public:

    enum AlignX
    {
        Left,
        Right,
        CenterX
    };

    enum AlignY
    {
        Top,
        Bottom,
        CenterY
    };

    template <typename... Args>
    TextWidget(
            std::string const & text,
            Args... args
    )
        :
          Widget(args...),
          text_(text),
          style_(sf::String::Style::Regular),
          font_size_(16),
          color_({255, 255, 255}),
          align_x_(Left),
          align_y_(Top)
    {}

    void set_font(sf::Font const & font)
    {
        text_obj_.SetFont(font);
    }

    float text_height() const
    {
        return text_obj_.GetRect().GetHeight();
    }

    /**
     * @brief Render the text.
     */
    void render_impl(sf::RenderTarget & target)
    {
        text_obj_.SetText(text_);
        text_obj_.SetSize(font_size_);
        text_obj_.SetStyle(style_);
        text_obj_.SetColor(color_);
        insert_line_breaks(text_obj_, rect_.GetWidth());
        set_position();
        target.Draw(text_obj_);
    }

    std::string text_; // the text
    sf::String::Style style_; // the text style
    double font_size_; // the font size
    sf::Color color_; // the text color
    AlignX align_x_; // the horizontal align
    AlignY align_y_; // the vertical align

private:

    void set_position()
    {
        float x  = rect_.Left;
        if (align_x_ == Left)
            x += 0;
        else if (align_x_ == Right)
            x += rect_.GetWidth() - text_obj_.GetRect().GetWidth();
        else
            x += (rect_.GetWidth() - text_obj_.GetRect().GetWidth()) / 2.0;

        float y = rect_.Top;
        if (align_y_ == Top)
            y += 0;
        else if (align_y_ == Bottom)
            y += rect_.GetHeight() - text_obj_.GetRect().GetHeight();
        else
            y += (rect_.GetHeight() - text_obj_.GetRect().GetHeight()) / 2.0;

        text_obj_.SetPosition(x, y);
    }

    sf::String text_obj_; // the text object that is rendered

};

/**
 * @brief Index for sprites.
 */
class SpriteIndex
{
public:

    explicit SpriteIndex(
            size_t cols = 0,
            size_t rows = 0
    )   :
          cols_(cols),
          rows_(rows),
          index(0)
    {}

    SpriteIndex & operator++()
    {
        index = (index+1) % (size());
        return *this;
    }

    size_t col() const
    {
        return index % cols_;
    }

    size_t row() const
    {
        return index / cols_;
    }

    size_t size() const
    {
        return cols_ * rows_;
    }

    size_t cols_;
    size_t rows_;
    size_t index;

};

/**
 * @brief Widget for animations.
 */
class AnimatedWidget : public Widget
{
public:

    template <typename... Args>
    AnimatedWidget(
            std::string const & filename,
            Args... args
    )
        :
          Widget(args...),
          i_(0),
          runtime_(0),
          running_(true)
    {
        std::ifstream f(filename);

        // Read the image filename.
        std::string image_filename;
        f >> image_filename;
        if (!image_.LoadFromFile(image_filename))
            throw std::runtime_error("Could not load image " + image_filename);

        // Read number of rows and columns.
        f >> i_.rows_ >> i_.cols_;

        // Read the frame times.
        for (size_t i = 0; i < i_.size(); ++i)
        {
            float t;
            f >> t;
            frametimes_.push_back(t);
        }
    }

    /**
     * @brief Start the animation.
     */
    void start()
    {
        running_ = true;
    }

    /**
     * @brief Reset the animation.
     */
    void reset()
    {
        runtime_ = 0;
        i_.index = 0;
    }

    /**
     * @brief Start the animation from the beginning.
     */
    void restart()
    {
        reset();
        start();
    }

    /**
     * @brief Stop the animation.
     */
    void stop()
    {
        running_ = false;
    }

protected:

    /**
     * @brief Update the runtime and eventually show the next frame.
     */
    void update_impl(float elapsed_time)
    {
        if (running_)
        {
            runtime_ += elapsed_time;
            auto f = frametimes_[i_.index];
            if (runtime_ > f)
            {
                ++i_;
                runtime_ -= f;
            }
        }
    }

    /**
     * @brief Draw the current frame.
     */
    void render_impl(sf::RenderTarget & target)
    {
        sf::Sprite spr(image_);
        spr.SetSubRect(sf::IntRect(x(), y(), x()+width(), y()+height()));
        spr.SetPosition(rect_.Left, rect_.Top);
        spr.SetScale(compute_scale());
        target.Draw(spr);
    }

private:

    /**
     * @brief Compute the scale.
     */
    sf::Vector2f compute_scale()
    {
        return {rect_.GetWidth() / (float)width(), rect_.GetHeight() / (float)height()};
    }

    /**
     * @brief Return the image width (single frame, not the whole sprite).
     */
    size_t width() const
    {
        return image_.GetWidth() / i_.cols_;
    }

    /**
     * @brief Return the image height (single frame, not the whole sprite).
     */
    size_t height() const
    {
        return image_.GetHeight() / i_.rows_;
    }

    /**
     * @brief Return the current read x position.
     */
    size_t x() const
    {
        return i_.col() * width();
    }

    /**
     * @brief Return the curent read y position.
     */
    size_t y() const
    {
        return i_.row() * height();
    }

    sf::Image image_; // the sprite image
    std::vector<float> frametimes_; // the length (in seconds) of the single frames
    SpriteIndex i_; // the sprite index
    float runtime_; // the current runtime
    bool running_; // whether the animation is currently running

};



}

#endif
