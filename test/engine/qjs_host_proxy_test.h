//
// Created on 2024/5/31.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#pragma once

#include <gtest/gtest.h>
int add(int a, int b) {
    return a + b;
}

TEST(add, zero) {
    EXPECT_EQ(0, add(0, 0));
}

TEST(add, positive_number) {
    EXPECT_EQ(3, add(1, 2));
}

TEST(add, negative_number) {
    EXPECT_EQ(-3, add(-1, -2));
}
// #include "qjs_host_proxy.h"
// #include "jsi_qjs_value_converter.h"

// class TestHostObject : public facebook::jsi::HostObject {
// public:
//   facebook::jsi::Value get(facebook::jsi::Runtime& rt, const facebook::jsi::PropNameID& name) override {
//     if (name.utf8(rt) == "x") {
//       return 42;
//     }
//     return facebook::jsi::Value::undefined();
//   }
//
//   void set(facebook::jsi::Runtime& rt, const facebook::jsi::PropNameID& name, const facebook::jsi::Value& value) override {
//     if (name.utf8(rt) == "x") {
//       x = value.asNumber();
//     }
//   }
// private:
//   double x = 0;
// };
//
// TEST(HostObjectProxyTest, TestGet) {
//   QuickJSRuntime runtime;
//   JSContext* ctx = (JSContext *)runtime.getQjsCtx();
//   HostObjectProxy::initHostObjectClass(JS_GetRuntime(ctx));
//
//   auto hostObject = std::make_shared<TestHostObject>();
//   HostObjectProxy proxy(runtime, ctx, hostObject);
//   proxy.BindFinalizer();
//
//   JSValue obj = proxy.weakHandle_;
//   ASSERT_TRUE(JS_IsObject(obj));
//
//   JSAtom atom = JS_NewAtom(ctx, "x");
//   JSValue ret = HostObjectProxy::Getter(ctx, obj, atom);
//
//   EXPECT_TRUE(JS_IsNumber(ret));
//   EXPECT_EQ(JS_VALUE_GET_FLOAT64(ret), 42);
//
//   JS_FreeAtom(ctx, atom);
//   JS_FreeValue(ctx, ret);
// }
//
// TEST(HostObjectProxyTest, TestSet) {
//   QuickJSRuntime runtime;
//   JSContext* ctx = (JSContext *)runtime.getQjsCtx();
//   HostObjectProxy::initHostObjectClass(JS_GetRuntime(ctx));
//
//   auto hostObject = std::make_shared<TestHostObject>();
//   HostObjectProxy proxy(runtime, ctx, hostObject);
//   proxy.BindFinalizer();
//
//   JSValue obj = proxy.weakHandle_;
//   ASSERT_TRUE(JS_IsObject(obj));
//
//   JSAtom atom = JS_NewAtom(ctx, "x");
//   JSValue value = JS_NewFloat64(ctx, 3.14);
//
//   EXPECT_EQ(HostObjectProxy::Setter(ctx, obj, atom, value), 0);
//
//   JSValue ret = HostObjectProxy::Getter(ctx, obj, atom);
//   EXPECT_TRUE(JS_IsNumber(ret));
//   EXPECT_EQ(JS_VALUE_GET_FLOAT64(ret), 3.14);
//
//   JS_FreeAtom(ctx, atom);
//   JS_FreeValue(ctx, value);
//   JS_FreeValue(ctx, ret);
// }
//
// TEST(HostObjectProxyTest, TestGetPropertyNames) {
//   QuickJSRuntime runtime;
//   JSContext* ctx = (JSContext *)runtime.getQjsCtx();
//   HostObjectProxy::initHostObjectClass(JS_GetRuntime(ctx));
//
//   auto hostObject = std::make_shared<TestHostObject>();
//   HostObjectProxy proxy(runtime, ctx, hostObject);
//   proxy.BindFinalizer();
//
//   JSValue obj = proxy.weakHandle_;
//   ASSERT_TRUE(JS_IsObject(obj));
//
//   JSValue names = HostObjectProxy::Enumerator(ctx, obj);
//   ASSERT_TRUE(JS_IsArray(ctx, names));
//
//   uint32_t len = 0;
//   JS_ToUint32(ctx, &len, JS_GetPropertyStr(ctx, names, "length"));
//   EXPECT_EQ(len, 1);
//
//   JSValue x = JS_GetPropertyUint32(ctx, names, 0);
//   EXPECT_TRUE(JS_IsString(x));
//   const char* xStr = JS_ToCString(ctx, x);
//   EXPECT_STREQ(xStr, "x");
//   JS_FreeCString(ctx, xStr);
//
//   JS_FreeValue(ctx, names);
//   JS_FreeValue(ctx, x);
// }
//
// TEST(HostFunctionProxyTest, TestCall) {
//   QuickJSRuntime runtime;
//   JSContext* ctx = (JSContext *)runtime.getQjsCtx();
//   HostFunctionProxy::initHostFunctionClass(JS_GetRuntime(ctx));
//
//   auto hostFunction = [](facebook::jsi::Runtime& rt,
//                       const facebook::jsi::Value& thisVal,
//                       const facebook::jsi::Value* args,
//                       size_t count) -> facebook::jsi::Value {
//     EXPECT_EQ(thisVal.asNumber(), 100);
//     EXPECT_EQ(count, 3);
//     EXPECT_EQ(args[0].asNumber(), 1);
//     EXPECT_EQ(args[1].asNumber(), 2);
//     EXPECT_EQ(args[2].asNumber(), 3);
//     return args[0].asNumber() + args[1].asNumber() + args[2].asNumber();
//   };
//
//   HostFunctionProxy proxy(runtime, ctx, hostFunction);
//   proxy.BindFinalizer();
//
//   JSValue func = proxy.weakHandle_;
//   ASSERT_TRUE(JS_IsFunction(ctx, func));
//
//   JSValueConst args[3];
//   args[0] = JS_NewInt32(ctx, 1);
//   args[1] = JS_NewInt32(ctx, 2);
//   args[2] = JS_NewInt32(ctx, 3);
//   JSValue thisVal = JS_NewInt32(ctx, 100);
//
//   JSValue ret = JS_Call(ctx, func, thisVal, 3, args);
//   EXPECT_TRUE(JS_IsNumber(ret));
//   EXPECT_EQ(JS_VALUE_GET_INT(ret), 6);
//
//   JS_FreeValue(ctx, thisVal);
//   JS_FreeValue(ctx, ret);
//   for (int i = 0; i < 3; ++i) {
//     JS_FreeValue(ctx, args[i]);
//   }
// }
