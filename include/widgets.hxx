#ifndef WIDGETS_HXX
#define WIDGETS_HXX

#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <string>
#include <stdexcept>

#include <SFML/Graphics.hpp>

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
          hovered_(false)
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
     * @brief Set the hovered state.
     */
    void hover(DiffType x, DiffType y)
    {
        auto previously_hovered = hovered_;
        hovered_ = rect_.Contains(x, y);
        x -= rect_.Left;
        y -= rect_.Top;
        for (auto w : widgets_)
            w->hover(x, y);

        if (previously_hovered && !hovered_)
            handle_mouse_leave_();
        if (!previously_hovered && hovered_)
            handle_mouse_enter_();
        if (hovered_)
            handle_hover_(x, y);
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
        render_impl(target);

        // For correct rendering, widgets must be rendered with ascending z index.
        // Since the widgets and their z index rarely change, the widgets
        // are probably already sorted from the last call to render().
        // Therefore calling both is_sorted and sort should be more efficient
        // than always calling sort.
        auto comp = [](WidgetPointer a, WidgetPointer b)
        {
            return a->z_index_ < b->z_index_;
        };
        if (!std::is_sorted(widgets_.begin(), widgets_.end(), comp))
            std::sort(widgets_.begin(), widgets_.end(), comp);
        for (auto w : widgets_)
        {
            w->rect_.Offset(rect_.Left, rect_.Top);
            w->render(target);
            w->rect_.Offset(-rect_.Left, -rect_.Top);
        }
    }

    sf::Rect<DiffType> rect_; // position and size
    int z_index_; // the z index
    float click_delay_; // the time that is needed in hovered state until a click occurs
    std::function<void()> handle_mouse_enter_; // callback for mouse enter events
    std::function<void()> handle_mouse_leave_; // callback for mouse leave events
    std::function<void(DiffType, DiffType)> handle_hover_; // callback for hover events

protected:

    virtual void update_impl(float elapsed_time)
    {}

    virtual void render_impl(sf::RenderTarget & target)
    {}

private:

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
     * @brief Draw a rectangle.
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

    template <typename... Args>
    ImageWidget(
            std::string const & filename,
            Args... args
    )
        :
          Widget(args...)
    {
        if (!image_.LoadFromFile(filename))
            throw std::runtime_error("Could not load image " + filename);
    }

protected:

    /**
     * @brief Draw the stored image.
     */
    void render_impl(sf::RenderTarget & target)
    {
        sf::Vector2f const scale(Widget::rect_.GetWidth() / static_cast<float>(image_.GetWidth()),
                                 Widget::rect_.GetHeight() / static_cast<float>(image_.GetHeight()));
        sf::Vector2f const pos(Widget::rect_.Left, Widget::rect_.Top);
        sf::Sprite spr(image_, pos, scale);
        target.Draw(spr);
    }

private:

    sf::Image image_; // the image

};



}

#endif
