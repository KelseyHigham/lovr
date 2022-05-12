#include "spv.h"
#include <string.h>

// Notes about this spirv parser:
// - It sucks!!!  Don't use it

typedef union {
  struct {
    uint16_t location;
    uint16_t name;
  } attribute;
  struct {
    uint8_t group;
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

static void spv_parse_feature(const uint32_t* words, uint32_t wordCount, const uint32_t* instruction, spv_info* info);

spv_result spv_parse(const void* source, uint32_t size, spv_stage stage, spv_info* info) {
  const uint32_t* words = source;
  uint32_t wordCount = size / sizeof(uint32_t);

  // Ensure the shader is of a reasonable size and says the magic word
  if (wordCount < 16 || words[0] != 0x07230203) {
    return SPV_INVALID;
  }

  // We use 16 bits for ids sometimes
  uint32_t bound = words[3];
  if (bound >= 0xffff) {
    return SPV_TOO_BIG;
  }

  spv_cache cache[65536];
  memset(cache, 0xff, sizeof(cache));

  const uint32_t* instruction = words + 5;

  info->featureCount = 0;

  while (instruction < words + wordCount) {
    uint16_t op = instruction[0] & 0xffff;
    uint16_t length = instruction[0] >> 16;

    if (length == 0 || instruction + length > words + wordCount) {
      return SPV_INVALID;
    }

    switch (op) {
      case /*OpCapability*/ 17: spv_parse_feature(words, wordCount, instruction, info); break;
    }

    instruction += length;
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

static void spv_parse_feature(const uint32_t* words, uint32_t wordCount, const uint32_t* instruction, spv_info* info) {
  if (info->features) info->features[info->featureCount] = instruction[1];
  info->featureCount++;
}
