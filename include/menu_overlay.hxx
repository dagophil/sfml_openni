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
            std::string const & text = "",
            DiffType x = 0,
            DiffType y = 0,
            DiffType width = 0,
            DiffType height = 0
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
        text_grid.set_y_sizes(0.3, 0.7);
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

//        {

//            float x = -0.0187 * width;
//            float y = -0.162 * height;
//            float w = 1.0491 * width;
//            float h = 1.362 * height;
//            auto imw = std::make_shared<ImageWidget>("images/menu_item_bg.png", -1);
//            imw->set_x(x);
//            imw->set_y(y);
//            imw->set_width(w);
//            imw->set_height(h);
//            add_widget(imw);

//        }


    }

protected:

    /**
     * @brief Align top of the description text with the bottom of the title text.
     */
    void render_impl(sf::RenderTarget & target)
    {
        Parent::render_impl(target);
        text_w_->set_y(title_w_->text_height()+10);
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
    std::shared_ptr<AnimatedWidget> mouse_; // the mouse
    std::shared_ptr<Widget> actual_scroll_bar_; // the actual scroll widget
    sf::Vector2f scroll_wheel_; // mouse coordinates for the scroll wheel
    float scroll_movement_time_; // time that is currently used to scroll

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
      scroll_movement_time_(0)
{
    double const item_x = 0.04 * screen_width;
    double const item_width = 0.66 * screen_width;
    double const item_height = 0.15 * screen_height;
    double const scroll_height = 0.1 * screen_height;
    double const close_x = 0.82 * screen_width;

    auto grid_ptr = std::make_shared<GridWidget>(2,1);
    auto &grid = *grid_ptr;
    add_widget(grid_ptr);
    grid.set_x_sizes(0.82, 0.18);

    // Create the menu item container.
    item_container_ = std::make_shared<Widget>();
    item_container_->set_x(item_x);
    item_container_->set_y(0.16 * screen_height);
    item_container_->set_width(item_width);
    item_container_->set_height(item_height);
    add_widget(item_container_);

    // Create the menu items.
    if (xml_filename == "")
    {
        create_default_menu_items();
    }
    else
    {
        load_xml_menu_items(xml_filename);
    }

    // Create the close button.
    auto close_button = std::make_shared<HoverclickWidget<ImageWidget> >("images/exit_symbol.png", 1);
    close_button->set_x(0.82);
    close_button->set_y(0);
    close_button->set_width(0.18);
    close_button->set_height(0.1);
    close_button->align_y_ = Top;
    add_widget(close_button);
    close_button->handle_click_ = [&](DiffType x, DiffType y){
        handle_close_();
    };
    close_button->handle_mouse_enter_ = [this](DiffType x, DiffType y){
        mouse_->restart();
    };
    close_button->handle_mouse_leave_ = [this](DiffType x, DiffType y){
        mouse_->reset();
        mouse_->stop();
    };

    // Create the top scroll button.
    sf::Color const scroll_color(170, 191, 212, 190);
    scroll_top_ = std::make_shared<ColorWidget>(scroll_color, 1);
//    scroll_top_->set_x(0.5);
    scroll_top_->set_y(0);
    scroll_top_->set_width(0.8);
    scroll_top_->set_height(0.1);
    scroll_top_->scale_= None;
    scroll_top_->align_x_ = CenterX;
    scroll_top_->align_y_ = Top;
    grid(0)->add_widget(scroll_top_);
//    add_widget(scroll_top_);

    // Create the bottom scroll button.
    scroll_bottom_ = std::make_shared<ColorWidget>(*scroll_top_);
    scroll_bottom_->set_height(scroll_height);
//    add_widget(scroll_bottom_);

    // Create the right scroll bar.
    auto scroll_bar = std::make_shared<ColorWidget>(scroll_color, 1);
    scroll_bar->set_x(close_x);
    scroll_bar->set_y(scroll_height);
    scroll_bar->set_width(screen_width-close_x);
    scroll_bar->set_height(screen_height-scroll_height);
    add_widget(scroll_bar);
    actual_scroll_bar_ = std::make_shared<Widget>(2);
    actual_scroll_bar_->set_x(0);
    actual_scroll_bar_->set_y(0.4 * scroll_bar->get_height());
    actual_scroll_bar_->set_width(scroll_bar->get_width());
    actual_scroll_bar_->set_height(0.2 * scroll_bar->get_height());

    actual_scroll_bar_->handle_mouse_enter_ = [&](DiffType x, DiffType y) {
        scroll_wheel_.x = x;
        scroll_wheel_.y = y;
        scroll_movement_time_ = 0;
    };
    actual_scroll_bar_->handle_mouse_leave_ = [&](DiffType x, DiffType y) {
        scroll_wheel_.x = x - scroll_wheel_.x;
        scroll_wheel_.y = y - scroll_wheel_.y;
    };
    scroll_bar->add_widget(actual_scroll_bar_);

    // Create the mouse animation.
    mouse_ = std::make_shared<AnimatedWidget>("animations/hand_load_2s.pf", 999);
    mouse_->set_x(screen_width);
    mouse_->set_y(screen_height);
    mouse_->set_width(75);
    mouse_->set_height(75);
    mouse_->hoverable_ = false;
    mouse_->stop();
    add_widget(mouse_);
    handle_hover_ = [&](DiffType x, DiffType y)
    {
        auto old_x = mouse_->get_x();
        auto old_y = mouse_->get_y();
        auto w = mouse_->get_width();
        auto h = mouse_->get_height();
        mouse_->set_x(mouse_->get_x() + x-old_x - 0.45*w);
        mouse_->set_y(mouse_->get_y() + y-old_y - 0.17*h);
    };
}

void MenuOverlay::add_menu_item(
        std::string const & image,
        std::string const & title,
        std::string const & description,
        std::string const & command
){
    auto width = item_container_->get_width();
    auto height = item_container_->get_height();
    auto y = item_container_->widgets().size() * (height + gap_);
    auto w = std::make_shared<MenuItem>(
                image,
                title,
                description,
                0,
                y,
                width,
                height
    );
    w->handle_click_ = [this, command](DiffType x, DiffType y)
    {
        handle_menu_item_click_(command);
    };
    w->handle_mouse_enter_ = [this](DiffType x, DiffType y){
        mouse_->restart();
    };
    w->handle_mouse_leave_ = [this](DiffType x, DiffType y){
        mouse_->reset();
        mouse_->stop();
    };
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
        item_container_->set_y(item_container_->get_y() + elapsed_time * get_height() / 3);

    // Bottom scroll button.
    if (scroll_bottom_->hovered())
        item_container_->set_y(item_container_->get_y() - elapsed_time * get_height() / 3);

    // Scroll bar.
    if (actual_scroll_bar_->hovered())
    {
        scroll_movement_time_ += elapsed_time;
    }
    else if (scroll_movement_time_ > 0)
    {
        auto strength = render_rect_.GetHeight() * scroll_wheel_.y / (1000 * scroll_movement_time_);
        if (std::abs(strength) > 1000)
        {
            if (item_container_->actions().empty())
                item_container_->add_action(std::make_shared<MoveByAction>(sf::Vector2f(0, strength), 1.5, MoveByAction::Interpolation::Quadratic));
            else
                item_container_->clear_actions();
        }

        scroll_movement_time_ = 0;
    }

    // Make sure that we did not scroll to far.
    auto pos = item_container_->get_y() + item_container_->widgets().front()->get_y() + item_container_->widgets().front()->get_height();
    auto diff = scroll_bottom_->get_y() - pos - gap_;
    if (diff < 0)
    {
        item_container_->clear_actions();
        item_container_->set_y(item_container_->get_y() + diff);
    }
    pos = item_container_->get_y() + item_container_->widgets().back()->get_y();
    diff = scroll_top_->get_y() + scroll_top_->get_height() - pos + gap_;
    if (diff > 0)
    {
        item_container_->clear_actions();
        item_container_->set_y(item_container_->get_y() + diff);
    }
}

void MenuOverlay::hide_mouse()
{
    // Move the mouse out of the visible part and stop the animation.
    auto dx = get_x() + get_width() - mouse_->get_x() + 1;
    auto dy = get_y() + get_height() - mouse_->get_y() + 1;
    mouse_->set_x(mouse_->get_x() + dx);
    mouse_->set_y(mouse_->get_y() + dy);
    mouse_->stop();
    mouse_->reset();
}



}

#endif
