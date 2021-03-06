//
// Created by Ion Agorria on 8/04/18
//
#include "engine/core/utils.h"
#include "engine/io/log.h"
#include "asset_palette.h"
#include "asset_image.h"
#include "engine/graphics/renderer.h"
#include "engine/core/engine.h"
#include "asset_manager.h"
#include "asset_processor.h"

//Rect pack
#define STB_RECT_PACK_IMPLEMENTATION
//#define STBRP_LARGE_RECTS
#include "stb_rect_pack.h"

AssetManager::AssetManager(std::shared_ptr<Engine> engine): engine(std::move(engine)) {
    log = Log::get("Assets");
}

AssetManager::~AssetManager() {
    log->debug("Closing");
    clearAssets();
    processors.clear();
}

void AssetManager::addAssetProcessor(std::unique_ptr<IAssetProcessor> processor) {
    processor->setManager(this);
    processors.push_back(std::move(processor));
}

const std::unordered_map<asset_path_t, std::unique_ptr<Asset>>& AssetManager::getAssets() const {
    return assets;
}

bool AssetManager::addAsset(std::unique_ptr<Asset> asset) {
    if (!asset) {
        error = "Asset to add is null";
        return false;
    }
    if (asset->hasError()) {
        error = "Asset to add has error set:\n" + asset->getError();
        return false;
    }
    const std::string& path = asset->getPath();
    if (assets[path]) {
        error = "Asset already present: '" + path + "'";
        return false;
    }
    assets[path] = std::move(asset);
    assetsCount++;
    return true;
}

bool AssetManager::removeAsset(const asset_path_t& path) {
    if (assets.erase(path) == 0) {
        error = "Asset is not present: '" + path + "'";
        return false;
    }
    assetsCount--;
    return true;
}

std::shared_ptr<Image> AssetManager::getImage(const asset_path_t& path) const {
    std::shared_ptr<Image> image;
    AssetImage* assetImage = getAsset<AssetImage>(path);
    if (assetImage) {
        image = assetImage->getImage();
    }
    return image;
}

int AssetManager::getAssetsCount() {
    return assetsCount;
}

void AssetManager::clearAssets() {
    //Delete all stored assets
    if (assetsCount) {
        log->debug("Deleting assets");
    }
    assets.clear();
    assetsCount = 0;
}

void AssetManager::registerAssetContainer(const std::string& containerName, bool required) {
    assetContainers[containerName] = required;
}

void AssetManager::loadAssets() {
    //Clear any old assets
    clearAssets();

    //Load roots
    std::vector<std::string> roots;
    Utils::getRootPaths(std::string(GAME_ASSETS_DIR) + DIR_SEP, roots);

    //Load each registered containers
    for (auto& pair : assetContainers) {
        loadAssetContainer(roots, pair.first, pair.second);
        if (hasError()) return;
    }

    //Process the assets
    processIntermediates();
    if (hasError()) return;

    //Refresh the assets
    refreshAssets();
    if (hasError()) return;

    //Print loaded assets
    log->debug("Loaded {0} assets", getAssetsCount());
    //for (std::pair<asset_path, std::shared_ptr<Asset>> pair : assetManager->getAssets()) log->debug(pair.first);
}

void AssetManager::loadAssetContainer(const std::vector<std::string>& assetRoots, const std::string& containerName, bool required) {
    log->debug("Loading from '{0}'", containerName);
    //Scan assets from containers by checking different paths that might contain assets
    bool found = false;
    for (const std::string& path : assetRoots) {
        if (found) break;
        for (std::unique_ptr<IAssetProcessor>& processor : processors) {
            found |= processor->scanContainer(path, containerName);
            if (found) break;
            if (processor->hasError()) {
                error += processor->getError() + "\n";
            }
        }
    }

    if (!found) {
        if (required) {
            //Required is missing
            std::string text = "Error loading game data for directory/file '" + containerName + "'\n";
            if (error.empty()) {
                text += "Check if game files are correctly set and are accessible inside following paths: \n";
                for (const std::string& path : assetRoots) {
                    text += path + "\n";
                }
            } else {
                text += error;
            }
            error = text;
            return;
        } else {
            //Warn it but since its not required dismiss it
            log->warn("Optional game data '" + containerName + "' is not available:\n" + error);
        }
    }
    //Dismiss any error
    error = "";
}

void AssetManager::processIntermediates() {
    //Process intermediate assets to extract assets inside assets
    for (std::unique_ptr<IAssetProcessor>& processor : processors) {
        processor->processIntermediates();
        error = processor->getError();
        if (!error.empty()) return;
    }
}

void AssetManager::refreshAssets() {
    unsigned int textureSize = MINIMUM_TEXTURE_SIZE;
    if (!Utils::isFlag(FLAG_HEADLESS)) {
        Renderer* renderer = engine->getRenderer();
        if (!renderer) {
            error = "Renderer is not available";
            return;
        }
        textureSize = renderer->getMaxTextureSize();
    }
    unsigned int batchSize = (textureSize * textureSize) / (64 * 64);
    log->debug("Using texture size {0} batch size {1}", textureSize, batchSize);

    //Iterate all assets and handle by asset type
    std::vector<AssetImage*> assetImages;
    std::vector<AssetImage*> assetImagesWithPalettes;
    for (auto& asset : assets) {
        //Handle image assets
        AssetImage* assetImage = dynamic_cast<AssetImage*>(asset.second.get());
        if (assetImage) {
            assetImage->assignImage(nullptr);
            std::shared_ptr<AssetPalette> assetPalette = assetImage->getAssetPalette();
            if (assetPalette) {
                assetPalette->assignPalette(nullptr);
                assetImagesWithPalettes.push_back(assetImage);
            } else {
                assetImages.push_back(assetImage);
            }
            continue;
        }
    }

    //Process the images without palettes
    log->debug("Processing {0} images", assetImages.size());
    processImages(textureSize, batchSize, assetImages, false);
    if (!error.empty()) return;

    //Palettes must be refreshed before images
    for (AssetImage* assetImage : assetImagesWithPalettes) {
        //Create a new palette
        std::shared_ptr<Palette> palette = std::make_shared<Palette>(ASSET_PALETTE_COUNT, false);
        error = palette->getError();
        if (!error.empty()) return;

        //Set or replace with the new palette
        std::shared_ptr<AssetPalette> assetPalette = assetImage->getAssetPalette();
        if (!assetPalette->assignPalette(palette)) {
            error = assetPalette->getError();
            return;
        }
    }

    //Process the images with palettes
    log->debug("Processing {0} palette images", assetImages.size());
    processImages(textureSize, batchSize, assetImagesWithPalettes, true);
    if (!error.empty()) return;

    //Refresh the images with processors
    for (std::unique_ptr<IAssetProcessor>& processor : processors) {
        processor->refreshAssets();
        error = processor->getError();
        if (!error.empty()) return;
    }
}

void AssetManager::processImages(
        const unsigned int textureSize, unsigned int batchSize,
        std::vector<AssetImage*>& assetImages, bool withPalette
    ) {
    //Init structures
    stbrp_context context;
    std::vector<stbrp_node> nodes(textureSize * 2);
    std::vector<stbrp_rect> rects(batchSize);

    int retryCount = 0;
    size_t totalCount = assetImages.size();
    size_t lastSize = 0;
    size_t atlasIndex = 0;
    //This is used to detect stalled packing (no advances in successive attempts)
    while (!assetImages.empty() && lastSize != assetImages.size()) {
        lastSize = assetImages.size();

        //Create the base image
        std::shared_ptr<Image> atlasImage = std::make_shared<Image>(Vector2(static_cast<int>(textureSize)), withPalette);
        error = atlasImage->getError();
        if (!error.empty()) return;

        //Setup the rects and add the index so we know which image does reference
        unsigned int imageCount = (unsigned int) std::min((size_t) batchSize, lastSize);
        for (unsigned int i = 0; i < imageCount; ++i) {
            unsigned int index = imageCount - 1 - i;
            stbrp_rect& rect = rects.at(index);
            AssetImage* assetImage = assetImages.at(index);
            Vector2 imageSize = assetImage->getImageSize();
            if (0 > imageSize.x || 0 > imageSize.y) {
                error = "This asset image size is negative " + assetImage->getPath() + " " + imageSize.toString();
                return;
            }
            //Expand the image size
            imageSize += (EXTRA_TEXTURE_SIZE * 2);
            if (textureSize < (unsigned) imageSize.x || textureSize < (unsigned) imageSize.y) {
                error = "This asset image size exceeds the maximum texture size allowed " + assetImage->getPath() + " " + imageSize.toString();
                return;
            }
            rect.id = static_cast<int>(index);
            rect.w = imageSize.x;
            rect.h = imageSize.y;
            rect.x = 0;
            rect.y = 0;
            rect.was_packed = 0;
        }

        //Prepare the rect packer and do it
        stbrp_init_target(
                &context,
                static_cast<int>(textureSize),
                static_cast<int>(textureSize),
                nodes.data(),
                static_cast<int>(textureSize)
        );
        stbrp_pack_rects(&context, rects.data(), static_cast<int>(imageCount));

        //Get the rects that were packed and use them for actual image setup
        for (unsigned int i = 0; i < imageCount; ++i) {
            unsigned int index = imageCount - 1 - i;
            //Get rectangle
            stbrp_rect& rect = rects.at(index);
            if ((unsigned) rect.id != index) {
                //This shouldn't happen unless rects become reordered
                error = "Rect id doesn't match index";
                return;
            }
            if (rect.was_packed == 0) {
                //Okay, to next round
                //log->debug("Retry {0}", index);
                retryCount++;
                continue;
            }
            //Since extra space was used we need to remove the extra space on each side
            Rectangle rectangle(
                    rect.x + EXTRA_TEXTURE_SIZE,
                    rect.y + EXTRA_TEXTURE_SIZE,
                    rect.w - EXTRA_TEXTURE_SIZE * 2,
                    rect.h - EXTRA_TEXTURE_SIZE * 2
            );

            //Fetch and remove asset image from queue
            std::vector<AssetImage*>::iterator it = assetImages.begin() + index;
            AssetImage* assetImage = *it;
            assetImages.erase(it);

            //Create a subset image which inherits the atlas image using the rectangle
            std::shared_ptr<Image> subImage = std::make_shared<Image>(rectangle, withPalette, atlasImage);
            error = subImage->getError();
            if (!error.empty()) return;

            //Assign palette
            if (withPalette) {
                std::shared_ptr<AssetPalette> assetPalette = assetImage->getAssetPalette();
                subImage->setPalette(assetPalette->getPalette());
            }

            //Assign the subset image to the asset, this loads the asset data to image
            //log->debug("{0}:{1} {2} {3}", atlasIndex, i, rectangle.toString(), assetImage->getPath());
            bool result = assetImage->assignImage(subImage);
            if (!result) {
                error = assetImage->getError();
                if (error.empty()) error = "Error assigning image to asset";
                error += "\nAsset: " + assetImage->getPath();
                return;
            }
        }

        log->debug("Atlas {0} contains {1} images", atlasIndex, lastSize - assetImages.size());
        atlasIndex++;
    }

    //Done
    log->debug("Packing atlas count {0} image count {1} retry count {2}", atlasIndex, totalCount, retryCount);
    if (!assetImages.empty()) {
        error = "Packing failed for " + std::to_string(assetImages.size()) + " assets";
        return;
    }
}
