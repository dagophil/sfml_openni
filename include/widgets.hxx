#ifndef WIDGETS_HXX
#define WIDGETS_HXX

#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <string>
#include <stdexcept>
#include <fstream>
#include <stack>

#include <SFML/Graphics.hpp>

#include "utility.hxx"
#include "ndarray.hxx"
#include "resource_manager.hxx"

namespace kin
{

class Widget;
typedef std::shared_ptr<Widget> WidgetPointer;

/**
 * @brief Parent class for all actions.
 */
class Action
{
public:

    Action()
        :
          handle_finished_(detail::do_nothing0),
          finished_(false)
    {}

    /**
     * @brief If not finished, perform the action. Return whether the action is finished.
     */
    bool act(Widget & w, float elapsed_time)
    {
        if (!finished_)
        {
            finished_ = act_impl(w, elapsed_time);
            if (finished_)
                handle_finished_();
        }
        return finished_;
    }

    std::function<void()> handle_finished_; // callback for the finish event

protected:

    virtual bool act_impl(Widget & w, float elapsed_time)
    {
        return true;
    }

private:

    bool finished_; // whether the action is finished

};

typedef std::shared_ptr<Action> ActionPointer;

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

enum Scale // this is for widgets
{
    None,
    Stretch,
    ScaleInX,
    ScaleInY
};

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

    typedef int DiffType;

    explicit Widget(int z_index = 0);

    /**
     * @brief Store a new widget.
     */
    void add_widget(WidgetPointer w);

    /**
     * @brief Remove a widget.
     */
    void remove_widget(WidgetPointer w);

    /**
     * @brief Clear all widgets.
     */
    void clear_widgets()
    {
        widgets_.clear();
    }

    /**
     * @brief Return the sub widgets.
     */
    std::vector<WidgetPointer> const & widgets() const
    {
        return widgets_;
    }

    /**
     * @brief Add a new action.
     */
    void add_action(ActionPointer a);

    /**
     * @brief Clear all actions.
     */
    void clear_actions();

    /**
     * @brief Return the actions.
     */
    std::vector<ActionPointer> const & actions() const
    {
        return actions_;
    }

    /**
     * @brief Set the hovered state. Returns the hovered widget with its z indices.
     */
    void hover(
            DiffType x,
            DiffType y,
            bool just_unhover = false
    );

    /**
     * @brief Whether the widget contains (x, y).
     */
    bool contains(
            DiffType x,
            DiffType y
    ) const;

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
    void update(float elapsed_time);

    /**
     * @brief Render the widget.
     */
    void render(
            sf::RenderTarget & target,
            DiffType parent_x = 0,
            DiffType parent_y = 0,
            DiffType parent_width = -1,
            DiffType parent_height = -1
    );

    /**
     * @brief Show the widget.
     */
    void show()
    {
        visible_ = true;
    }

    /**
     * @brief Hide the widget.
     */
    void hide()
    {
        visible_ = false;
    }

    /**
     * @brief Return whether the widget is visible.
     */
    bool visible() const
    {
        return visible_;
    }

    /**
     * @brief Getter for x.
     */
    float get_x() const
    {
        return rel_x_;
    }

    /**
     * @brief Getter for y.
     */
    float get_y() const
    {
        return rel_y_;
    }

    /**
     * @brief Setter for x.
     */
    void set_x(float x)
    {
        rel_x_ = x;
    }

    /**
     * @brief Setter for y.
     */
    void set_y(float y)
    {
        rel_y_ = y;
    }

    void move_x(float x)
    {
        set_x(get_x()+x);
    }

    void move_y(float y)
    {
        set_y(get_y()+y);
    }

    void move(float x, float y)
    {
        move_x(x);
        move_y(y);
    }

    /**
     * @brief Getter for the width.
     */
    float get_width() const
    {
        return rel_width_;
    }

    /**
     * @brief Getter for the height.
     */
    float get_height() const
    {
        return rel_height_;
    }

    /**
     * @brief Setter for width.
     */
    void set_width(float w)
    {
        rel_width_ = w;
        if (scale_ == Stretch)
            scale_ = None;
    }

    /**
     * @brief Setter for height.
     */
    void set_height(float h)
    {
        rel_height_ = h;
        if (scale_ == Stretch)
            scale_ = None;
    }

    /**
     * @brief Give a rectangle with absolute (pixel) values. All relative widget variables (x, y, width, height) are then ignored.
     */
    void overwrite_render_rectangle(sf::Rect<DiffType> const & r)
    {
        absolute_rect_ = r;
        overwrite_render_ = true;
    }

    /**
     * @brief Use the relative rectangle again.
     */
    void use_relative_rectangle()
    {
        overwrite_render_ = false;
    }

    /**
     * @brief Return the absolute rectangle.
     */
    sf::Rect<DiffType> const & get_absolute_rectangle() const
    {
        return absolute_rect_;
    }

    /**
     * @brief Return the render rectangle.
     */
    sf::Rect<DiffType> const & get_render_rectangle() const
    {
        return render_rect_;
    }

    int z_index_; // the z index
    std::function<void(DiffType, DiffType)> handle_mouse_enter_; // callback for mouse enter events
    std::function<void(DiffType, DiffType)> handle_mouse_leave_; // callback for mouse leave events
    std::function<void(DiffType, DiffType)> handle_hover_; // callback for hover events
    bool hoverable_; // whether the widget reacts for hovers
    AlignX align_x_; // the horizontal align
    AlignY align_y_; // the vertical align
    Scale scale_; // the scale method
    float ratio_; // scale ratio (width divided by height), used if scale_ is ScaleInX or ScaleInY.

protected:

    virtual void hover_impl(
            DiffType x,
            DiffType y,
            bool just_unhover
    )
    {}

    virtual void update_impl(float elapsed_time)
    {}

    virtual void render_impl(sf::RenderTarget & target)
    {}

    sf::Rect<DiffType> render_rect_; // the rectangle that is used for rendering

private:

    /**
     * @brief Sort the contained widgets by ascending z index.
     *
     * This will be called very often. Since the widgets and their z index
     * change only seldom, they will probably already be sorted. Therefore
     * calling both is_sorted and sort should be more efficient than always
     * calling sort.
     */
    void sort_widgets();

    std::vector<WidgetPointer> widgets_; // contained widgets
    bool hovered_; // whether the widget is hovered
    bool visible_; // whether the widget is visible
    std::vector<ActionPointer> actions_; // the actions

    float rel_x_; // x position relative to the parent
    float rel_y_; // y position relative to the parent
    float rel_width_; // width relative to the parent (0.5: half the parent width)
    float rel_height_; // height relative to the parent (0.5: half the parent height)

    bool overwrite_render_; // whether to overwrite the render rectangle with some absolute values
    sf::Rect<DiffType> absolute_rect_; // the rectangle that is used to overwrite the render rectangle

};

Widget::Widget(
        int z_index
)
    :
      z_index_(z_index),
      handle_mouse_enter_(detail::do_nothing2<DiffType, DiffType>),
      handle_mouse_leave_(detail::do_nothing2<DiffType, DiffType>),
      handle_hover_(detail::do_nothing2<DiffType, DiffType>),
      hoverable_(true),
      align_x_(Left),
      align_y_(Top),
      scale_(Stretch),
      hovered_(false),
      visible_(true),
      rel_x_(0.0),
      rel_y_(0.0),
      rel_width_(1.0),
      rel_height_(1.0),
      render_rect_(0, 0, 0, 0),
      overwrite_render_(false)
{}

void Widget::add_widget(WidgetPointer w)
{
    widgets_.push_back(w);
}

void Widget::remove_widget(WidgetPointer w)
{
    auto it = std::find(widgets_.begin(), widgets_.end(), w);
    if (it != widgets_.end())
        widgets_.erase(it);
}

void Widget::add_action(ActionPointer a)
{
    actions_.push_back(a);
}

void Widget::clear_actions()
{
    actions_.clear();
}

void Widget::hover(
        DiffType x,
        DiffType y,
        bool just_unhover
){
    auto previously_hovered = hovered_;

    if (just_unhover)
    {
        // Just unhover this and all sub widgets.
        hovered_ = false;
        for (auto w : widgets_)
            w->hover(x, y, true);
    }
    else
    {
        // Check if the mouse is inside and set the hover state accordingly.
        // If a sub widget is hovered, all other sub widgets cannot be hovered.
        hovered_ = render_rect_.Contains(x, y);
        sort_widgets();
        for (auto it = widgets_.rbegin(); it != widgets_.rend(); ++it)
        {
            auto & w = *it;
            w->hover(x, y, just_unhover);
            if (w->hovered() && w->hoverable_ && w->visible())
                just_unhover = true;
        }
    }

    hover_impl(x, y, just_unhover);

    // Raise the hover events.
    if (visible())
    {
        if (previously_hovered && !hovered_)
            handle_mouse_leave_(x, y);
        if (!previously_hovered && hovered_)
            handle_mouse_enter_(x, y);
        if (hovered_)
            handle_hover_(x, y);
    }
}

bool Widget::contains(DiffType x, DiffType y) const
{
    return render_rect_.Contains(x, y);
}

void Widget::update(float elapsed_time)
{
    update_impl(elapsed_time);
    auto action_copy = actions_;
    for (auto a : action_copy)
    {
        if (a->act(*this, elapsed_time))
        {
            auto it = std::find(actions_.begin(), actions_.end(), a);
            if (it != actions_.end())
                actions_.erase(it);
        }
    }
    for (auto w : widgets_)
        w->update(elapsed_time);
}

void Widget::render(
        sf::RenderTarget & target,
        DiffType parent_x,
        DiffType parent_y,
        DiffType parent_width,
        DiffType parent_height
){
    if (parent_width == -1)
        parent_width = target.GetWidth();
    if (parent_height == -1)
        parent_height = target.GetHeight();

    if (overwrite_render_)
    {
        // Overwrite the render rectangle with the given one.
        render_rect_ = absolute_rect_;
    }
    else
    {
        // Compute the correct render width and height.
        render_rect_.Left = 0;
        render_rect_.Top = 0;
        if (scale_ == Stretch)
        {
            render_rect_.Right = parent_width;
            render_rect_.Bottom = parent_height;
        }
        else if (scale_ == ScaleInX)
        {
            render_rect_.Bottom = std::round(rel_height_ * parent_height);
            render_rect_.Right = std::round(ratio_ * render_rect_.Bottom);
        }
        else if (scale_ == ScaleInY)
        {
            render_rect_.Right = std::round(rel_width_ * parent_width);
            render_rect_.Bottom = std::round(render_rect_.Right / ratio_);
        }
        else // scale_ == None
        {
            render_rect_.Right = std::round(rel_width_ * parent_width);
            render_rect_.Bottom = std::round(rel_height_ * parent_height);
        }

        // Compute the correct render position in x.
        if (align_x_ == Left)
            render_rect_.Offset(std::round(rel_x_ * parent_width), 0);
        else if (align_x_ == Right)
            render_rect_.Offset(std::round((1 - rel_x_) * parent_width - render_rect_.GetWidth()), 0);
        else // align_x_ == CenterX
            render_rect_.Offset(std::round(0.5 * (parent_width - render_rect_.GetWidth()) + rel_x_*parent_width), 0);

        // Compute the correct render position in y.
        if (align_y_ == Top)
            render_rect_.Offset(0, std::round(rel_y_ * parent_height));
        else if (align_y_ == Bottom)
            render_rect_.Offset(0, std::round((1 - rel_y_) * parent_height - render_rect_.GetHeight()));
        else // align_y_ == CenterY
            render_rect_.Offset(0, std::round(0.5 * (parent_height - render_rect_.GetHeight()) + rel_y_*parent_height));

        // Move the widget relative to the parent.
        render_rect_.Offset(parent_x, parent_y);
    }

    // Draw the widget.
    if (visible_)
    {
        // For correct rendering, widgets must be rendered with ascending z index.
        render_impl(target);
        sort_widgets();
        for (auto w : widgets_)
        {
            w->render(target, render_rect_.Left, render_rect_.Top, render_rect_.GetWidth(), render_rect_.GetHeight());
        }
    }
}

void Widget::sort_widgets()
{
    auto comp = [](WidgetPointer a, WidgetPointer b)
    {
        return a->z_index_ < b->z_index_;
    };
    if (!std::is_sorted(widgets_.begin(), widgets_.end(), comp))
        std::sort(widgets_.begin(), widgets_.end(), comp);
}

/**
 * @brief The grid widget.
 */
class GridWidget : public Widget
{

public:

    typedef std::shared_ptr<Widget> WidgetPointer;

    GridWidget(size_t n_x, size_t n_y)
        :
          n_x_(n_x),
          n_y_(n_y),
          grid_(n_x, n_y, nullptr)
    {
        hoverable_ = false;
        for (auto & w : grid_)
        {
            w = std::make_shared<Widget>();
            w->hoverable_ = false;
        }
    }

    /**
     * @brief Return the element at (x, y).
     */
    WidgetPointer & operator()(int x, int y)
    {
        return grid_(x, y);
    }

    /**
     * @brief Return the element at (x, y).
     */
    WidgetPointer const & operator()(int x, int y) const
    {
        return grid_(x, y);
    }

    /**
     * @brief Return the i-th element of a 1-dimensional grid.
     */
    WidgetPointer & operator()(int i)
    {
        if (n_x_ == 1)
            return grid_(0, i);
        else if (n_y_ == 1)
            return grid_(i, 0);
        else
            throw std::runtime_error("GridWidget::operator(): Tried to access 2-dimensional grid with single index.");
    }

    /**
     * @brief Return the i-th element of a 1-dimensional grid.
     */
    WidgetPointer const & operator()(int i) const
    {
        if (n_x_ == 1)
            return grid_(0, i);
        else if (n_y_ == 1)
            return grid_(i, 0);
        else
            throw std::runtime_error("GridWidget::operator(): Tried to access 2-dimensional grid with single index.");
    }

    /**
     * @brief Set the column sizes.
     */
    template <typename... Args>
    void set_x_sizes(Args&& ... args)
    {
        set_sizes(x_pos_, n_x_, args...);
    }

    /**
     * @brief Set the row sizes.
     */
    template <typename... Args>
    void set_y_sizes(Args&& ... args)
    {
        set_sizes(y_pos_, n_y_, args...);
    }

    /**
     * @brief Clear the column sizes.
     */
    void clear_x_sizes()
    {
        x_pos_.clear();
    }

    /**
     * @brief Clear the row sizes.
     */
    void clear_y_sizes()
    {
        y_pos_.clear();
    }

    size_t const n_x_; // number of widgets in x direction
    size_t const n_y_; // number of widgets in y direction

protected:

    void hover_impl(
            DiffType mx,
            DiffType my,
            bool just_unhover
    ){
        for (auto & w : grid_)
            w->hover(mx, my, just_unhover);
    }

    /**
     * @brief Update all contained widgets.
     */
    void update_impl(float elapsed_time)
    {
        for (auto & w : grid_)
            w->update(elapsed_time);
    }

    /**
     * @brief Render all contained widgets.
     */
    void render_impl(sf::RenderTarget & target)
    {
        for (size_t y = 0; y < n_y_; ++y)
        {
            for (size_t x = 0; x < n_x_; ++x)
            {
                if (grid_(x, y) != nullptr)
                {
                    // Draw the current grid element.
                    auto pos = render_pos(x, y);
                    auto size = render_size(x, y);
                    grid_(x, y)->render(target, std::round(pos.x), std::round(pos.y), std::round(size.x), std::round(size.y));
                }
            }
        }
    }

private:

    /**
     * @brief Normalize v, accumulate its values, and add a leading zero.
     */
    template <typename... Args>
    void set_sizes(std::vector<double> & v, size_t n, Args&& ... args)
    {
        // Write the sizes into the vector.
        v = {args...};
        if (v.size() != n)
            throw std::runtime_error("GridWidget::set_sizes(): Wrong number of arguments.");

        // Normalize the values.
        auto sum = std::accumulate(v.begin(), v.end(), 0.0);
        for (auto & x : v)
            x /= sum;

        // Accumulate the sizes so they become a position.
        for (size_t i = 1; i < v.size(); ++i)
            v[i] += v[i-1];

        // Add a leading zero. Now (v[i], v[i+1]) are the borders of the i-th element.
        v.insert(v.begin(), 0);
    }

    /**
     * @brief Compute the render position of the element at (x, y).
     */
    sf::Vector2f render_pos(size_t x, size_t y)
    {
        sf::Vector2f pos;
        if (x_pos_.empty())
            pos.x = render_rect_.GetWidth() * x / static_cast<float>(n_x_) + render_rect_.Left;
        else
            pos.x = render_rect_.GetWidth() * x_pos_[x] + render_rect_.Left;
        if (y_pos_.empty())
            pos.y = render_rect_.GetHeight() * y / static_cast<float>(n_y_) + render_rect_.Top;
        else
            pos.y = render_rect_.GetHeight() * y_pos_[y] + render_rect_.Top;
        return pos;
    }

    /**
     * @brief Compute the render size of the element at (x, y).
     */
    sf::Vector2f render_size(size_t x, size_t y)
    {
        sf::Vector2f size;
        if (x_pos_.empty())
            size.x = render_rect_.GetWidth() / static_cast<float>(n_x_);
        else
            size.x = render_rect_.GetWidth() * (x_pos_[x+1] - x_pos_[x]);
        if (y_pos_.empty())
            size.y = render_rect_.GetHeight() / static_cast<float>(n_y_);
        else
            size.y = render_rect_.GetHeight() * (y_pos_[y+1] - y_pos_[y]);
        return size;
    }

    Array2D<WidgetPointer> grid_; // the grid
    std::vector<double> x_pos_; // the x-positions of the grid (the borders of the i-th column are x_pos_[i], x_pos_[i+1]).
    std::vector<double> y_pos_; // the y-positions of the grid (the borders of the i-th row are y_pos_[i], y_pos_[i+1]).

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
            Args&& ... args
    )
        :
          T(args...),
          handle_click_(detail::do_nothing2<DiffType, DiffType>),
          click_delay_(2.0),
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
            handle_click_(T::render_rect_.GetWidth()/2, T::render_rect_.GetHeight()/2);
            clicked_ = true;
        }

        T::update_impl(elapsed_time);
    }

private:

    float hover_time_; // the current hover time
    bool clicked_; // whether a click event has been raised in the current hover phase

};

///////////////////////////////////////////////
//           Widget implementations          //
///////////////////////////////////////////////

/**
 * @brief A widget that is filled with the given color.
 */
class ColorWidget : public Widget
{
public:

    template <typename... Args>
    ColorWidget(
            sf::Color const & color,
            Args&& ... args
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
        auto bg = sf::Shape::Rectangle(render_rect_.Left, render_rect_.Top, render_rect_.Right, render_rect_.Bottom, color_);
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
            Args&& ... args
    )
        :
          Widget(args...),
          image_(resource_manager.get_image(filename))
    {
        ratio_ = image_.GetWidth() / static_cast<float>(image_.GetHeight());
    }

protected:

    /**
     * @brief Render the stored image.
     */
    void render_impl(sf::RenderTarget & target)
    {
        auto x = static_cast<float>(render_rect_.Left);
        auto y = static_cast<float>(render_rect_.Top);
        auto scale_x = render_rect_.GetWidth() / static_cast<float>(image_.GetWidth());
        auto scale_y = render_rect_.GetHeight() / static_cast<float>(image_.GetHeight());
        sf::Sprite spr(image_, {x, y}, {scale_x, scale_y});
        target.Draw(spr);
    }

private:

    sf::Image & image_; // the image

};

/**
 * @brief A widget for displaying text.
 */
class TextWidget : public Widget
{
public:

    template <typename... Args>
    TextWidget(
            std::string const & text,
            Args&& ... args
    )
        :
          Widget(args...),
          text_(text),
          style_(sf::String::Style::Regular),
          font_size_(16),
          color_({255, 255, 255}),
          text_align_x_(Left),
          text_align_y_(Top),
          bg_color_({0, 0, 0, 0})
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
        insert_line_breaks(text_obj_, render_rect_.GetWidth());
        set_position();

        auto bg = sf::Shape::Rectangle(render_rect_.Left, render_rect_.Top, render_rect_.Right, render_rect_.Bottom, bg_color_);
        target.Draw(bg);
        target.Draw(text_obj_);
    }

    std::string text_; // the text
    sf::String::Style style_; // the text style
    double font_size_; // the font size
    sf::Color color_; // the text color
    AlignX text_align_x_; // the horizontal align
    AlignY text_align_y_; // the vertical align
    sf::Color bg_color_; // the background color

private:

    void set_position()
    {
        text_obj_.SetPosition(render_rect_.Left, render_rect_.Top);
        float x  = render_rect_.Left;
        if (text_align_x_ == Left)
            x += 0;
        else if (text_align_x_ == Right)
            x += render_rect_.GetWidth() - text_obj_.GetRect().GetWidth();
        else
            x += (render_rect_.GetWidth() - text_obj_.GetRect().GetWidth()) / 2.0;

        float y = render_rect_.Top;
        if (text_align_y_ == Top)
            y += 0;
        else if (text_align_y_ == Bottom)
            y += render_rect_.GetHeight() - text_obj_.GetRect().GetHeight();
        else
            y += (render_rect_.GetHeight() - text_obj_.GetRect().GetHeight()) / 2.0;

        text_obj_.SetPosition(x, y);
    }

    sf::String text_obj_; // the text object that is rendered

};

namespace detail
{
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
            index = (index+1) % size();
            return *this;
        }

        SpriteIndex & operator--()
        {
            if (index == 0)
                index = size()-1;
            else
                --index;
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
} // namespace detail

std::string read_animation_image_file(std::string const & filename)
{
    std::ifstream f(filename);
    std::string image_filename;
    f >> image_filename;
    return image_filename;
}

/**
 * @brief Widget for animations.
 */
class AnimatedWidget : public Widget
{
public:

    template <typename... Args>
    AnimatedWidget(
            std::string const & filename,
            Args&& ... args
    )
        :
          Widget(args...),
          repeatable_(true),
          backwards_(false),
          freeze_finish_(false),
          i_(0),
          runtime_(0),
          running_(true),
          image_(resource_manager.get_image(read_animation_image_file(filename)))
    {
        std::ifstream f(filename);

        // Read the image filename.
        std::string image_filename;
        f >> image_filename;

        // Read number of rows and columns.
        f >> i_.rows_ >> i_.cols_;

        // Read the frame times.
        bool found_frametime = false;
        for (size_t i = 0; i < i_.size(); ++i)
        {
            float t;
            f >> t;
            if (f.fail())
            {
                if (!found_frametime)
                    throw std::runtime_error("Error in animation " + filename);
                frametimes_.push_back(frametimes_.back());
            }
            else
            {
                frametimes_.push_back(t);
                found_frametime = true;
            }
        }

        // Set the ratio.
        ratio_ = width() / static_cast<float>(height());
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
        if (backwards_)
            i_.index = i_.size()-1;
        else
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

    /**
     * @brief Set the direction to backwards.
     */
    void backwards(bool b)
    {
        backwards_ = b;
    }

    /**
     * @brief Return whether the animation run backwards.
     */
    bool backwards() const
    {
        return backwards_;
    }

    /**
     * @brief Return whether the animation currently runs.
     */
    bool running() const
    {
        return running_;
    }

    /**
     * @brief Go to the next frame.
     * @warning repeatable and freeze_finish will be ignored.
     */
    void next_frame()
    {
        if (backwards_)
            --i_;
        else
            ++i_;
    }

    bool repeatable_; // whether the animations is repeated
    bool freeze_finish_; // whether the animation finishs at the last frame

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
                if (backwards_)
                {
                    --i_;
                    if (i_.index == i_.size()-1 && !repeatable_)
                    {
                        stop_backwards();
                    }
                }
                else
                {
                    ++i_;
                    if (i_.index == 0 && !repeatable_)
                    {
                        stop_forwards();
                    }
                }
                runtime_ -= f;
            }
        }
    }

    /**
     * @brief Draw the current frame.
     */
    void render_impl(sf::RenderTarget & target)
    {
        auto px = static_cast<float>(render_rect_.Left);
        auto py = static_cast<float>(render_rect_.Top);
        auto scale_x = render_rect_.GetWidth() / static_cast<float>(width());
        auto scale_y = render_rect_.GetHeight() / static_cast<float>(height());

        sf::Sprite spr(image_);
        spr.SetSubRect(sf::IntRect(x(), y(), x()+width(), y()+height()));
        spr.SetPosition(px, py);
        spr.SetScale(scale_x, scale_y);
        target.Draw(spr);
    }

private:

    /**
     * @brief Stop the animation (running backwards).
     */
    void stop_backwards()
    {
        stop();
        if (freeze_finish_)
            i_.index = 0;
        else
            i_.index = i_.size()-1;
    }

    /**
     * @brief Stop the animation (running forwards).
     */
    void stop_forwards()
    {
        stop();
        if (freeze_finish_)
            i_.index = i_.size()-1;
        else
            i_.index = 0;

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

    sf::Image & image_; // the sprite image
    std::vector<float> frametimes_; // the length (in seconds) of the single frames
    detail::SpriteIndex i_; // the sprite index
    float runtime_; // the current runtime
    bool running_; // whether the animation is currently running
    bool backwards_; // whether the animation runs backwards

};

///////////////////////////////////////////////
//           Action implementations          //
///////////////////////////////////////////////

/**
 * @brief Wait the given time, then perform the action.
 */
class DelayedAction : public Action
{
public:

    DelayedAction(float delay, ActionPointer action)
        :
          elapsed_time_(0),
          delay_(delay),
          action_(action)
    {}

protected:

    /**
     * @brief If the delay is over, call the stored action.
     */
    bool act_impl(Widget & w, float elapsed_time)
    {
        elapsed_time_ += elapsed_time;
        if (elapsed_time_ >= delay_)
            return action_->act(w, elapsed_time);
        else
            return false;
    }

private:

    float elapsed_time_; // the elapsed time
    float const delay_; // the time until the action is performed
    ActionPointer action_; // the action
};

/**
 * @brief Act with the given function.
 */
class FunctionAction : public Action
{
public:
    FunctionAction(std::function<bool(Widget &, float)> f)
        :
          f_(f)
    {}
protected:
    bool act_impl(Widget &w, float elapsed_time)
    {
        return f_(w, elapsed_time);
    }
private:
    std::function<bool(Widget &, float)> f_;
};

/**
 * @brief Perform the stored actions one after another.
 */
class ChainedAction : public Action
{
public:

    template <typename... Args>
    ChainedAction(Args&& ... args)
    {
        fill_actions(args...);
    }

protected:

    /**
     * @brief Act with the current action. Removed an action when it finished.
     */
    bool act_impl(Widget & w, float elapsed_time)
    {
        if (actions_.empty())
        {
            return true;
        }
        else
        {
            auto remove = actions_.top()->act(w, elapsed_time);
            if (remove)
                actions_.pop();
            return actions_.empty();
        }
    }

private:

    void fill_actions()
    {}

    template <typename... Args>
    void fill_actions(ActionPointer a, Args&& ... args)
    {
        fill_actions(args...);
        actions_.push(a);
    }

    std::stack<ActionPointer> actions_; // the actions
};

/**
 * @brief Hide the widget.
 */
class HideAction : public Action
{
protected:
    bool act_impl(Widget & w, float elapsed_time)
    {
        w.hide();
        return true;
    }
};

/**
 * @brief Show the widget.
 */
class ShowAction : public Action
{
protected:
    bool act_impl(Widget &w, float elapsed_time)
    {
        w.show();
        return true;
    }
};

/**
 * @brief Shrink a widget in a given time.
 */
class ShrinkAction : public Action
{

public:

    ShrinkAction(float time)
        :
          elapsed_time_(0),
          time_(time),
          width_original_(0),
          first_frame_(true)
    {}

protected:

    bool act_impl(Widget &w, float elapsed_time)
    {
        // Save the original width and height.
        if(first_frame_)
        {
            width_original_ = w.get_width();
            height_original_ = w.get_height();
            first_frame_ = false;
        }

        // Update the elapsed time.
        elapsed_time_ += elapsed_time;

        if(elapsed_time_ < time_)
        {
            // Rescale the widget.
            float t = 1 - elapsed_time_ / time_;
            w.set_width(t * width_original_);
            w.set_height(t * height_original_);
            return false;
        }
        else
        {
            return true;
        }
    }

private:

    float time_; //the given time.
    float elapsed_time_; //the already elapsed time.
    float width_original_; // the beginning widget width.
    float height_original_; //the beginning widget height.
    bool first_frame_; //frist frame to save beginning sizes.
};

/**
 * @brief Move the widget by the given amount in the given time.
 */
class MoveByAction : public Action
{
public:

    enum Interpolation
    {
        Linear,
        Quadratic
//        AntiQuadratic
    };

    MoveByAction(sf::Vector2f delta, float time, Interpolation inter = Linear)
        :
          delta_(delta),
          time_(time),
          elapsed_time_(0),
          current_delta_({0, 0}),
          inter_(inter)
    {}

protected:

    bool act_impl(Widget & w, float elapsed_time)
    {
        elapsed_time_ += elapsed_time;

        float t;
        if (inter_ == Linear)
            t = elapsed_time_ / time_;
        else if (inter_ == Quadratic)
        {
            t = (time_-elapsed_time_) / time_;
            t = 1.0 - t*t;
        }
        else
        {
            throw std::runtime_error("Unknown interpolation.");
        }

        if (elapsed_time_ >= time_)
        {
            auto d = delta_ - current_delta_;
            w.set_x(w.get_x() + d.x);
            w.set_y(w.get_y() + d.y);
            return true;
        }
        else
        {
            auto delta = t * delta_;
            auto d = delta - current_delta_;
            w.move(d.x, d.y);
            current_delta_ = delta;
            return false;
        }
    }

private:

    sf::Vector2f const delta_; // the total move amount
    float const time_; // the total move time
    float elapsed_time_; // the elapsed time
    sf::Vector2f current_delta_; // the move amount up to the current frame
    Interpolation const inter_; // the interpolation
};

/**
 * @brief Make the widget blink.
 */
class BlinkAction : public Action
{
public:

    BlinkAction(float delta)
        :
          delta_(delta),
          elapsed_time_(0.0)
    {}

protected:

    bool act_impl(Widget & w, float p_elapsed_time)
    {
        elapsed_time_ += p_elapsed_time;
        if (elapsed_time_ >= delta_)
        {
            elapsed_time_ -= delta_;
            if (w.visible())
                w.hide();
            else
                w.show();
        }
        return false;
    }

private:

    float const delta_;
    float elapsed_time_;
};



}

#endif
