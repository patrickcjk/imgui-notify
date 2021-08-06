# imgui-notify
Is a header-only wrapper made to create notifications with [Dear ImGui](https://github.com/ocornut/imgui). As I couldn't find any library for this I just decided to create my own. We will use [Font Awesome 5](https://fontawesome.com/) for icons.

[![forthebadge](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)](https://forthebadge.com)
[![forthebadge](https://forthebadge.com/images/badges/built-with-love.svg)](https://forthebadge.com)

## Requirements
- You must use a font other than the default one. Font-Awesome icons used in this library cannot be merged with default font. You can use Tahoma (provided in the example project).
- If you load the font using AddFontFromMemoryTTF (from memory, instead of from a file on disk) and memory is read-only as in the example, you must pass a ImFontConfig structure with FontDataOwnedByAtlas = false to prevent imgui from attempting to free the buffer (which will lead into a crash).

## Changes in version 2
- Toast now contains a title. If no title is provided, a default one is used (ImGuiToastType_Success will result in "Success")
- Added getters and setters to get/set private properties
- Added assertions
- Constructors will only accept a content (formatting is possible), title must be set with ImGuiToast::set_title() (see example)
- "notify" namespace was removed, we now use "ImGui" namespace for a better implementation
- "notify::init" was removed, you must now call "ImGui::MergeIconsWithLatestFont" after EACH loaded font in your initialisation (see example)
- "notify::render" was renamed to "ImGui::RenderNotifications()"
- By default, NOTIFY_USE_SEPARATOR is defined which will add a separator (horizontal line) between the title and the content.
- Now supporting multiline content (1/3 of the total view port width as max-width)

## Usage
### Include
```c++
#include "src/imgui_notify.h"
#include "tahoma.h" // <-- Required font!
```
### Initialisation (after impl call, e.g ImGui_ImplDX12_Init)
```c++
ImGuiIO* io = &ImGui::GetIO();

ImFontConfig font_cfg;
font_cfg.FontDataOwnedByAtlas = false;
io->Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);

// Initialize notify
ImGui::MergeIconsWithLatestFont(16.f, false);

// If you use multiple fonts, repeat the same thing!
// io->Fonts->AddFontFromMemoryTTF((void*)another_font, sizeof(another_font), 17.f, &font_cfg);
// ImGui::MergeIconsWithLatestFont(16.f, false);
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
// Render toasts on top of everything, at the end of your code!
// You should push style vars here
ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f); // Round borders
ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f)); // Background color
ImGui::RenderNotifications(); // <-- Here we render all notifications
ImGui::PopStyleVar(1); // Don't forget to Pop()
ImGui::PopStyleColor(1);
```

## Showcase
![Showcase](https://i.imgur.com/ckcpOHJ.gif)

## License
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/patrickcjk/imgui-notify/blob/main/LICENSE)
