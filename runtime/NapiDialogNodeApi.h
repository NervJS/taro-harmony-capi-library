//
// Created on 2024/6/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include <arkui/native_dialog.h>
namespace TaroRuntime {
class NapiDialogNodeApi {
    public:
    static NapiDialogNodeApi* GetInstance();
    ArkUI_NativeDialogAPI_1* GetAPI();

    private:
    static NapiDialogNodeApi* instance;
    ArkUI_NativeDialogAPI_1* api;
};
} // namespace TaroRuntime
