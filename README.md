# imgui-notify
Is a header-only wrapper made to create notifications with [Dear ImGui](https://github.com/ocornut/imgui). As I couldn't find any library for this I just decided to create my own. We will use [Font Awesome 5](https://fontawesome.com/) for icons.

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

// Render toasts, at the end of your rendering
notify::render();
```

## TODO
Add a fade in and out movement, add text colors based on toast type...

## Result
[![Result](https://i.gyazo.com/a6ae4c884aa5a0ea24acdb80501b8f6f.gif)](https://gyazo.com/a6ae4c884aa5a0ea24acdb80501b8f6f)
