#include "spv.h"
#include <string.h>

// Notes about this spirv parser:
// - It sucks!!!  Don't use it!
// - This is a smol version of spirv-reflect for lovr's purposes, it may even be good to switch to
//   spirv-reflect in the future if the amount of parsing required grows
// - Its job is to just tell you what's in the file, you have to decide if you consider it valid
// - Limitations:
//   - Max ID bound is 65534
//   - Doesn't support multiple entry points in one module (I think?)
//   - Input variables with a location > 31 are ignored since they don't fit in the 32-bit mask
//   - Max supported descriptor set or binding number is 255
//   - Doesn't parse stuff lovr doesn't care about: texel buffers, geometry/tessellation, etc. etc.

typedef union {
  struct {
    uint8_t location;
  } input;
  struct {
    uint8_t set;
    uint8_t binding;
    uint16_t name;
  } resource;
  struct {
    uint16_t number;
    uint16_t name;
  } flag;
  struct {
    uint32_t word;
  } constant;
  struct {
    uint16_t word;
    uint16_t name;
  } type;
} spv_cache;

typedef struct {
  const uint32_t* words;
  uint32_t wordCount;
  uint32_t bound;
  spv_cache* cache;
} spv_context;

typedef struct {
  const uint32_t* words;
  uint16_t code;
  uint16_t count;
} spv_op;

static spv_result spv_parse_capability(spv_context* spv, spv_op* op, spv_info* info);
static spv_result spv_parse_name(spv_context* spv, spv_op* op, spv_info* info);
static spv_result spv_parse_decoration(spv_context* spv, spv_op* op, spv_info* info);
static spv_result spv_parse_type(spv_context* spv, spv_op* op, spv_info* info);
static spv_result spv_parse_spec_constant(spv_context* spv, spv_op* op, spv_info* info);
static spv_result spv_parse_constant(spv_context* spv, spv_op* op, spv_info* info);
static spv_result spv_parse_variable(spv_context* spv, spv_op* op, spv_info* info);

spv_result spv_parse(const void* source, uint32_t size, spv_info* info) {
  spv_context spv;

  spv.words = source;
  spv.wordCount = size / sizeof(uint32_t);

  if (spv.wordCount < 16 || spv.words[0] != 0x07230203) {
    return SPV_INVALID;
  }

  spv.bound = spv.words[3];

  if (spv.bound >= 0xffff) {
    return SPV_TOO_BIG;
  }

  spv_cache cache[65536];
  memset(cache, 0xff, spv.bound * sizeof(spv_cache));
  spv.cache = cache;

  info->inputLocationMask = 0;
  info->featureCount = 0;
  info->specConstantCount = 0;
  info->pushConstantCount = 0;
  info->pushConstantSize = 0;
  info->resourceCount = 0;

  spv_op op = { .words = spv.words + 5 };

  while (op.words < spv.words + spv.wordCount) {
    op.code = op.words[0] & 0xffff;
    op.count = op.words[0] >> 16;

    if (op.count == 0 || op.words + op.count > spv.words + spv.wordCount) {
      return SPV_INVALID;
    }

    spv_result result = SPV_OK;

    switch (op.code) {
      case 17: // OpCapability
        result = spv_parse_capability(&spv, &op, info);
        break;
      case 5: // OpName
        result = spv_parse_name(&spv, &op, info);
        break;
      case 71: // OpDecorate
        result = spv_parse_decoration(&spv, &op, info);
        break;
      case 19: // OpTypeVoid
      case 20: // OpTypeBool
      case 21: // OpTypeInt
      case 22: // OpTypeFloat
      case 23: // OpTypeVector
      case 24: // OpTypeMatrix
      case 25: // OpTypeImage
      case 26: // OpTypeSampler
      case 27: // OpTypeSampledImage
      case 28: // OpTypeArray
      case 29: // OpTypeRuntimeArray
      case 30: // OpTypeStruct
      case 31: // OpTypeOpaque
      case 32: // OpTypePointer
        result = spv_parse_type(&spv, &op, info);
        break;
      case 48: // OpSpecConstantTrue
      case 49: // OpSpecConstantFalse
      case 50: // OpSpecConstant
        result = spv_parse_spec_constant(&spv, &op, info);
        break;
      case 43: // OpConstant
        result = spv_parse_constant(&spv, &op, info);
        break;
      case 59: // OpVariable
        result = spv_parse_variable(&spv, &op, info);
        break;
      case 54: // OpFunction
        op.words = spv.words + spv.wordCount; // Can exit early upon reaching actual code
        op.count = 0;
        break;
    }

    if (result) {
      return result;
    }

    op.words += op.count;
  }

  return SPV_OK;
}

const char* spv_result_to_string(spv_result result) {
  switch (result) {
    case SPV_OK: return "OK";
    case SPV_INVALID: return "Invalid SPIR-V";
    case SPV_TOO_BIG: return "SPIR-V contains too many types/variables (max ID is 65534)";
    default: return NULL;
  }
}

static spv_result spv_parse_capability(spv_context* spv, spv_op* op, spv_info* info) {
  if (op->count != 2) {
    return SPV_INVALID;
  }

  if (info->features) {
    info->features[info->featureCount] = op->words[1];
  }

  info->featureCount++;

  return SPV_OK;
}

static spv_result spv_parse_name(spv_context* spv, spv_op* op, spv_info* info) {
  if (op->count < 3 || op->words[1] > spv->bound) {
    return SPV_INVALID;
  }

  // Track the word index of the name of the type with the given id
  spv->cache[op->words[1]].type.name = &op->words[2] - spv->words;

  return SPV_OK;
}

static spv_result spv_parse_decoration(spv_context* spv, spv_op* op, spv_info* info) {
  uint32_t id = op->words[1];
  uint32_t decoration = op->words[2];

  if (op->count < 3 || id > spv->bound) {
    return SPV_INVALID;
  }

  switch (decoration) {
    case 33: spv->cache[id].resource.binding = op->words[3]; break; // Binding
    case 34: spv->cache[id].resource.set = op->words[3]; break; // Set
    case 30: spv->cache[id].input.location = op->words[3]; break; // Location
    case 1: spv->cache[id].flag.number = op->words[3]; break; // SpecID
    default: break;
  }

  return SPV_OK;
}

static spv_result spv_parse_type(spv_context* spv, spv_op* op, spv_info* info) {
  if (op->count < 2 || op->words[1] > spv->bound) {
    return SPV_INVALID;
  }

  // Track the word index of the declaration of the type with the given ID
  spv->cache[op->words[1]].type.word = op->words - spv->words;

  return SPV_OK;
}

static spv_result spv_parse_spec_constant(spv_context* spv, spv_op* op, spv_info* info) {
  if (op->count < 2 || op->words[1] >= spv->bound) {
    return SPV_INVALID;
  }

  uint32_t id = op->words[2];

  if (spv->cache[id].flag.number == 0xffff) {
    return SPV_INVALID;
  }

  if (info->specConstants) {
    spv_spec_constant* constant = &info->specConstants[info->specConstantCount];
    constant->id = spv->cache[id].flag.number;

    if (spv->cache[id].flag.name != 0xffff) {
      constant->name = (char*) (spv->words + spv->cache[id].flag.name);
    }

    // TODO parse type
  }

  info->specConstantCount++;

  // Tricky thing: the cache currently contains {id,name} for the constant.  It gets replaced
  // with the index of this word as a u32 so that array types using the specialization constants for
  // their lengths can find this word to get the array length.
  spv->cache[id].constant.word = op->words - spv->words;

  return SPV_OK;
}

static spv_result spv_parse_constant(spv_context* spv, spv_op* op, spv_info* info) {
  if (op->count < 3 || op->words[2] > spv->bound) {
    return SPV_INVALID;
  }

  // Track the index of the word declaring the constant with this ID, so arrays can find it later
  spv->cache[op->words[2]].constant.word = op->words - spv->words;

  return SPV_OK;
}

static spv_result spv_parse_variable(spv_context* spv, spv_op* op, spv_info* info) {
  if (op->count < 4 || op->words[2] >= spv->bound) {
    return SPV_INVALID;
  }

  uint32_t type = op->words[1];
  uint32_t id = op->words[2];
  uint32_t storageClass = op->words[3];

  if (spv->cache[id].input.location < 32 && storageClass == 1) { // Input
    info->inputLocationMask |= (1 << spv->cache[id].input.location);
    return SPV_OK;
  }

  uint32_t set = spv->cache[id].resource.set;
  uint32_t binding = spv->cache[id].resource.binding;

  // Ignore output variables (storageClass 3) and anything without a set/binding decoration
  if (storageClass == 3 || set == 0xff || binding == 0xff) {
    return SPV_OK;
  }

  if (info->resources) {
    spv_resource* resource = &info->resources[info->resourceCount];
    resource->set = set;
    resource->binding = binding;
    // TODO get type
    // TODO name (different for buffer vs. texture/sampler)
  }

  info->resourceCount++;

  return SPV_OK;
}
