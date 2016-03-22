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

    sf::Image & get_image(std::string const & filename)
    {
        auto it = images_.find(filename);
        if (it != images_.end())
        {
            return it->second;
        }
        else
        {
            auto p = images_.insert({filename, sf::Image()});
            auto it = p.first;
            if (!it->second.LoadFromFile(filename))
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

    std::map<std::string, sf::Image> images_;

};

ResourceManager resource_manager;

#endif
