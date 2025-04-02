/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "common.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {

        void BoundingBox::traver(std::function<void(const std::string&, const Dimension&)> callback) {
            if (top.has_value()) {
                callback("top", top.value());
            }
            if (right.has_value()) {
                callback("right", right.value());
            }
            if (bottom.has_value()) {
                callback("bottom", bottom.value());
            }
            if (left.has_value()) {
                callback("left", left.value());
            }
        }
        bool BoundingBox::operator==(const BoundingBox& other) const {
            return (this->top == other.top && this->left == other.left &&
                    this->right == other.right && this->bottom == other.bottom);
        }
        bool BoundingBox::operator!=(const BoundingBox& other) const {
            return !(*this == other);
        }

        TaroHelper::Optional<Dimension>& BoundingBox::operator[](
            const std::string& key) {
            if (key == "left") {
                return left;
            } else if (key == "top") {
                return top;
            } else if (key == "right") {
                return right;
            } else if (key == "bottom") {
                return bottom;
            }
        }

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime
