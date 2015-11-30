#ifndef MENU_OVERLAY_HXX
#define MENU_OVERLAY_HXX

#include <string>
#include <vector>

namespace kin
{

/**
 * @brief The MenuItem class represents a single menu item with image and text.
 */
class MenuItem
{

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
    MenuOverlay(std::string const & xml_filename);

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
    void draw(sf::Sprite & target);

private:

    int current_selection_;

    std::vector<MenuItem> items_;

};

MenuOverlay::MenuOverlay(std::string const & xml_filename)
    :
      current_selection_(-1)
{
    // TODO: Read the xml file and fill items_.
}

}

#endif
