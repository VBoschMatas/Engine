
# UPC Game Engine

A game engine developed using C++ and utilising OpenGL for the 'Programming for AAA games' Course at UPC Barcelona

https://github.com/VBoschMatas/Engine

https://www.talent.upc.edu/ing/


## Screenshots

![App Screenshot](https://github.com/VBoschMatas/Engine/blob/Assigment-2/Screenshot/Screenshot.jpg?raw=true)


## Team

**Vicenç Bosch Matas** 
- https://github.com/VBoschMatas

**Edward Regas-Duncan** 
- https://github.com/EdwardRegasDuncan

## Usage

- **Left click** to select GameObjects and navigate the menus
- Hold **Right click** to aim the camera
- Hold **Middle mouse button** to pan through the scene
- With **Right click** held, **W, A, S, D, Q, E** for first person camera controls
- With an object selected **Left Alt + hold Left mouse button** to orbit
- **F** with object selected to focus it in the scene
- **Mouse wheel** or **Left Alt + hold Right mouse button** to zoom

## Menus
- **Scene:** The viewport for which GameObjects can be manipulated in 3D space
- **Hierarchy:** a list of every GameObject in the scene
- **Resources:** all the assets currently available in the software
- **Console:** Output for information about Engine events
- **Inspector:** Information and controls for the currently selected GameObject
- **Ambient Light:** Controls for the overall lighting in the scene
- **Editor Camera:** Controls for the editor camera

## Features

- Drag and Drop asset importer
- Light Source Management
- QuadTree Culling
- Dynamic UI
- Performance Monitoring


## Notes
Gizmo tools appear when a GameObject is selected but do not function yet, please edit the objects transform in the inspector for now