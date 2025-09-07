#pragma once

#include "imgui.h"

#include <string>
#include <vector>

namespace gui {
namespace explorer {

class ExplorerDialog;

// Favourites/bookmarks component
class ExplorerFavourites
{
public:
    ExplorerFavourites(ExplorerDialog* parent);
    ~ExplorerFavourites();

    void Render();
    void AddFavourite(const std::string& path, const std::string& name = "");

private:
    struct Favourite
    {
        std::string name;
        std::string path;
    };

    ExplorerDialog* m_parent;
    std::vector<Favourite> m_favourites;

    void LoadFavourites();
    void SaveFavourites();
    void NavigateToFavourite(const Favourite& fav);
};

} // namespace explorer
} // namespace gui