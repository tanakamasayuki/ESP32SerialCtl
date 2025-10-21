# Changelog

## Unreleased
- (EN) Added conditional I2C (`scan/read/write`) CLI commands with multi-bus detection and `--bus` selector when `Wire` is available.
- (JA) `Wire` 利用時に I2C（`scan/read/write`）CLI コマンドを条件付きで追加し、複数バスの検出と `--bus` 指定に対応。
- (EN) Default RGB LED pin now follows `RGB_BUILTIN` when the platform defines it.
- (JA) プラットフォームで `RGB_BUILTIN` が定義されている場合、その値を RGB LED のデフォルトピンとして使用。
- (EN) Added GPIO (`mode/read/write/toggle`), ADC, PWM, and RGB command handlers with consistent OK/body output format.
- (JA) GPIO（`mode/read/write/toggle`）、ADC、PWM、RGB コマンドを追加し、OK 先行の出力形式に統一。
- (EN) Introduced `sys time` (ISO 8601) and enhanced `sys info`/`sys mem` reporting.
- (JA) `sys time`（ISO 8601）を追加し、`sys info` と `sys mem` の情報を拡張。
- (EN) Locked PWM to 12-bit resolution and improved duty scaling.
- (JA) PWM を 12 ビット固定とし、duty のスケーリングを改善。
- (EN) Added automatic GPIO output-mode adjustments and richer RGB streaming options.
- (JA) GPIO 出力時に必要なモードへ自動切り替えを行い、RGB ストリーミングオプションを強化。
