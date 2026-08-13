#!/usr/bin/env node

import { execFileSync } from 'node:child_process'
import { mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs'
import { tmpdir } from 'node:os'
import { basename, dirname, join, resolve } from 'node:path'

const [, , inputArg, outputArg, version = '4.2.0-wb.7'] = process.argv

if (!inputArg || !outputArg) {
  console.error('Usage: node scripts/patch-selector-query-har.mjs <input.har> <output.har> [version]')
  process.exit(1)
}

const input = resolve(inputArg)
const output = resolve(outputArg)
const stage = mkdtempSync(join(tmpdir(), 'taro-selector-query-har-'))
const sharedFiles = [
  'src/main/ets/npm/@tarojs/shared/native-apis.js',
  'src/main/ets/npm/@tarojs/shared/shared.esm.js',
  'src/main/ets/npm/@tarojs/shared/index.cjs.js',
]
const taroFile = 'src/main/ets/npm/@tarojs/taro/index.js'

const patchDelayRef = (source, relativePath) => {
  if (/createSelectorQuery'\) \{\s*(?:0\s*;)?\s*[A-Za-z_$][\w$]*\(\.\.\.[A-Za-z_$][\w$]*\);return;/.test(source)) {
    return source
  }

  const pattern = /(function delayRef\([^}]+?return function \([^}]+?const [^;]+;const [^;]+;[^=]+?= function \([^}]+?\{)([A-Za-z_$][\w$]*)\.nextTick\(\(\) => ([A-Za-z_$][\w$]*)\(\.\.\.([A-Za-z_$][\w$]*)\)\);/s
  const match = source.match(pattern)
  if (!match) {
    throw new Error(`Unable to locate delayRef in ${relativePath}`)
  }

  const [, prefix, taroVar, rawVar, argsVar] = match
  const nameMatch = prefix.match(/function delayRef\([^,]+,[^,]+,\s*([A-Za-z_$][\w$]*)\s*,/)
  if (!nameMatch) {
    throw new Error(`Unable to identify delayRef name argument in ${relativePath}`)
  }

  const replacement = `${prefix}if (${nameMatch[1]} === 'createSelectorQuery') {${rawVar}(...${argsVar});return;}${taroVar}.nextTick(() => ${rawVar}(...${argsVar}));`
  return source.replace(match[0], replacement)
}

const patchSelectorQueryBatch = (source, relativePath) => {
  let patched = source

  if (!patched.includes('callback(Current.nativeModule.querySelectDOMSync(')) {
    const pattern = /Current\.nativeModule\.querySelectDOM\(([^,()]+),\s*([^,()]+),\s*([^,()]+),\s*\(([A-Za-z_$][\w$]*)\)\s*=>\s*\{/g
    const matches = [...patched.matchAll(pattern)]
    if (matches.length !== 1) {
      throw new Error(`Expected one SelectorQuery batch call in ${relativePath}, found ${matches.length}`)
    }

    patched = patched.replace(pattern, (_, node, selector, selectAll, result) => {
      return `((callback) => callback(Current.nativeModule.querySelectDOMSync(${node}, ${selector}, ${selectAll})))((` +
        `${result}) => {`
    })
  }

  if (!patched.includes('item.single ? null : []')) {
    const guardPattern = /function queryBat\(([A-Za-z_$][\w$]*),\s*([A-Za-z_$][\w$]*)\)\s*\{(.{0,1000}?)if\s*\(!([A-Za-z_$][\w$]*)\)\s*return null;/s
    const guardMatch = patched.match(guardPattern)
    if (!guardMatch) {
      throw new Error(`Unable to locate SelectorQuery root guard in ${relativePath}`)
    }
    const [guardSource, queue, callback, prefix, element] = guardMatch
    const replacement = `function queryBat(${queue}, ${callback}) {${prefix}if (!${element}) {${callback}(${queue}.map(item => item.single ? null : []));return null;}`
    patched = patched.replace(guardSource, replacement)
  }

  return patched
}

try {
  execFileSync('tar', ['-xzf', input, '-C', stage], { stdio: 'inherit' })
  const packageRoot = join(stage, 'package')

  for (const relativePath of sharedFiles) {
    const file = join(packageRoot, relativePath)
    const source = readFileSync(file, 'utf8')
    const patched = patchDelayRef(source, relativePath)
    if (!patched.includes("=== 'createSelectorQuery'")) {
      throw new Error(`SelectorQuery direct-exec verification failed for ${relativePath}`)
    }
    writeFileSync(file, patched)
  }

  const taroPath = join(packageRoot, taroFile)
  const taroSource = readFileSync(taroPath, 'utf8')
  const patchedTaro = patchSelectorQueryBatch(taroSource, taroFile)
  if (!patchedTaro.includes('callback(Current.nativeModule.querySelectDOMSync(') ||
      !patchedTaro.includes('item.single ? null : []')) {
    throw new Error(`SelectorQuery sync-batch verification failed for ${taroFile}`)
  }
  writeFileSync(taroPath, patchedTaro)

  const manifestPath = join(packageRoot, 'oh-package.json5')
  const manifest = readFileSync(manifestPath, 'utf8')
  const originalVersion = manifest.match(/"version"\s*:\s*"([^"]+)/)?.[1]
  const patchedManifest = manifest.replace(/("version"\s*:\s*")[^"]+/, `$1${version}`)
  if (patchedManifest === manifest && !manifest.includes(`"version": "${version}"`)) {
    throw new Error('Unable to update HAR version')
  }
  writeFileSync(manifestPath, patchedManifest)

  // Keep the native diagnostic version aligned when only the generated HAR is repacked.
  if (originalVersion && originalVersion.length === version.length) {
    const nativePath = join(packageRoot, 'libs/arm64-v8a/libTaroHarmonyLibrary.so')
    const nativeBinary = readFileSync(nativePath)
    const from = Buffer.from(originalVersion)
    const to = Buffer.from(version)
    let offset = 0
    let replacements = 0
    while ((offset = nativeBinary.indexOf(from, offset)) !== -1) {
      to.copy(nativeBinary, offset)
      offset += to.length
      replacements += 1
    }
    if (replacements > 0) {
      writeFileSync(nativePath, nativeBinary)
    }
  }

  execFileSync('mkdir', ['-p', dirname(output)])
  execFileSync('tar', ['-czf', output, '-C', stage, 'package'], { stdio: 'inherit' })
  console.info(`Patched HAR: ${basename(output)} (${version})`)
} finally {
  rmSync(stage, { recursive: true, force: true })
}
