# Open Source Video Recorder
Video Recorder application that written in C++ with QT and OpenCV libraries

## Development & build:
* Install QT and OpenCV libraries
* Run "cmake" command in build folder: `cmake ..`
* Run "make" command in build folder: `make`

Then Run "VideoRecorder" applciation: `./VideoRecorder`

### Troubleshooting:
* In some cases(specially in Mac OS), you need to point out C++ compiler. It should be passing as parameter into "make" command:
`make CXX=/opt/homebrew/Cellar/gcc/14.2.0_1/lib/gcc`

### To-Do
* Sound Recording will be implemented
* Selecting resolutions will be implemented
* Multiple camera support will be implemented
