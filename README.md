# imgui-notify
Is a header-only wrapper made to create notifications with [Dear ImGui](https://github.com/ocornut/imgui). As I couldn't find any library for this I just decided to create my own. We will use [Font Awesome 5](https://fontawesome.com/) for icons.

[![forthebadge](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)](https://forthebadge.com)
[![forthebadge](https://forthebadge.com/images/badges/built-with-love.svg)](https://forthebadge.com)

## Usage
```c++
#include "notify.h"

// Initialisation, once at the beggining of your code, usually after ImGui_ImplDX12_Init()
notify::init();

// Add a new toast
if (ImGui::Button("success!"))
{
    notify::insert({ toast_type::toast_type_success, 3000, "Hello World! This is a success! %s", "We can also format here:)" });
}

if (ImGui::Button("warning!"))
{
    notify::insert({ toast_type::toast_type_warning, 3000, "Hello World! This is a warning!" });
}

if (ImGui::Button("error!"))
{
    notify::insert({ toast_type::toast_type_error, 3000, "Hello World! This is an error!" });
}

if (ImGui::Button("info!"))
{
    notify::insert({ toast_type::toast_type_info, 3000, "Hello World! This is an info!" });
}

if (ImGui::Button("info!"))
{
    notify::insert({ toast_type::toast_type_info, 3000, "Hello World! This is an info! Yes I also support multiline text! Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation" });
}

// Render toasts, at the end of your rendering
notify::render();
```

## TODO
Add a fade in and out movement...

## Result
[![Image from Gyazo](https://i.gyazo.com/2c8a6b7aa030a80e54e993058ca80559.gif)](https://gyazo.com/2c8a6b7aa030a80e54e993058ca80559)

### License
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/patrickcjk/imgui-notify/blob/main/LICENSE)
