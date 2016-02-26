#ifndef MENU_OVERLAY_HXX
#define MENU_OVERLAY_HXX

#include <string>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include <tinyxml2.h>

#include "utility.hxx"
#include "widgets.hxx"

namespace kin
{



/**
 * @brief The MenuItem class represents a single menu item with image and text.
 */
class MenuItem : public HoverclickWidget<ColorWidget>
{
public:

    typedef HoverclickWidget<ColorWidget> Parent;

    explicit MenuItem(
            std::string const & img_filename = "",
            std::string const & title = "",
            std::string const & text = "",
            DiffType x = 0,
            DiffType y = 0,
            DiffType width = 0,
            DiffType height = 0
    )   :
        Parent(sf::Color(200, 200, 200), x, y, width, height, 0)
    {
        float image_width = 0.38 * width;

        // Create the image widget
        auto img_w = std::make_shared<ImageWidget>(img_filename, 0, 0, image_width, height, 0);
        img_w->scale_style_ = ImageWidget::ScaleStyle::Fit;
        img_w->align_x_ = ImageWidget::AlignX::Right;
        img_w->align_y_ = ImageWidget::AlignY::Top;
        add_widget(img_w);

        // Create the text widget for the title.
        title_w_ = std::make_shared<TextWidget>(title, image_width, 0, width - image_width, height, 0);
        title_w_->align_x_ = TextWidget::AlignX::CenterX;
        title_w_->style_ = sf::String::Style::Bold;
        title_w_->color_ = sf::Color(0, 0, 0);
        add_widget(title_w_);

        // Create the text widget for the description.
        text_w_ = std::make_shared<TextWidget>(text, image_width, 0, width - image_width, height, 0);
        text_w_->color_ = sf::Color(0, 0, 0);
        text_w_->align_x_ = TextWidget::AlignX::CenterX;
        add_widget(text_w_);
    }

protected:

    /**
     * @brief Align top of the description text with the bottom of the title text.
     */
    void render_impl(sf::RenderTarget & target)
    {
        ColorWidget::render_impl(target);
        text_w_->rect_.Top = title_w_->text_height()+10;
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
class MenuOverlay : public Widget
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

};

MenuOverlay::MenuOverlay(
        std::string const & xml_filename,
        size_t screen_width,
        size_t screen_height
)   :
      Widget(0, 0, screen_width, screen_height, 0),
      handle_menu_item_click_(detail::do_nothing1<std::string const &>),
      handle_close_(detail::do_nothing0),
      gap_(10)
{
    double const item_x = 0.04 * screen_width;
    double const item_width = 0.66 * screen_width;
    double const item_height = 0.15 * screen_height;
    double const scroll_height = 0.1 * screen_height;
    double const close_x = 0.82 * screen_width;

    // Create the menu item container.
    item_container_ = std::make_shared<Widget>(
                item_x,
                0.16 * screen_height,
                item_width,
                item_height,
                0
    );
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
    auto close_button = std::make_shared<HoverclickWidget<ImageWidget> >(
                "images/exit_button.png",
                close_x,
                0,
                screen_width-close_x,
                scroll_height,
                1
    );
    add_widget(close_button);
    close_button->handle_click_ = [&](DiffType x, DiffType y){
        handle_close_();
    };
    close_button->handle_mouse_enter_ = [this](){
        mouse_->restart();
    };
    close_button->handle_mouse_leave_ = [this](){
        mouse_->reset();
        mouse_->stop();
    };

    // Create the top scroll button.
    sf::Color const gray(120, 120, 120);
    scroll_top_ = std::make_shared<ColorWidget>(
                gray,
                item_x,
                0,
                item_width,
                scroll_height,
                1
    );
    add_widget(scroll_top_);

    // Create the bottom scroll button.
    scroll_bottom_ = std::make_shared<ColorWidget>(*scroll_top_);
    scroll_bottom_->rect_.Top = screen_height - scroll_height;
    scroll_bottom_->rect_.Bottom = screen_height;
    add_widget(scroll_bottom_);

    // Create the right scroll bar.
    auto scroll_bar = std::make_shared<ColorWidget>(
                gray,
                close_x,
                scroll_height,
                screen_width-close_x,
                screen_height-scroll_height,
                1
    );
    add_widget(scroll_bar);

    // Create the mouse animation.
    mouse_ = std::make_shared<AnimatedWidget>(
                "animations/hand_load.pf",
                screen_width, screen_height,
                75, 75,
                999
    );
    mouse_->hoverable_ = false;
    mouse_->stop();
    add_widget(mouse_);
    handle_hover_ = [&](DiffType x, DiffType y)
    {
        auto old_x = mouse_->rect_.Left;
        auto old_y = mouse_->rect_.Top;
        auto w = mouse_->rect_.GetWidth();
        auto h = mouse_->rect_.GetHeight();
        mouse_->rect_.Offset(x-old_x - 0.45*w, y-old_y - 0.17*h);
    };
}

void MenuOverlay::add_menu_item(
        std::string const & image,
        std::string const & title,
        std::string const & description,
        std::string const & command
){
    auto width = item_container_->rect_.GetWidth();
    auto height = item_container_->rect_.GetHeight();
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
    w->handle_mouse_enter_ = [this](){
        mouse_->restart();
    };
    w->handle_mouse_leave_ = [this](){
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
        item_container_->rect_.Offset(0, 100*elapsed_time);

    // Bottom scroll button.
    if (scroll_bottom_->hovered())
        item_container_->rect_.Offset(0, -100*elapsed_time);

    // Make sure that we did not scroll to far.
    auto pos = item_container_->rect_.Top + item_container_->widgets().front()->rect_.Bottom;
    auto diff = scroll_bottom_->rect_.Top - pos - gap_;
    if (diff < 0)
    {
        item_container_->rect_.Offset(0, diff);
    }
    pos = item_container_->rect_.Top + item_container_->widgets().back()->rect_.Top;
    diff = scroll_top_->rect_.Bottom - pos + gap_;
    if (diff > 0)
    {
        item_container_->rect_.Offset(0, diff);
    }
}



}

#endif
