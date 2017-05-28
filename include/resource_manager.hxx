#ifndef RESOURCE_MANAGER_HXX
#define RESOURCE_MANAGER_HXX

#include <map>
#include <string>
#include <stdexcept>

#include <SFML/Graphics.hpp>

class ResourceManager
{
public:

    ResourceManager()
    {}

    sf::Texture & get_image(std::string const & filename)
    {
        auto it = images_.find(filename);
        if (it != images_.end())
        {
            return it->second;
        }
        else
        {
            auto p = images_.insert({filename, sf::Texture()});
            auto it = p.first;
            if (!it->second.loadFromFile(filename))
            {
                images_.erase(it);
                throw std::runtime_error("Could not load image " + filename);
            }
            else
            {
                return it->second;
            }
        }
    }

private:

    std::map<std::string, sf::Texture> images_;

};

ResourceManager resource_manager;

#endif
