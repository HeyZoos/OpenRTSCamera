# OpenRTSCamera

- [Installing from GitHub](https://github.com/HeyZoos/OpenRTSCamera/wiki/Installing-from-GitHub)
- [Getting Started](https://github.com/HeyZoos/OpenRTSCamera/wiki/Getting-Started)

## Features

- Smoothed Movement
- Ground Height Adaptation
- Edge Scrolling
- [Follow Target](https://github.com/HeyZoos/OpenRTSCamera/wiki/Follow-Camera)
- [Mouse + Keyboard Controls](https://github.com/HeyZoos/OpenRTSCamera/wiki/Movement-Controls)
- [Gamepad Controls](https://github.com/HeyZoos/OpenRTSCamera/wiki/Movement-Controls)
- [Camera Bounds](https://github.com/HeyZoos/OpenRTSCamera/wiki/Camera-Bounds)

# Changelog

### 0.18.0

- Fix crash when using the camera in a networked context
- **No longer take over view target automatically, the view target must be explicitly set, this is documented in the "Getting Started" section of the wiki**

### 0.17.0

- Fix [#27](https://github.com/HeyZoos/OpenRTSCamera/issues/27) by tying camera movement to delta time (thanks [@theMyll](https://github.com/theMyll))
- **This will result in slower movement across the board, if you notice your camera moving more slowly, up the speed values by about 100x. For example, the new camera blueprint speed defaults are 5000**
