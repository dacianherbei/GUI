#include "ExplorerFavourites.h"

#include "ExplorerDialog.h"

#include <filesystem>

namespace gui {
namespace explorer {

// ExplorerFavourites implementation
ExplorerFavourites::ExplorerFavourites(ExplorerDialog* parent)
    : m_parent(parent)
{
    LoadFavourites();
}

ExplorerFavourites::~ExplorerFavourites()
{
}

void ExplorerFavourites::Render()
{
    ImGui::Text("Favourites");
    ImGui::Separator();

    // Add some default favourites if empty
    if (m_favourites.empty())
    {
        AddFavourite(".", "Current Directory");
        AddFavourite(std::filesystem::current_path().string(), "Home");
    }

    for (const auto& fav : m_favourites)
    {
        if (ImGui::Selectable(fav.name.c_str()))
        {
            NavigateToFavourite(fav);
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("%s", fav.path.c_str());
        }
    }
}

void ExplorerFavourites::AddFavourite(const std::string& path, const std::string& name)
{
    Favourite fav;
    fav.path = path;
    fav.name = name.empty() ? std::filesystem::path(path).filename().string() : name;
    m_favourites.push_back(fav);
}

void ExplorerFavourites::LoadFavourites()
{
    // TODO: Load from file or registry
    // For now, just add some default entries
}

void ExplorerFavourites::SaveFavourites()
{
    // TODO: Save to file or registry
}

void ExplorerFavourites::NavigateToFavourite(const Favourite& fav)
{
    if (m_parent) {
        m_parent->OnDirectoryChanged(fav.path);
    }
}

} // namespace explorer
} // namespace gui