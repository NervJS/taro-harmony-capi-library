/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
