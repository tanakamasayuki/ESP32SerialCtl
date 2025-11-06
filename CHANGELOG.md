# Changelog

## Unreleased
- (EN) Updated the Help Docs references to open the README, Basic CLI example, Arduino docs, and ESP-IDF docs directly, adding localized labels for the Arduino entry.
- (JA) README や Basic CLI の例、Arduino ドキュメント、ESP-IDF ドキュメントへ直接アクセスするリンクチップに更新し、Arduino 項目の翻訳を追加。
- (EN) Refreshed the web console theme so the idle state is monochrome, switching to the blue accent palette only after a successful connection.
- (JA) 未接続状態をモノトーンにし、接続後にだけ青系アクセントへ切り替わるよう Web コンソールのテーマを更新。
- (EN) Added a connection failure dialog with guidance to close other apps that hold the serial port and a retry shortcut.
- (JA) シリアルポートを占有している別アプリを閉じて再試行するよう案内する接続失敗ダイアログとリトライ操作を追加。
- (EN) After connecting, retry the `help` command for 10 seconds, show firmware guidance, and disconnect automatically if no response is received.
- (JA) 接続後に 10 秒間 `help` コマンドを再試行し、応答がなければファームウェア更新の案内を表示して自動的に切断するように変更。
- (EN) Added a localized firmware flashing workflow that auto-detects `docs/firmware.json`, surfaces manifest options, and programs ESP32 targets via esptool-js WebSerial support.
- (JA) `docs/firmware.json` の自動検出や manifest オプション表示に対応したローカライズ済みファームウェア書き込みフローを追加し、esptool-js の WebSerial で ESP32 へ転送できるようにした。
- (EN) Improved the flashing modal with scrollable content, wider log area, and a post-flash button that switches to “Connect Device” and triggers the main connection flow.
- (JA) ファームウェア書き込みモーダルをスクロール対応・ログ領域拡大に調整し、書き込み成功後はボタンを「デバイスに接続」に切り替えてメイン接続フローを自動で起動するよう改良。
- (EN) Added an “Erase Flash” action to the firmware modal that leverages `erase_flash`, reuses the WebSerial transport, and logs localized status updates.
- (JA) ファームウェアモーダルに「フラッシュを消去」ボタンを追加し、`erase_flash` を実行する WebSerial ワークフローとローカライズ済みステータス表示を実装。
- (EN) Added a default-enabled “Erase before flashing” checkbox so the combined workflow wipes flash with `erase_flash` before programming, with localized labels.
- (JA) デフォルトでオンの「書き込み前にフラッシュを消去」チェックボックスを追加し、書き込み前に `erase_flash` を自動実行するように変更。
- (EN) Increased esptool-js flashing/erase timeouts to better tolerate large images and slower transports.
- (JA) 大容量イメージや低速転送に耐えられるよう、esptool-js の書き込み・消去タイムアウトを延長。
- (EN) Abort flashing if the detected chip (e.g., "Detecting chip type... ESP32-S3") does not match the manifest’s expected target, with localized error messages.
- (JA) 接続ログから検出したチップ名が manifest 想定と一致しない場合、ローカライズ済みメッセージで書き込みを中断するように変更。

## 1.0.0
- (EN) Added conditional I2C (`scan/read/write`) CLI commands with multi-bus detection and `--bus` selector when `Wire` is available.
- (JA) `Wire` 利用時に I2C（`scan/read/write`）CLI コマンドを条件付きで追加し、複数バスの検出と `--bus` 指定に対応。
- (EN) Default RGB LED pin now follows `RGB_BUILTIN` when the platform defines it.
- (JA) プラットフォームで `RGB_BUILTIN` が定義されている場合、その値を RGB LED のデフォルトピンとして使用。
- (EN) Added GPIO (`mode/read/write/toggle`), ADC, PWM, and RGB command handlers with consistent OK/body output format.
- (JA) GPIO（`mode/read/write/toggle`）、ADC、PWM、RGB コマンドを追加し、OK 先行の出力形式に統一。
- (EN) Introduced `sys time [datetime]` (ISO 8601) with optional RTC setting and enhanced `sys info`/`sys mem` reporting.
- (JA) `sys time [datetime]`（ISO 8601）を追加し、任意で RTC を設定できるようにしつつ `sys info` と `sys mem` の情報を拡張。
- (EN) Locked PWM to 12-bit resolution and improved duty scaling.
- (JA) PWM を 12 ビット固定とし、duty のスケーリングを改善。
- (EN) Added automatic GPIO output-mode adjustments and richer RGB streaming options.
- (JA) GPIO 出力時に必要なモードへ自動切り替えを行い、RGB ストリーミングオプションを強化。
- (EN) Introduced storage management (`storage list/use/status`) and filesystem tooling (`fs ls/cat/write/rm/stat/mkdir/mv`) with Base64 transfer helpers (`fs b64read/b64write`).
- (JA) ストレージ管理（`storage list/use/status`）とファイルシステム操作（`fs ls/cat/write/rm/stat/mkdir/mv`）、Base64 転送ヘルパー（`fs b64read/b64write`）を追加。
- (EN) `fs hash` now reports the file size alongside `sha256` / `md5` digests.
- (JA) `fs hash` が `sha256` / `md5` のダイジェストに加えてファイルサイズも表示するよう改善。
- (EN) Added `sys timezone [tz]` with persisted NVS storage, startup restore, and configurable default via `ESP32SERIALCTL_DEFAULT_TIMEZONE`.
- (JA) `sys timezone [tz]` コマンドを追加し、NVS への保存・起動時の復元に対応、デフォルトは `ESP32SERIALCTL_DEFAULT_TIMEZONE` で設定可能。
- (EN) Added `wifi` command suite (`auto/list/add/del/connect/disconnect/status`) with NVS-backed credentials and optional auto-connect on boot.
- (JA) `wifi` コマンド群（`auto/list/add/del/connect/disconnect/status`）を追加し、NVS による認証情報管理と起動時の自動接続に対応。
- (EN) Added `ntp` commands (`status/set/enable/disable/auto`) with persistent server list, timezone integration, and web console controls.
- (JA) `ntp` コマンド（`status/set/enable/disable/auto`）を追加し、サーバー設定の保存やタイムゾーン連携、Web コンソールからの操作に対応。
