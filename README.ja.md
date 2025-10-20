# ESP32SerialCtl

[English](README.md) | 日本語

ESP32SerialCtl は Arduino 互換 ESP32 プロジェクト向けのヘッダーオンリーな
シリアル CLI ライブラリです。人間と機械の双方が扱いやすい、予測可能で軽量な
テキストプロトコルを目指しています。ライブラリ本体は `src/ESP32SerialCtl.h`
に配置されており、Arduino ライブラリフォルダへコピーするだけで利用できます。

## 特長
- ヘッダーオンリー。`#include <ESP32SerialCtl.h>` だけで利用開始
- 可変長バッファをテンプレートで指定でき、最小限の RAM 消費
- `group action [args]` 形式のコマンドをシリアルでパース
- `;` による複数コマンド連結、`#` によるコメント除去に対応
- `\"` / `\n` / `\t` / `\\` など C 互換エスケープを備えた引用符付き文字列
- `--name value` / `--name=value` 形式のオプション解析
- 真偽値（`on/off/true/false/1/0`）や数値＋単位（`ms`, `s`, `Hz`, `kHz`, `%`）
  のパーサを標準提供
- `OK` / `ERR` / ` - ` / `| ` など規約に沿ったレスポンス出力ヘルパー

## はじめよう

最も簡単な使い方は、組み込みのコマンドセットを提供する
`esp32serialctl::ESP32SerialCtl` を利用する方法です。グローバルインスタンスを
用意し、`setup()` でシリアルを初期化、`loop()` 内で入力処理を行います。

```cpp
#include <Arduino.h>
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

void setup() {
  Serial.begin(115200);
}

void loop() {
  esp32SerialCtl.service();
}
```

デフォルト構成には `sys` グループ（`info`, `uptime`, `mem`, `reset`）と
`help` / `?` コマンドが含まれているため、ユーザーコードを追加しなくても
便利な情報を取得できます。

プロンプトは最初の `service()` 実行時と、各レスポンス（`OK` / `ERR`）の後に自動表示されます。

## カスタムコマンド

独自機能を追加したい場合は、`SerialCtl` を直接インスタンス化し、
コマンドテーブルにハンドラを登録します。

```cpp
#include <ESP32SerialCtl.h>

using CLI = esp32serialctl::SerialCtl<192, 16>;

static void handlePing(CLI::Context &ctx) {
  ctx.printBody("pong");
  ctx.printOK("ping");
}

static const CLI::Command COMMANDS[] = {
    {nullptr, "ping", handlePing, "Reply with pong"},
};

static CLI cli(Serial, COMMANDS, sizeof(COMMANDS) / sizeof(COMMANDS[0]));

void setup() {
  Serial.begin(115200);
}

void loop() {
  cli.service();
}
```

### 引数のパース
- `ctx.arg(index).toBool(value)` で `on` や `false` といった真偽値を取得
- `ctx.arg(index).toNumber(parsed)` で 10 進 / 16 進数値や単位付き数値を取得
- `ctx.findOption("name")` でオプションを検索し、`toBool` や `toNumber` などを利用可能

### レスポンス出力
- `ctx.printOK("message")` -> `OK message`
- `ctx.printError(code, "reason")` -> `ERR code reason`
- `ctx.printList("text")` -> ` - text`
- `ctx.printBody("text")` -> `| text`

### パーサが出力する主なエラーコード
- `ERR 404 Unknown command`
- `ERR 431 Too many options`
- `ERR 413 Too many tokens`
- `ERR 500 No handler`
- `ERR 508 Missing closing quote`
- `ERR 510 Line too long`

これらはハンドラ実行前に自動的に返されます。

## ビルトインコマンド
- `sys info` : チップモデル、リビジョン、CPU クロック、フラッシュサイズ、SDK を表示
- `sys uptime` : 稼働時間を `hh:mm:ss`（およびミリ秒）で表示
- `sys mem` : ヒープ / PSRAM の残量と最大割当可能サイズを表示
- `sys reset` : メッセージ送信後に `ESP.restart()` を実行
- `help` / `?` : 登録済みコマンドと説明を一覧表示

## サンプル

`examples/BasicCli/BasicCli.ino` は上記の最小構成をそのまま再現しており、
ライブラリを追加した直後から CLI を試せます。

## ライセンス

ESP32SerialCtl は MIT ライセンスで提供されています。詳細は `LICENSE` を参照してください。

[English version](README.md)
