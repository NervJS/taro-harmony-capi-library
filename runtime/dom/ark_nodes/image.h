//
// Created on 2024/6/9.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#pragma once

#include <arkui/drawable_descriptor.h>

#include "arkui_node.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroImageNode : public TaroRenderNode {
        public:
        // 图片原图的宽高
        float image_raw_width = NAN;
        float image_raw_height = NAN;
        std::string mode_;

        public:
        TaroImageNode(const TaroElementRef element);

        ~TaroImageNode() override;

        void Build() override;

        void BuildSpanImage();

        void Layout() override;
    
        void SetStyle(StylesheetRef style_ref) override;
        /**
         * 在一些宽度或者高度自适应的mode，需要通过图片信息算出没有显示设置的尺寸
         * 这个方法的触发时机有，mode设置的时候，图片宽高设置的时候（element/image），computed_style(设置的时候)
         * 方法逻辑:
         *
         * - 没有图片宽高时 return （image_raw_width, image_raw_height）
         * - 在display：none的时候return
         * - mode 为 widthFill 同时设了宽没设高 -> 算出高
         * - mode 为 heightFill 同时设了高没设宽 -> 算出宽
         * - mode 为 scaleToFill 宽和高没设的设成图片的宽或高
         */
        void repairSizeIfNeed();

        void setDraggable(bool enabled);

        void setImageSrc(std::string src, const double height = 0.0, const double width = 0.0, bool lazyLoad = false);
        void setImageSrc(ArkUI_DrawableDescriptor *drawableDescriptor);
        void setImageFillColor(uint32_t color);
        void setAlt(const std::string &uri);
        void setErrorHolder(const std::string &uri);
        void setMode(const std::string &mode);

        void bindImageComplete();
    
        private:
        std::string current_src_;
        double height_;
        double width_;
        bool lazyLoad_;
    };

} // namespace TaroDOM
} // namespace TaroRuntime
