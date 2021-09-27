# Mesmerize

Mesmerize is a picture viewer that creates an animated slideshow. You just need to point it at a 
directory with png and jpg files, and it will create this effect:

<video controls autoplay muted loop>
    <source src="https://www.blinkingcaret.com/wp-content/uploads/2021/09/MesmerizeDemo.mp4" type="video/mp4">
    Your browser does not support video
</video>

    usage: [--window|--full-screen] [--resolution=widthxheight|--resolution=max] [--display-picture-duration=duration_in_seconds]
    [--opacity-animation-duration=duration_in_seconds] [--zoom-animation-duration=duration_in_seconds]
    [--pan-animation-duration=duration_in_seconds] [--zoom-percentage=number_1_to_100]
    [--pan-percentage=number_1_to_100] path_to_folder_with_images_jpg_and_png
    
    
    Defaults:
    --window
    --resolution=max (1920x1080 is what works best on a pi 4)
    --display-picture-duration=15
    --opacity-animation-duration=4
    --zoom-animation-duration=20
    --pan-animation-duration=10
    --zoom-percentage=5
    --pan-percentage=5

To compile from source (make sure you have cmake installed):

    $ git clone --recurse-submodules this repo
    $ ./configure-debug.sh
    $ cd build
    $ make

If you run configure debug the shaders in the shaders' folder will be used (they are just text files you can edit). 
In release mode the shaders that are hardcoded in `Picture.cpp` are used instead.

To compile in debug mode:

    $ ./configure.sh
    $ cd build
    $ make

If you want to run with extra debug info that shows fps and has controls to change image run make like this:

    $ make CXXFLAGS=-DENABLE_IMGUI

