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
- [Unit Selection](https://github.com/HeyZoos/OpenRTSCamera/wiki/Unit-Selection)

### [Camera Bounds](https://github.com/HeyZoos/OpenRTSCamera/wiki/Camera-Bounds)

https://user-images.githubusercontent.com/9408481/223589311-9d6b1cfd-76b4-4650-a6a3-0386a483bb96.mp4

### [Jump To](https://github.com/HeyZoos/OpenRTSCamera/wiki/Jump-To)

https://user-images.githubusercontent.com/9408481/223585144-d7e9c1c2-2e36-4628-9bbd-da91229e39e1.mp4

# Changelog

### 0.21.0

- Add [Unit Selection](https://github.com/HeyZoos/OpenRTSCamera/wiki/Unit-Selection)

### 0.20.0

- Build for Unreal Engine v5.3.X

### 0.19.0

- Add "Jump To" method
- Start overhauling documentation

### 0.18.0

- Fix crash when using the camera in a networked context
- **No longer take over view target automatically, the view target must be explicitly set, this is documented in the "Getting Started" section of the wiki**

### 0.17.0

- Fix [#27](https://github.com/HeyZoos/OpenRTSCamera/issues/27) by tying camera movement to delta time (thanks [@theMyll](https://github.com/theMyll))
- **This will result in slower movement across the board, if you notice your camera moving more slowly, up the speed values by about 100x. For example, the new camera blueprint speed defaults are 5000**
