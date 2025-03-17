//
// Created on 2024/8/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "./background_image_param.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
const BackgroundImageItem BackgroundImageItem::emptyImg = {
    .type = PIC,
    .src = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABAQMAAAAl21bKAAAAA1BMVEUAAACnej3aAAAAAXRSTlMAQObYZgAAAApJREFUCNdjYAAAAAIAAeIhvDMAAAAASUVORK5CYII="};
}