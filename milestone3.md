## Milestone3

## Final demo of UI functional menus 
- Used DirectX API to check mouse positions and events, integrate with Prime Engine codes
- Add more buttons to manipulate the gameobject positions because original Prime Engine does not have a feature to directly move the gameobject in runtime. 
(You can only move the camera with keyboard in the default version. I might have said 'mouse' in the video, but correcting it to 'keyboard' in the document here.)

- Add the feature of Creating lights - Point light (red), Directional light (green), Spot light(blue) directly with the buttons, instead of using pyclient external menu
- Modified text mesh shader code and real-time updating the color for buttons
- Change the color of each button when toggle_buttons are activated for better UX 
- Toggle show/hide the debug information
- Toggle show/hide submenus for several buttons
- Printing out whether camera is moving left/right/up/down for both keyboard and mouse-button inputs
- clean up code
- google drive link: https://drive.google.com/drive/folders/19KwIBqbyGhLypsi379TS569kn0Dh8-77?usp=sharing
or https://tinyurl.com/ss-csci522m3


## Milestone 1~2 notes

Overall milestones will focus on UI system. 

### milestone 1
- Add more information on text, render interactive/dynamically changing information on Text Overlay
- Manipulate/Customize overlay 2D Texts inside PE: color change, shader files,
	- worked on some stuff related to graphics / shader since Text renderer deals with shader files and buffers
- Successfully imported IMGUI library and customized the menus
- Interact with Camera, implementing WASD features into UI Button components, output camera positions information
- Add color picker to dynamically change the colors of texts that are directly rendered on overlay 2D of Prime Engine
	- Different color for different information
- Directly changing the Soldier NPC position XYZ with number inputs via UI system.
- And more on video explanations... (GOOGLE DRIVE)

https://drive.google.com/drive/folders/1KD5Fkr0bCyA_NpHm9UMjKnlWtMkqS2iy?usp=sharing 

### milestone2
- Find viewport offset position in screen: 
- position of textmesh
- mouse click interaction events
	- Save Text positions at init, don't change until the window size changes
	- check if mouse is clicking any of the -5 < text positions < 5
	- seems like the text coordinate posx posy is left bottom coordinate. (not left top) 
- button click event
- input of number?
	- change the object position


- Basic_X_level / lightingTest.level load
  - LIGHTING process

1. StandardEvents.cpp::int Event_CREATE_LIGHT::l_Construct(lua_State* luaVM)

2. void GameObjectManager::do_CREATE_LIGHT(Events::Event *pEvt)
- peuuid: (assign random int, non-zero)
- havObject: false

3. error
```cpp
// MemoryPool.h
// Exception thrown: read access violation.
// **this** was nullptr.
	void freeBlock(unsigned int memoryBlockIndex)
	{
		m_freeBlockList[m_header.m_nBlocksFree++] = memoryBlockIndex;
	}
```

### SHADER Variables

The values from bufferCPU in TextMesh.cpp are transferred into pIn.iNormalW inside the shader code through the process of vertex buffer binding and drawing.

Here's a step-by-step explanation:

In TextMesh.cpp, you're creating a NormalBufferCPU and filling it with RGB values. This buffer represents the normals of the vertices of your mesh.
When you call loadFromMeshCPU_needsRC or updateGeoFromMeshCPU_needsRC, these buffers are transferred to the GPU. This is done by creating a GPU buffer (VertexBufferGPU or IndexBufferGPU) and copying the data from the CPU buffer to the GPU buffer.

When you draw the mesh, you bind these GPU buffers to the input assembler stage of the GPU pipeline. This means that when the GPU is processing the vertices, it will use the data from these buffers.

In your vertex shader (StdMesh_2D_VS), you have an input parameter vIn of type STD_MESH_VS_IN. This struct has a member iNormal, which corresponds to the normal buffer you've created.

So, when the vertex shader is executed for each vertex, vIn.iNormal will contain the corresponding value from the normal buffer, which you've filled with RGB values in TextMesh.cpp.

### plan for milestone3
- compare the latest version with the original PE. think of it as company submission demo
- try similar logic/way with EVENT_CREATE_SKELETON?
	- replicate int Event_CREATE_SKELETON::l_Construct(lua_State* luaVM)
	- call void GameObjectManager::do_CREATE_SKELETON(Events::Event *pEvt) directly
	- bypass lua stuff
  - Review: it was much more complicated than Create_light because it is grabbing more resources from maya, lua files.
- add more scenenode obj buttons.

[x] add Create-event-light with button on top of PE

[x] toggle each menus 

[x] increasing the textSceneNode from 64 to 128 made flickering go away