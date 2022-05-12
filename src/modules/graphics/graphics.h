#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#pragma once

struct Blob;
struct Image;

typedef struct Buffer Buffer;
typedef struct Texture Texture;
typedef struct Sampler Sampler;
typedef struct Shader Shader;
typedef struct Pass Pass;

typedef struct {
  uint32_t deviceId;
  uint32_t vendorId;
  const char* name;
  const char* renderer;
  uint32_t subgroupSize;
  bool discrete;
} GraphicsDevice;

typedef struct {
  bool textureBC;
  bool textureASTC;
  bool wireframe;
  bool depthClamp;
  bool indirectDrawFirstInstance;
  bool float64;
  bool int64;
  bool int16;
} GraphicsFeatures;

typedef struct {
  uint32_t textureSize2D;
  uint32_t textureSize3D;
  uint32_t textureSizeCube;
  uint32_t textureLayers;
  uint32_t renderSize[3];
  uint32_t uniformBufferRange;
  uint32_t storageBufferRange;
  uint32_t uniformBufferAlign;
  uint32_t storageBufferAlign;
  uint32_t vertexAttributes;
  uint32_t vertexBufferStride;
  uint32_t vertexShaderOutputs;
  uint32_t clipDistances;
  uint32_t cullDistances;
  uint32_t clipAndCullDistances;
  uint32_t computeDispatchCount[3];
  uint32_t computeWorkgroupSize[3];
  uint32_t computeWorkgroupVolume;
  uint32_t computeSharedMemory;
  uint32_t shaderConstantSize;
  uint32_t indirectDrawCount;
  uint32_t instances;
  float anisotropy;
  float pointSize;
} GraphicsLimits;

enum {
  TEXTURE_FEATURE_SAMPLE   = (1 << 0),
  TEXTURE_FEATURE_FILTER   = (1 << 1),
  TEXTURE_FEATURE_RENDER   = (1 << 2),
  TEXTURE_FEATURE_BLEND    = (1 << 3),
  TEXTURE_FEATURE_STORAGE  = (1 << 4),
  TEXTURE_FEATURE_ATOMIC   = (1 << 5),
  TEXTURE_FEATURE_BLIT_SRC = (1 << 6),
  TEXTURE_FEATURE_BLIT_DST = (1 << 7)
};

bool lovrGraphicsInit(bool debug, bool vsync);
void lovrGraphicsDestroy(void);

void lovrGraphicsGetDevice(GraphicsDevice* device);
void lovrGraphicsGetFeatures(GraphicsFeatures* features);
void lovrGraphicsGetLimits(GraphicsLimits* limits);
bool lovrGraphicsIsFormatSupported(uint32_t format, uint32_t features);

void lovrGraphicsGetBackground(float background[4]);
void lovrGraphicsSetBackground(float background[4]);

void lovrGraphicsSubmit(Pass** passes, uint32_t count);
void lovrGraphicsWait(void);

// Buffer

typedef enum {
  FIELD_I8x4,
  FIELD_U8x4,
  FIELD_SN8x4,
  FIELD_UN8x4,
  FIELD_UN10x3,
  FIELD_I16,
  FIELD_I16x2,
  FIELD_I16x4,
  FIELD_U16,
  FIELD_U16x2,
  FIELD_U16x4,
  FIELD_SN16x2,
  FIELD_SN16x4,
  FIELD_UN16x2,
  FIELD_UN16x4,
  FIELD_I32,
  FIELD_I32x2,
  FIELD_I32x3,
  FIELD_I32x4,
  FIELD_U32,
  FIELD_U32x2,
  FIELD_U32x3,
  FIELD_U32x4,
  FIELD_F16x2,
  FIELD_F16x4,
  FIELD_F32,
  FIELD_F32x2,
  FIELD_F32x3,
  FIELD_F32x4,
  FIELD_MAT2,
  FIELD_MAT3,
  FIELD_MAT4
} FieldType;

typedef struct {
  uint16_t offset;
  uint8_t location;
  uint8_t type;
} BufferField;

typedef enum {
  LAYOUT_PACKED,
  LAYOUT_STD140,
  LAYOUT_STD430
} BufferLayout;

typedef struct {
  uint32_t length;
  uint32_t stride;
  uint32_t fieldCount;
  BufferField fields[16];
  void** pointer;
  const char* label;
  uintptr_t handle;
} BufferInfo;

Buffer* lovrGraphicsGetBuffer(BufferInfo* info, void** data);
Buffer* lovrBufferCreate(BufferInfo* info, void** data);
void lovrBufferDestroy(void* ref);
const BufferInfo* lovrBufferGetInfo(Buffer* buffer);
bool lovrBufferIsTemporary(Buffer* buffer);
void* lovrBufferMap(Buffer* buffer, uint32_t offset, uint32_t size);
void lovrBufferClear(Buffer* buffer, uint32_t offset, uint32_t size);

// Texture

typedef enum {
  TEXTURE_2D,
  TEXTURE_3D,
  TEXTURE_CUBE,
  TEXTURE_ARRAY
} TextureType;

enum {
  TEXTURE_SAMPLE   = (1 << 0),
  TEXTURE_RENDER   = (1 << 1),
  TEXTURE_STORAGE  = (1 << 2),
  TEXTURE_TRANSFER = (1 << 3)
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
  uint32_t usage;
  bool srgb;
  uintptr_t handle;
  uint32_t imageCount;
  struct Image** images;
  const char* label;
} TextureInfo;

Texture* lovrGraphicsGetWindowTexture(void);
Texture* lovrTextureCreate(TextureInfo* info);
Texture* lovrTextureCreateView(TextureViewInfo* view);
void lovrTextureDestroy(void* ref);
const TextureInfo* lovrTextureGetInfo(Texture* texture);

// Sampler

typedef enum {
  FILTER_NEAREST,
  FILTER_LINEAR
} FilterMode;

typedef enum {
  WRAP_CLAMP,
  WRAP_REPEAT,
  WRAP_MIRROR
} WrapMode;

typedef enum {
  COMPARE_NONE,
  COMPARE_EQUAL,
  COMPARE_NEQUAL,
  COMPARE_LESS,
  COMPARE_LEQUAL,
  COMPARE_GREATER,
  COMPARE_GEQUAL
} CompareMode;

typedef struct {
  FilterMode min, mag, mip;
  WrapMode wrap[3];
  CompareMode compare;
  float anisotropy;
  float range[2];
} SamplerInfo;

Sampler* lovrSamplerCreate(SamplerInfo* info);
void lovrSamplerDestroy(void* ref);
const SamplerInfo* lovrSamplerGetInfo(Sampler* sampler);

// Shader

typedef enum {
  SHADER_GRAPHICS,
  SHADER_COMPUTE
} ShaderType;

typedef enum {
  STAGE_VERTEX,
  STAGE_FRAGMENT,
  STAGE_COMPUTE
} ShaderStage;

typedef struct {
  const char* name;
  uint32_t id;
  double value;
} ShaderFlag;

typedef struct {
  ShaderType type;
  struct Blob* stages[2];
  uint32_t flagCount;
  ShaderFlag* flags;
  const char* label;
} ShaderInfo;

struct Blob* lovrGraphicsCompileShader(ShaderStage stage, struct Blob* source);
Shader* lovrShaderCreate(ShaderInfo* info);
void lovrShaderDestroy(void* ref);
const ShaderInfo* lovrShaderGetInfo(Shader* shader);

// Pass

typedef enum {
  PASS_RENDER,
  PASS_COMPUTE,
  PASS_TRANSFER
} PassType;

typedef enum {
  STACK_TRANSFORM,
  STACK_PIPELINE
} StackType;

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
  CULL_NONE,
  CULL_FRONT,
  CULL_BACK
} CullMode;

typedef enum {
  STENCIL_KEEP,
  STENCIL_ZERO,
  STENCIL_REPLACE,
  STENCIL_INCREMENT,
  STENCIL_DECREMENT,
  STENCIL_INCREMENT_WRAP,
  STENCIL_DECREMENT_WRAP,
  STENCIL_INVERT
} StencilAction;

typedef enum {
  WINDING_COUNTERCLOCKWISE,
  WINDING_CLOCKWISE
} Winding;

typedef enum {
  LOAD_KEEP,
  LOAD_CLEAR,
  LOAD_DISCARD
} LoadAction;

typedef struct {
  Texture* texture;
  uint32_t format;
  LoadAction load;
  float clear;
} DepthBuffer;

typedef struct {
  Texture* textures[4];
  LoadAction loads[4];
  float clears[4][4];
  DepthBuffer depth;
  uint32_t samples;
} Canvas;

typedef struct {
  PassType type;
  Canvas canvas;
  const char* label;
} PassInfo;

Pass* lovrGraphicsGetPass(PassInfo* info);
void lovrPassDestroy(void* ref);
const PassInfo* lovrPassGetInfo(Pass* pass);

// Render
void lovrPassPush(Pass* pass, StackType stack);
void lovrPassPop(Pass* pass, StackType stack);
void lovrPassOrigin(Pass* pass);
void lovrPassTranslate(Pass* pass, float* translation);
void lovrPassRotate(Pass* pass, float* rotation);
void lovrPassScale(Pass* pass, float* scale);
void lovrPassTransform(Pass* pass, float* transform);
void lovrPassSetAlphaToCoverage(Pass* pass, bool enabled);
void lovrPassSetBlendMode(Pass* pass, BlendMode mode, BlendAlphaMode alphaMode);
void lovrPassSetColor(Pass* pass, float color[4]);
void lovrPassSetColorWrite(Pass* pass, bool r, bool g, bool b, bool a);
void lovrPassSetCullMode(Pass* pass, CullMode mode);
void lovrPassSetDepthTest(Pass* pass, CompareMode test);
void lovrPassSetDepthWrite(Pass* pass, bool write);
void lovrPassSetDepthOffset(Pass* pass, float offset, float sloped);
void lovrPassSetDepthClamp(Pass* pass, bool clamp);
void lovrPassSetShader(Pass* pass, Shader* shader);
void lovrPassSetStencilTest(Pass* pass, CompareMode test, uint8_t value, uint8_t mask);
void lovrPassSetStencilWrite(Pass* pass, StencilAction actions[3], uint8_t value, uint8_t mask);
void lovrPassSetWinding(Pass* pass, Winding winding);
void lovrPassSetWireframe(Pass* pass, bool wireframe);
void lovrPassSendBuffer(Pass* pass, const char* name, size_t length, uint32_t slot, Buffer* buffer, uint32_t offset, uint32_t extent);
void lovrPassSendTexture(Pass* pass, const char* name, size_t length, uint32_t slot, Texture* texture);
void lovrPassSendSampler(Pass* pass, const char* name, size_t length, uint32_t slot, Sampler* sampler);
void lovrPassSendConstant(Pass* pass, const char* name, size_t length, void** data, FieldType* type);
