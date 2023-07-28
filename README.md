#  ImGuiNotify
Is a header-only wrapper made to create notifications with [Dear ImGui](https://github.com/ocornut/imgui). Fork of [imgui-notify](https://github.com/patrickcjk/imgui-notify) by [patrickcjk](https://github.com/patrickcjk).

Requires [Font Awesome 6](https://fontawesome.com/) for icons ([Setup example](https://github.com/juliettef/IconFontCppHeaders)).

## Latest changes (v0.0.1)
### Added
- Linux support
- Documentation

### Fixed
- Notifications now render above all other windows
- Notifications now render in the correct position when the main window is moved
- Compilation warnings about incorrect usage of ```ImGui::Text()```

### Changed
- Code readability improved
- Switched to Font Awesome 6 icons
- Visual changes to the notifications (can be customized in the ```main.cpp``` file)
- Default ImGui theme changed to *Embrace The Darkness* by [janekb04](https://github.com/janekb04)

## [FULL CHANGELOG](https://github.com/TyomaVader/ImGuiNotify/blob/Dev/CHANGELOG.md)

## Usage
### Include
```c++
#include "ImGuiNotify.hpp"
#include "IconsFontAwesome6.h"
```
### Initialisation (after impl call: ImGui_ImplDX12_Init, ImGui_ImplVulkan_Init, etc.)
```c++
io.Fonts->AddFontDefault();

float baseFontSize = 16.0f; // Default font size
float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

// Check if FONT_ICON_FILE_NAME_FAS is a valid path
std::ifstream fontAwesomeFile(FONT_ICON_FILE_NAME_FAS);

if (!fontAwesomeFile.good())
{
    // If it's not good, then we can't find the font and should abort
    std::cerr << "Could not find the FontAwesome font file." << std::endl;
    abort();
}

static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
ImFontConfig icons_config;
icons_config.MergeMode = true;
icons_config.PixelSnapH = true;
icons_config.GlyphMinAdvanceX = iconFontSize;
io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, iconFontSize, &icons_config, icons_ranges);
```
### Create notifications
```c++
// A few examples... (no title provided, default one used!)
ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Hello World! This is a success! %s", "We can also format here:)" });
ImGui::InsertNotification({ ImGuiToastType_Warning, 3000, "Hello World! This is a warning! %d", 0x1337 });
ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Hello World! This is an error! 0x%X", 0xDEADBEEF });
ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Hello World! This is an info!" });
ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation" });

// Now using a custom title...
ImGuiToast toast(ImGuiToastType_Success, 3000); // <-- content can also be passed here as above
toast.set_title("This is a %s title", "wonderful");
toast.set_content("Lorem ipsum dolor sit amet");
ImGui::InsertNotification(toast);
```
### Rendering
```c++
// Notifications style setup
ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f); // Disable round borders
ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f); // Disable borders

// Notifications color setup
ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.00f)); // Background color


// Main rendering function
ImGui::RenderNotifications();


//——————————————————————————————— WARNING ———————————————————————————————
// Argument MUST match the amount of ImGui::PushStyleVar() calls 
ImGui::PopStyleVar(2);
// Argument MUST match the amount of ImGui::PushStyleColor() calls 
ImGui::PopStyleColor(1);
```

## Showcase
![Showcase](https://s11.gifyu.com/images/Sc3BQ.gif)

## License
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/TyomaVader/ImGuiNotify/blob/Dev/LICENSE)
