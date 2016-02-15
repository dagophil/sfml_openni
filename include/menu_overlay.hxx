#ifndef MENU_OVERLAY_HXX
#define MENU_OVERLAY_HXX

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include <tinyxml2.h>

#include "utility.hxx"

namespace kin
{

/**
 * @brief The MenuItem class represents a single menu item with image and text.
 */
class MenuItem
{
public:

    explicit MenuItem(
            std::string const & img_filename = "",
            std::string const & title = "",
            std::string const & text = "",
            std::string const & call_command = ""
    )   :
        title_(title),
        text_(text),
        call_command_(call_command),
        hover_time_(0)
    {
        title_.SetSize(16);
        title_.SetColor(sf::Color(0, 0, 0, 255));
        text_.SetSize(16);
        text_.SetColor(sf::Color(0, 0, 0, 255));

        // Load image.
        if (!img_.LoadFromFile(img_filename))
            throw std::runtime_error("Could not load image " + img_filename);

        bg_ = sf::Shape::Rectangle(
                    sf::Vector2f(0, 0),
                    sf::Vector2f(1, 1),
                    sf::Color(200, 200, 200, 255));
    }

    void draw(
            sf::RenderTarget & target,
            sf::Font & font,
            double offset_x,
            double offset_y,
            double width,
            double height
    ){
        double const max_img_width = 0.38 * width;
        double const max_text_width = 0.62 * width;

        // Draw the background.
        bg_.SetPosition(sf::Vector2f(offset_x, offset_y));
        bg_.SetScale(width, height);
        target.Draw(bg_);

        // Compute scale factor.
        double factor = height / img_.GetHeight();
        if (factor*img_.GetWidth() > max_img_width)
            factor = width / img_.GetWidth();

        // Draw the image.
        sf::Sprite im_spr(img_, sf::Vector2f(max_img_width - factor*img_.GetWidth(), 0), sf::Vector2f(factor, factor));
        im_spr.Move(offset_x, offset_y);
        target.Draw(im_spr);

        // Draw the text.
        title_.SetFont(font);
        title_.SetStyle(sf::String::Bold);
        insert_line_breaks(title_, max_text_width-10);
        title_.SetPosition(offset_x + max_img_width + max_text_width/2.0 - title_.GetRect().GetWidth()/2.0, offset_y);
        target.Draw(title_);

        text_.SetFont(font);
        insert_line_breaks(text_, max_text_width-30);
        text_.SetPosition(offset_x + max_img_width + max_text_width/2.0 - text_.GetRect().GetWidth()/2.0, offset_y + title_.GetRect().GetHeight() +10);
        target.Draw(text_);
    }

    bool hover(float elapsed_time)
    {
        hover_time_ += elapsed_time;
        if (hover_time_ > 3 && event_sent_ == false)
        {
            event_sent_ = true;
            return true;
        }
        return false;
    }

    void unhover()
    {
        event_sent_ = false;
        hover_time_ = 0;
    }

    std::string const & get_call_command() const
    {
        return call_command_;
    }

private:
    sf::String title_;
    sf::String text_;
    sf::Image img_;
    std::string call_command_;
    sf::Shape bg_;
    float hover_time_;
    bool event_sent_;
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

tinyxml2::XMLElement * check_xml_element(tinyxml2::XMLElement * elt)
{
    if (elt != nullptr)
        return elt;
    else
        throw std::runtime_error("XML Error: Could not find XML element.");
}

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
class MenuOverlay
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
     * @brief Scroll the menu for the given amount.
     */
    void scroll(int amount, float scroll_time);

    /**
     * @brief Update the menu item states and return the id of the menu item that shall be executed.
     * @param elapsed_time elapsed time since last frame
     * @param mouse_x mouse position x
     * @param mouse_y mouse position y
     * @return id of the menu item that shall be executed
     */
    int update(float elapsed_time, double mouse_x, double mouse_y);

    /**
     * @brief Draw the menu overlay on the given sprite.
     */
    void draw(sf::RenderTarget & target, sf::Font & font);

    /**
     * @brief Return the call command of the given menu item.
     */
    std::string get_call_command(int item) const;

private:

    size_t const screen_width_;

    size_t const screen_height_;

    int current_selection_;

    double scroll_amount_;

    std::vector<MenuItem> items_;

    Rect top_scroll_rect_;
    Rect bottom_scroll_rect_;
    Rect right_scroll_rect_;
    sf::Shape top_scroll_;
    sf::Shape bottom_scroll_;
    sf::Shape right_scroll_;

};

MenuOverlay::MenuOverlay(
        std::string const & xml_filename,
        size_t screen_width,
        size_t screen_height
)   :
      screen_width_(screen_width),
      screen_height_(screen_height),
      current_selection_(-1)
{
    if (xml_filename == "")
    {
        items_.emplace_back("images/mario.png", "Mario", "Mario game ist echt ein supertolles Spiel.", "gedit /home/philip/game0.txt");
        items_.emplace_back("images/luigi.jpg", "Luigi", "Luigi game", "gedit /home/philip/game1.txt");
        items_.emplace_back("images/mario.png", "Mario 2", "Mario game 2", "gedit /home/philip/game2.txt");
    }
    else
    {
        // Open the xml file.
        tinyxml2::XMLDocument doc;
        std::cout << xml_filename << std::endl;
        check_xml_error(doc.LoadFile(xml_filename.c_str()));
        auto root = doc.FirstChild();
        if (std::string(root->Value()) != "MENU")
            throw std::runtime_error("XML error: Root node should be MENU.");

        // Loop over the menu items in the xml file.
        for (auto elt = root->FirstChildElement("MENUITEM");
             elt != nullptr;
             elt = elt->NextSiblingElement("MENUITEM"))
        {
            auto img = check_xml_element(elt->FirstChildElement("IMAGE"));
            auto title = check_xml_element(elt->FirstChildElement("TITLE"));
            auto description = check_xml_element(elt->FirstChildElement("DESCRIPTION"));
            auto command = check_xml_element(elt->FirstChildElement("COMMAND"));
            items_.emplace_back(img->GetText(), title->GetText(), description->GetText(), command->GetText());
        }
    }
    scroll_amount_ = 0.16 * screen_height;

    // Create the scroll buttons.
    sf::Color gray(120, 120, 120);
    {
        double const left_x = 0.04 * screen_width;
        double const right_x = 0.7 * screen_width;
        double const scroll_height = 0.05 * screen_height;
        top_scroll_rect_ = Rect(left_x, 0, right_x, scroll_height);
        bottom_scroll_rect_ = Rect(left_x, screen_height - scroll_height, right_x, screen_height);
        top_scroll_ = sf::Shape::Rectangle(top_scroll_rect_.Left, top_scroll_rect_.Top,
                                           top_scroll_rect_.Right, top_scroll_rect_.Bottom, gray);
        bottom_scroll_ = sf::Shape::Rectangle(bottom_scroll_rect_.Left, bottom_scroll_rect_.Top,
                                              bottom_scroll_rect_.Right, bottom_scroll_rect_.Bottom, gray);
    }
    {
        double const width = 0.18 * screen_width;
        right_scroll_rect_ = Rect(screen_width - width, 0, screen_width, screen_height);
        right_scroll_ = sf::Shape::Rectangle(right_scroll_rect_.Left, right_scroll_rect_.Top,
                                             right_scroll_rect_.Right, right_scroll_rect_.Bottom, gray);
    }
}

int MenuOverlay::update(float elapsed_time, double mouse_x, double mouse_y)
{
    if (top_scroll_rect_.Contains(mouse_x, mouse_y))
    {
        scroll_amount_ += 100 * elapsed_time;
    }
    else if (bottom_scroll_rect_.Contains(mouse_x, mouse_y))
    {
        scroll_amount_ -= 100 * elapsed_time;
    }
    else
    {
        double const item_width = 0.66 * screen_width_;
        double const item_height = 0.15 * screen_height_;
        double const item_offset_x = 0.04 * screen_width_;
        for (size_t i = 0; i < items_.size(); ++i)
        {
            double const item_offset_y = scroll_amount_ + i * (item_height+10);
            Rect r(item_offset_x, item_offset_y, item_offset_x + item_width, item_offset_y + item_height);
            if (r.Contains(mouse_x, mouse_y))
            {
                if (items_[i].hover(elapsed_time))
                {
                    return i;
                }
            }
            else
            {
                items_[i].unhover();
            }
        }
    }
    return -1;
}

void MenuOverlay::draw(sf::RenderTarget & target, sf::Font & font)
{
    double const item_width = 0.66 * screen_width_;
    double const item_height = 0.15 * screen_height_;
    double const item_offset_x = 0.04 * screen_width_;
    for (size_t i = 0; i < items_.size(); ++i)
    {
        double const item_offset_y = scroll_amount_ + i * (item_height+10);
        items_[i].draw(target, font, item_offset_x, item_offset_y, item_width, item_height);
    }
    target.Draw(top_scroll_);
    target.Draw(bottom_scroll_);
    target.Draw(right_scroll_);
}

std::string MenuOverlay::get_call_command(int item) const
{
    return items_[item].get_call_command();
}



}

#endif
