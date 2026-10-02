# Miivolution

Miivolution is a (mostly) drop-in compatibility layer for RVLFaceLib with modern enhancements and features to power PC ports, GUI editors, or any other Mii behavior you may want.

## Dependencies

* A Dolphin source compatibility layer (E.g. [Aurora](https://github.com/encounter/aurora))

## Architecture

Miivolution's include layout mirrors RVLFaceLib almost exactly, but adds a `Miivolution/` include directory where you can access a suite of extra functionality, such as:
* Exporting a Mii to disc using the Mii format
* Importing a Mii from disc using the Mii format
* Eventually getting raw model data from a Mii, for rendering in other applications
* Eventually exporting a Mii to modern model formats (`obj`, `gltf`, etc.)

## Not-planned Features (for now)

* NWC24 support
* Wiimote Mii support
