# Butter: Video Editing Software

**Welcome to Butter!** This is a scripting-based video editing software intended primarily for motion graphics, but its customizability will allow it to support a diverse range of users.

This is a hobby project still in the development stage, so there is currently no release build. This means that bugs or missing features are possible, but the program should still function correctly in almost all cases.

### Getting started:

All dependences used in this project are free to distribute, so they are all packaged under `lib/`. The default `CMakeLists` should be configured to build the main Butter executable for Windows (Linux compatibility coming soon!) GCC 14.2.0 is recommended for compiling.

When the program is run, it searches for the `res/` directory using the `respath.txt` file in the executable directory. The default path should match where `res/` was installed, but it's possible that the install configuration doesn't match what was expected. In this case, simply edit the `respath.txt` file to point to the correct absolute path. In the future, this will be fixed with an auto-installer.

Finally, feel free to peruse the source code in `src/`!


### Using the program:

The interface functions like a typical editing software. A lot of functionality is currently integrated into the terminal (use `Ctrl+P` to access). Available terminal commands are listed below:

```
> log [value: String]

> ui_scale [value: Int]

> new

> save

> save_as [name: String]

> load [name: String]

> create_clip [start: Uint] [length: Uint], [color: String]

> select_all

> deselect

> new_color_media [name: String] [hex_color: String]

> new_image_media [name: String] [filepath: String]
```

### Currently available features:
- Creating colors in Media module
- Importing images to Media module
- Dragging onto timeline
- Changing clip times & duration
- Previewing output in rendering window
- Viewing project information
- Project saving & loading

### WIP features:
- Multithreaded rendering & exporting
- Video importing, previewing, and exporting
- Directly dragging images/videos into media module

### Coming soon!
- Multiple layers in timeline
- Advanced timeline controls
- Controlling timeline transforms through Python scripts
- Improved user interface for creating projects
- GLSL shaders in the timeline!
- Sub-timelines
- Generators (dynamic scenes controlled with a single Python script)