# ImageToIcon

A Tracker add-on for [Haiku](https://www.haiku-os.org) which creates old BeOS style raster icons from image files.  This is a fork of the original BeOS add-on by Vincent Cedric.

## Usage Instructions

Right click an image file and choose the `ImageToIcon` add-on.  The icons will be saved as BFS attributes of the image file that was selected.  They are stored using the traditional BeOS `BEOS:L:STD_ICON` and `BEOS:M:STD_ICON` attribute names.

## Build Instructions

You must have the `jamfile_engine` package installed in order to build.

```
~> cd ImageToIcon
~/ImageToIcon> jam
~/ImageToIcon> jam install
```

*You may need to reboot(or restart Tracker) after installing for the first time*
