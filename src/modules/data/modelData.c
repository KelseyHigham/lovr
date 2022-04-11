#include "data/modelData.h"
#include "data/blob.h"
#include "data/image.h"
#include "util.h"
#include <stdlib.h>

ModelData* lovrModelDataCreate(Blob* source, ModelDataIO* io) {
  ModelData* model = calloc(1, sizeof(ModelData));
  lovrAssert(model, "Out of memory");
  model->ref = 1;

  if (lovrModelDataInitGltf(model, source, io)) {
    return model;
  } else if (lovrModelDataInitObj(model, source, io)) {
    return model;
  } else if (lovrModelDataInitStl(model, source, io)) {
    return model;
  }

  // Computed properties happen here, in one place

  // TODO image srgb
  // TODO attribute pointer/stride
  // TODO total vertices/indices
  // TODO max index stride/type (aka hasU32)

  for (uint32_t i = 0; i < model->nodeCount; i++) {
    model->nodes[i].parent = ~0u;
  }

  for (uint32_t i = 0; i < model->nodeCount; i++) {
    ModelNode* node = &model->nodes[i];
    for (uint32_t j = 0; j < node->childCount; j++) {
      model->nodes[node->children[j]].parent = i;
    }
  }

  for (uint32_t i = 0; i < model->skinCount; i++) {
    ModelSkin* skin = &model->skins[i];
    for (uint32_t j = 0; j < skin->jointCount; j++) {
      model->nodes[skin->joints[j]].skinfluence |= (1 << i);
    }
  }

  lovrThrow("Unable to load model from '%s'", source->name);
  return NULL;
}

void lovrModelDataDestroy(void* ref) {
  ModelData* model = ref;
  for (uint32_t i = 0; i < model->blobCount; i++) {
    lovrRelease(model->blobs[i], lovrBlobDestroy);
  }
  for (uint32_t i = 0; i < model->imageCount; i++) {
    lovrRelease(model->images[i], lovrImageDestroy);
  }
  map_free(&model->animationMap);
  map_free(&model->materialMap);
  map_free(&model->nodeMap);
  free(model->data);
  free(model);
}

// Note: this code is a scary optimization
void lovrModelDataAllocate(ModelData* model) {
  size_t totalSize = 0;
  size_t sizes[13];
  size_t alignment = 8;
  totalSize += sizes[0] = ALIGN(model->blobCount * sizeof(Blob*), alignment);
  totalSize += sizes[1] = ALIGN(model->imageCount * sizeof(Image*), alignment);
  totalSize += sizes[2] = ALIGN(model->bufferCount * sizeof(ModelBuffer), alignment);
  totalSize += sizes[3] = ALIGN(model->attributeCount * sizeof(ModelAttribute), alignment);
  totalSize += sizes[4] = ALIGN(model->primitiveCount * sizeof(ModelPrimitive), alignment);
  totalSize += sizes[5] = ALIGN(model->materialCount * sizeof(ModelMaterial), alignment);
  totalSize += sizes[6] = ALIGN(model->animationCount * sizeof(ModelAnimation), alignment);
  totalSize += sizes[7] = ALIGN(model->skinCount * sizeof(ModelSkin), alignment);
  totalSize += sizes[8] = ALIGN(model->nodeCount * sizeof(ModelNode), alignment);
  totalSize += sizes[9] = ALIGN(model->channelCount * sizeof(ModelAnimationChannel), alignment);
  totalSize += sizes[10] = ALIGN(model->childCount * sizeof(uint32_t), alignment);
  totalSize += sizes[11] = ALIGN(model->jointCount * sizeof(uint32_t), alignment);
  totalSize += sizes[12] = model->charCount * sizeof(char);

  size_t offset = 0;
  char* p = model->data = calloc(1, totalSize);
  lovrAssert(model->data, "Out of memory");
  model->blobs = (Blob**) (p + offset), offset += sizes[0];
  model->images = (Image**) (p + offset), offset += sizes[1];
  model->buffers = (ModelBuffer*) (p + offset), offset += sizes[2];
  model->attributes = (ModelAttribute*) (p + offset), offset += sizes[3];
  model->primitives = (ModelPrimitive*) (p + offset), offset += sizes[4];
  model->materials = (ModelMaterial*) (p + offset), offset += sizes[5];
  model->animations = (ModelAnimation*) (p + offset), offset += sizes[6];
  model->skins = (ModelSkin*) (p + offset), offset += sizes[7];
  model->nodes = (ModelNode*) (p + offset), offset += sizes[8];
  model->channels = (ModelAnimationChannel*) (p + offset), offset += sizes[9];
  model->children = (uint32_t*) (p + offset), offset += sizes[10];
  model->joints = (uint32_t*) (p + offset), offset += sizes[11];
  model->chars = (char*) (p + offset), offset += sizes[12];

  map_init(&model->animationMap, model->animationCount);
  map_init(&model->materialMap, model->materialCount);
  map_init(&model->nodeMap, model->nodeCount);
}
