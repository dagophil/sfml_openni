#ifndef MENU_OVERLAY_HXX
#define MENU_OVERLAY_HXX

#include <string>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include <tinyxml2.h>

#include "utility.hxx"
#include "widgets.hxx"
#include "options.hxx"

namespace kin
{



/**
 * @brief The MenuItem class represents a single menu item with image and text.
 */
class MenuItem : public HoverclickWidget<Widget>
{
public:

    typedef HoverclickWidget<Widget> Parent;

    explicit MenuItem(
            std::string const & img_filename = "",
            std::string const & title = "",
            std::string const & text = ""
    ){
        // Create the grid.
        auto grid_ptr = std::make_shared<GridWidget>(2, 1);
        auto & grid = *grid_ptr;
        grid.set_x_sizes(0.38, 0.62);
        add_widget(grid_ptr);

        // Create the image widget
        auto img_w = std::make_shared<ImageWidget>(img_filename, 0);
        img_w->set_height(1.0);
        img_w->scale_ = ScaleInX;
        img_w->align_x_ = Right;
        grid(0) = img_w;

        // Create the grid for the texts.
        auto text_grid_ptr = std::make_shared<GridWidget>(1, 2);
        auto & text_grid = *text_grid_ptr;
        text_grid.set_y(0.1);
        text_grid.set_y_sizes(0.36, 0.64);
        grid(1) = text_grid_ptr;

        // Create the text widget for the title.
        title_w_ = std::make_shared<TextWidget>(title);
        title_w_->style_ = sf::String::Style::Bold;
        title_w_->color_ = sf::Color(0, 0, 0);
        title_w_->set_font(opts.default_font());
        text_grid(0) = title_w_;

        // Create the text widget for the description.
        text_w_ = std::make_shared<TextWidget>(text);
        text_w_->color_ = sf::Color(0, 0, 0);
        text_w_->set_font(opts.default_font());
        text_grid(1) = text_w_;

        // Create the background image.
        auto im_bg = std::make_shared<ImageWidget>("images/menu_item_bg.png", -1);
        im_bg->scale_ = None;
        im_bg->set_x(-0.0187);
        im_bg->set_y(-0.17);
        im_bg->set_width(1.0491);
        im_bg->set_height(1.36154);
        add_widget(im_bg);
    }

protected:

    void render_impl(sf::RenderTarget &target)
    {
        title_w_->font_size_ = render_rect_.GetHeight() / 5.0;
        text_w_->font_size_ = render_rect_.GetHeight() / 6.0;
    }

private:

    std::shared_ptr<TextWidget> title_w_; // the title
    std::shared_ptr<TextWidget> text_w_; // the text

};

/**
 * @brief Throws an exception when error is an XML error.
 */
void check_xml_error(tinyxml2::XMLError error)
{
    using namespace tinyxml2;

    std::map<XMLError, std::string> const error_messages = {
        {tinyxml2::XML_ERROR_COUNT, "Count"},
        {tinyxml2::XML_ERROR_ELEMENT_MISMATCH, "Element mismatch"},
        {tinyxml2::XML_ERROR_EMPTY_DOCUMENT, "Empty document"},
        {tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED, "File could not be opened"},
        {tinyxml2::XML_ERROR_FILE_NOT_FOUND, "File not found"},
        {tinyxml2::XML_ERROR_FILE_READ_ERROR, "File read error"},
        {tinyxml2::XML_ERROR_IDENTIFYING_TAG, "Identifying tag"},
        {tinyxml2::XML_ERROR_MISMATCHED_ELEMENT, "Mismatched element"},
        {tinyxml2::XML_ERROR_PARSING, "Parsing"},
        {tinyxml2::XML_ERROR_PARSING_ATTRIBUTE, "Parsing attribute"},
        {tinyxml2::XML_ERROR_PARSING_CDATA, "Parsing CData"},
        {tinyxml2::XML_ERROR_PARSING_COMMENT, "Parsing comment"},
        {tinyxml2::XML_ERROR_PARSING_DECLARATION, "Parsing declaration"},
        {tinyxml2::XML_ERROR_PARSING_ELEMENT, "Parsing element"},
        {tinyxml2::XML_ERROR_PARSING_TEXT, "Parsing text"},
        {tinyxml2::XML_ERROR_PARSING_UNKNOWN, "Parsing unknown"}
    };

    if (error != tinyxml2::XML_NO_ERROR)
        throw std::runtime_error("XML Error: " + error_messages.at(error));
}

/**
 * @brief Throws an exception if the given pointer is a null pointer and returns the pointer otherwise.
 */
tinyxml2::XMLElement * check_xml_element(tinyxml2::XMLElement * elt)
{
    if (elt != nullptr)
        return elt;
    else
        throw std::runtime_error("XML Error: Could not find XML element.");
}

/**
 * @brief Throws an exception if the given pointer is a null pointer and returns the pointer otherwise.
 */
tinyxml2::XMLElement const * check_xml_element(tinyxml2::XMLElement const * elt)
{
    if (elt != nullptr)
        return elt;
    else
        throw std::runtime_error("XML Error: Could not find XML element.");
}

/**
 * @brief The MenuOverlay class loads menu items from an xml file and draws them.
 */
class MenuOverlay : public ColorWidget
{
public:

    typedef sf::Rect<double> Rect;

    /**
     * @brief Load the xml file.
     */
    MenuOverlay(
            std::string const & xml_filename,
            size_t screen_width,
            size_t screen_height
    );

    /**
     * @brief Add a menu item to the container.
     */
    void add_menu_item(
            std::string const & image,
            std::string const & title,
            std::string const & description,
            std::string const & command
    );

    /**
     * @brief Hide the mouse.
     */
    void hide_mouse();

    std::function<void(std::string const &)> handle_menu_item_click_; // the callback for a click on a menu item
    std::function<void()> handle_close_; // the callback for the close event

protected:

    /**
     * @brief Do the scrolling of the top and bottom scroll button.
     */
    void update_impl(float elapsed_time);

private:

    void create_default_menu_items(); // create some default menu items
    void load_xml_menu_items(std::string const & filename); // load the menu items from an xml file

    int const gap_; // distance between two menu items
    std::shared_ptr<Widget> item_container_; // the container for the menu items
    std::shared_ptr<ColorWidget> scroll_top_; // the top scroll button
    std::shared_ptr<ColorWidget> scroll_bottom_; // the bottom scroll button
    std::shared_ptr<Widget> actual_scroll_bar_; // the actual scroll widget
    sf::Vector2f scroll_wheel_; // mouse coordinates for the scroll wheel
    float scroll_movement_time_; // time that is currently used to scroll

    size_t const screen_width_;
    size_t const screen_height_;

};

MenuOverlay::MenuOverlay(
        std::string const & xml_filename,
        size_t screen_width,
        size_t screen_height
)   :
      ColorWidget(sf::Color(255, 255, 255)),
      handle_menu_item_click_(detail::do_nothing1<std::string const &>),
      handle_close_(detail::do_nothing0),
      gap_(screen_height / 20),
      scroll_movement_time_(0),
      screen_width_(screen_width),
      screen_height_(screen_height)
{
    // Create the grid.
    auto grid_ptr = std::make_shared<GridWidget>(4,1);
    auto & grid = *grid_ptr;
    add_widget(grid_ptr);
    grid.set_x_sizes(0.05, 0.7, 0.05, 0.2);

    // Create the menu item container.
    item_container_ = std::make_shared<Widget>();
    item_container_->set_y(0.16);
    grid(1)->add_widget(item_container_);

    // Create the menu items.
    if (xml_filename == "")
        create_default_menu_items();
    else
        load_xml_menu_items(xml_filename);

    // Create the close button.
    double close_height = 0.1;
    auto close_button = std::make_shared<HoverclickWidget<ImageWidget> >("images/exit_symbol.png");
    close_button->scale_ = None;
    close_button->set_height(close_height);
    grid(3)->add_widget(close_button);
    close_button->handle_click_ = [&](DiffType x, DiffType y){
        handle_close_();
    };
    attach_mouse_events(opts.mouse_, close_button);

    // Create the top scroll button.
    sf::Color const scroll_color(170, 191, 212, 190);
    scroll_top_ = std::make_shared<ColorWidget>(scroll_color, 1);
    scroll_top_->set_height(close_height);
    scroll_top_->scale_= None;
    grid(1)->add_widget(scroll_top_);

    // Create the bottom scroll button.
    scroll_bottom_ = std::make_shared<ColorWidget>(scroll_color, 1);
    scroll_bottom_->set_height(close_height);
    scroll_bottom_->scale_ = None;
    scroll_bottom_->align_y_ = Bottom;
    grid(1)->add_widget(scroll_bottom_);

    // Create the right scroll bar.
    auto scroll_bar = std::make_shared<ColorWidget>(scroll_color);
    scroll_bar->set_y(close_height);
    scroll_bar->set_height(1.0-close_height);
    grid(3)->add_widget(scroll_bar);
    actual_scroll_bar_ = std::make_shared<Widget>(1);
    actual_scroll_bar_->scale_ = None;
    actual_scroll_bar_->set_y(0.4);
    actual_scroll_bar_->set_height(0.2);
    scroll_bar->add_widget(actual_scroll_bar_);
    actual_scroll_bar_->handle_mouse_enter_ = [&](DiffType x, DiffType y) {
        scroll_wheel_.x = x;
        scroll_wheel_.y = y;
        scroll_movement_time_ = 0;
    };
    actual_scroll_bar_->handle_mouse_leave_ = [&](DiffType x, DiffType y) {
        scroll_wheel_.x = x - scroll_wheel_.x;
        scroll_wheel_.y = y - scroll_wheel_.y;
    };

    // Create the mouse animation.
    add_widget(opts.mouse_);
    handle_hover_ = [&](DiffType x, DiffType y)
    {
        opts.mouse_->show();
        DiffType w = opts.mouse_->get_absolute_rectangle().GetWidth();
        DiffType h = opts.mouse_->get_absolute_rectangle().GetHeight();
        DiffType xx = x-0.45*w;
        DiffType yy = y-0.17*h;
        opts.mouse_->overwrite_render_rectangle({xx, yy, xx+w, yy+h});
    };
}

void MenuOverlay::add_menu_item(
        std::string const & image,
        std::string const & title,
        std::string const & description,
        std::string const & command
){
    auto c = item_container_->widgets().size();
    auto w = std::make_shared<MenuItem>(
                image,
                title,
                description
    );
    w->set_height(0.15);
    w->set_y(c * 0.2);
    w->scale_ = None;
    w->handle_click_ = [this, command](DiffType x, DiffType y) {
        handle_menu_item_click_(command);
    };
    attach_mouse_events(opts.mouse_, w);
    item_container_->add_widget(w);
}

void MenuOverlay::create_default_menu_items()
{
    add_menu_item(
                "images/mario.png",
                "Mario",
                "Mario game ist echt ein supertolles Spiel.",
                "gedit /home/philip/game0.txt"
    );
    add_menu_item(
                "images/luigi.jpg",
                "Luigi",
                "Luigi game",
                "gedit /home/philip/game1.txt"
    );
    add_menu_item(
                "images/mario.png",
                "Mario 2",
                "Mario game 2",
                "gedit /home/philip/game2.txt"
    );
}

void MenuOverlay::load_xml_menu_items(std::string const & filename)
{
    // Open the xml file.
    tinyxml2::XMLDocument doc;
    check_xml_error(doc.LoadFile(filename.c_str()));
    auto root = doc.FirstChild();
    if (std::string(root->Value()) != "MENU")
        throw std::runtime_error("XML error: Root node should be MENU.");

    // Loop over the menu items in the xml file.
    bool found = false;
    for (auto elt = root->FirstChildElement("MENUITEM");
         elt != nullptr;
         elt = elt->NextSiblingElement("MENUITEM"))
    {
        found = true;

        auto img = check_xml_element(elt->FirstChildElement("IMAGE"));
        auto title = check_xml_element(elt->FirstChildElement("TITLE"));
        auto description = check_xml_element(elt->FirstChildElement("DESCRIPTION"));
        auto command = check_xml_element(elt->FirstChildElement("COMMAND"));

        add_menu_item(
                    img->GetText(),
                    title->GetText(),
                    description->GetText(),
                    command->GetText()
        );
    }
    if (!found)
        throw std::runtime_error("The menu is empty.");
}

void MenuOverlay::update_impl(
        float elapsed_time
){
    // Top scroll button.
    if (scroll_top_->hovered())
        item_container_->move_y(elapsed_time / 3.0);

    // Bottom scroll button.
    if (scroll_bottom_->hovered())
        item_container_->move_y(-elapsed_time / 3.0);

    // Scroll bar.
    if (actual_scroll_bar_->hovered())
    {
        scroll_movement_time_ += elapsed_time;
    }
    else if (scroll_movement_time_ > 0)
    {
        auto strength = scroll_wheel_.y / (1000 * scroll_movement_time_);
        if (std::abs(strength) > 1)
        {
            if (item_container_->actions().empty())
                item_container_->add_action(std::make_shared<MoveByAction>(sf::Vector2f(0, strength), 1.5, MoveByAction::Interpolation::Quadratic));
            else
                item_container_->clear_actions();
        }

        scroll_movement_time_ = 0;
    }

    // Make sure that we did not scroll to far.
    auto pos = item_container_->widgets().front()->get_render_rectangle().Bottom;
    double diff = scroll_bottom_->get_render_rectangle().Top - pos - gap_;
    if (diff < 0)
    {
        item_container_->clear_actions();
        item_container_->move_y(diff / screen_height_);
    }

    pos = item_container_->widgets().back()->get_render_rectangle().Top;
    diff = scroll_top_->get_render_rectangle().Bottom - pos + gap_;
    if (diff > 0)
    {
        item_container_->clear_actions();
        item_container_->move_y(diff / screen_height_);
    }
}

void MenuOverlay::hide_mouse()
{
    opts.mouse_->stop();
    opts.mouse_->reset();
    opts.mouse_->hide();
}



}

#endif
