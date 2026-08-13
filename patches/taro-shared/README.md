# SelectorQuery Harmony C++ hotfix

`selector-query-direct-exec.patch` is the source-level change for the Taro
`packages/shared/src/native-apis.ts` module. `selector-query-sync-batch.patch`
changes Harmony C++ SelectorQuery batching to use the existing synchronous
native query API, preventing `exec` promises from hanging when the asynchronous
native callback is lost. The Taro JavaScript sources live in the separate Taro
monorepo, so this repository keeps both patches next to the C++ runtime changes
used by the WBCar HAR.

After producing a base HAR, apply the equivalent generated-code patch with:

```sh
node scripts/patch-selector-query-har.mjs input.har output.har 4.2.0-wb.7
```

The script patches all three generated `@tarojs/shared` module formats, the
generated `@tarojs/taro` SelectorQuery batch implementation, and the HAR
manifest version. When the old and new version strings have equal length, it
also updates the embedded native diagnostic version. It does not rebuild the
native `.so` library.
