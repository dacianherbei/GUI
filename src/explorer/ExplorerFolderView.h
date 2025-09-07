#pragma once

#include "imgui.h"

#include <string>
#include <vector>

namespace gui {
namespace explorer {

class ExplorerDialog;

// Main folder view component
class ExplorerFolderView
{
public:
    struct FileEntry
    {
        std::string name;
        std::string fullPath;
        bool isDirectory;
        size_t fileSize;
        std::string lastModified;
    };

    ExplorerFolderView(ExplorerDialog* parent);
    ~ExplorerFolderView();

    void Render();
    void SetDirectory(const std::string& directory);
    void SetFileFilter(const std::string& filter);

    std::string GetSelectedFile() const { return m_selectedFile; }

private:
    ExplorerDialog* m_parent;
    std::string m_currentDirectory;
    std::string m_fileFilter;
    std::string m_selectedFile;
    std::vector<FileEntry> m_entries;

    void RefreshDirectory();
    void OnItemDoubleClicked(const FileEntry& entry);
    void OnItemSelected(const FileEntry& entry);
    bool MatchesFilter(const std::string& filename) const;
};

} // namespace explorer
} // namespace gui
