#ifndef MENU_OVERLAY_HXX
#define MENU_OVERLAY_HXX

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

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
            std::string const & text = ""
    )   :
        title_(title),
        text_(text)
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

private:
    sf::String title_;
    sf::String text_;
    sf::Image img_;
    sf::Shape bg_;
};

/**
 * @brief The MenuOverlay class loads menu items from an xml file and draws them.
 */
class MenuOverlay
{
public:

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
     * @brief update function
     */
    void update(float elapsed_time);

    /**
     * @brief Draw the menu overlay on the given sprite.
     */
    void draw(sf::RenderTarget & target, sf::Font & font);

private:

    int current_selection_;

    mutable int scroll_amount_;

    std::vector<MenuItem> items_;

};

MenuOverlay::MenuOverlay(
        std::string const & xml_filename,
        size_t screen_width,
        size_t screen_height
)   :
      current_selection_(-1)
{
    if (xml_filename == "")
    {
        items_.emplace_back("images/mario.png", "Mario", "Mario game ist echt ein supertolles Spiel. bla blubb und eins zwei drei, Philip ist toll :D");
        items_.emplace_back("images/luigi.jpg", "Luigi", "Luigi game");
        items_.emplace_back("images/mario.png", "Mario 2", "Mario game 2");
    }
    else
    {
        // TODO: Read the xml file and fill items_.
        throw std::runtime_error("TODO: Implement xml reader.");
    }
}

void MenuOverlay::draw(sf::RenderTarget & target, sf::Font & font)
{
    double const item_width = 0.66 * target.GetWidth();
    double const item_height = 0.15 * target.GetHeight();
    double const item_offset_x = 0.04 * target.GetWidth();
    for (size_t i = 0; i < items_.size(); ++i)
    {
        double const item_offset_y = scroll_amount_ + i * (item_height+10);
        items_[i].draw(target, font, item_offset_x, item_offset_y, item_width, item_height);
    }
}



}

#endif
