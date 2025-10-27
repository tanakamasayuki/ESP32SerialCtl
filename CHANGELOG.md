# Changelog

## Unreleased
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
