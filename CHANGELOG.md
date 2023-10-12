# Changelog

## WIP - 12.10.2023

### Changed
- README clarifications.(#5)

### Fixed
- Removed unnecessary files. (#5)

## [0.0.2] - 30.08.2023

### Added
- CMake support, see CMakeLists.txt for details
- Dismiss button for notifications (optional)
- Optional button in the notification that executes a user-defined function
- GitHub Actions for Linux and Windows builds
- Documentation examples

### Fixed
- Documentation fixes

### Changed
- Switched from classic enums to scoped enums
- Upgraded Dear ImGui version used in example to [v1.89.9 WIP](https://github.com/ocornut/imgui/commit/11613013860d149667302a258041dcd832069f36)

## [0.0.1] - 28.07.2023

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


[0.0.2]: https://github.com/TyomaVader/ImGuiNotify/releases/tag/v0.0.2
[0.0.1]: https://github.com/TyomaVader/ImGuiNotify/releases/tag/v0.0.1
