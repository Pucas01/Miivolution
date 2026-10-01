# Miivolution

Miivolution is a (mostly) drop-in compatibility layer for RVLFaceLib with modern enhancements and features to power PC ports, GUI editors, or any other Mii behavior you may want.

## Dependencies

* A Dolphin source compatibility layer (E.g. [Aurora](https://github.com/encounter/aurora))

## Architecture

Miivolution's include layout mirrors RVLFaceLib almost exactly, but adds a `Miivolution/` include directory where you can access a suite of extra functionality, such as:
* Exporting a Mii to disc using our custom format (`.mii`)
* Importing a Mii from disc using our custom format (`.mii`)
* Getting raw model data from a Mii, for rendering in other applications
* Exporting a Mii to modern model formats (`obj`, `gltf`, etc.)
