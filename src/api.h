#include "luax.h"

#ifdef _WIN32
#define LOVR_API __declspec(dllexport)
#else
#define LOVR_API
#endif

// Module loaders
LOVR_API int luaopen_lovr(lua_State* L);
LOVR_API int luaopen_lovr_audio(lua_State* L);
LOVR_API int luaopen_lovr_data(lua_State* L);
LOVR_API int luaopen_lovr_event(lua_State* L);
LOVR_API int luaopen_lovr_filesystem(lua_State* L);
LOVR_API int luaopen_lovr_graphics(lua_State* L);
LOVR_API int luaopen_lovr_headset(lua_State* L);
LOVR_API int luaopen_lovr_math(lua_State* L);
LOVR_API int luaopen_lovr_physics(lua_State* L);
LOVR_API int luaopen_lovr_thread(lua_State* L);
LOVR_API int luaopen_lovr_timer(lua_State* L);

// Objects
extern const luaL_Reg lovrAnimator[];
extern const luaL_Reg lovrAudioStream[];
extern const luaL_Reg lovrBallJoint[];
extern const luaL_Reg lovrBlob[];
extern const luaL_Reg lovrBoxShape[];
extern const luaL_Reg lovrCanvas[];
extern const luaL_Reg lovrCapsuleShape[];
extern const luaL_Reg lovrChannel[];
extern const luaL_Reg lovrController[];
extern const luaL_Reg lovrCylinderShape[];
extern const luaL_Reg lovrCollider[];
extern const luaL_Reg lovrDistanceJoint[];
extern const luaL_Reg lovrFont[];
extern const luaL_Reg lovrHingeJoint[];
extern const luaL_Reg lovrJoint[];
extern const luaL_Reg lovrMaterial[];
extern const luaL_Reg lovrMesh[];
extern const luaL_Reg lovrMicrophone[];
extern const luaL_Reg lovrModel[];
extern const luaL_Reg lovrModelData[];
extern const luaL_Reg lovrRandomGenerator[];
extern const luaL_Reg lovrRasterizer[];
extern const luaL_Reg lovrShader[];
extern const luaL_Reg lovrShaderBlock[];
extern const luaL_Reg lovrShape[];
extern const luaL_Reg lovrSliderJoint[];
extern const luaL_Reg lovrSoundData[];
extern const luaL_Reg lovrSource[];
extern const luaL_Reg lovrSphereShape[];
extern const luaL_Reg lovrTexture[];
extern const luaL_Reg lovrTextureData[];
extern const luaL_Reg lovrThread[];
extern const luaL_Reg lovrTransform[];
extern const luaL_Reg lovrVertexData[];
extern const luaL_Reg lovrWorld[];

// Enums
extern const char* ArcModes[];
extern const char* AttributeTypes[];
extern const char* BlendAlphaModes[];
extern const char* BlendModes[];
extern const char* BufferUsages[];
extern const char* CompareModes[];
extern const char* ControllerAxes[];
extern const char* ControllerButtons[];
extern const char* ControllerHands[];
extern const char* DrawModes[];
extern const char* EventTypes[];
extern const char* FilterModes[];
extern const char* HeadsetDrivers[];
extern const char* HeadsetEyes[];
extern const char* HeadsetOrigins[];
extern const char* HeadsetTypes[];
extern const char* HorizontalAligns[];
extern const char* JointTypes[];
extern const char* MaterialColors[];
extern const char* MaterialScalars[];
extern const char* MaterialTextures[];
extern const char* MeshDrawModes[];
extern const char* ShaderTypes[];
extern const char* ShapeTypes[];
extern const char* SourceTypes[];
extern const char* StencilActions[];
extern const char* TextureFormats[];
extern const char* TextureTypes[];
extern const char* TimeUnits[];
extern const char* UniformAccesses[];
extern const char* VerticalAligns[];
extern const char* Windings[];
extern const char* WrapModes[];
