//
// Created by Ion Agorria on 21/04/19
//
#ifndef OPENE2140_ASSET_PROCESSOR_LEVEL_H
#define OPENE2140_ASSET_PROCESSOR_LEVEL_H

#include "engine/assets/asset_processor.h"

/**
 * Decodes the level assets
 */
class AssetProcessorLevel: public IAssetProcessor {
private:
    virtual void processIntermediates() override;
};

#endif //OPENE2140_ASSET_PROCESSOR_LEVEL_H
