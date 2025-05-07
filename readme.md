## Test Fit to Monitor

This file was created to work on Issue: [[rcore] Desktop full screen offset on y-axis #4880](https://github.com/raysan5/raylib/issues/4880)

### Setup:
- Windows 10
- gcc 14.2.0
- raylib 5.6-dev (commit-id: [3d6e24a](https://github.com/raysan5/raylib/tree/3d6e24af4fb1a16982185f61704202c4f009fc51)) 
- OpenGL 3.3.0 

### Compilation:
```
gcc test_fitToWindow.c glad/src/gl.c  \
        -Iglfw-3.4.bin.WIN64/glfw-3.4.bin.WIN64/include \
        -Iglad/include \
        -Lglfw-3.4.bin.WIN64/glfw-3.4.bin.WIN64/lib-mingw-w64 \
        -lglfw3 -lgdi32 -lopengl32 \
        -o test.exe

./test
```
### Testing
change `screen_width` and `screen_height` and re-compile.

### Others stuff
Replacing variables like `CORE.Window.screen.width` to `screen_width` in vim
```regex
:%s/CORE\.Window\.\(screen\|display\|render\|currentFbo\)\.\(width\|height\)/\1_\2/g
```