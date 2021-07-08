#include <stdbool.h>
#include <stdint.h>

#pragma once

#define MAX_COLOR_ATTACHMENTS 4
#define MAX_VIEWS 6

struct Image;

typedef struct Buffer Buffer;
typedef struct Texture Texture;
typedef struct Canvas Canvas;
typedef struct Shader Shader;
typedef struct Batch Batch;

typedef struct {
  uint32_t vendorId;
  uint32_t deviceId;
  const char* deviceName;
  const char* renderer;
  uint32_t driverMajor;
  uint32_t driverMinor;
  uint32_t driverPatch;
  bool discrete;
} GraphicsHardware;

typedef struct {
  bool bptc;
  bool astc;
  bool pointSize;
  bool wireframe;
  bool multiblend;
  bool anisotropy;
  bool depthClamp;
  bool depthNudgeClamp;
  bool clipDistance;
  bool cullDistance;
  bool fullIndexBufferRange;
  bool indirectDrawFirstInstance;
  bool extraShaderInputs;
  bool dynamicIndexing;
  bool float64;
  bool int64;
  bool int16;
} GraphicsFeatures;

typedef struct {
  uint32_t textureSize2D;
  uint32_t textureSize3D;
  uint32_t textureSizeCube;
  uint32_t textureLayers;
  uint32_t renderWidth;
  uint32_t renderHeight;
  uint32_t renderViews;
  uint32_t bundleCount;
  uint32_t bundleSlots;
  uint32_t uniformBufferRange;
  uint32_t storageBufferRange;
  uint32_t uniformBufferAlign;
  uint32_t storageBufferAlign;
  uint32_t vertexAttributes;
  uint32_t vertexAttributeOffset;
  uint32_t vertexBuffers;
  uint32_t vertexBufferStride;
  uint32_t vertexShaderOutputs;
  uint32_t computeCount[3];
  uint32_t computeGroupSize[3];
  uint32_t computeGroupVolume;
  uint32_t computeSharedMemory;
  uint32_t indirectDrawCount;
  uint64_t allocationSize;
  uint32_t pointSize[2];
  float anisotropy;
} GraphicsLimits;

bool lovrGraphicsInit(bool debug);
void lovrGraphicsDestroy(void);
void lovrGraphicsGetHardware(GraphicsHardware* hardware);
void lovrGraphicsGetFeatures(GraphicsFeatures* features);
void lovrGraphicsGetLimits(GraphicsLimits* limits);
void lovrGraphicsBegin(void);
void lovrGraphicsSubmit(void);
void lovrGraphicsRender(Canvas* canvas, Batch* batch);

// Buffer

typedef enum {
  BUFFER_VERTEX,
  BUFFER_INDEX,
  BUFFER_UNIFORM,
  BUFFER_STORAGE
} BufferType;

typedef enum {
  FIELD_I8,
  FIELD_U8,
  FIELD_I16,
  FIELD_U16,
  FIELD_I32,
  FIELD_U32,
  FIELD_F32,
  FIELD_I8x2,
  FIELD_U8x2,
  FIELD_I8Nx2,
  FIELD_U8Nx2,
  FIELD_I16x2,
  FIELD_U16x2,
  FIELD_I16Nx2,
  FIELD_U16Nx2,
  FIELD_I32x2,
  FIELD_U32x2,
  FIELD_F32x2,
  FIELD_I32x3,
  FIELD_U32x3,
  FIELD_F32x3,
  FIELD_I8x4,
  FIELD_U8x4,
  FIELD_I8Nx4,
  FIELD_U8Nx4,
  FIELD_I16x4,
  FIELD_U16x4,
  FIELD_I16Nx4,
  FIELD_U16Nx4,
  FIELD_I32x4,
  FIELD_U32x4,
  FIELD_F32x4,
  FIELD_MAT2,
  FIELD_MAT3,
  FIELD_MAT4
} FieldType;

typedef struct {
  BufferType type;
  bool transient;
  uint16_t stride;
  uint32_t length;
  uint16_t fieldCount;
  FieldType types[16];
  uint16_t offsets[16];
  uintptr_t handle;
  const char* label;
} BufferInfo;

Buffer* lovrBufferCreate(BufferInfo* info);
void lovrBufferDestroy(void* ref);
const BufferInfo* lovrBufferGetInfo(Buffer* buffer);
void* lovrBufferMap(Buffer* buffer, uint32_t offset, uint32_t size);
void lovrBufferClear(Buffer* buffer, uint32_t offset, uint32_t size);
void lovrBufferCopy(Buffer* src, Buffer* dst, uint32_t srcOffset, uint32_t dstOffset, uint32_t size);
void lovrBufferRead(Buffer* buffer, uint32_t offset, uint32_t size, void (*callback)(void* data, uint32_t size, void* userdata), void* userdata);
void lovrBufferDrop(Buffer* buffer);

// Texture

typedef enum {
  TEXTURE_2D,
  TEXTURE_CUBE,
  TEXTURE_VOLUME,
  TEXTURE_ARRAY
} TextureType;

enum {
  TEXTURE_SAMPLE    = (1 << 0),
  TEXTURE_RENDER    = (1 << 1),
  TEXTURE_COMPUTE   = (1 << 2),
  TEXTURE_COPY      = (1 << 3),
  TEXTURE_TRANSIENT = (1 << 4)
};

typedef struct {
  Texture* parent;
  TextureType type;
  uint32_t layerIndex;
  uint32_t layerCount;
  uint32_t levelIndex;
  uint32_t levelCount;
} TextureViewInfo;

typedef struct {
  Texture* parent;
  TextureType type;
  uint32_t format;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t mipmaps;
  uint32_t samples;
  uint32_t flags;
  bool srgb;
  uintptr_t handle;
  const char* label;
} TextureInfo;

Texture* lovrTextureCreate(TextureInfo* info);
Texture* lovrTextureCreateView(TextureViewInfo* view);
void lovrTextureDestroy(void* ref);
const TextureInfo* lovrTextureGetInfo(Texture* texture);
void lovrTextureWrite(Texture* texture, uint16_t offset[4], uint16_t extent[3], void* data, uint32_t step[2]);
void lovrTexturePaste(Texture* texture, struct Image* image, uint16_t srcOffset[4], uint16_t dstOffset[2], uint16_t extent[2]);
void lovrTextureClear(Texture* texture, uint16_t layer, uint16_t layerCount, uint16_t level, uint16_t levelCount, float color[4]);
void lovrTextureRead(Texture* texture, uint16_t offset[4], uint16_t extent[3], void (*callback)(void* data, uint32_t size, void* userdata), void* userdata);
void lovrTextureCopy(Texture* src, Texture* dst, uint16_t srcOffset[4], uint16_t dstOffset[4], uint16_t extent[3]);
void lovrTextureBlit(Texture* src, Texture* dst, uint16_t srcOffset[4], uint16_t dstOffset[4], uint16_t srcExtent[3], uint16_t dstExtent[3], bool nearest);

// Pipeline

typedef enum {
  BLEND_ALPHA_MULTIPLY,
  BLEND_PREMULTIPLIED
} BlendAlphaMode;

typedef enum {
  BLEND_ALPHA,
  BLEND_ADD,
  BLEND_SUBTRACT,
  BLEND_MULTIPLY,
  BLEND_LIGHTEN,
  BLEND_DARKEN,
  BLEND_SCREEN,
  BLEND_NONE
} BlendMode;

typedef enum {
  COMPARE_NONE,
  COMPARE_EQUAL,
  COMPARE_NEQUAL,
  COMPARE_LESS,
  COMPARE_LEQUAL,
  COMPARE_GREATER,
  COMPARE_GEQUAL
} CompareMode;

typedef enum {
  CULL_NONE,
  CULL_FRONT,
  CULL_BACK
} CullMode;

typedef enum {
  DRAW_POINTS,
  DRAW_LINES,
  DRAW_TRIANGLES
} DrawMode;

typedef enum {
  STENCIL_KEEP,
  STENCIL_REPLACE,
  STENCIL_INCREMENT,
  STENCIL_DECREMENT,
  STENCIL_INCREMENT_WRAP,
  STENCIL_DECREMENT_WRAP,
  STENCIL_INVERT
} StencilAction;

typedef struct {
  uint8_t location;
  uint8_t buffer;
  uint8_t fieldType;
  uint8_t offset;
} VertexAttribute;

typedef enum {
  WINDING_COUNTERCLOCKWISE,
  WINDING_CLOCKWISE
} Winding;

// Canvas

typedef enum {
  LOAD_KEEP,
  LOAD_CLEAR,
  LOAD_DISCARD
} LoadAction;

typedef enum {
  SAVE_KEEP,
  SAVE_DISCARD
} SaveAction;

typedef struct {
  Texture* texture;
  LoadAction load;
  SaveAction save;
} ColorAttachment;

typedef struct {
  bool enabled;
  uint32_t format;
  Texture* texture;
  LoadAction load, stencilLoad;
  SaveAction save, stencilSave;
} DepthAttachment;

typedef struct {
  ColorAttachment color[MAX_COLOR_ATTACHMENTS];
  DepthAttachment depth;
  uint32_t count;
  uint32_t samples;
  uint32_t views;
  const char* label;
} CanvasInfo;

Canvas* lovrCanvasCreate(CanvasInfo* info);
Canvas* lovrCanvasGetTemporary(CanvasInfo* info);
Canvas* lovrCanvasGetWindow(void);
void lovrCanvasDestroy(void* ref);
const CanvasInfo* lovrCanvasGetInfo(Canvas* canvas);
uint32_t lovrCanvasGetWidth(Canvas* canvas);
uint32_t lovrCanvasGetHeight(Canvas* canvas);
void lovrCanvasGetViewMatrix(Canvas* canvas, uint32_t index, float* viewMatrix);
void lovrCanvasSetViewMatrix(Canvas* canvas, uint32_t index, float* viewMatrix);
void lovrCanvasGetProjection(Canvas* canvas, uint32_t index, float* projection);
void lovrCanvasSetProjection(Canvas* canvas, uint32_t index, float* projection);
void lovrCanvasGetClear(Canvas* canvas, float color[MAX_COLOR_ATTACHMENTS][4], float* depth, uint8_t* stencil);
void lovrCanvasSetClear(Canvas* canvas, float color[MAX_COLOR_ATTACHMENTS][4], float depth, uint8_t stencil);
void lovrCanvasGetTextures(Canvas* canvas, Texture* textures[4], Texture** depth);
void lovrCanvasSetTextures(Canvas* canvas, Texture* textures[4], Texture* depth);

// Shader

typedef enum {
  SHADER_GRAPHICS,
  SHADER_COMPUTE
} ShaderType;

typedef struct {
  ShaderType type;
  const void* source[2];
  uint32_t length[2];
  const char** dynamicBuffers;
  uint32_t dynamicBufferCount;
  const char* label;
} ShaderInfo;

Shader* lovrShaderCreate(ShaderInfo* info);
void lovrShaderDestroy(void* ref);
const ShaderInfo* lovrShaderGetInfo(Shader* shader);
bool lovrShaderResolveName(Shader* shader, uint64_t hash, uint32_t* group, uint32_t* id);
void lovrShaderCompute(Shader* shader, uint32_t x, uint32_t y, uint32_t z);
void lovrShaderComputeIndirect(Shader* shader, Buffer* buffer, uint32_t offset);

// Batch

typedef struct {
  DrawMode mode;
  uint32_t bufferCount;
  Buffer* vertexBuffers[8];
  Buffer* indexBuffer;
  uint32_t start;
  uint32_t count;
  uint32_t instances;
  Buffer* indirectBuffer;
  uint32_t indirectOffset;
  uint32_t indirectCount;
  float transform[16];
} DrawInfo;

typedef struct {
  uint32_t capacity;
  bool transient;
} BatchInfo;

Batch* lovrBatchCreate(BatchInfo* info);
void lovrBatchDestroy(void* ref);
void lovrBatchReset(Batch* batch);
void lovrBatchPush(Batch* batch);
void lovrBatchPop(Batch* batch);
void lovrBatchOrigin(Batch* batch);
void lovrBatchTranslate(Batch* batch, float* translation);
void lovrBatchRotate(Batch* batch, float* rotation);
void lovrBatchScale(Batch* batch, float* scale);
void lovrBatchTransform(Batch* batch, float* transform);
bool lovrBatchGetAlphaToCoverage(Batch* batch);
void lovrBatchSetAlphaToCoverage(Batch* batch, bool enabled);
void lovrBatchGetBlendMode(Batch* batch, uint32_t target, BlendMode* mode, BlendAlphaMode* alphaMode);
void lovrBatchSetBlendMode(Batch* batch, uint32_t target, BlendMode mode, BlendAlphaMode alphaMode);
void lovrBatchGetColorMask(Batch* batch, uint32_t target, bool* r, bool* g, bool* b, bool* a);
void lovrBatchSetColorMask(Batch* batch, uint32_t target, bool r, bool g, bool b, bool a);
CullMode lovrBatchGetCullMode(Batch* batch);
void lovrBatchSetCullMode(Batch* batch, CullMode mode);
void lovrBatchGetDepthTest(Batch* batch, CompareMode* test, bool* write);
void lovrBatchSetDepthTest(Batch* batch, CompareMode test, bool write);
void lovrBatchGetDepthNudge(Batch* batch, float* nudge, float* sloped, float* clamp);
void lovrBatchSetDepthNudge(Batch* batch, float nudge, float sloped, float clamp);
bool lovrBatchGetDepthClamp(Batch* batch);
void lovrBatchSetDepthClamp(Batch* batch, bool clamp);
Shader* lovrBatchGetShader(Batch* batch);
void lovrBatchSetShader(Batch* batch, Shader* shader);
void lovrBatchGetStencilTest(Batch* batch, CompareMode* test, uint8_t* value);
void lovrBatchSetStencilTest(Batch* batch, CompareMode test, uint8_t value);
void lovrBatchGetVertexFormat(Batch* batch, VertexAttribute attributes[16], uint32_t* count);
void lovrBatchSetVertexFormat(Batch* batch, VertexAttribute attributes[16], uint32_t count);
Winding lovrBatchGetWinding(Batch* batch);
void lovrBatchSetWinding(Batch* batch, Winding winding);
bool lovrBatchIsWireframe(Batch* batch);
void lovrBatchSetWireframe(Batch* batch, bool wireframe);
void lovrBatchDraw(Batch* batch, DrawInfo* info);
