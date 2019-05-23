//
// Created by Ion Agorria on 21/04/19
//

#include "io/file.h"
#include "assets/asset_manager.h"
#include "assets/asset.h"
#include "assets/asset_palette.h"
#include "assets/asset_image.h"
#include "asset_processor_datpal.h"

void AssetProcessorDatPal::processIntermediates() {
    //Iterate all assets
    for (const std::pair<const asset_path, std::unique_ptr<Asset>>& pair : manager->getAssets()) {
        //Get the "extension" of asset
        asset_path assetPath = pair.first;
        std::string::size_type size = assetPath.size();
        if (4 > size) {
            continue;
        }
        std::string ext = assetPath.substr(size - 4, 4);

        //Handle special extensions
        Asset* asset = pair.second.get();
        if (ext == ".PAL") {
            //Create palette asset and store it
            std::shared_ptr<AssetPalette> assetPalette = std::make_shared<AssetPalette>(
                    assetPath, asset->getFile(), asset->offset(), asset->size()
            );

            //Get the image asset from palette path
            std::string::size_type assetSize = assetPath.size();
            asset_path imagePath = assetPath.substr(0, assetSize - 4) + ".DAT";
            asset = manager->getAsset(imagePath);
            if (!asset) {
                error = assetPath + " doesn't have image counterpart";
                return;
            }

            //Pass the image size struct to fill it
            size_t readSize = sizeof(SSize16) + 2;
            Vector2 imageSize;
            SSize16 imageSizeStruct;
            if (!asset->readAll(imageSizeStruct)) {
                error = "Error reading '" + imagePath + "' image size\n" + asset->getError();
                return;
            }
            imageSize.set(imageSizeStruct.width, imageSizeStruct.height);

            //Skip 2 bytes
            asset->seek(2);
            error = asset->getError();
            if (!error.empty()) {
                error = "Error reading '" + imagePath + "' when seeking " + asset->getError();
                return;
            }

            //Create image asset with the palette indexes data and store it
            std::unique_ptr<AssetImage> assetImage = std::make_unique<AssetImage>(
                    imagePath, asset->getFile(), asset->offset() + readSize, asset->size() - readSize, imageSize, assetPalette
            );
            if (!manager->addAsset(std::move(assetImage))) {
                error = "Couldn't create asset from processed asset\n" + error;
                return;
            }

            //Remove the old assets
            if (!manager->removeAsset(assetPath) || !manager->removeAsset(imagePath)) {
                error = "Couldn't remove processed asset\n" + error;
                return;
            }
        }
    }
}