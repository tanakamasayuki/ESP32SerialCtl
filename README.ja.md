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
- Wi-Fi と Preferences を併用できる環境では、`wifi` / `ntp` コマンドで NVS へ設定を保存し、自動接続や NTP 同期を制御可能

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
- `sys uptime` : 稼働時間を `hh:mm:ss` とミリ秒で表示
- `sys time` : 現在時刻を ISO 8601 形式で表示
- `sys timezone` : 永続化されるタイムゾーン文字列を取得・設定（NTP 同期でも利用）
- `sys mem` : ヒープ/PSRAM の合計・空き・最小・最大ブロックとスタック余裕を表示
- `sys reset` : リクエストを確認後に `ESP.restart()` を実行
- `conf list/get/set/del` : アプリで定義した設定スロットを NVS に保存・取得します。`conf list --lang ja` で多言語説明を指定できます。初期値は `ESP32SerialCtl::setConfigEntries` で登録します。
- `storage list/use/status` : `SD` / `SPIFFS` / `LittleFS` / `FFat` などインクルード済みのストレージを列挙・選択し、容量や使用量を確認
- `fs ls/cat/write/rm/stat/mkdir/mv` : `storage use` で選択したストレージ（または `--storage <name>` 指定）に対して、ファイル一覧・内容表示・テキスト書き込み・削除・情報表示・ディレクトリ作成・移動/リネームを行う
- `fs b64read/b64write` : Base64 でエンコードしたチャンクを送受信し、バイナリデータも転送可能。複数チャンクは `--append` で追記。
- `fs hash` : `sha256`（既定）や `md5` のハッシュとファイルサイズを表示し、転送後の整合性を確認
- `gpio mode/read/write/toggle` : GPIO のモード設定・入出力・トグルを制御
- `adc read` : ADC をサンプリングし、`samples` オプションで平均化
- `pwm set/stop` : LEDC PWM を自動割り当てで開始/停止（`pwm set <pin> <freq> <duty>`、解像度 12bit 固定・duty は 0..4095 または % 指定）
- `rgb pin/set/stream` : `rgbLedWrite` で RGB LED を制御（デフォルト pin 対応）
- `i2c scan/read/write` : `Wire` をインクルードしている場合に自動提供され、複数 I2C コントローラでは `--bus` で `Wire` / `Wire1` などを指定可能
- `wifi auto/list/add/del/connect/disconnect/status` : NVS に保存された Wi-Fi アクセスポイントを管理し、自動接続や現在のリンク状態を制御
- `ntp status/set/enable/disable/auto` : NTP サーバー設定と同期状態、起動時自動実行の切り替えを行う（利用前に `sys timezone` でタイムゾーンを設定してください）
- `help` / `?` : 登録済みコマンドと説明を一覧表示

スケッチから RGB のデフォルト pin を設定する場合は `esp32serialctl::ESP32SerialCtl<>::setDefaultRgbPin(pin);` を呼び出してください。
`RGB_BUILTIN` が定義されているプラットフォームでは、その値が自動的に既定 pin として利用されます。

アプリ固有の設定を CLI に公開する場合は、起動時に `ConfigEntry` 配列を登録します。

```cpp
static constexpr esp32serialctl::ConfigEntry kAppConfig[] = {
    {"api_key", "", {{"ja", "外部API連携用のトークン"}, {"en", "External API key"}}},
};

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl(kAppConfig, "my_app_config");
```

`conf list` はデフォルトで登録済みの多言語説明をすべて表示し、`--lang ja` のように指定すると特定言語のみに絞り込めます。値の取得・更新は名前で行い（`conf get <name> [--lang ja]` / `conf set <name> "value"` / `conf del <name>`）、これらのコマンドでは説明文は出力されません。スケッチ側では `esp32SerialCtl.configGet("api_key")` で実際に利用される文字列を取得します。設定や名前空間を実行時に切り替える必要がある場合は、`ESP32SerialCtl<>::setConfigNamespace(...)` / `ESP32SerialCtl<>::setConfigEntries(...)` を利用してください。
`ConfigEntry` を登録していない場合は `conf` 系コマンドは自動的に無効化され、ヘルプや CLI の一覧にも表示されません。

## 時刻とネットワークのヘルパー

### 時刻関連 (`sys`)
- `sys time [datetime]` : 引数なしで現在のローカル時刻を表示します。`2024-04-05T12:34:56` のような ISO 8601 文字列を渡すと、その場で時刻を設定します。
- `sys timezone [tz]` : TZ 文字列を NVS（名前空間 `serial_ctl`）に保存/取得します。保存した値は起動時および NTP 再設定の前に適用されます。既定値を変更したい場合は `ESP32SERIALCTL_DEFAULT_TIMEZONE` をコンパイル時に定義してください（初期値は空文字）。
- タイムゾーンを更新すると、手動の `sys time` 設定や `ntp` コマンドによる SNTP セッションにも反映されます。

### Wi-Fi コマンド群
- `wifi auto <on/off>` : 初回の `service()` 呼び出し時に自動接続を行うかどうかを NVS に保存します。既定では自動接続が有効で、`wifi_auto` プレファレンスで管理されています。
- `wifi list` : 保存済み AP をスロット番号付きで列挙します。
- `wifi add <ssid> <key>` : 最初に空いたスロットへ資格情報を保存します（キーは `wifi{N}_ssid` / `wifi{N}_key`）。登録上限は `ESP32SERIALCTL_WIFI_MAX_NETWORKS`（デフォルト 8）で、返されるリストインデックスは `wifi del` に利用できます。
- `wifi del <index>` : `wifi list` が示すインデックスを削除します。
- `wifi connect [ssid] [key]` : 保存済みリストを `WiFiMulti` で試行します。任意で一時的な SSID/パスワードを追加可能です。タイムアウトは `ESP32SERIALCTL_WIFI_CONNECT_TIMEOUT_MS`（初期値 10 秒）で、接続成功後に NTP 設定を再適用します。
- `wifi disconnect` / `wifi status` : 現在のリンクを切断するか、接続状態・MAC・IP・RSSI・チャネル・自動接続設定を表示します。

### NTP コマンド群
- `ntp status` : 自動/有効フラグ、現在のタイムゾーン、登録済みサーバー、同期状況を表示します。
- `ntp set <server> [server2] [server3]` : 最大 `ESP32SERIALCTL_NTP_MAX_SERVERS`（標準 3）件のサーバーを保存します。既定サーバーを変更したい場合は `ESP32SERIALCTL_DEFAULT_NTP_SERVER` を上書きしてください（初期値 `pool.ntp.org`）。
- `ntp enable` : タイムゾーンが設定済みであることを確認したうえで SNTP を開始し、初回同期を最大 30 秒待機します。成功時は同期時刻を表示します。
- `ntp disable` : SNTP を停止し、有効フラグをオフにします。
- `ntp auto <on/off>` : Wi-Fi 接続とタイムゾーンが揃った時点で SNTP を自動開始するかどうかを切り替えます。

## サンプル

`examples/BasicCli/BasicCli.ino` は上記の最小構成をそのまま再現しており、
ライブラリを追加した直後から CLI を試せます。

## ライセンス

ESP32SerialCtl は MIT ライセンスで提供されています。詳細は `LICENSE` を参照してください。

[English version](README.md)
