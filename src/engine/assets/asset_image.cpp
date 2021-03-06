//
// Created by Ion Agorria on 21/04/18
//
#include "engine/core/utils.h"
#include "engine/graphics/image.h"
#include "engine/graphics/color.h"
#include "asset_palette.h"
#include "asset_image.h"

AssetImage::AssetImage(const asset_path_t& path, const std::shared_ptr<File> file, long fileOffset, long fileSize,
                       const Vector2& size, const std::shared_ptr<AssetPalette> assetPalette) :
        Asset(path, file, fileOffset, fileSize), imageSize(size), assetPalette(assetPalette) {
}

AssetImage::AssetImage(const asset_path_t& path, const std::shared_ptr<File> file, long fileOffset, long fileSize,
                       const Vector2& size) :
        AssetImage(path, file, fileOffset, fileSize, size, nullptr) {
}

const Vector2& AssetImage::getImageSize() const {
    return imageSize;
}

bool AssetImage::assignImage(const std::shared_ptr<Image>& assigningImage) {
    if (!error.empty()) return false;
    if (assigningImage) {
        bool result = false;
        size_t imagePixelsCount = static_cast<size_t>(imageSize.x) * imageSize.y;

        //Check if image has palette
        if (!assetPalette != !assigningImage->getPalette()) {
            error = "Provided image or asset image mismatch in palette usage";
            return false;
        }

        //Check if assigning image size matches
        Vector2 assigningImageSize;
        assigningImage->getRectangle().getSize(assigningImageSize);
        if (imageSize != assigningImageSize) {
            error = "Provided image size doesn't match with asset size";
            return false;
        }

        //Seek to initial position
        if (seek(0, true) < 0 || !error.empty()) {
            error = "Error seeking before writing image " + error;
            return false;
        }

        //Handle according to asset image type (paletted or raw)
        if (assetPalette) {
            //Check if pixel count is same as byte count (8 bit indexed)
            if (imagePixelsCount == static_cast<size_t>(size())) {
                //Create buffer, read asset into it and load to image
                std::unique_ptr<byte_array_t> buffer = Utils::createBuffer(imagePixelsCount);
                if (readAll(buffer.get(), imagePixelsCount)) {
                    result = assigningImage->loadFromIndexed8(buffer.get());
                    error = assigningImage->getError();
                }
            } else {
                error = "AssetImage size doesn't match image size (has palette)";
                return false;
            }
        } else {
            //Check size
            if (imagePixelsCount * 2 != static_cast<size_t>(size())) {
                error = "Asset size doesn't match image size";
                return false;
            }

            //Create buffer, read asset into it and load to image
            std::unique_ptr<byte_array_t> buffer = Utils::createBuffer(imagePixelsCount * 2);
            if (readAll(buffer.get(), imagePixelsCount * 2)) {
                result = assigningImage->loadFromRGB565(buffer.get());
                error = assigningImage->getError();
            }
        }

        //Set error if no result
        if (!result && error.empty()) {
            error = "Image was not loaded";
        }
    }

    //Assign the image to this asset
    if (error.empty()) {
        if (assigningImage) {
            this->image = assigningImage;
        } else {
            this->image.reset();
        }
    }

    //Return result
    return error.empty();
}

std::shared_ptr<AssetPalette> AssetImage::getAssetPalette() const {
    return assetPalette;
}

std::shared_ptr<Image> AssetImage::getImage() const {
    return image;
}

std::string AssetImage::toStringContent() const {
    return Asset::toStringContent()
        + " Palette: " + (assetPalette ? assetPalette->toString() : "null")
    ;
}
