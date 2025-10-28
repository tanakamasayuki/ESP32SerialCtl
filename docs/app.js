document.addEventListener('DOMContentLoaded', () => {
  const LANGUAGE_STORAGE_KEY = 'esp32SerialCtl.language';
  const LANGUAGE_FALLBACK = 'en';
  const SUPPORTED_LANGUAGES = ['ja', 'en', 'zh'];
  const LANGUAGE_ALIASES = {
    ja: 'ja',
    'ja-jp': 'ja',
    en: 'en',
    'en-us': 'en',
    'en-gb': 'en',
    zh: 'zh',
    'zh-cn': 'zh',
    'zh-sg': 'zh',
    'zh-hans': 'zh',
    'zh-hant': 'zh',
    'zh-tw': 'zh',
    'zh-hk': 'zh'
  };

  const DATETIME_LOCALE = {
    ja: 'ja-JP',
    en: 'en-US',
    zh: 'zh-CN'
  };

  const translations = {
    "ja": {
      "meta": {
        "title": "ESP32 WebSerial Control"
      },
      "header": {
        "title": "ESP32 Serial Control",
        "subtitle": "WebSerial を利用して ESP32 の標準コマンドとユーザー定義コマンドを管理する Web コンソールです。"
      },
      "connection": {
        "status": {
          "disconnected": "未接続",
          "connecting": "接続中…",
          "disconnecting": "切断中…",
          "connected": "接続済み",
          "unsupported": "WebSerial 非対応"
        },
        "actions": {
          "connect": "デバイスに接続",
          "disconnect": "切断"
        },
        "info": {
          "disabledTitle": "UI プレビューのため現在は操作できません",
          "placeholderStatus": "準備完了 - 接続待機",
          "unsupportedHint": "対応ブラウザでアクセスしてください",
          "connectFirst": "先にデバイスへ接続してください。",
          "disconnectFirst": "切断してから再度接続できます。",
          "waitDisconnect": "現在切断処理中です。完了するまでお待ちください。"
        }
      },
      "language": {
        "label": "表示言語",
        "ariaLabel": "表示言語を選択",
        "option": {
          "en": "English",
          "ja": "日本語",
          "zh": "中文"
        }
      },
      "nav": {
        "heading": "操作グループ",
        "tabs": {
          "system": {
            "label": "システム",
            "desc": "sys info / uptime / time / timezone / reset"
          },
          "wifi": {
            "label": "Wi-Fi",
            "desc": "wifi status / connect"
          },
          "config": {
            "label": "コンフィグ",
            "desc": "conf list / get / set / del"
          },
          "storage": {
            "label": "ストレージ",
            "desc": "storage list / use / status"
          },
          "filesystem": {
            "label": "ファイル操作",
            "desc": "fs ls / write / hash"
          },
          "peripherals": {
            "label": "ペリフェラル",
            "desc": "gpio / adc / pwm / rgb / i2c"
          },
          "help": {
            "label": "ヘルプ",
            "desc": "help / log"
          }
        }
      },
      "sections": {
        "system": {
          "title": "システムコマンド",
          "description": "ESP32 の動作状態の確認やシステム制御に関する標準コマンドとユーザー定義コマンドをまとめています。",
          "ariaTablist": "システムコマンド"
        },
        "wifi": {
          "title": "Wi-Fi コマンド",
          "description": "Wi-Fi 接続の状態確認や登録済みアクセスポイントの管理、自動接続設定を行います。NTP を利用する場合は先に sys timezone でタイムゾーンを設定してください。",
          "ariaTablist": "Wi-Fiコマンド"
        },
        "config": {
          "title": "アプリ設定",
          "description": "アプリケーションが定義した設定スロットを一覧表示し、値の更新や既定値へのリセットを行います。",
          "actions": {
            "refresh": "最新の設定を取得",
            "save": "更新",
            "reset": "既定に戻す"
          },
          "table": {
            "name": "設定名",
            "value": "値",
            "description": "説明",
            "actions": "操作"
          },
          "labels": {
            "stored": "保存済み",
            "notStored": "未保存",
            "defaultValue": "既定値",
            "empty": "（空）"
          },
          "empty": "設定が見つかりません。取得ボタンを押してください。",
          "ariaTablist": "アプリ設定"
        },
        "storage": {
          "title": "ストレージコマンド",
          "description": "接続されたストレージデバイスの管理と切り替えに利用します。"
        },
        "filesystem": {
          "title": "ファイル操作コマンド",
          "description": "<code>fs</code> 系のコマンドをカテゴリ別に整理しています。"
        },
        "peripherals": {
          "title": "ペリフェラルコマンド",
          "description": "GPIO / ADC / PWM / RGB LED / I2C の制御を行います。"
        },
        "help": {
          "title": "ヘルプとログ",
          "description": "コマンド一覧の参照や応答ログの確認に利用します。"
        }
      },
      "results": {
        "latest": "最新結果 (自動取得)",
        "latestManual": "最新結果",
        "placeholder": "結果がここに表示されます",
        "pending": "実行中…"
      },
      "storage": {
        "list": {
          "title": "ストレージ一覧",
          "description": "タブを開くと自動で <code>storage list</code> を実行します。",
          "selectorLabel": "選択可能なストレージ",
          "selectorTitle": "使用するストレージを選択",
          "selectorEmpty": "マウント済みのストレージがありません",
          "selectorHint": "選択すると <code>storage use</code> を送信し、一覧とステータスを更新します。",
          "table": {
            "name": "名前",
            "type": "種類",
            "state": "状態"
          }
        },
        "status": {
          "title": "ストレージステータス",
          "description": "<code>storage status</code> の最新結果を表示します。"
        }
      },
      "commands": {
        "system": {
          "sys-info": {
            "description": "チップモデル、リビジョン、クロック、SDK/IDF バージョンなどを取得します。",
            "action": "sys info を送信"
          },
          "sys-uptime": {
            "description": "起動からの経過時間を hh:mm:ss とミリ秒で表示します。",
            "action": "sys uptime を送信"
          },
          "sys-time": {
            "description": "現在時刻を ISO 8601 形式で取得・設定します。RTC やタイムゾーンの確認にも利用できます。",
            "field": {
              "label": "日時 (YYYY-MM-DDTHH:MM:SS)",
              "placeholder": "2024-01-01T12:34:56"
            },
            "actions": {
              "get": "現在時刻を取得",
              "set": "指定時刻を設定",
              "useBrowser": "ブラウザ時刻を入力"
            },
            "errors": {
              "required": "日時を入力してください。",
              "invalid": "YYYY-MM-DDTHH:MM:SS 形式で入力してください。"
            }
          },
          "sys-timezone": {
            "description": "永続化されるタイムゾーンを照会・設定します。起動時の RTC 初期化や NTP 同期で利用されます。",
            "field": {
              "label": "タイムゾーン (例: JST-9/CST-8/UTC+0)",
              "placeholder": "JST-9"
            },
            "actions": {
              "get": "タイムゾーンを取得",
              "set": "タイムゾーンを設定",
              "useBrowser": "ブラウザタイムゾーンを入力"
            },
            "errors": {
              "required": "タイムゾーンを入力してください。"
            }
          },
          "sys-mem": {
            "description": "ヒープおよび PSRAM の総量・空き・最小値・最大ブロックを確認します。",
            "action": "sys mem を送信"
          },
          "sys-reset": {
            "description": "ESP32 をソフトリセットします。",
            "action": "sys reset を送信"
          },
          "conf-list": {
            "description": "アプリケーションで定義した設定スロットを一覧表示します。言語を指定しない場合は登録済みの説明をすべて表示し、`--lang` で特定言語に絞り込めます。",
            "actions": {
              "run": "conf list を送信"
            }
          },
          "conf-get": {
            "description": "指定した設定名の値を取得し、保存済みか既定値かを表示します。説明文が必要な場合は `conf list`（必要に応じて `--lang`）を利用してください。",
            "field": {
              "label": "設定名",
              "placeholder": "api_key"
            },
            "actions": {
              "run": "conf get を送信"
            },
            "errors": {
              "required": "設定名を入力してください。"
            }
          },
          "conf-set": {
            "description": "指定した設定名に文字列値を保存します。複数単語は引用符で囲ってください。",
            "fields": {
              "index": {
                "label": "設定名",
                "placeholder": "api_key"
              },
              "value": {
                "label": "保存する値",
                "placeholder": "my-value"
              }
            },
            "actions": {
              "run": "conf set を送信"
            },
            "errors": {
              "indexRequired": "設定名を入力してください。",
              "valueRequired": "値を入力してください。"
            }
          },
          "conf-del": {
            "description": "指定した設定名の保存済み値を削除し、既定値へ戻します。",
            "field": {
              "label": "設定名",
              "placeholder": "api_key"
            },
            "actions": {
              "run": "conf del を送信"
            },
            "errors": {
              "required": "設定名を入力してください。"
            }
          }
        },
        "wifi": {
          "status": {
            "description": "現在の Wi-Fi 接続状態や MAC/IP アドレス、RSSI などを表示します。",
            "actions": {
              "run": "wifi status を送信"
            }
          },
          "list": {
            "description": "NVS に保存されているアクセスポイントの一覧を表示します。",
            "actions": {
              "run": "wifi list を送信"
            }
          },
          "add": {
            "description": "新しいアクセスポイントの SSID とパスワードを NVS に登録します (最大 8 件)。",
            "fields": {
              "ssid": {
                "label": "SSID",
                "placeholder": "MyWiFi"
              },
              "key": {
                "label": "パスワード",
                "placeholder": "(空欄可)"
              }
            },
            "actions": {
              "run": "wifi add を送信"
            },
            "errors": {
              "ssidRequired": "SSID を入力してください。"
            }
          },
          "del": {
            "description": "wifi list で表示されたインデックスの登録情報を削除します。",
            "fields": {
              "index": {
                "label": "インデックス",
                "placeholder": "0"
              }
            },
            "actions": {
              "run": "wifi del を送信"
            },
            "errors": {
              "indexRequired": "インデックスを入力してください。"
            }
          },
          "auto": {
            "description": "起動時に登録済みネットワークへ自動接続するかを切り替えます。",
            "fields": {
              "mode": {
                "label": "自動接続設定",
                "optionOn": "on",
                "optionOff": "off"
              }
            },
            "actions": {
              "run": "wifi auto を送信"
            }
          },
          "connect": {
            "description": "登録済みネットワーク、または指定した SSID/鍵で Wi-Fi に接続します。",
            "fields": {
              "ssid": {
                "label": "SSID (任意)",
                "placeholder": ""
              },
              "key": {
                "label": "パスワード (任意)",
                "placeholder": ""
              }
            },
            "actions": {
              "run": "wifi connect を送信"
            },
            "errors": {
              "keyWithoutSsid": "SSID を指定せずにパスワードは使用できません。"
            }
          },
          "disconnect": {
            "description": "現在の Wi-Fi 接続を切断します。",
            "actions": {
              "run": "wifi disconnect を送信"
            }
          },
          "ntp": {
            "status": {
              "description": "NTP の自動設定や同期状態、利用中のサーバーを表示します。利用前に sys timezone を設定してください。",
              "actions": {
                "run": "ntp status を送信"
              }
            },
            "set": {
              "description": "NTP サーバーを最大 3 台まで登録します。先に sys timezone を設定してください。",
              "fields": {
                "server1": {
                  "label": "サーバー 1",
                  "placeholder": "pool.ntp.org"
                },
                "server2": {
                  "label": "サーバー 2 (任意)",
                  "placeholder": ""
                },
                "server3": {
                  "label": "サーバー 3 (任意)",
                  "placeholder": ""
                }
              },
              "actions": {
                "run": "ntp set を送信"
              },
              "errors": {
                "serverRequired": "サーバー名を入力してください。"
              }
            },
            "enable": {
              "description": "NTP 同期を開始します。sys timezone を設定し、Wi-Fi 接続を確認してください。",
              "actions": {
                "run": "ntp enable を送信"
              }
            },
            "disable": {
              "description": "NTP 同期を停止します。",
              "actions": {
                "run": "ntp disable を送信"
              }
            },
            "auto": {
              "description": "起動時に NTP を自動設定するかを切り替えます。",
              "fields": {
                "mode": {
                  "label": "自動設定",
                  "optionOn": "on",
                  "optionOff": "off"
                }
              },
              "actions": {
                "run": "ntp auto を送信"
              }
            }
          }
        },
        "help": {
          "help": {
            "description": "登録されているコマンドと説明を表示します。",
            "action": "help を送信",
            "placeholder": "コマンド一覧がここに表示されます"
          },
          "question": {
            "description": "<code>help</code> と同一で短縮入力です。キーボード操作向け。",
            "action": "? を送信"
          },
          "log": {
            "title": "ログビューア",
            "description": "シリアル応答やエラーを表示するコンソール領域です。",
            "placeholder": "ESP32 からの応答ログがここに表示されます",
            "clear": "ログをクリア",
            "save": "ログを保存"
          },
          "docs": {
            "title": "関連ドキュメント",
            "description": "操作時に参照したい資料へのショートカットです。",
            "links": [
              "README.md",
              "examples/BasicCli",
              "ESP-IDF Docs"
            ],
            "action": "リンクを開く"
          }
        }
      },
      "filesystem": {
        "list": {
          "title": "ファイル一覧",
          "description": "現在のストレージを対象に <code>fs ls</code> を再帰的に実行して構造を表示します。",
          "stepsTitle": "手順",
          "steps": [
            "ストレージを選択すると自動で最新の一覧を取得します。",
            "ディレクトリは展開して中身を確認できます。",
            "ファイルまたはディレクトリをクリックすると右側に詳細が表示されます。"
          ],
          "refresh": "一覧を更新",
          "refreshing": "更新中..."
        },
        "detail": {
          "title": "詳細と追加操作",
          "selectedPathTitle": "選択中のパス",
          "dirTitle": "fs ls (ディレクトリ)",
          "catTitle": "fs cat",
          "b64Title": "fs b64read",
          "hashTitle": "fs hash",
          "previewTitle": "プレビュー"
        },
        "hints": {
          "spiffs": "SPIFFS ではディレクトリが利用できないため、すべてのファイルは <code>/</code> 直下に作成されます。",
          "littlefs": "LittleFS はディレクトリ階層を利用できます。必要に応じてルート以外のパスも取得します。"
        },
        "messages": {
          "selectStorage": "ストレージタブで対象を選択してください。",
          "noSample": "このストレージのサンプルデータはまだ用意されていません。",
          "loading": "ストレージを選択...",
          "refreshing": "ファイル一覧を取得しています...",
          "fetchFailed": "ファイル一覧の取得に失敗しました。",
          "noDetail": "ファイルまたはディレクトリを選択すると詳細が表示されます。",
          "dirPlaceholder": "ディレクトリの内容がここに表示されます",
          "catPlaceholder": "ファイル内容がここに表示されます",
          "b64Placeholder": "Base64 形式の読み出し結果がここに表示されます",
          "hashPlaceholder": "ハッシュ結果がここに表示されます",
          "previewPlaceholder": "プレビュー可能なデータがまだありません。fs cat または fs b64read の結果が表示されると自動で更新されます。",
          "previewBinary": "バイナリファイルのプレビューには対応していません。Base64 データをダウンロードして確認してください。"
        },
        "table": {
          "type": "タイプ",
          "size": "サイズ",
          "storage": "ストレージ",
          "children": "子要素"
        },
        "details": {
          "file": "ファイル",
          "directory": "ディレクトリ",
          "childCountSuffix": " 件"
        },
        "actions": {
          "mkdir": {
            "title": "ディレクトリを追加 (fs mkdir)",
            "pathLabel": "作成するディレクトリパス",
            "pathPlaceholder": "/logs",
            "action": "fs mkdir を送信"
          },
          "write": {
            "title": "テキストファイルを追加 (fs write)",
            "pathLabel": "ファイル名",
            "pathPlaceholder": "notes.txt",
            "contentLabel": "内容 (短いテキスト向け)",
            "contentPlaceholder": "hello",
            "action": "fs write を送信",
            "errors": {
              "nameRequired": "ファイル名を入力してください。"
            }
          },
          "b64write": {
            "title": "ファイルをアップロード (fs b64write)",
            "hint": "シリアルの 1 行は最大 128 バイトのため、Base64 文字列は 60 バイト程度で分割し、2 行目以降は <code>fs b64write &lt;path&gt; &quot;&lt;chunk&gt;&quot; --append</code> を使用します。",
            "pathLabel": "パス",
            "pathPlaceholder": "/image.bin",
            "chunkLabel": "Base64 チャンク (最大 60 バイト程度)",
            "chunkPlaceholder": "fs b64write /image.bin \"QUJD...\"",
            "firstButton": "1 行目 (append なし)",
            "appendButton": "2 行目以降 (--append)"
          }
        }
      },
      "peripherals": {
        "common": {
          "busLabel": "バス",
          "pinLabel": "ピン"
        },
        "actions": {
          "scan": "i2c scan を送信",
          "read": "i2c read を送信",
          "write": "i2c write を送信",
          "gpioMode": "gpio mode を送信",
          "gpioRead": "gpio read を送信",
          "gpioWrite": "gpio write を送信",
          "gpioToggle": "gpio toggle を送信",
          "adcRead": "adc read を送信",
          "pwmSet": "pwm set を送信",
          "pwmStop": "pwm stop を送信",
          "rgbPin": "rgb pin を送信",
          "rgbSet": "rgb set を送信",
          "rgbStreamStart": "rgb stream を開始"
        },
        "commands": {
          "i2c-scan": {
            "description": "指定した I2C バス上の応答アドレスをスキャンします。"
          },
          "i2c-read": {
            "description": "指定アドレスからデータを読み取ります。"
          },
          "i2c-write": {
            "description": "バイト列をデバイスへ送信します。"
          },
          "gpio-mode": {
            "description": "GPIO の入出力設定を変更します。"
          },
          "gpio-read": {
            "description": "入力ピンの状態を読み取ります。"
          },
          "gpio-write": {
            "description": "出力ピンを HIGH/LOW に設定します。"
          },
          "gpio-toggle": {
            "description": "出力ピンの状態をトグルします。"
          },
          "adc-read": {
            "description": "ADC チャンネルをサンプリングします。"
          },
          "pwm-set": {
            "description": "LEDC PWM 信号を設定します（12bit、duty は 0-4095 または %）。"
          },
          "pwm-stop": {
            "description": "指定ピンの PWM を停止します。"
          },
          "rgb-pin": {
            "description": "デフォルトの RGB LED 出力ピンを設定します。"
          },
          "rgb-set": {
            "description": "単色で RGB LED を発光させます。"
          },
          "rgb-stream": {
            "description": "連続した RGB データをストリーミングし、アニメーションを表示します。"
          }
        },
        "options": {
          "gpioMode": {
            "input": "input",
            "output": "output",
            "input_pullup": "input_pullup",
            "input_pulldown": "input_pulldown"
          },
          "gpioValue": {
            "HIGH": "HIGH",
            "LOW": "LOW"
          }
        },
        "labels": {
          "address": "アドレス (0x--)",
          "length": "読み取りバイト数 (省略可)",
          "payload": "送信データ (スペース区切り 0x)",
          "pin": "ピン",
          "mode": "モード",
          "value": "値",
          "duty": "デューティ (0-4095 または %)",
          "frequency": "周波数 (Hz)",
          "channel": "チャネル",
          "atten": "減衰モード",
          "sampleRate": "サンプル (ms/BPS)",
          "average": "平均回数",
          "red": "R (0-255)",
          "green": "G (0-255)",
          "blue": "B (0-255)"
        },
        "placeholders": {
          "address": "例: 0x3C",
          "length": "例: 16",
          "payload": "例: 0x01 0x02",
          "pin": "例: 21",
          "value": "例: high",
          "duty": "例: 2048 または 50%",
          "frequency": "例: 5000",
          "channel": "例: 0",
          "atten": "例: 11db",
          "sampleRate": "例: 100ms",
          "average": "例: 8",
          "red": "例: 255",
          "green": "例: 128",
          "blue": "例: 64"
        },
        "notes": {
          "stream": "チャンク送信の進捗やエラーを表示予定"
        }
      },
      "help": {
        "tabsLabel": "ヘルプコマンド"
      },
      "footer": {
        "hint": "<strong>ヒント:</strong> WebSerial API は HTTPS または localhost 上でのみ利用できます。GitHub Pages での運用を前提にしています。",
        "links": {
          "webSerial": "Web Serial API Docs",
          "githubPages": "GitHub Pages"
        }
      }
    },
    "en": {
      "meta": {
        "title": "ESP32 WebSerial Control"
      },
      "header": {
        "title": "ESP32 Serial Control",
        "subtitle": "Web console for managing ESP32 built-in and user-defined commands via WebSerial."
      },
      "connection": {
        "status": {
          "disconnected": "Disconnected",
          "connecting": "Connecting…",
          "disconnecting": "Disconnecting…",
          "connected": "Connected",
          "unsupported": "WebSerial Unsupported"
        },
        "actions": {
          "connect": "Connect Device",
          "disconnect": "Disconnect"
        },
        "info": {
          "disabledTitle": "Preview only – controls are disabled",
          "placeholderStatus": "Ready – waiting for connection",
          "unsupportedHint": "Try a browser that supports the Web Serial API.",
          "connectFirst": "Please connect to the device before running commands.",
          "disconnectFirst": "Disconnect first if you need to switch devices.",
          "waitDisconnect": "Disconnect in progress. Please wait."
        }
      },
      "language": {
        "label": "Display Language",
        "ariaLabel": "Select display language",
        "option": {
          "en": "English",
          "ja": "Japanese",
          "zh": "Chinese"
        }
      },
      "nav": {
        "heading": "Command Groups",
        "tabs": {
          "system": {
            "label": "System",
            "desc": "sys info / uptime / time / timezone / reset"
          },
          "wifi": {
            "label": "Wi-Fi",
            "desc": "wifi status / connect"
          },
          "storage": {
            "label": "Storage",
            "desc": "storage list / use / status"
          },
          "config": {
            "label": "Config",
            "desc": "conf list / get / set / del"
          },
          "filesystem": {
            "label": "Filesystem",
            "desc": "fs ls / write / hash"
          },
          "peripherals": {
            "label": "Peripherals",
            "desc": "gpio / adc / pwm / rgb / i2c"
          },
          "help": {
            "label": "Help",
            "desc": "help / log"
          }
        }
      },
      "sections": {
        "system": {
          "title": "System Commands",
          "description": "Collection of standard and user-defined commands for monitoring and controlling ESP32 system state.",
          "ariaTablist": "System Commands"
        },
        "wifi": {
          "title": "Wi-Fi Commands",
          "description": "Inspect connection status, manage stored access points, and control auto-connect behavior. Configure sys timezone before using NTP features.",
          "ariaTablist": "Wi-Fi Commands"
        },
        "config": {
          "title": "Application Settings",
          "description": "List and update application-defined configuration slots stored via conf commands.",
          "actions": {
            "refresh": "Refresh settings",
            "save": "Save",
            "reset": "Reset to default"
          },
          "table": {
            "name": "Name",
            "value": "Value",
            "description": "Description",
            "actions": "Actions"
          },
          "labels": {
            "stored": "Stored",
            "notStored": "Using default",
            "defaultValue": "Default",
            "empty": "(empty)"
          },
          "empty": "No settings found. Fetch the latest list.",
          "ariaTablist": "Application Settings"
        },
        "storage": {
          "title": "Storage Commands",
          "description": "Manage connected storage devices and switch active targets."
        },
        "filesystem": {
          "title": "Filesystem Commands",
          "description": "<code>fs</code> commands organized by category."
        },
        "peripherals": {
          "title": "Peripheral Commands",
          "description": "Control GPIO, ADC, PWM, RGB LED, and I2C peripherals."
        },
        "help": {
          "title": "Help & Logs",
          "description": "Review command references and inspect command output logs."
        }
      },
      "results": {
        "latest": "Latest Result (auto)",
        "latestManual": "Latest Result",
        "placeholder": "Results will appear here",
        "pending": "Running…"
      },
      "storage": {
        "list": {
          "title": "Storage List",
          "description": "Automatically runs <code>storage list</code> when the tab opens.",
          "selectorLabel": "Available Storage",
          "selectorTitle": "Select storage to use",
          "selectorEmpty": "No mounted storage found",
          "selectorHint": "Selecting an entry sends <code>storage use</code> and refreshes the list and status.",
          "table": {
            "name": "Name",
            "type": "Type",
            "state": "State"
          }
        },
        "status": {
          "title": "Storage Status",
          "description": "Shows the latest <code>storage status</code> response."
        }
      },
      "commands": {
        "system": {
          "sys-info": {
            "description": "Retrieve chip model, revision, clocks, and SDK/IDF versions.",
            "action": "Send sys info"
          },
          "sys-uptime": {
            "description": "Show uptime in hh:mm:ss and milliseconds.",
            "action": "Send sys uptime"
          },
          "sys-time": {
            "description": "Fetch or set the current time in ISO 8601 format for RTC and timezone validation.",
            "field": {
              "label": "Datetime (YYYY-MM-DDTHH:MM:SS)",
              "placeholder": "2024-01-01T12:34:56"
            },
            "actions": {
              "get": "Fetch current time",
              "set": "Apply datetime",
              "useBrowser": "Use browser time"
            },
            "errors": {
              "required": "Enter a datetime value.",
              "invalid": "Use YYYY-MM-DDTHH:MM:SS format."
            }
          },
          "sys-timezone": {
            "description": "Inspect or update the persisted timezone used during startup initialization and NTP synchronization.",
            "field": {
              "label": "Timezone (e.g. JST-9/CST-8/UTC+0)",
              "placeholder": "JST-9"
            },
            "actions": {
              "get": "Fetch timezone",
              "set": "Apply timezone",
              "useBrowser": "Use browser timezone"
            },
            "errors": {
              "required": "Enter a timezone value."
            }
          },
          "sys-mem": {
            "description": "Inspect heap and PSRAM totals, free space, minimum, and largest block.",
            "action": "Send sys mem"
          },
          "sys-reset": {
            "description": "Soft reset the ESP32.",
            "action": "Send sys reset"
          },
          "conf-list": {
            "description": "List application-defined configuration slots. All available descriptions are shown unless `--lang` is provided to filter a specific language.",
            "actions": {
              "run": "Send conf list"
            }
          },
          "conf-get": {
            "description": "Read a slot by name and report whether a stored value overrides the default. Use `conf list` (optionally with `--lang`) to review localized descriptions.",
            "field": {
              "label": "Config name",
              "placeholder": "api_key"
            },
            "actions": {
              "run": "Send conf get"
            },
            "errors": {
              "required": "Enter a config name."
            }
          },
          "conf-set": {
            "description": "Persist a string value for the selected slot name. Use quotes when including spaces.",
            "fields": {
              "index": {
                "label": "Config name",
                "placeholder": "api_key"
              },
              "value": {
                "label": "Value",
                "placeholder": "my-value"
              }
            },
            "actions": {
              "run": "Send conf set"
            },
            "errors": {
              "indexRequired": "Enter a config name.",
              "valueRequired": "Enter a value."
            }
          },
          "conf-del": {
            "description": "Delete the stored value for the specified slot name so the default is used again.",
            "field": {
              "label": "Config name",
              "placeholder": "api_key"
            },
            "actions": {
              "run": "Send conf del"
            },
            "errors": {
              "required": "Enter a config name."
            }
          }
        },
        "wifi": {
          "status": {
            "description": "Show current Wi-Fi status, MAC/IP information, and RSSI.",
            "actions": {
              "run": "Send wifi status"
            }
          },
          "list": {
            "description": "List access points stored in NVS.",
            "actions": {
              "run": "Send wifi list"
            }
          },
          "add": {
            "description": "Store a new SSID and password in NVS (up to 8 networks).",
            "fields": {
              "ssid": {
                "label": "SSID",
                "placeholder": "MyWiFi"
              },
              "key": {
                "label": "Password",
                "placeholder": "(optional)"
              }
            },
            "actions": {
              "run": "Send wifi add"
            },
            "errors": {
              "ssidRequired": "Enter an SSID."
            }
          },
          "del": {
            "description": "Remove a stored network by the index shown in wifi list.",
            "fields": {
              "index": {
                "label": "Index",
                "placeholder": "0"
              }
            },
            "actions": {
              "run": "Send wifi del"
            },
            "errors": {
              "indexRequired": "Enter an index value."
            }
          },
          "auto": {
            "description": "Toggle automatic connection to stored networks on startup.",
            "fields": {
              "mode": {
                "label": "Auto-connect setting",
                "optionOn": "on",
                "optionOff": "off"
              }
            },
            "actions": {
              "run": "Send wifi auto"
            }
          },
          "connect": {
            "description": "Connect using stored credentials or explicit SSID/password overrides.",
            "fields": {
              "ssid": {
                "label": "SSID (optional)",
                "placeholder": ""
              },
              "key": {
                "label": "Password (optional)",
                "placeholder": ""
              }
            },
            "actions": {
              "run": "Send wifi connect"
            },
            "errors": {
              "keyWithoutSsid": "Provide an SSID when supplying a password."
            }
          },
          "disconnect": {
            "description": "Disconnect from the current Wi-Fi network.",
            "actions": {
              "run": "Send wifi disconnect"
            }
          },
          "ntp": {
            "status": {
              "description": "Show NTP auto settings, sync status, and configured servers. Set sys timezone first before enabling NTP.",
              "actions": {
                "run": "Send ntp status"
              }
            },
            "set": {
              "description": "Store up to three NTP servers. Configure sys timezone first.",
              "fields": {
                "server1": {
                  "label": "Server 1",
                  "placeholder": "pool.ntp.org"
                },
                "server2": {
                  "label": "Server 2 (optional)",
                  "placeholder": ""
                },
                "server3": {
                  "label": "Server 3 (optional)",
                  "placeholder": ""
                }
              },
              "actions": {
                "run": "Send ntp set"
              },
              "errors": {
                "serverRequired": "Enter a primary server."
              }
            },
            "enable": {
              "description": "Enable NTP synchronization. Ensure sys timezone is set and Wi-Fi is connected.",
              "actions": {
                "run": "Send ntp enable"
              }
            },
            "disable": {
              "description": "Disable NTP synchronization.",
              "actions": {
                "run": "Send ntp disable"
              }
            },
            "auto": {
              "description": "Toggle automatic NTP configuration on startup.",
              "fields": {
                "mode": {
                  "label": "Auto setting",
                  "optionOn": "on",
                  "optionOff": "off"
                }
              },
              "actions": {
                "run": "Send ntp auto"
              }
            }
          }
        },
        "help": {
          "help": {
            "description": "Display registered commands and their descriptions.",
            "action": "Send help",
            "placeholder": "Command list will appear here"
          },
          "question": {
            "description": "<code>help</code> alias for keyboard-friendly shorthand.",
            "action": "Send ?"
          },
          "log": {
            "title": "Log Viewer",
            "description": "Console area for serial responses and errors.",
            "placeholder": "Responses from the ESP32 will appear here.",
            "clear": "Clear Log",
            "save": "Save Log"
          },
          "docs": {
            "title": "Related Documents",
            "description": "Shortcuts to reference materials while operating.",
            "links": [
              "README.md",
              "examples/BasicCli",
              "ESP-IDF Docs"
            ],
            "action": "Open Reference"
          }
        }
      },
      "filesystem": {
        "list": {
          "title": "File List",
          "description": "Run <code>fs ls</code> recursively on the active storage to display its structure.",
          "stepsTitle": "Steps",
          "steps": [
            "Selecting a storage automatically fetches the latest listing.",
            "Expand directories to inspect their contents.",
            "Click a file or directory to show details on the right."
          ],
          "refresh": "Refresh list",
          "refreshing": "Refreshing..."
        },
        "detail": {
          "title": "Details & Additional Actions",
          "selectedPathTitle": "Selected Path",
          "dirTitle": "fs ls (Directory)",
          "catTitle": "fs cat",
          "b64Title": "fs b64read",
          "hashTitle": "fs hash",
          "previewTitle": "Preview"
        },
        "hints": {
          "spiffs": "SPIFFS does not support directories; all files are created under <code>/</code>.",
          "littlefs": "LittleFS supports directory hierarchies and can fetch non-root paths when needed."
        },
        "messages": {
          "selectStorage": "Select a storage target from the storage tab.",
          "noSample": "Sample data for this storage has not been prepared yet.",
          "loading": "Select a storage...",
          "refreshing": "Fetching filesystem list...",
          "fetchFailed": "Failed to fetch filesystem list.",
          "noDetail": "Select a file or directory to view details.",
          "dirPlaceholder": "Directory contents will appear here.",
          "catPlaceholder": "File contents will appear here.",
          "b64Placeholder": "Base64 read results will appear here.",
          "hashPlaceholder": "Hash results will appear here.",
          "previewPlaceholder": "No preview available yet. fs cat or fs b64read output will show up here automatically.",
          "previewBinary": "Binary preview is not supported. Please download the Base64 data instead."
        },
        "table": {
          "type": "Type",
          "size": "Size",
          "storage": "Storage",
          "children": "Children"
        },
        "details": {
          "file": "File",
          "directory": "Directory",
          "childCountSuffix": " items"
        },
        "actions": {
          "mkdir": {
            "title": "Create Directory (fs mkdir)",
            "pathLabel": "Directory path",
            "pathPlaceholder": "/logs",
            "action": "Send fs mkdir"
          },
          "write": {
            "title": "Create Text File (fs write)",
            "pathLabel": "File name",
            "pathPlaceholder": "notes.txt",
            "contentLabel": "Content (for short text)",
            "contentPlaceholder": "hello",
            "action": "Send fs write",
            "errors": {
              "nameRequired": "File name is required."
            }
          },
          "b64write": {
            "title": "Upload File (fs b64write)",
            "hint": "Each serial line is up to 128 bytes; split Base64 chunks to ~60 bytes and use <code>fs b64write &lt;path&gt; &quot;&lt;chunk&gt;&quot; --append</code> from the second line.",
            "pathLabel": "Path",
            "pathPlaceholder": "/image.bin",
            "chunkLabel": "Base64 chunk (up to ~60 bytes)",
            "chunkPlaceholder": "fs b64write /image.bin \"QUJD...\"",
            "firstButton": "First line (without append)",
            "appendButton": "Additional lines (--append)"
          }
        }
      },
      "peripherals": {
        "common": {
          "busLabel": "Bus",
          "pinLabel": "Pin"
        },
        "actions": {
          "scan": "Send i2c scan",
          "read": "Send i2c read",
          "write": "Send i2c write",
          "gpioMode": "Send gpio mode",
          "gpioRead": "Send gpio read",
          "gpioWrite": "Send gpio write",
          "gpioToggle": "Send gpio toggle",
          "adcRead": "Send adc read",
          "pwmSet": "Send pwm set",
          "pwmStop": "Send pwm stop",
          "rgbPin": "Send rgb pin",
          "rgbSet": "Send rgb set",
          "rgbStreamStart": "Start rgb stream"
        },
        "commands": {
          "i2c-scan": {
            "description": "Scan for responsive addresses on the selected I2C bus."
          },
          "i2c-read": {
            "description": "Read data from the specified address."
          },
          "i2c-write": {
            "description": "Send bytes to the device."
          },
          "gpio-mode": {
            "description": "Change the GPIO direction or mode."
          },
          "gpio-read": {
            "description": "Read the state of an input pin."
          },
          "gpio-write": {
            "description": "Set an output pin to HIGH or LOW."
          },
          "gpio-toggle": {
            "description": "Toggle the state of an output pin."
          },
          "adc-read": {
            "description": "Sample an ADC channel."
          },
          "pwm-set": {
            "description": "Configure the LEDC PWM signal (12-bit; duty accepts 0-4095 or %)."
          },
          "pwm-stop": {
            "description": "Stop PWM output on the specified pin."
          },
          "rgb-pin": {
            "description": "Set the default RGB LED output pin."
          },
          "rgb-set": {
            "description": "Drive the RGB LED with a solid color."
          },
          "rgb-stream": {
            "description": "Stream continuous RGB data to play animations."
          }
        },
        "options": {
          "gpioMode": {
            "input": "input",
            "output": "output",
            "input_pullup": "input_pullup",
            "input_pulldown": "input_pulldown"
          },
          "gpioValue": {
            "HIGH": "HIGH",
            "LOW": "LOW"
          }
        },
        "labels": {
          "address": "Address (0x--)",
          "length": "Read length (optional)",
          "payload": "Payload (space-separated 0x)",
          "pin": "Pin",
          "mode": "Mode",
          "value": "Value",
          "duty": "Duty (0-4095 or %)",
          "frequency": "Frequency (Hz)",
          "channel": "Channel",
          "atten": "Attenuation",
          "sampleRate": "Sample interval (ms/BPS)",
          "average": "Average count",
          "red": "R (0-255)",
          "green": "G (0-255)",
          "blue": "B (0-255)"
        },
        "placeholders": {
          "address": "e.g. 0x3C",
          "length": "e.g. 16",
          "payload": "e.g. 0x01 0x02",
          "pin": "e.g. 21",
          "value": "e.g. high",
          "duty": "e.g. 2048 or 50%",
          "frequency": "e.g. 5000",
          "channel": "e.g. 0",
          "atten": "e.g. 11db",
          "sampleRate": "e.g. 100ms",
          "average": "e.g. 8",
          "red": "e.g. 255",
          "green": "e.g. 128",
          "blue": "e.g. 64"
        },
        "notes": {
          "stream": "Progress and errors for chunk streaming will appear here."
        }
      },
      "help": {
        "tabsLabel": "Help Commands"
      },
      "footer": {
        "hint": "<strong>Tip:</strong> The WebSerial API is available only over HTTPS or localhost. This UI is designed for deployment on GitHub Pages.",
        "links": {
          "webSerial": "Web Serial API Docs",
          "githubPages": "GitHub Pages"
        }
      }
    },
    "zh": {
      "meta": {
        "title": "ESP32 WebSerial 控制台"
      },
      "header": {
        "title": "ESP32 串口控制台",
        "subtitle": "通过 WebSerial 管理 ESP32 内置命令与用户自定义命令的 Web 控制台。"
      },
      "connection": {
        "status": {
          "disconnected": "未连接",
          "connecting": "连接中…",
          "disconnecting": "断开中…",
          "connected": "已连接",
          "unsupported": "不支持 WebSerial"
        },
        "actions": {
          "connect": "连接设备",
          "disconnect": "断开连接"
        },
        "info": {
          "disabledTitle": "预览模式，控件已禁用",
          "placeholderStatus": "已就绪 - 等待连接",
          "unsupportedHint": "请使用支持 Web Serial API 的浏览器。",
          "connectFirst": "请先连接设备再执行指令。",
          "disconnectFirst": "若要切换设备，请先断开连接。",
          "waitDisconnect": "正在断开连接，请稍候。"
        }
      },
      "language": {
        "label": "显示语言",
        "ariaLabel": "选择显示语言",
        "option": {
          "en": "英语",
          "ja": "日语",
          "zh": "中文"
        }
      },
      "nav": {
        "heading": "指令分组",
        "tabs": {
          "system": {
            "label": "系统",
            "desc": "sys info / uptime / time / timezone / reset"
          },
          "wifi": {
            "label": "Wi-Fi",
            "desc": "wifi status / connect"
          },
          "config": {
            "label": "应用配置",
            "desc": "conf list / get / set / del"
          },
          "storage": {
            "label": "存储",
            "desc": "storage list / use / status"
          },
          "filesystem": {
            "label": "文件系统",
            "desc": "fs ls / write / hash"
          },
          "peripherals": {
            "label": "外设",
            "desc": "gpio / adc / pwm / rgb / i2c"
          },
          "help": {
            "label": "帮助",
            "desc": "help / log"
          }
        }
      },
      "sections": {
        "system": {
          "title": "系统指令",
          "description": "汇集用于监控 ESP32 状态与执行系统控制的标准指令和用户自定义指令。",
          "ariaTablist": "系统指令"
        },
        "wifi": {
          "title": "Wi-Fi 指令",
          "description": "查看连接状态、管理已保存的接入点，并控制开机自动连接。启用 NTP 前请先通过 sys timezone 设置时区。",
          "ariaTablist": "Wi-Fi 指令"
        },
        "config": {
          "title": "应用配置",
          "description": "列出应用定义的配置项，并支持更新或恢复默认值。",
          "actions": {
            "refresh": "刷新配置",
            "save": "保存",
            "reset": "恢复默认"
          },
          "table": {
            "name": "名称",
            "value": "值",
            "description": "说明",
            "actions": "操作"
          },
          "labels": {
            "stored": "已保存",
            "notStored": "使用默认值",
            "defaultValue": "默认值",
            "empty": "（空）"
          },
          "empty": "未找到配置，请点击刷新按钮。",
          "ariaTablist": "应用配置"
        },
        "storage": {
          "title": "存储指令",
          "description": "用于管理已连接的存储设备并切换当前目标。"
        },
        "filesystem": {
          "title": "文件操作指令",
          "description": "<code>fs</code> 系列指令按类别整理。"
        },
        "peripherals": {
          "title": "外设指令",
          "description": "控制 GPIO、ADC、PWM、RGB LED 与 I2C 等外设。"
        },
        "help": {
          "title": "帮助与日志",
          "description": "用于查看指令列表和响应日志。"
        }
      },
      "results": {
        "latest": "最新结果（自动）",
        "latestManual": "最新结果",
        "placeholder": "结果将在此显示",
        "pending": "执行中…"
      },
      "storage": {
        "list": {
          "title": "存储列表",
          "description": "打开选项卡时自动执行 <code>storage list</code>。",
          "selectorLabel": "可用存储",
          "selectorTitle": "选择要使用的存储",
          "selectorEmpty": "未找到已挂载的存储",
          "selectorHint": "选择后会发送 <code>storage use</code> 并刷新列表与状态。",
          "table": {
            "name": "名称",
            "type": "类型",
            "state": "状态"
          }
        },
        "status": {
          "title": "存储状态",
          "description": "显示最新的 <code>storage status</code> 结果。"
        }
      },
      "commands": {
        "system": {
          "sys-info": {
            "description": "获取芯片型号、修订版本、时钟以及 SDK/IDF 版本等信息。",
            "action": "发送 sys info"
          },
          "sys-uptime": {
            "description": "以 hh:mm:ss 和毫秒显示开机后经过的时间。",
            "action": "发送 sys uptime"
          },
          "sys-time": {
            "description": "以 ISO 8601 格式获取或设置当前时间，可用于校验时区与 RTC。",
            "field": {
              "label": "日期时间 (YYYY-MM-DDTHH:MM:SS)",
              "placeholder": "2024-01-01T12:34:56"
            },
            "actions": {
              "get": "获取当前时间",
              "set": "设置日期时间",
              "useBrowser": "填入浏览器时间"
            },
            "errors": {
              "required": "请输入日期时间。",
              "invalid": "请使用 YYYY-MM-DDTHH:MM:SS 格式。"
            }
          },
          "sys-timezone": {
            "description": "查询或更新启动时复用的持久化时区，并用于 NTP 同步。",
            "field": {
              "label": "时区 (例如: JST-9/CST-8/UTC+0)",
              "placeholder": "JST-9"
            },
            "actions": {
              "get": "获取时区",
              "set": "设置时区",
              "useBrowser": "填入浏览器时区"
            },
            "errors": {
              "required": "请输入时区。"
            }
          },
          "sys-mem": {
            "description": "查看堆与 PSRAM 的总量、空闲、最小值以及最大块。",
            "action": "发送 sys mem"
          },
          "sys-reset": {
            "description": "对 ESP32 执行软重启。",
            "action": "发送 sys reset"
          },
          "conf-list": {
            "description": "列出应用定义的配置槽位。若未指定 `--lang`，将显示全部语言的说明；使用 `--lang` 可仅输出某一种语言。",
            "actions": {
              "run": "发送 conf list"
            }
          },
          "conf-get": {
            "description": "按名称读取配置，并提示是已有存储值还是默认值。描述文本请在 `conf list` 中查看（可配合 `--lang` 过滤）。",
            "field": {
              "label": "配置名称",
              "placeholder": "api_key"
            },
            "actions": {
              "run": "发送 conf get"
            },
            "errors": {
              "required": "请输入配置名称。"
            }
          },
          "conf-set": {
            "description": "为指定名称保存字符串值，若包含空格请使用引号。",
            "fields": {
              "index": {
                "label": "配置名称",
                "placeholder": "api_key"
              },
              "value": {
                "label": "保存的值",
                "placeholder": "my-value"
              }
            },
            "actions": {
              "run": "发送 conf set"
            },
            "errors": {
              "indexRequired": "请输入配置名称。",
              "valueRequired": "请输入值。"
            }
          },
          "conf-del": {
            "description": "删除指定名称已保存的值，恢复为默认值。",
            "field": {
              "label": "配置名称",
              "placeholder": "api_key"
            },
            "actions": {
              "run": "发送 conf del"
            },
            "errors": {
              "required": "请输入配置名称。"
            }
          }
        },
        "wifi": {
          "status": {
            "description": "显示当前 Wi-Fi 状态、MAC/IP 信息以及 RSSI。",
            "actions": {
              "run": "发送 wifi status"
            }
          },
          "list": {
            "description": "列出存储在 NVS 中的接入点。",
            "actions": {
              "run": "发送 wifi list"
            }
          },
          "add": {
            "description": "向 NVS 注册新的 SSID 与密码（最多 8 个）。",
            "fields": {
              "ssid": {
                "label": "SSID",
                "placeholder": "MyWiFi"
              },
              "key": {
                "label": "密码",
                "placeholder": "（可留空）"
              }
            },
            "actions": {
              "run": "发送 wifi add"
            },
            "errors": {
              "ssidRequired": "请输入 SSID。"
            }
          },
          "del": {
            "description": "根据 wifi list 显示的索引删除存储的网络。",
            "fields": {
              "index": {
                "label": "索引",
                "placeholder": "0"
              }
            },
            "actions": {
              "run": "发送 wifi del"
            },
            "errors": {
              "indexRequired": "请输入索引值。"
            }
          },
          "auto": {
            "description": "切换启动时是否自动连接已保存的网络。",
            "fields": {
              "mode": {
                "label": "自动连接设置",
                "optionOn": "on",
                "optionOff": "off"
              }
            },
            "actions": {
              "run": "发送 wifi auto"
            }
          },
          "connect": {
            "description": "使用已保存的凭据或指定的 SSID/密码进行连接。",
            "fields": {
              "ssid": {
                "label": "SSID（可选）",
                "placeholder": ""
              },
              "key": {
                "label": "密码（可选）",
                "placeholder": ""
              }
            },
            "actions": {
              "run": "发送 wifi connect"
            },
            "errors": {
              "keyWithoutSsid": "仅输入密码时必须指定 SSID。"
            }
          },
          "disconnect": {
            "description": "断开当前的 Wi-Fi 连接。",
            "actions": {
              "run": "发送 wifi disconnect"
            }
          },
          "ntp": {
            "status": {
              "description": "显示 NTP 自动设置、同步状态及当前使用的服务器。启用前请先设置 sys timezone。",
              "actions": {
                "run": "发送 ntp status"
              }
            },
            "set": {
              "description": "注册最多 3 个 NTP 服务器，使用前请先设置 sys timezone。",
              "fields": {
                "server1": {
                  "label": "服务器 1",
                  "placeholder": "pool.ntp.org"
                },
                "server2": {
                  "label": "服务器 2 (可选)",
                  "placeholder": ""
                },
                "server3": {
                  "label": "服务器 3 (可选)",
                  "placeholder": ""
                }
              },
              "actions": {
                "run": "发送 ntp set"
              },
              "errors": {
                "serverRequired": "请输入主要服务器。"
              }
            },
            "enable": {
              "description": "启用 NTP 同步。请确认已设置 sys timezone 且 Wi-Fi 已连接。",
              "actions": {
                "run": "发送 ntp enable"
              }
            },
            "disable": {
              "description": "停止 NTP 同步。",
              "actions": {
                "run": "发送 ntp disable"
              }
            },
            "auto": {
              "description": "切换开机时是否自动配置 NTP。",
              "fields": {
                "mode": {
                  "label": "自动配置",
                  "optionOn": "on",
                  "optionOff": "off"
                }
              },
              "actions": {
                "run": "发送 ntp auto"
              }
            }
          }
        },
        "help": {
          "help": {
            "description": "显示已注册的指令及说明。",
            "action": "发送 help",
            "placeholder": "指令列表将在此显示。"
          },
          "question": {
            "description": "<code>help</code> 的简写形式，方便键盘输入。",
            "action": "发送 ?"
          },
          "log": {
            "title": "日志查看器",
            "description": "显示串口响应与错误的控制台区域。",
            "placeholder": "来自 ESP32 的响应将显示在此。",
            "clear": "清除日志",
            "save": "保存日志"
          },
          "docs": {
            "title": "相关文档",
            "description": "操作时可快速跳转的参考资料。",
            "links": [
              "README.md",
              "examples/BasicCli",
              "ESP-IDF Docs"
            ],
            "action": "打开链接"
          }
        }
      },
      "filesystem": {
        "list": {
          "title": "文件列表",
          "description": "针对当前存储递归执行 <code>fs ls</code> 并显示结构。",
          "stepsTitle": "步骤",
          "steps": [
            "选择存储后会自动获取最新列表。",
            "可展开目录查看其中内容。",
            "点击文件或目录可在右侧显示详细信息。"
          ],
          "refresh": "刷新列表",
          "refreshing": "刷新中..."
        },
        "detail": {
          "title": "详细信息与附加操作",
          "selectedPathTitle": "当前路径",
          "dirTitle": "fs ls（目录）",
          "catTitle": "fs cat",
          "b64Title": "fs b64read",
          "hashTitle": "fs hash",
          "previewTitle": "预览"
        },
        "hints": {
          "spiffs": "SPIFFS 不支持目录，所有文件都会创建在 <code>/</code> 根目录下。",
          "littlefs": "LittleFS 支持目录层级，可根据需要获取非根路径。"
        },
        "messages": {
          "selectStorage": "请在存储选项卡中选择目标。",
          "noSample": "该存储的示例数据尚未准备。",
          "loading": "请选择存储...",
          "refreshing": "正在获取文件列表...",
          "fetchFailed": "获取文件列表失败。",
          "noDetail": "选择文件或目录以查看详细信息。",
          "dirPlaceholder": "目录内容将在此显示。",
          "catPlaceholder": "文件内容将在此显示。",
          "b64Placeholder": "Base64 读取结果将在此显示。",
          "hashPlaceholder": "哈希结果将在此显示。",
          "previewPlaceholder": "暂时无法预览，等待 fs cat 或 fs b64read 的结果显示在此处。",
          "previewBinary": "不支持二进制预览，请下载 Base64 数据。"
        },
        "table": {
          "type": "类型",
          "size": "大小",
          "storage": "存储",
          "children": "子项"
        },
        "details": {
          "file": "文件",
          "directory": "目录",
          "childCountSuffix": " 个"
        },
        "actions": {
          "mkdir": {
            "title": "创建目录 (fs mkdir)",
            "pathLabel": "目录路径",
            "pathPlaceholder": "/logs",
            "action": "发送 fs mkdir"
          },
          "write": {
            "title": "创建文本文件 (fs write)",
            "pathLabel": "文件名",
            "pathPlaceholder": "notes.txt",
            "contentLabel": "内容（适用于短文本）",
            "contentPlaceholder": "hello",
            "action": "发送 fs write",
            "errors": {
              "nameRequired": "请输入文件名。"
            }
          },
          "b64write": {
            "title": "上传文件 (fs b64write)",
            "hint": "串口单行最多 128 字节，请将 Base64 分段为约 60 字节，第二行起使用 <code>fs b64write &lt;path&gt; &quot;&lt;chunk&gt;&quot; --append</code>。",
            "pathLabel": "路径",
            "pathPlaceholder": "/image.bin",
            "chunkLabel": "Base64 分段（最多约 60 字节）",
            "chunkPlaceholder": "fs b64write /image.bin \"QUJD...\"",
            "firstButton": "首行（无 append）",
            "appendButton": "后续行 (--append)"
          }
        }
      },
      "peripherals": {
        "common": {
          "busLabel": "总线",
          "pinLabel": "引脚"
        },
        "actions": {
          "scan": "发送 i2c scan",
          "read": "发送 i2c read",
          "write": "发送 i2c write",
          "gpioMode": "发送 gpio mode",
          "gpioRead": "发送 gpio read",
          "gpioWrite": "发送 gpio write",
          "gpioToggle": "发送 gpio toggle",
          "adcRead": "发送 adc read",
          "pwmSet": "发送 pwm set",
          "pwmStop": "发送 pwm stop",
          "rgbPin": "发送 rgb pin",
          "rgbSet": "发送 rgb set",
          "rgbStreamStart": "启动 rgb stream"
        },
        "commands": {
          "i2c-scan": {
            "description": "在所选 I2C 总线上扫描响应地址。"
          },
          "i2c-read": {
            "description": "从指定地址读取数据。"
          },
          "i2c-write": {
            "description": "向设备发送字节序列。"
          },
          "gpio-mode": {
            "description": "修改 GPIO 的输入输出模式。"
          },
          "gpio-read": {
            "description": "读取输入引脚状态。"
          },
          "gpio-write": {
            "description": "将输出引脚设为 HIGH/LOW。"
          },
          "gpio-toggle": {
            "description": "切换输出引脚的状态。"
          },
          "adc-read": {
            "description": "采样指定 ADC 通道。"
          },
          "pwm-set": {
            "description": "配置 LEDC PWM 信号（12 位，占空比支持 0-4095 或 %）。"
          },
          "pwm-stop": {
            "description": "停止指定引脚的 PWM 输出。"
          },
          "rgb-pin": {
            "description": "设置默认的 RGB LED 输出引脚。"
          },
          "rgb-set": {
            "description": "以单色点亮 RGB LED。"
          },
          "rgb-stream": {
            "description": "持续发送 RGB 数据以显示动画。"
          }
        },
        "options": {
          "gpioMode": {
            "input": "input",
            "output": "output",
            "input_pullup": "input_pullup",
            "input_pulldown": "input_pulldown"
          },
          "gpioValue": {
            "HIGH": "HIGH",
            "LOW": "LOW"
          }
        },
        "labels": {
          "address": "地址 (0x--)",
          "length": "读取字节数（可选）",
          "payload": "发送数据（空格分隔 0x）",
          "pin": "引脚",
          "mode": "模式",
          "value": "值",
          "duty": "占空比 (0-4095 或 %)",
          "frequency": "频率 (Hz)",
          "channel": "通道",
          "atten": "衰减模式",
          "sampleRate": "采样间隔 (ms/BPS)",
          "average": "平均次数",
          "red": "R (0-255)",
          "green": "G (0-255)",
          "blue": "B (0-255)"
        },
        "placeholders": {
          "address": "例如: 0x3C",
          "length": "例如: 16",
          "payload": "例如: 0x01 0x02",
          "pin": "例如: 21",
          "value": "例如: high",
          "duty": "例如: 2048 或 50%",
          "frequency": "例如: 5000",
          "channel": "例如: 0",
          "atten": "例如: 11db",
          "sampleRate": "例如: 100ms",
          "average": "例如: 8",
          "red": "例如: 255",
          "green": "例如: 128",
          "blue": "例如: 64"
        },
        "notes": {
          "stream": "分段传输的进度与错误将在此显示。"
        }
      },
      "help": {
        "tabsLabel": "帮助指令"
      },
      "footer": {
        "hint": "<strong>提示：</strong>WebSerial API 仅能在 HTTPS 或 localhost 环境下使用。本界面适用于 GitHub Pages 部署。",
        "links": {
          "webSerial": "Web Serial API 文档",
          "githubPages": "GitHub Pages"
        }
      }
    }
  };

  const tabCommandRequirements = {
    system: ['sys'],
    wifi: ['wifi', 'ntp'],
    config: ['conf'],
    storage: ['storage'],
    filesystem: ['fs'],
    peripherals: ['gpio', 'adc', 'i2c', 'pwm', 'rgb']
  };

  const tabSupportState = new Map();
  let availableCommandPrefixes = new Set();

  const storageSamples = {
    list: `OK storage list
|   sd (SD card) [not mounted]
|   spiffs (SPIFFS) [mounted]
|   littlefs (LittleFS) [not mounted]
|   fatfs (FAT FS) [not mounted]`,
    statusNone: `ERR 412 No storage selected`,
    statusById: {
      spiffs: `OK storage status
| storage: spiffs
| mounted: yes
| total: 1318001 bytes
| used: 1506 bytes
| free: 1316495 bytes`
    }
  };

  const fsSamples = {
    spiffs: {
      noteKey: 'filesystem.hints.spiffs',
      listRaw: `OK fs ls
| storage: spiffs path: /
|          9 aaa.txt
|         13 base64.txt
|         12 test.txt
|          8 aaa2.txt
|          1 base.txt`,
      tree: [
        {
          name: 'aaa.txt',
          path: '/aaa.txt',
          type: 'file',
          size: '9 bytes',
          cat: `OK fs cat
| storage: spiffs path: /aaa.txt offset: 0
| length: until EOF
| テスト`,
          stat: `OK fs stat
| storage: spiffs path: /aaa.txt
| type: file
| size: 9 bytes
| modified: 2695`,
          b64read: null,
          hash: `OK fs hash
| storage: spiffs path: /aaa.txt
| algo: sha256
| size: 9 bytes
| digest: --`
        },
        {
          name: 'base64.txt',
          path: '/base64.txt',
          type: 'file',
          size: '13 bytes',
          cat: `OK fs cat
| storage: spiffs path: /base64.txt offset: 0
| length: until EOF
| (Base64 サンプルデータ)`,
          stat: `OK fs stat
| storage: spiffs path: /base64.txt
| type: file
| size: 13 bytes
| modified: 2695`,
          b64read: null,
          hash: `OK fs hash
| storage: spiffs path: /base64.txt
| algo: sha256
| size: 13 bytes
| digest: --`
        },
        {
          name: 'test.txt',
          path: '/test.txt',
          type: 'file',
          size: '12 bytes',
          cat: `OK fs cat
| storage: spiffs path: /test.txt offset: 0
| length: until EOF
| test string`,
          stat: `OK fs stat
| storage: spiffs path: /test.txt
| type: file
| size: 12 bytes
| modified: 2695`,
          b64read: `OK fs b64read
| storage: spiffs path: /test.txt
| chunk: 48
| data[0]: dGVzdCBzdHJpbmcK
| chunks: 1`,
          hash: `OK fs hash
| storage: spiffs path: /test.txt
| algo: sha256
| size: 12 bytes
| digest: 37d2046a395cbfcb2712ff5c96a727b1966876080047c56717009dbbc235f566`
        },
        {
          name: 'aaa2.txt',
          path: '/aaa2.txt',
          type: 'file',
          size: '8 bytes',
          cat: `OK fs cat
| storage: spiffs path: /aaa2.txt offset: 0
| length: until EOF
| test str`,
          stat: `OK fs stat
| storage: spiffs path: /aaa2.txt
| type: file
| size: 8 bytes
| modified: 2695`,
          b64read: null,
          hash: `OK fs hash
| storage: spiffs path: /aaa2.txt
| algo: sha256
| size: 8 bytes
| digest: --`
        },
        {
          name: 'base.txt',
          path: '/base.txt',
          type: 'file',
          size: '1 bytes',
          cat: `OK fs cat
| storage: spiffs path: /base.txt offset: 0
| length: until EOF
| A`,
          stat: `OK fs stat
| storage: spiffs path: /base.txt
| type: file
| size: 1 bytes
| modified: 2695`,
          b64read: null,
          hash: `OK fs hash
| storage: spiffs path: /base.txt
| algo: sha256
| size: 1 bytes
| digest: --`
        }
      ]
    },
    littlefs: {
      noteKey: 'filesystem.hints.littlefs',
      listRaw: `OK fs ls
| storage: littlefs path: /
|         12 test.txt
| <DIR> test

OK fs ls
| storage: littlefs path: /test
|          3 aaa.txt`,
      tree: [
        {
          name: 'test.txt',
          path: '/test.txt',
          type: 'file',
          size: '12 bytes',
          cat: `OK fs cat
| storage: littlefs path: /test.txt offset: 0
| length: until EOF
| test string`,
          stat: `OK fs stat
| storage: littlefs path: /test.txt
| type: file
| size: 12 bytes
| modified: 2695`,
          b64read: null,
          hash: `OK fs hash
| storage: littlefs path: /test.txt
| algo: sha256
| size: 12 bytes
| digest: --`
        },
        {
          name: 'test',
          path: '/test',
          type: 'dir',
          size: '--',
          lsRaw: `OK fs ls
| storage: littlefs path: /test
|          3 aaa.txt`,
          stat: `OK fs stat
| storage: littlefs path: /test
| type: dir
| size: 3 bytes
| entries: 1`,
          children: [
            {
              name: 'aaa.txt',
              path: '/test/aaa.txt',
              type: 'file',
              size: '3 bytes',
              cat: `OK fs cat
| storage: littlefs path: /test/aaa.txt offset: 0
| length: until EOF
| aaa`,
              stat: `OK fs stat
| storage: littlefs path: /test/aaa.txt
| type: file
| size: 3 bytes
| modified: 2695`,
              b64read: null,
              hash: `OK fs hash
| storage: littlefs path: /test/aaa.txt
| algo: sha256
| size: 3 bytes
| digest: --`
            }
          ]
        }
      ]
    }
  };

  const normalizeLanguage = (value) => {
    if (!value || typeof value !== 'string') {
      return LANGUAGE_FALLBACK;
    }
    const lowered = value.toLowerCase();
    if (SUPPORTED_LANGUAGES.includes(lowered)) {
      return lowered;
    }
    const alias = LANGUAGE_ALIASES[lowered];
    if (alias) {
      return alias;
    }
    const base = lowered.split('-')[0];
    return LANGUAGE_ALIASES[base] || LANGUAGE_FALLBACK;
  };

  const getTranslationValue = (lang, key) => {
    const segments = key.split('.');
    let current = translations[lang];
    for (const segment of segments) {
      if (current && Object.prototype.hasOwnProperty.call(current, segment)) {
        current = current[segment];
      } else {
        current = undefined;
        break;
      }
    }
    if (current === undefined && lang !== LANGUAGE_FALLBACK) {
      return getTranslationValue(LANGUAGE_FALLBACK, key);
    }
    return current;
  };

  const formatTranslationValue = (value, { html = false } = {}) => {
    if (Array.isArray(value)) {
      if (html) {
        return value.map((item) => `<li>${item}</li>`).join('');
      }
      return value.join(', ');
    }
    return value;
  };

  const translate = (key, options = {}) =>
    formatTranslationValue(getTranslationValue(currentLanguage, key), options);

  const applyAttributeTranslations = (element) => {
    const attrMap = [
      { datasetKey: 'i18nTitle', attr: 'title' },
      { datasetKey: 'i18nPlaceholder', attr: 'placeholder' },
      { datasetKey: 'i18nAriaLabel', attr: 'aria-label' }
    ];
    attrMap.forEach(({ datasetKey, attr }) => {
      const key = element.dataset[datasetKey];
      if (!key) {
        return;
      }
      const value = getTranslationValue(currentLanguage, key);
      if (value !== undefined) {
        element.setAttribute(attr, formatTranslationValue(value));
      }
    });
  };

  const applyTranslations = () => {
    document.documentElement.lang = currentLanguage;
    const pageTitle = getTranslationValue(currentLanguage, 'meta.title');
    if (pageTitle) {
      document.title = pageTitle;
    }

    const textNodes = document.querySelectorAll('[data-i18n]');
    textNodes.forEach((element) => {
      const key = element.dataset.i18n;
      if (!key) {
        return;
      }
      const value = getTranslationValue(currentLanguage, key);
      if (value !== undefined) {
        element.textContent = formatTranslationValue(value);
      }
      applyAttributeTranslations(element);
    });

    const htmlNodes = document.querySelectorAll('[data-i18n-html]');
    htmlNodes.forEach((element) => {
      const key = element.dataset.i18nHtml;
      if (!key) {
        return;
      }
      const value = getTranslationValue(currentLanguage, key);
      if (value !== undefined) {
        element.innerHTML = formatTranslationValue(value, { html: true });
      }
      applyAttributeTranslations(element);
    });

    const attrNodes = document.querySelectorAll(
      '[data-i18n-title],[data-i18n-placeholder],[data-i18n-aria-label]'
    );
    attrNodes.forEach(applyAttributeTranslations);
  };

  const resolveInitialLanguage = () => {
    let stored = null;
    try {
      stored = localStorage.getItem(LANGUAGE_STORAGE_KEY);
    } catch {
      stored = null;
    }
    if (stored) {
      return normalizeLanguage(stored);
    }
    const candidates = Array.isArray(navigator.languages)
      ? navigator.languages
      : [navigator.language];
    for (const candidate of candidates) {
      const normalized = normalizeLanguage(candidate);
      if (SUPPORTED_LANGUAGES.includes(normalized)) {
        return normalized;
      }
    }
    return LANGUAGE_FALLBACK;
  };

  let currentLanguage = LANGUAGE_FALLBACK;
  const languageSelect = document.querySelector('#language-select');
  const connectButton = document.querySelector('#connect-button');
  const disconnectButton = document.querySelector('#disconnect-button');
  const statusLabel = document.querySelector('#connection-status-label');
  const statusPill = document.querySelector('.status-pill');
  const logOutput = document.querySelector('[data-log-output]');
  const commandButtons = Array.from(
    document.querySelectorAll(
      '#tab-system .command-panel .card-actions button:not([data-system-utility="true"]), ' +
      '#tab-wifi .command-panel .card-actions button:not([data-system-utility="true"])'
    )
  );
  const commandPanels = new Map();
  commandButtons.forEach((button) => {
    const panel = button.closest('.command-panel');
    const commandId = panel?.dataset.command;
    if (commandId && panel && !commandPanels.has(commandId)) {
      commandPanels.set(commandId, { panel, button });
    }
    button.disabled = false;
    button.removeAttribute('disabled');
  });

  document
    .querySelectorAll('#tab-system .command-panel, #tab-wifi .command-panel')
    .forEach((panel) => {
      const commandId = panel?.dataset.command;
      if (!commandId || commandPanels.has(commandId)) {
        return;
      }
      const defaultButton = panel.querySelector(
        '.card-actions button:not([data-system-utility="true"])'
      );
      commandPanels.set(commandId, { panel, button: defaultButton || null });
    });

  const sysTimeInput = document.querySelector('#sys-time-input');
  const sysTimeSetButton = document.querySelector('#sys-time-set');
  const sysTimeUseBrowserButton = document.querySelector('#sys-time-use-browser');
  const sysTimezoneInput = document.querySelector('#sys-timezone-input');
  const sysTimezoneSetButton = document.querySelector('#sys-timezone-set');
  const sysTimezoneUseBrowserButton = document.querySelector('#sys-timezone-use-browser');
  const wifiAutoSelect = document.querySelector('#wifi-auto-select');
  const wifiAutoRunButton = document.querySelector('#wifi-auto-run');
  const wifiAddSsidInput = document.querySelector('#wifi-add-ssid');
  const wifiAddKeyInput = document.querySelector('#wifi-add-key');
  const wifiAddRunButton = document.querySelector('#wifi-add-run');
  const wifiDelIndexInput = document.querySelector('#wifi-del-index');
  const wifiDelRunButton = document.querySelector('#wifi-del-run');
  const wifiConnectSsidInput = document.querySelector('#wifi-connect-ssid');
  const wifiConnectKeyInput = document.querySelector('#wifi-connect-key');
  const wifiConnectRunButton = document.querySelector('#wifi-connect-run');
  const ntpSetServerInputs = [
    document.querySelector('#ntp-set-server1'),
    document.querySelector('#ntp-set-server2'),
    document.querySelector('#ntp-set-server3')
  ];
  const ntpSetRunButton = document.querySelector('#ntp-set-run');
  const ntpEnableRunButton = document.querySelector('#ntp-enable-run');
  const ntpDisableRunButton = document.querySelector('#ntp-disable-run');
  const ntpAutoSelect = document.querySelector('#ntp-auto-select');
  const ntpAutoRunButton = document.querySelector('#ntp-auto-run');
  const configRefreshButton = document.querySelector('#config-refresh-button');
  const configTableBody = document.querySelector('#config-table-body');
  const configEmptyState = document.querySelector('#config-empty-state');

  const helpElements = {
    helpButton: document.querySelector('#command-help-help .card-actions button'),
    questionButton: document.querySelector('#command-help-question .card-actions button'),
    output: document.querySelector('[data-help-output]')
  };
  let helpOutputState = 'placeholder';
  let lastHelpOutputRaw = '';
  let helpOutputErrorKey = null;

  const applyHelpOutput = (text) => {
    if (!helpElements.output) {
      return;
    }
    helpElements.output.textContent = text;
  };

  const updateHelpOutput = (text) => {
    const rawText = text || '';
    const trimmed = rawText.trim();
    if (!trimmed) {
      helpOutputState = 'placeholder';
      lastHelpOutputRaw = '';
      applyHelpOutput(translate('commands.help.help.placeholder'));
      helpOutputErrorKey = null;
      return;
    }
    helpOutputState = 'content';
    lastHelpOutputRaw = rawText;
    helpOutputErrorKey = null;
    applyHelpOutput(rawText);
  };

  const setHelpOutputPending = () => {
    if (!helpElements.output) {
      return;
    }
    helpOutputState = 'pending';
    lastHelpOutputRaw = '';
    helpOutputErrorKey = null;
    applyHelpOutput(translate('results.pending'));
  };

  const setHelpOutputError = (message, { i18nKey = null } = {}) => {
    if (!helpElements.output) {
      return;
    }
    const text = i18nKey ? translate(i18nKey) : message || translate('results.placeholder');
    helpOutputState = 'error';
    lastHelpOutputRaw = text;
    helpOutputErrorKey = i18nKey;
    applyHelpOutput(text);
  };

  const runHelpCommand = (commandText) => {
    if (!helpElements.output) {
      return;
    }
    const id = commandText === '?' ? 'help-alias' : 'help';
    runSerialCommand(commandText, {
      id,
      onStart: () => {
        setHelpOutputPending();
      },
      onUpdate: (buffer) => {
        if (buffer) {
          updateHelpOutput(buffer);
        }
      },
      onFinalize: ({ output }) => {
        updateHelpOutput(output);
        if (output) {
          processHelpOutput(output);
        }
      }
    }).catch((error) => {
      if (connectionState === 'connected') {
        setHelpOutputError(error?.message || translate('results.placeholder'));
      } else {
        setHelpOutputError(null, { i18nKey: 'connection.info.connectFirst' });
      }
    });
  };

  if (helpElements.helpButton) {
    helpElements.helpButton.disabled = false;
    helpElements.helpButton.removeAttribute('disabled');
    helpElements.helpButton.addEventListener('click', () => {
      runHelpCommand('help');
    });
  }
  if (helpElements.questionButton) {
    helpElements.questionButton.disabled = false;
    helpElements.questionButton.removeAttribute('disabled');
    helpElements.questionButton.addEventListener('click', () => {
      runHelpCommand('?');
    });
  }

  const disabledElements = [
    connectButton,
    disconnectButton,
    ...document.querySelectorAll(
      'button[disabled], select[disabled], input[disabled], textarea[disabled]'
    )
  ].filter(Boolean);

  const applyDisabledTitles = () => {
    disabledElements.forEach((element) => {
      if (!element) {
        return;
      }
      if (element.disabled) {
        element.setAttribute('title', translate('connection.info.disabledTitle'));
      } else {
        element.removeAttribute('title');
      }
    });
  };

  let connectionState = 'disconnected';
  let refreshConnectionLabel = () => { };

  const autoCommandIds = new Set([
    'sys-info',
    'sys-uptime',
    'sys-time',
    'sys-timezone',
    'sys-mem',
    'wifi-status',
    'wifi-list',
    'ntp-status'
  ]);
  let autoCommandQueue = [];

  const resetAutoCommandQueue = () => {
    autoCommandQueue = [];
  };

  const processAutoCommandQueue = () => {
    if (activeCommand) {
      return;
    }
    if (connectionState !== 'connected') {
      resetAutoCommandQueue();
      return;
    }
    const nextCommand = autoCommandQueue.shift();
    if (!nextCommand) {
      return;
    }
    sendSystemCommand(nextCommand);
  };

  const enqueueAutoCommand = (commandId) => {
    if (!autoCommandIds.has(commandId)) {
      return;
    }
    if (connectionState !== 'connected') {
      return;
    }
    if (!commandPanels.has(commandId)) {
      return;
    }
    if (activeCommand && activeCommand.id === commandId) {
      return;
    }
    if (autoCommandQueue.includes(commandId)) {
      return;
    }
    autoCommandQueue.push(commandId);
    processAutoCommandQueue();
  };

  const triggerActiveAutoCommand = (tabId) => {
    const container = document.querySelector(`#tab-${tabId}`);
    if (!container) {
      return;
    }
    const activeTab = container.querySelector('.command-tab.is-active');
    const commandId = activeTab?.dataset.command;
    if (commandId) {
      enqueueAutoCommand(commandId);
    }
  };

  const setLanguage = (lang, { persist = true } = {}) => {
    const normalized = normalizeLanguage(lang);
    currentLanguage = normalized;
    if (persist) {
      try {
        localStorage.setItem(LANGUAGE_STORAGE_KEY, normalized);
      } catch {
        /* ignore storage errors */
      }
    }
    applyTranslations();
    refreshConnectionLabel();
    applyDisabledTitles();
    refreshLanguageSensitiveUI();
    if (languageSelect && languageSelect.value !== normalized) {
      languageSelect.value = normalized;
    }
  };

  const storageElements = {
    select: document.querySelector('#storage-select'),
    listTableBody: document.querySelector('[data-storage-list-table] tbody'),
    listRaw: document.querySelector('[data-storage-list-raw]'),
    listTimestamp: document.querySelector('[data-storage-list-timestamp]'),
    statusTable: document.querySelector('[data-storage-status-table]'),
    statusTableBody: document.querySelector('[data-storage-status-table] tbody'),
    statusRaw: document.querySelector('[data-storage-status-raw]'),
    statusTimestamp: document.querySelector('[data-storage-status-timestamp]')
  };

  const fsElements = {
    tree: document.querySelector('[data-fs-tree]'),
    empty: document.querySelector('[data-fs-empty]'),
    listRaw: document.querySelector('[data-fs-list-raw]'),
    listTimestamp: document.querySelector('[data-fs-list-timestamp]'),
    listRefreshButton: document.querySelector('#fs-list-refresh-button'),
    infoMessage: document.querySelector('[data-fs-info-message]'),
    selectedPath: document.querySelector('[data-fs-selected-path]'),
    detailTable: document.querySelector('[data-fs-detail-table]'),
    detailTableBody: document.querySelector('[data-fs-detail-table] tbody'),
    dirSection: document.querySelector('[data-fs-dir-section]'),
    dirRaw: document.querySelector('[data-fs-dir-raw]'),
    catSection: document.querySelector('[data-fs-cat-section]'),
    catRaw: document.querySelector('[data-fs-cat-raw]'),
    b64Section: document.querySelector('[data-fs-b64-section]'),
    b64Raw: document.querySelector('[data-fs-b64-raw]'),
    hashSection: document.querySelector('[data-fs-hash-section]'),
    hashRaw: document.querySelector('[data-fs-hash-raw]'),
    previewSection: document.querySelector('[data-fs-preview-section]'),
    previewEmpty: document.querySelector('[data-fs-preview-empty]'),
    previewText: document.querySelector('[data-fs-preview-text]'),
    previewImage: document.querySelector('[data-fs-preview-image]'),
    mkdirSection: document.querySelector('[data-fs-action-section="mkdir"]'),
    mkdirPathInput: document.querySelector('#fs-mkdir-path'),
    mkdirRunButton: document.querySelector('#fs-mkdir-run'),
    writeSection: document.querySelector('[data-fs-action-section="write"]'),
    writePathInput: document.querySelector('#fs-write-path'),
    writeContentInput: document.querySelector('#fs-write-content'),
    writeRunButton: document.querySelector('#fs-write-run'),
    b64writeSection: document.querySelector('[data-fs-action-section="b64write"]'),
    b64writePathInput: document.querySelector('#fs-b64write-path'),
    b64writeChunkInput: document.querySelector('#fs-b64write-chunk'),
    b64writeFirstButton: document.querySelector('#fs-b64write-first'),
    b64writeAppendButton: document.querySelector('#fs-b64write-append')
  };

  const tabButtons = document.querySelectorAll('.tab-button');
  const tabPanels = document.querySelectorAll('.tab-content');
  const tabButtonMap = new Map();
  const tabPanelMap = new Map();

  tabButtons.forEach((button) => {
    const tabId = button.dataset.tab;
    if (!tabId) {
      return;
    }
    tabButtonMap.set(tabId, button);
    if (!tabSupportState.has(tabId)) {
      tabSupportState.set(tabId, true);
    }
  });

  tabPanels.forEach((panel) => {
    const tabId = panel.id.replace(/^tab-/, '');
    tabPanelMap.set(tabId, panel);
    if (!tabSupportState.has(tabId)) {
      tabSupportState.set(tabId, true);
    }
  });

  const isTabSupported = (tabId) => tabSupportState.get(tabId) !== false;

  const applyTabVisibility = (tabId, supported) => {
    const previous = tabSupportState.get(tabId);
    tabSupportState.set(tabId, supported);
    const button = tabButtonMap.get(tabId);
    if (button) {
      button.classList.toggle('is-hidden', !supported);
      button.setAttribute('aria-hidden', supported ? 'false' : 'true');
      button.setAttribute('tabindex', supported ? '0' : '-1');
      if (!supported) {
        button.classList.remove('is-active');
      }
    }
    const listItem = button ? button.closest('li') : null;
    if (listItem) {
      listItem.classList.toggle('is-hidden', !supported);
      listItem.setAttribute('aria-hidden', supported ? 'false' : 'true');
    }
    const panel = tabPanelMap.get(tabId);
    if (panel) {
      panel.classList.toggle('is-hidden', !supported);
      if (!supported) {
        panel.classList.remove('is-active');
      }
      panel.setAttribute('aria-hidden', supported ? 'false' : 'true');
    }
    if (previous !== supported) {
      appendLogEntry('debug', `UI: tab ${tabId} ${supported ? 'visible' : 'hidden'} (help)`);
    }
  };

  const ensureActiveTabVisible = () => {
    const currentButton = tabButtonMap.get(currentTab);
    if (currentButton && !currentButton.classList.contains('is-hidden')) {
      return;
    }
    const fallback = Array.from(tabButtons).find((btn) => !btn.classList.contains('is-hidden'));
    if (fallback) {
      activateTab(fallback.dataset.tab);
    }
  };

  const updateAvailableCommandGroups = (prefixSet) => {
    const normalized = new Set(Array.from(prefixSet).map((value) => value.toLowerCase()));
    availableCommandPrefixes = normalized;
    const handledTabs = new Set();
    Object.entries(tabCommandRequirements).forEach(([tabId, requirements]) => {
      const supported = requirements.some((req) => normalized.has(req));
      applyTabVisibility(tabId, supported);
      handledTabs.add(tabId);
    });
    tabButtonMap.forEach((_, tabId) => {
      if (!handledTabs.has(tabId)) {
        applyTabVisibility(tabId, true);
      }
    });
    ensureActiveTabVisible();
    if (!isTabSupported('config')) {
      configEntries = [];
      configNeedsInitialFetch = true;
      renderConfigTable([]);
    } else {
      renderConfigTable(configEntries);
      if (configNeedsInitialFetch && connectionState === 'connected') {
        refreshConfigList({ silent: true }).catch(() => {
          /* handled via log */
        });
      }
    }
  };

  const parseHelpOutput = (raw) => {
    const prefixes = new Set();
    if (!raw) {
      return prefixes;
    }
    const sanitized = String(raw).replace(/\r/g, '');
    sanitized.split('\n').forEach((line) => {
      const trimmed = line.trim();
      if (!trimmed) {
        return;
      }
      let content = trimmed;
      if (trimmed.startsWith('|')) {
        content = trimmed.slice(1).trim();
      } else if (trimmed.startsWith('-') || trimmed.startsWith('*')) {
        content = trimmed.slice(1).trim();
      }
      if (!content) {
        return;
      }
      const match = content.match(/^([A-Za-z0-9_]+)/);
      if (match) {
        prefixes.add(match[1].toLowerCase());
      }
    });
    return prefixes;
  };

  const processHelpOutput = (raw) => {
    const prefixes = parseHelpOutput(raw);
    if (!prefixes.size) {
      return;
    }
    if (!prefixes.size) {
      appendLogEntry('debug', 'UI: no command prefixes detected in help output');
      return;
    }
    appendLogEntry('debug', `UI: help command prefixes -> ${Array.from(prefixes).join(', ')}`);
    updateAvailableCommandGroups(prefixes);
  };

  const fetchHelpCommandList = async () => {
    if (!isSerialReady()) {
      return;
    }
    if (activeCommand && typeof activeCommand.id === 'string' && activeCommand.id.startsWith('help')) {
      return;
    }
    try {
      await runSerialCommand('help', {
        id: 'help-auto',
        onFinalize: (result) => {
          if (!result?.error && result.output) {
            processHelpOutput(result.output);
            if (helpOutputState === 'placeholder') {
              updateHelpOutput(result.output);
            }
          }
        }
      });
    } catch (error) {
      appendLogEntry('error', error?.message || 'help command failed');
    }
  };

  let storageInitialized = false;
  let currentStorageId = null;
  let currentTab = 'system';
  let currentFsSelection = null;
  let fsPathMap = new Map();
  let fsFetching = false;
  let lastFsData = null;
  let lastStorageListRaw = storageSamples.list;
  let lastStorageStatusRaw = storageSamples.statusNone;
  let configInitialized = false;
  let configLoading = false;
  let configEntries = [];
  let configNeedsInitialFetch = true;

  if (fsElements.tree && !fsElements.tree.dataset.bound) {
    fsElements.tree.addEventListener('click', (event) => {
      const node = event.target.closest('.fs-node');
      if (!node) {
        return;
      }
      const path = node.dataset.fsPath;
      if (path) {
        selectFsPath(path);
      }
    });
    fsElements.tree.dataset.bound = 'true';
  }

  const getStorageListRaw = (selectedId) =>
    storageSamples.list
      .split('\n')
      .map((line) => {
        const match = line.match(/\|\s+([^\s]+)\s+\(([^)]+)\)\s+\[(.+)\]/);
        if (!match) {
          return line;
        }
        const id = match[1];
        const state = selectedId && id === selectedId ? 'mounted' : 'not mounted';
        return line.replace(/\[(.+)\]/, `[${state}]`);
      })
      .join('\n');

  const escapeSelector = (value) => {
    if (window.CSS && typeof window.CSS.escape === 'function') {
      return window.CSS.escape(value);
    }
    return value.replace(/([\\.\\#:\\[\\]])/g, '\\\\$1');
  };

  const activateTab = (targetId) => {
    let nextTab = targetId;
    const targetButton = tabButtonMap.get(targetId);
    if (!targetButton || targetButton.classList.contains('is-hidden')) {
      const firstVisible = Array.from(tabButtons).find((button) => !button.classList.contains('is-hidden'));
      if (!firstVisible) {
        return;
      }
      nextTab = firstVisible.dataset.tab;
    }
    currentTab = nextTab;
    tabButtons.forEach((button) => {
      const isActive = button.dataset.tab === currentTab;
      button.classList.toggle('is-active', isActive);
      button.setAttribute('aria-selected', isActive ? 'true' : 'false');
    });

    tabPanels.forEach((panel) => {
      const isActive = panel.id === `tab-${currentTab}`;
      panel.classList.toggle('is-active', isActive);
    });

    if (currentTab === 'system' || currentTab === 'wifi') {
      triggerActiveAutoCommand(currentTab);
    }
    if (currentTab === 'storage') {
      runStorageAutoFetch().catch(() => {
        /* handled via log */
      });
    }
    if (currentTab === 'filesystem') {
      runFsAutoFetch({ silent: true });
    }
    if (currentTab === 'config') {
      ensureConfigInitialized();
      if (configNeedsInitialFetch && isTabSupported('config')) {
        refreshConfigList({ silent: true }).catch(() => {
          /* handled via log */
        });
      } else {
        renderConfigTable(configEntries);
      }
    }
  };

  tabButtons.forEach((button) => {
    button.addEventListener('click', () => {
      const targetTab = button.dataset.tab;
      activateTab(targetTab);
    });
  });

  activateTab('system');

  const systemMockRaw = {
    'sys-info': `OK sys info
| Board: ESP32_DEV
| Variant: esp32
| FQBN: esp32:esp32:esp32:DebugLevel=verbose
| Chip: ESP32-D0WD-V3 rev 301
| Cores: 2
| CPU Frequency: 240 MHz
| Chip Features: WiFi/BT/BLE
| Flash: 4194304 bytes @ 80000000 Hz
| MAC: 88:88:88:88:88:88
| IDF: v5.5.1-255-g07e9bf4970
| Arduino Core: 3_3_2
| Build: Oct 21 2025 00:00:00`,
    'sys-uptime': `OK sys uptime
| Uptime: 0:23:28 (1408444 ms)`,
    'sys-time': `OK sys time
| localtime: 1970-01-01T00:23:32+00:00`,
    'sys-timezone': `OK sys timezone
| tz: JST-9`,
    'sys-mem': `OK sys mem
| Heap Total: 306664 bytes
| Heap Free: 256336 bytes
| Heap Min Free: 245608 bytes
| Heap Largest Block: 110580 bytes
| Internal Total: 361160 bytes
| Internal Free: 309980 bytes
| Internal Min Free: 299208 bytes
| Internal Largest Block: 110580 bytes
| RTOS Heap Free: 256336 bytes
| RTOS Heap Min Free: 245608 bytes
| Task Stack High Water: 3952 words`,
    'wifi-status': `OK wifi status
| status: connected
| auto: on
| ssid: DemoNet
| ip: 192.168.0.10
| mac: AA:BB:CC:DD:EE:FF
| bssid: 11:22:33:44:55:66
| channel: 6
| rssi: -42 dBm`,
    'wifi-list': `OK wifi list
| entries: 2
| #0 slot:0 ssid:DemoNet
| #1 slot:1 ssid:Guest`,
    'ntp-status': `OK ntp status
| auto: on
| enabled: on
| timezone: JST-9
| server0: pool.ntp.org
| running: yes
| sync: completed`
  };

  const extractPipeValue = (raw, key) => {
    if (!raw) {
      return '';
    }
    const regex = new RegExp(`^\\|\\s*${key}\\s*:\\s*(.+)$`, 'mi');
    const match = regex.exec(raw);
    return match ? match[1].trim() : '';
  };

  const updateSysTimeInputFromOutput = (raw) => {
    if (!sysTimeInput) {
      return;
    }
    const value = extractPipeValue(raw, 'localtime');
    if (!value) {
      return;
    }
    const match = value.match(/^(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2})/);
    if (match) {
      sysTimeInput.value = match[1];
    } else {
      sysTimeInput.value = value;
    }
  };

  const updateSysTimezoneInputFromOutput = (raw) => {
    if (!sysTimezoneInput) {
      return;
    }
    const value = extractPipeValue(raw, 'tz');
    if (value) {
      sysTimezoneInput.value = value;
    }
  };

  const pad2 = (value) => value.toString().padStart(2, '0');

  const formatDateTimeLocalForInput = (date) =>
    `${date.getFullYear()}-${pad2(date.getMonth() + 1)}-${pad2(date.getDate())}T${pad2(
      date.getHours()
    )}:${pad2(date.getMinutes())}:${pad2(date.getSeconds())}`;

  const TIMEZONE_ABBREVIATION_OVERRIDES = {
    'Asia/Tokyo': 'JST',
    'Asia/Seoul': 'KST',
    'Asia/Shanghai': 'CST',
    'Asia/Taipei': 'CST',
    'Asia/Hong_Kong': 'HKT',
    'Asia/Singapore': 'SGT',
    'America/New_York': 'EST',
    'America/Chicago': 'CST',
    'America/Denver': 'MST',
    'America/Los_Angeles': 'PST',
    'America/Phoenix': 'MST',
    'Europe/London': 'GMT',
    'Europe/Berlin': 'CET',
    'Europe/Paris': 'CET',
    'Australia/Sydney': 'AEST'
  };

  const resolveAbbreviation = (timeZone) => {
    if (!timeZone) {
      return 'UTC';
    }
    if (TIMEZONE_ABBREVIATION_OVERRIDES[timeZone]) {
      return TIMEZONE_ABBREVIATION_OVERRIDES[timeZone];
    }
    try {
      const formatter = new Intl.DateTimeFormat('en-US', {
        timeZone,
        timeZoneName: 'short'
      });
      const parts = formatter.formatToParts(new Date());
      const zonePart = parts.find((part) => part.type === 'timeZoneName')?.value || '';
      const letters = zonePart.replace(/[^A-Za-z]/g, '').toUpperCase();
      if (letters && letters !== 'GMT' && letters.length <= 4) {
        return letters;
      }
    } catch {
      /* ignore */
    }
    const fallback = timeZone.split(/[\\/]/).pop() || 'UTC';
    const condensed = fallback.replace(/_/g, '').toUpperCase();
    if (condensed.length >= 2) {
      return condensed.slice(0, Math.min(4, condensed.length));
    }
    return 'UTC';
  };

  const computeBrowserTimezoneString = () => {
    const { timeZone } = Intl.DateTimeFormat().resolvedOptions();
    const abbreviation = resolveAbbreviation(timeZone);
    const offsetMinutes = new Date().getTimezoneOffset();
    const absoluteMinutes = Math.abs(offsetMinutes);
    const hours = Math.trunc(absoluteMinutes / 60);
    const minutes = absoluteMinutes % 60;
    let sign = '';
    if (offsetMinutes !== 0) {
      sign = offsetMinutes <= 0 ? '-' : '+';
    }
    const minutePart = minutes ? `:${minutes.toString().padStart(2, '0')}` : '';
    return `${abbreviation}${sign}${hours}${minutePart}`;
  };

  const parsePipeTable = (raw) =>
    raw
      .split('\n')
      .filter((line) => line.trim().startsWith('|'))
      .map((line) => line.replace(/^\|\s*/, ''))
      .map((line) => {
        const [key, ...rest] = line.split(':');
        return {
          key: key.trim(),
          value: rest.join(':').trim()
        };
      });

  const formatTimeStamp = () =>
    new Intl.DateTimeFormat(DATETIME_LOCALE[currentLanguage] || DATETIME_LOCALE[LANGUAGE_FALLBACK], {
      hour: '2-digit',
      minute: '2-digit',
      second: '2-digit'
    }).format(new Date());

  const updateResultSection = (panel, commandId) => {
    if (connectionState === 'connected') {
      return;
    }
    const raw = systemMockRaw[commandId];
    if (!raw) {
      return;
    }

    const timestampEl = panel.querySelector('[data-result-timestamp]');
    const rawEl = panel.querySelector('[data-result-raw]');
    const tableEl = panel.querySelector('[data-result-table]');
    const tableBody = tableEl ? tableEl.querySelector('tbody') : null;

    if (timestampEl) {
      timestampEl.textContent = formatTimeStamp();
    }
    if (rawEl) {
      rawEl.textContent = raw;
    }
    if (tableEl && tableBody) {
      const rows = parsePipeTable(raw);
      tableBody.innerHTML = '';
      rows.forEach(({ key, value }) => {
        const row = document.createElement('tr');
        const keyCell = document.createElement('th');
        keyCell.textContent = key;
        const valueCell = document.createElement('td');
        valueCell.textContent = value;
        row.append(keyCell, valueCell);
        tableBody.append(row);
      });
      tableEl.hidden = rows.length === 0;
    }
  };

  const helpAutoIds = new Set(['help', 'question']);

  document.querySelectorAll('.command-layout').forEach((layout) => {
    const commandTabs = layout.querySelectorAll('.command-tab');
    const commandPanels = layout.querySelectorAll('.command-panel');

    if (!commandTabs.length) {
      return;
    }

    const activateCommand = (commandId) => {
      commandTabs.forEach((tab) => {
        const isActive = tab.dataset.command === commandId;
        tab.classList.toggle('is-active', isActive);
        tab.setAttribute('aria-selected', isActive ? 'true' : 'false');
      });

      let activePanel = null;
      commandPanels.forEach((panel) => {
        const isActive = panel.dataset.command === commandId;
        panel.classList.toggle('is-active', isActive);
        if (isActive) {
          activePanel = panel;
        }
      });

      if (activePanel && activePanel.dataset.autoFetch === 'true') {
        updateResultSection(activePanel, commandId);
        if (layout.closest('#tab-system') || layout.closest('#tab-wifi')) {
          enqueueAutoCommand(commandId);
        } else if (helpAutoIds.has(commandId) && connectionState === 'connected') {
          sendSystemCommand(commandId, { panel: activePanel });
        }
      }
    };

    commandTabs.forEach((tab) => {
      tab.addEventListener('click', () => {
        activateCommand(tab.dataset.command);
      });
    });

    activateCommand(commandTabs[0].dataset.command);
  });

  const parseStorageList = (raw) =>
    raw
      .split('\n')
      .filter((line) => line.trim().startsWith('|'))
      .map((line) => line.replace(/^\|\s*/, '').trim())
      .map((line) => {
        const match = line.match(/^([^\s]+)\s*\(([^)]+)\)\s*\[(.+)\]$/);
        if (!match) {
          return null;
        }
        const [, id, label, status] = match;
        const mounted = status.trim() === 'mounted';
        return {
          id,
          label,
          status: status.trim(),
          mounted
        };
      })
      .filter(Boolean);

  const renderStorageList = (raw, selectedId = '') => {
    if (storageElements.listTimestamp) {
      storageElements.listTimestamp.textContent = formatTimeStamp();
    }
    if (storageElements.listRaw) {
      storageElements.listRaw.textContent = raw;
    }
    const entries = parseStorageList(raw);
    if (storageElements.listTableBody) {
      storageElements.listTableBody.innerHTML = '';
      entries.forEach((entry) => {
        const row = document.createElement('tr');
        row.classList.toggle('is-mounted', entry.mounted);
        const nameCell = document.createElement('td');
        nameCell.textContent = entry.id;
        const typeCell = document.createElement('td');
        typeCell.textContent = entry.label;
        const statusCell = document.createElement('td');
        statusCell.textContent = entry.status;
        row.append(nameCell, typeCell, statusCell);
        storageElements.listTableBody.append(row);
      });
    }
    updateStorageSelect(entries, selectedId);
    return entries;
  };

  const renderStorageStatus = (raw) => {
    if (storageElements.statusTimestamp) {
      storageElements.statusTimestamp.textContent = formatTimeStamp();
    }
    if (storageElements.statusRaw) {
      storageElements.statusRaw.textContent = raw;
    }
    const rows = parsePipeTable(raw);
    if (storageElements.statusTable && storageElements.statusTableBody) {
      storageElements.statusTableBody.innerHTML = '';
      rows.forEach(({ key, value }) => {
        const row = document.createElement('tr');
        const keyCell = document.createElement('th');
        keyCell.textContent = key;
        const valueCell = document.createElement('td');
        valueCell.textContent = value;
        row.append(keyCell, valueCell);
        storageElements.statusTableBody.append(row);
      });
      storageElements.statusTable.hidden = rows.length === 0;
    }
  };

  const updateStorageSelect = (entries, selectedId = '') => {
    if (!storageElements.select) {
      return;
    }
    storageElements.select.innerHTML = '';
    const mountedEntries = entries.filter((entry) => entry.mounted);
    if (!mountedEntries.length) {
      const option = document.createElement('option');
      option.value = '';
      option.textContent = translate('storage.list.selectorEmpty');
      storageElements.select.append(option);
      storageElements.select.disabled = true;
      return;
    }
    const placeholder = document.createElement('option');
    placeholder.value = '';
    placeholder.textContent = translate('filesystem.messages.loading');
    storageElements.select.append(placeholder);
    mountedEntries.forEach((entry) => {
      const option = document.createElement('option');
      option.value = entry.id;
      option.textContent = `${entry.id} (${entry.label})`;
      storageElements.select.append(option);
    });
    storageElements.select.disabled = false;
    storageElements.select.value = selectedId;
  };

  const setStorageListPending = () => {
    if (storageElements.listTimestamp) {
      storageElements.listTimestamp.textContent = '--:--';
    }
    if (storageElements.listRaw) {
      storageElements.listRaw.textContent = translate('results.pending');
    }
    if (storageElements.listTableBody) {
      storageElements.listTableBody.innerHTML = '';
    }
  };

  const setStorageStatusPending = () => {
    if (storageElements.statusTimestamp) {
      storageElements.statusTimestamp.textContent = '--:--';
    }
    if (storageElements.statusRaw) {
      storageElements.statusRaw.textContent = translate('results.pending');
    }
    if (storageElements.statusTableBody) {
      storageElements.statusTableBody.innerHTML = '';
    }
    if (storageElements.statusTable) {
      storageElements.statusTable.hidden = true;
    }
  };

  const executeStorageList = async () => {
    const raw = await runSerialCommand('storage list', {
      id: 'storage-list',
      onStart: () => {
        setStorageListPending();
      }
    });
    lastStorageListRaw = raw;
    return raw;
  };

  const executeStorageStatus = async (storageId) => {
    const commandText = storageId ? `storage status ${storageId}` : 'storage status';
    const raw = await runSerialCommand(commandText, {
      id: storageId ? `storage-status-${storageId}` : 'storage-status',
      onStart: () => {
        setStorageStatusPending();
      }
    });
    lastStorageStatusRaw = raw;
    return raw;
  };

  const runStorageAutoFetch = async () => {
    if (!storageInitialized && storageElements.select) {
      storageElements.select.addEventListener('change', (event) => {
        if (!event.target.value) {
          return;
        }
        handleStorageSelection(event.target.value).catch(() => {
          /* handled via log */
        });
      });
      storageInitialized = true;
    }

    if (!isSerialReady()) {
      const fallbackRaw = lastStorageListRaw || getStorageListRaw(currentStorageId || '');
      renderStorageList(fallbackRaw, currentStorageId || '');
      renderStorageStatus(lastStorageStatusRaw);
      return;
    }

    try {
      const listRaw = await executeStorageList();
      const entries = renderStorageList(listRaw, currentStorageId || '');
      if (!currentStorageId) {
        const mounted = entries.find((entry) => entry.mounted) || entries[0];
        if (mounted) {
          currentStorageId = mounted.id;
        }
      }
      const statusRaw = await executeStorageStatus(currentStorageId);
      renderStorageStatus(statusRaw);
    } catch (error) {
      renderStorageList(lastStorageListRaw, currentStorageId || '');
      renderStorageStatus(lastStorageStatusRaw);
    }
  };

  const handleStorageSelection = async (storageId) => {
    if (!storageId) {
      return;
    }
    const previousStorageId = currentStorageId;
    currentFsSelection = null;
    setStorageListPending();
    setStorageStatusPending();
    if (storageElements.select) {
      storageElements.select.disabled = true;
    }
    try {
      const useRaw = await runSerialCommand(`storage use ${storageId}`, {
        id: `storage-use-${storageId}`
      });

      let listRaw = lastStorageListRaw;
      try {
        listRaw = await executeStorageList();
      } catch (error) {
        appendLogEntry('error', `storage list refresh failed: ${error?.message || 'unknown error'}`);
      }
      renderStorageList(listRaw, storageId);

      let statusRaw = '';
      try {
        statusRaw = await executeStorageStatus(storageId);
      } catch (error) {
        appendLogEntry('error', `storage status refresh failed: ${error?.message || 'unknown error'}`);
      }

      const mergedRaw = [useRaw, statusRaw].filter(Boolean).join('\n').trim();
      if (mergedRaw) {
        lastStorageStatusRaw = mergedRaw;
        renderStorageStatus(mergedRaw);
      } else {
        renderStorageStatus(useRaw);
      }

      currentStorageId = storageId;
      await runFsAutoFetch();
    } catch (error) {
      currentStorageId = previousStorageId;
      renderStorageList(lastStorageListRaw, previousStorageId || '');
      renderStorageStatus(lastStorageStatusRaw);
      throw error;
    } finally {
      if (storageElements.select) {
        storageElements.select.disabled = false;
        storageElements.select.value = currentStorageId || '';
      }
      updateFsRefreshButtonState();
    }
  };

  const normalizeConfigLang = (value) => {
    if (!value) {
      return '';
    }
    return normalizeLanguage(value);
  };

  const parseConfigListOutput = (raw) => {
    const result = { count: 0, entries: [] };
    if (!raw) {
      return result;
    }
    const sanitized = String(raw).replace(/\r/g, '');
    const lines = sanitized.split('\n');
    let current = null;

    const ensureCurrent = () => {
      if (!current) {
        current = { descriptions: [] };
      }
    };

    lines.forEach((rawLine) => {
      const trimmed = rawLine.trim();
      if (!trimmed.startsWith('|')) {
        return;
      }
      const line = trimmed.replace(/^\|\s*/, '');
      if (!line) {
        return;
      }
      const colonIndex = line.indexOf(':');
      const value = colonIndex >= 0 ? line.slice(colonIndex + 1).trimStart() : '';
      if (line.startsWith('config_count:')) {
        result.count = parseInt(value, 10) || 0;
        return;
      }
      if (line.startsWith('index:')) {
        if (current) {
          result.entries.push(current);
        }
        current = {
          index: parseInt(value, 10) || 0,
          descriptions: []
        };
        return;
      }
      if (!current) {
        return;
      }
      if (line.startsWith('name:')) {
        current.name = value.trim();
      } else if (line.startsWith('stored:')) {
        current.stored = value.trim().toLowerCase() === 'true';
      } else if (line.startsWith('value:')) {
        current.value = value.replace(/\s+$/g, '');
      } else if (line.startsWith('default:')) {
        current.default = value.replace(/\s+$/g, '');
      } else if (line.startsWith('desc_count:')) {
        current.desc_count = parseInt(value, 10) || 0;
      } else if (line.startsWith('desc[')) {
        const match = line.match(/^desc\[(\d+)\]\.(lang|text):\s*(.*)$/);
        if (!match) {
          return;
        }
        const idx = parseInt(match[1], 10) || 0;
        const field = match[2];
        const descValue = match[3];
        while (current.descriptions.length <= idx) {
          current.descriptions.push({ lang: '', text: '' });
        }
        current.descriptions[idx][field] = descValue;
      }
    });

    if (current) {
      result.entries.push(current);
    }
    return result;
  };

  const pickConfigDescription = (entry) => {
    const descriptions = Array.isArray(entry.descriptions) ? entry.descriptions : [];
    if (!descriptions.length) {
      return { text: '', lang: '' };
    }
    const priorities = [normalizeConfigLang(currentLanguage)];
    if (!priorities.includes('en')) {
      priorities.push('en');
    }
    descriptions.forEach((desc) => {
      const lang = normalizeConfigLang(desc.lang || '');
      if (lang && !priorities.includes(lang)) {
        priorities.push(lang);
      }
    });
    priorities.push('');
    for (const code of priorities) {
      const match = descriptions.find((desc) => normalizeConfigLang(desc.lang || '') === code);
      if (match && match.text) {
        return { text: match.text, lang: match.lang || code };
      }
    }
    return { text: descriptions[0].text || '', lang: descriptions[0].lang || '' };
  };

  const updateConfigRowState = (row) => {
    if (!row) {
      return;
    }
    const input = row.querySelector('.config-input');
    const saveButton = row.querySelector('button[data-action="save"]');
    const resetButton = row.querySelector('button[data-action="reset"]');
    const baseDisabled = connectionState !== 'connected' || configLoading || row.dataset.loading === 'true';
    if (input) {
      input.disabled = baseDisabled || row.dataset.loading === 'true';
    }
    if (saveButton) {
      const original = input ? input.dataset.originalValue || '' : '';
      const currentValue = input ? input.value : '';
      saveButton.disabled = baseDisabled || currentValue === original;
    }
    if (resetButton) {
      const stored = row.dataset.stored === 'true';
      resetButton.disabled = baseDisabled || !stored;
    }
  };

  const updateConfigControlsState = () => {
    const configSupported = isTabSupported('config');
    const refreshDisabled = !configSupported || connectionState !== 'connected' || configLoading;
    if (configRefreshButton) {
      if (refreshDisabled) {
        configRefreshButton.disabled = true;
        configRefreshButton.setAttribute('disabled', '');
      } else {
        configRefreshButton.disabled = false;
        configRefreshButton.removeAttribute('disabled');
      }
    }
    if (!configSupported) {
      if (configEmptyState) {
        configEmptyState.hidden = false;
        configEmptyState.textContent = translate('sections.config.empty');
      }
      if (configTableBody) {
        configTableBody.innerHTML = '';
      }
      return;
    }
    if (!configTableBody) {
      return;
    }
    configTableBody.querySelectorAll('.config-row').forEach((row) => {
      updateConfigRowState(row);
    });
  };

  const setConfigPending = () => {
    configLoading = true;
    updateConfigControlsState();
    if (configTableBody) {
      configTableBody.innerHTML = '';
    }
    if (configEmptyState) {
      configEmptyState.hidden = false;
      configEmptyState.textContent = translate('results.pending');
    }
  };

  const clearConfigPending = () => {
    configLoading = false;
    updateConfigControlsState();
  };

  const createConfigRow = (entry) => {
    const row = document.createElement('div');
    row.className = 'config-row';
    row.dataset.configName = entry.name || '';
    row.dataset.stored = entry.stored ? 'true' : 'false';

    const nameCell = document.createElement('div');
    nameCell.className = 'config-cell config-cell--name';
    const nameTitle = document.createElement('div');
    nameTitle.className = 'config-name';
    nameTitle.textContent = entry.name || '';
    nameCell.append(nameTitle);

    const statusBadge = document.createElement('span');
    statusBadge.className = `config-status ${entry.stored ? 'config-status--stored' : 'config-status--default'}`;
    statusBadge.textContent = translate(
      entry.stored ? 'sections.config.labels.stored' : 'sections.config.labels.notStored'
    );
    nameCell.append(statusBadge);

    const valueCell = document.createElement('div');
    valueCell.className = 'config-cell config-cell--value';
    const valueInput = document.createElement('input');
    valueInput.type = 'text';
    valueInput.className = 'config-input';
    valueInput.value = entry.value || '';
    valueInput.placeholder = entry.default || translate('sections.config.labels.empty');
    valueInput.dataset.originalValue = entry.value || '';
    valueInput.dataset.configName = entry.name || '';
    valueCell.append(valueInput);

    const defaultValue = typeof entry.default === 'string' ? entry.default : '';
    const defaultHint = document.createElement('div');
    defaultHint.className = 'config-default-hint';
    defaultHint.textContent = `${translate('sections.config.labels.defaultValue')}: ${defaultValue || translate('sections.config.labels.empty')
      }`;
    valueCell.append(defaultHint);

    const descCell = document.createElement('div');
    descCell.className = 'config-cell config-cell--description';
    const description = pickConfigDescription(entry);
    if (description.text) {
      const descText = document.createElement('div');
      descText.className = 'config-description-text';
      descText.textContent = description.text;
      descCell.append(descText);
      if (description.lang) {
        const descLang = document.createElement('span');
        descLang.className = 'config-description-lang';
        descLang.textContent = description.lang.toUpperCase();
        descCell.append(descLang);
      }
    } else {
      const descPlaceholder = document.createElement('div');
      descPlaceholder.className = 'config-description-text config-description-text--empty';
      descPlaceholder.textContent = translate('sections.config.labels.empty');
      descCell.append(descPlaceholder);
    }

    const actionsCell = document.createElement('div');
    actionsCell.className = 'config-cell config-cell--actions';
    const saveButton = document.createElement('button');
    saveButton.type = 'button';
    saveButton.className = 'btn btn-primary config-action-save';
    saveButton.dataset.action = 'save';
    saveButton.dataset.configName = entry.name || '';
    saveButton.textContent = translate('sections.config.actions.save');
    actionsCell.append(saveButton);

    const resetButton = document.createElement('button');
    resetButton.type = 'button';
    resetButton.className = 'btn btn-secondary config-action-reset';
    resetButton.dataset.action = 'reset';
    resetButton.dataset.configName = entry.name || '';
    resetButton.textContent = translate('sections.config.actions.reset');
    actionsCell.append(resetButton);

    row.append(nameCell, valueCell, descCell, actionsCell);
    updateConfigRowState(row);
    return row;
  };

  const renderConfigTable = (entriesToRender = configEntries) => {
    if (!configTableBody) {
      return;
    }
    if (!isTabSupported('config')) {
      configTableBody.innerHTML = '';
      if (configEmptyState) {
        configEmptyState.hidden = false;
        configEmptyState.textContent = translate('sections.config.empty');
      }
      updateConfigControlsState();
      return;
    }
    configTableBody.innerHTML = '';
    if (!Array.isArray(entriesToRender) || !entriesToRender.length) {
      if (configEmptyState) {
        configEmptyState.hidden = false;
        configEmptyState.textContent = translate('sections.config.empty');
      }
      updateConfigControlsState();
      return;
    }
    if (configEmptyState) {
      configEmptyState.hidden = true;
    }
    entriesToRender.forEach((entry) => {
      const row = createConfigRow(entry);
      configTableBody.append(row);
    });
    updateConfigControlsState();
  };

  const refreshConfigList = async ({ silent = false } = {}) => {
    ensureConfigInitialized();
    if (configLoading) {
      return;
    }
    if (!isTabSupported('config')) {
      return;
    }
    if (!isSerialReady()) {
      if (!silent) {
        appendLogEntry('error', translate('connection.info.connectFirst'));
      }
      return;
    }
    try {
      const raw = await runSerialCommand('conf list', {
        id: 'conf-list',
        onStart: () => {
          setConfigPending();
        }
      });
      const parsed = parseConfigListOutput(raw);
      configEntries = parsed.entries;
      configNeedsInitialFetch = false;
      renderConfigTable(configEntries);
    } catch (error) {
      appendLogEntry('error', error?.message || 'conf list failed');
      renderConfigTable(configEntries);
    } finally {
      clearConfigPending();
    }
  };

  const handleConfigSave = async (row) => {
    if (!row) {
      return;
    }
    const name = row.dataset.configName;
    const input = row.querySelector('.config-input');
    if (!name || !input) {
      return;
    }
    const value = input.value;
    if (!isSerialReady()) {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    if (value === input.dataset.originalValue) {
      return;
    }
    row.dataset.loading = 'true';
    updateConfigRowState(row);
    try {
      await runSerialCommand(`conf set ${quoteArgument(name)} ${quoteArgument(value)}`, {
        id: `conf-set-${name}`
      });
      await refreshConfigList({ silent: true });
    } catch (error) {
      appendLogEntry('error', error?.message || `conf set ${name} failed`);
    } finally {
      delete row.dataset.loading;
      updateConfigRowState(row);
    }
  };

  const handleConfigReset = async (row) => {
    if (!row) {
      return;
    }
    const name = row.dataset.configName;
    if (!name) {
      return;
    }
    if (!isSerialReady()) {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    row.dataset.loading = 'true';
    updateConfigRowState(row);
    try {
      await runSerialCommand(`conf del ${quoteArgument(name)}`, {
        id: `conf-del-${name}`
      });
      await refreshConfigList({ silent: true });
    } catch (error) {
      appendLogEntry('error', error?.message || `conf del ${name} failed`);
    } finally {
      delete row.dataset.loading;
      updateConfigRowState(row);
    }
  };

  const ensureConfigInitialized = () => {
    if (configInitialized) {
      return;
    }
    configInitialized = true;
    if (configRefreshButton && !configRefreshButton.dataset.bound) {
      configRefreshButton.addEventListener('click', () => {
        refreshConfigList().catch(() => {
          /* handled via log */
        });
      });
      configRefreshButton.dataset.bound = 'true';
    }
    if (configTableBody && !configTableBody.dataset.bound) {
      configTableBody.addEventListener('input', (event) => {
        const input = event.target.closest('.config-input');
        if (!input) {
          return;
        }
        const row = input.closest('.config-row');
        if (row) {
          updateConfigRowState(row);
        }
      });
      configTableBody.addEventListener('click', (event) => {
        const actionButton = event.target.closest('button[data-action]');
        if (!actionButton) {
          return;
        }
        const row = actionButton.closest('.config-row');
        const action = actionButton.dataset.action;
        if (action === 'save') {
          handleConfigSave(row).catch(() => {
            /* handled via log */
          });
        } else if (action === 'reset') {
          handleConfigReset(row).catch(() => {
            /* handled via log */
          });
        }
      });
      configTableBody.dataset.bound = 'true';
    }
    updateConfigControlsState();
    renderConfigTable(configEntries);
  };

  const resetFsDetails = () => {
    if (fsElements.selectedPath) {
      fsElements.selectedPath.textContent = '--';
    }
    if (fsElements.detailTable) {
      fsElements.detailTable.hidden = true;
    }
    if (fsElements.detailTableBody) {
      fsElements.detailTableBody.innerHTML = '';
    }
    if (fsElements.dirSection) {
      fsElements.dirSection.hidden = true;
    }
    if (fsElements.dirRaw) {
      fsElements.dirRaw.textContent = translate('filesystem.messages.dirPlaceholder');
    }
    if (fsElements.catSection) {
      fsElements.catSection.hidden = true;
    }
    if (fsElements.catRaw) {
      fsElements.catRaw.textContent = translate('filesystem.messages.catPlaceholder');
    }
    if (fsElements.b64Section) {
      fsElements.b64Section.hidden = true;
    }
    if (fsElements.b64Raw) {
      fsElements.b64Raw.textContent = translate('filesystem.messages.b64Placeholder');
    }
    if (fsElements.hashSection) {
      fsElements.hashSection.hidden = true;
    }
    if (fsElements.hashRaw) {
      fsElements.hashRaw.textContent = translate('filesystem.messages.hashPlaceholder');
    }
    if (fsElements.previewSection) {
      fsElements.previewSection.hidden = true;
    }
    if (fsElements.previewText) {
      fsElements.previewText.hidden = true;
      fsElements.previewText.textContent = '';
    }
    if (fsElements.previewImage) {
      fsElements.previewImage.hidden = true;
      fsElements.previewImage.removeAttribute('src');
      fsElements.previewImage.removeAttribute('alt');
    }
    if (fsElements.previewEmpty) {
      fsElements.previewEmpty.hidden = false;
      fsElements.previewEmpty.textContent = translate('filesystem.messages.previewPlaceholder');
    }
    if (fsElements.mkdirSection) {
      fsElements.mkdirSection.hidden = true;
    }
    if (fsElements.mkdirPathInput) {
      fsElements.mkdirPathInput.value = '';
    }
    if (fsElements.writeSection) {
      fsElements.writeSection.hidden = true;
    }
    if (fsElements.writePathInput) {
      fsElements.writePathInput.value = '';
      delete fsElements.writePathInput.dataset.basePath;
      fsElements.writePathInput.disabled = true;
      fsElements.writePathInput.setAttribute('disabled', '');
    }
    if (fsElements.writeContentInput) {
      fsElements.writeContentInput.value = '';
      fsElements.writeContentInput.disabled = true;
      fsElements.writeContentInput.setAttribute('disabled', '');
    }
    if (fsElements.writeRunButton) {
      fsElements.writeRunButton.disabled = true;
      fsElements.writeRunButton.setAttribute('disabled', '');
    }
    if (fsElements.b64writeSection) {
      fsElements.b64writeSection.hidden = true;
    }
    if (fsElements.b64writePathInput) {
      fsElements.b64writePathInput.value = '';
    }
    if (fsElements.b64writeChunkInput) {
      fsElements.b64writeChunkInput.value = '';
    }
    currentFsSelection = null;
    applyDisabledTitles();
  };

  const clearFsView = (message) => {
    fsPathMap = new Map();
    if (fsElements.tree) {
      fsElements.tree.innerHTML = '';
      const p = document.createElement('p');
      p.className = 'fs-empty';
      p.textContent = message;
      fsElements.tree.append(p);
    }
    if (fsElements.listRaw) {
      fsElements.listRaw.textContent = translate('results.placeholder');
    }
    if (fsElements.listTimestamp) {
      fsElements.listTimestamp.textContent = '--:--';
    }
    if (fsElements.infoMessage) {
      fsElements.infoMessage.textContent = translate('filesystem.messages.noDetail');
    }
    resetFsDetails();
    lastFsData = null;
    fsFetching = false;
  };

  const getFsHintKey = (storageId) => {
    if (!storageId) {
      return null;
    }
    return fsSamples[storageId]?.noteKey || null;
  };

  const setFsPending = () => {
    if (fsElements.listTimestamp) {
      fsElements.listTimestamp.textContent = '--:--';
    }
    if (fsElements.listRaw) {
      fsElements.listRaw.textContent = translate('results.pending');
    }
    if (fsElements.tree) {
      fsElements.tree.innerHTML = '';
      const p = document.createElement('p');
      p.className = 'fs-empty';
      p.textContent = translate('filesystem.messages.refreshing');
      fsElements.tree.append(p);
    }
    if (fsElements.infoMessage) {
      fsElements.infoMessage.textContent = translate('filesystem.messages.refreshing');
    }
    resetFsDetails();
    lastFsData = null;
  };

  const renderFsError = (messageKey) => {
    fsPathMap = new Map();
    if (fsElements.tree) {
      fsElements.tree.innerHTML = '';
      const p = document.createElement('p');
      p.className = 'fs-empty';
      p.textContent = translate(messageKey || 'filesystem.messages.fetchFailed');
      fsElements.tree.append(p);
    }
    if (fsElements.listRaw) {
      fsElements.listRaw.textContent = translate('results.placeholder');
    }
    if (fsElements.listTimestamp) {
      fsElements.listTimestamp.textContent = '--:--';
    }
    if (fsElements.infoMessage) {
      fsElements.infoMessage.textContent = translate(messageKey || 'filesystem.messages.fetchFailed');
    }
    resetFsDetails();
    lastFsData = null;
  };

  const includeFsRootNode = (items = [], listRaw = null) => {
    const hasRoot = items.some((item) => item.path === '/' && item.type === 'dir');
    if (hasRoot) {
      return items;
    }
    return [
      {
        name: '/',
        path: '/',
        type: 'dir',
        size: '--',
        lsRaw: listRaw,
        children: items.map((item) => item)
      }
    ];
  };

  const buildFsTree = (items = []) => {
    const ul = document.createElement('ul');
    ul.className = 'fs-tree-list';
    items.forEach((item) => {
      fsPathMap.set(item.path, item);
      const li = document.createElement('li');
      li.classList.add('fs-tree-item', item.type === 'dir' ? 'is-dir' : 'is-file');
      const button = document.createElement('button');
      button.type = 'button';
      button.className = 'fs-node';
      button.dataset.fsPath = item.path;
      button.dataset.fsType = item.type;
      const isRoot = item.type === 'dir' && item.path === '/';
      button.textContent = isRoot ? '/' : item.type === 'dir' ? `${item.name}/` : item.name;
      li.append(button);
      if (item.type === 'dir' && item.children && item.children.length) {
        li.append(buildFsTree(item.children));
      }
      ul.append(li);
    });
    return ul;
  };

  const findFirstSelectableNode = (items = []) => {
    for (const item of items) {
      if (item.type === 'file') {
        return item;
      }
      if (item.type === 'dir' && item.children && item.children.length) {
        const found = findFirstSelectableNode(item.children);
        if (found) {
          return found;
        }
      }
    }
    return items[0] || null;
  };

  const renderFsView = (data, { preserveSelection = true, preferredPath = null } = {}) => {
    if (!fsElements.tree) {
      return;
    }
    const previousSelection = preserveSelection ? (preferredPath || currentFsSelection) : null;
    const listRaw = data?.listRaw || '';
    const timestamp = data?.timestamp || '--:--';
    const infoKey = data?.infoKey || getFsHintKey(currentStorageId);

    if (fsElements.listTimestamp) {
      fsElements.listTimestamp.textContent = timestamp;
    }
    if (fsElements.listRaw) {
      fsElements.listRaw.textContent = listRaw || translate('results.placeholder');
    }
    if (fsElements.infoMessage) {
      if (infoKey && getTranslationValue(currentLanguage, infoKey) !== undefined) {
        fsElements.infoMessage.innerHTML = translate(infoKey, { html: true });
      } else {
        fsElements.infoMessage.textContent = translate('filesystem.messages.noDetail');
      }
    }

    fsPathMap = new Map();
    fsElements.tree.innerHTML = '';
    const items = Array.isArray(data?.tree) ? data.tree : [];
    if (!items.length) {
      const p = document.createElement('p');
      p.className = 'fs-empty';
      p.textContent = translate('filesystem.messages.noDetail');
      fsElements.tree.append(p);
      currentFsSelection = null;
      resetFsDetails();
      return;
    }

    const treeWithRoot = includeFsRootNode(items, data?.rootRaw || listRaw || null);
    fsElements.tree.append(buildFsTree(treeWithRoot));

    let targetPath = null;
    if (previousSelection && fsPathMap.has(previousSelection)) {
      targetPath = previousSelection;
    } else {
      const preferredNode = fsPathMap.get('/') || null;
      const defaultNode = preferredNode || findFirstSelectableNode(treeWithRoot);
      targetPath = defaultNode ? defaultNode.path : null;
    }

    if (targetPath) {
      selectFsPath(targetPath);
    } else {
      currentFsSelection = null;
      resetFsDetails();
    }
  };

  const selectFsPath = (path) => {
    if (!fsElements.tree) {
      return;
    }
    const node = fsPathMap.get(path);
    if (!node) {
      return;
    }
    const previous = fsElements.tree.querySelector('.fs-node.is-active');
    if (previous) {
      previous.classList.remove('is-active');
    }
    const currentButton = fsElements.tree.querySelector(`[data-fs-path=\"${CSS.escape(path)}\"]`);
    if (currentButton) {
      currentButton.classList.add('is-active');
    }
    currentFsSelection = path;
    updateFsDetail(node);
  };

  const normalizeFsDirPath = (value) => {
    if (!value || typeof value !== 'string') {
      return '/';
    }
    if (value === '/') {
      return '/';
    }
    const trimmed = value.replace(/\/+$/g, '');
    if (!trimmed) {
      return '/';
    }
    return trimmed.startsWith('/') ? trimmed : `/${trimmed}`;
  };

  const joinFsPath = (dirPath, childPath) => {
    const base = normalizeFsDirPath(dirPath);
    const child = (childPath || '').toString().replace(/^\/+/, '');
    if (!child) {
      return base;
    }
    if (base === '/') {
      return `/${child}`;
    }
    return `${base}/${child}`;
  };

  const parseFsLsOutput = (raw) => {
    const result = {
      path: '/',
      entries: []
    };
    if (!raw) {
      return result;
    }
    const sanitized = String(raw).replace(/\r/g, '');
    const lines = sanitized.split('\n');
    lines.forEach((line) => {
      const trimmed = line.trimEnd();
      if (!trimmed) {
        return;
      }
      if (!trimmed.startsWith('|')) {
        return;
      }
      const content = trimmed.replace(/^\|\s*/, '');
      if (!content) {
        return;
      }
      const headerMatch = content.match(/^storage:\s+([^\s]+)\s+path:\s*(.+)$/i);
      if (headerMatch) {
        const [, , path] = headerMatch;
        result.path = normalizeFsDirPath(path);
        return;
      }
      if (content === '(empty)') {
        return;
      }
      if (/^<DIR>\s+/i.test(content)) {
        const nameRaw = content.replace(/^<DIR>\s+/i, '');
        const baseName = nameRaw.split('/').filter(Boolean).pop() || nameRaw;
        const fullPath = normalizeFsDirPath(joinFsPath(result.path, nameRaw));
        result.entries.push({
          type: 'dir',
          name: baseName,
          path: fullPath,
          size: '--'
        });
        return;
      }
      if (/^file\s+/i.test(content)) {
        const nameRaw = content.replace(/^file\s+/i, '');
        const baseName = nameRaw.split('/').filter(Boolean).pop() || nameRaw;
        const fullPath = joinFsPath(result.path, nameRaw);
        result.entries.push({
          type: 'file',
          name: baseName,
          path: fullPath,
          size: '--'
        });
        return;
      }
      const fileMatch = content.match(/^(\d+)\s+(.+)$/);
      if (fileMatch) {
        const sizeBytes = Number(fileMatch[1]);
        const nameRaw = fileMatch[2];
        const baseName = nameRaw.split('/').filter(Boolean).pop() || nameRaw;
        const fullPath = joinFsPath(result.path, nameRaw);
        result.entries.push({
          type: 'file',
          name: baseName,
          path: fullPath,
          size: Number.isFinite(sizeBytes) ? `${sizeBytes} bytes` : '--'
        });
        return;
      }
      const fallbackName = content;
      const baseName = fallbackName.split('/').filter(Boolean).pop() || fallbackName;
      const fullPath = joinFsPath(result.path, fallbackName);
      result.entries.push({
        type: 'file',
        name: baseName,
        path: fullPath,
        size: '--'
      });
    });
    return result;
  };

  const fetchFsTree = async (storageId) => {
    const processed = new Set();
    const queue = ['/'];
    const directoryMap = new Map();
    const combinedOutputs = [];
    let rootRaw = '';

    while (queue.length) {
      const targetDir = queue.shift();
      if (processed.has(targetDir)) {
        continue;
      }
      processed.add(targetDir);
      const commandText = targetDir === '/' ? 'fs ls' : `fs ls ${quoteArgument(targetDir)}`;
      const raw = await runSerialCommand(commandText, {
        id: `fs-ls-${storageId || 'default'}-${targetDir.replace(/\//g, '_')}`
      });
      const trimmed = raw.trim();
      if (!/^OK\s+fs\s+ls/i.test(trimmed)) {
        const error = new Error(trimmed || `fs ls failed for ${targetDir}`);
        error.commandText = commandText;
        throw error;
      }
      if (!rootRaw) {
        rootRaw = trimmed;
      }
      combinedOutputs.push(trimmed);
      const parsed = parseFsLsOutput(trimmed);
      const dirPath = parsed.path;
      const dirKey = normalizeFsDirPath(dirPath);
      let dirNode = directoryMap.get(dirKey);
      if (!dirNode) {
        dirNode = {
          name: dirKey === '/' ? '/' : dirKey.split('/').filter(Boolean).pop() || dirKey,
          path: dirKey,
          type: 'dir',
          size: '--',
          children: []
        };
        directoryMap.set(dirKey, dirNode);
      }
      const children = parsed.entries.map((entry) => {
        if (entry.type === 'dir') {
          const childPath = normalizeFsDirPath(entry.path);
          let childNode = directoryMap.get(childPath);
          if (!childNode) {
            childNode = {
              name: entry.name,
              path: childPath,
              type: 'dir',
              size: '--',
              children: []
            };
            directoryMap.set(childPath, childNode);
          } else {
            childNode.name = entry.name;
          }
          if (!processed.has(childPath) && !queue.includes(childPath)) {
            queue.push(childPath);
          }
          return childNode;
        }
        return {
          name: entry.name,
          path: entry.path,
          type: 'file',
          size: entry.size || '--',
          cat: null,
          stat: null,
          b64read: null,
          hash: null
        };
      });
      dirNode.children = children;
    }

    const rootNode = directoryMap.get('/') || { children: [] };
    return {
      tree: Array.isArray(rootNode.children) ? rootNode.children : [],
      listRaw: combinedOutputs.join('\n\n'),
      rootRaw
    };
  };

  const updateFsActionsForNode = (node) => {
    const isDir = node?.type === 'dir';
    const canCreateDir = isDir && currentStorageId !== 'spiffs';
    if (fsElements.mkdirSection) {
      fsElements.mkdirSection.hidden = !canCreateDir;
    }
    if (fsElements.writeSection) {
      fsElements.writeSection.hidden = !isDir;
    }
    if (fsElements.b64writeSection) {
      fsElements.b64writeSection.hidden = !isDir;
    }
    if (!isDir) {
      if (fsElements.mkdirPathInput) {
        fsElements.mkdirPathInput.value = '';
      }
      if (fsElements.writePathInput) {
        fsElements.writePathInput.value = '';
        delete fsElements.writePathInput.dataset.basePath;
        fsElements.writePathInput.disabled = true;
        fsElements.writePathInput.setAttribute('disabled', '');
      }
      if (fsElements.writeContentInput) {
        fsElements.writeContentInput.value = '';
        fsElements.writeContentInput.disabled = true;
        fsElements.writeContentInput.setAttribute('disabled', '');
      }
      if (fsElements.writeRunButton) {
        fsElements.writeRunButton.disabled = true;
        fsElements.writeRunButton.setAttribute('disabled', '');
      }
      applyDisabledTitles();
      return;
    }
    const basePath = node?.path || '/';
    if (fsElements.mkdirPathInput) {
      const placeholder = translate('filesystem.actions.mkdir.pathPlaceholder') || '/logs';
      fsElements.mkdirPathInput.value = canCreateDir ? joinFsPath(basePath, placeholder) : '';
    }
    if (fsElements.writePathInput) {
      const placeholder = translate('filesystem.actions.write.pathPlaceholder') || 'notes.txt';
      fsElements.writePathInput.disabled = false;
      fsElements.writePathInput.removeAttribute('disabled');
      fsElements.writePathInput.dataset.basePath = basePath;
      fsElements.writePathInput.value = '';
      fsElements.writePathInput.setAttribute('placeholder', placeholder);
    }
    if (fsElements.writeContentInput) {
      fsElements.writeContentInput.disabled = false;
      fsElements.writeContentInput.removeAttribute('disabled');
      fsElements.writeContentInput.value = '';
    }
    if (fsElements.writeRunButton) {
      fsElements.writeRunButton.disabled = false;
      fsElements.writeRunButton.removeAttribute('disabled');
    }
    if (fsElements.b64writePathInput) {
      const placeholder = translate('filesystem.actions.b64write.pathPlaceholder') || '/image.bin';
      fsElements.b64writePathInput.value = joinFsPath(basePath, placeholder);
    }
    if (fsElements.b64writeChunkInput) {
      fsElements.b64writeChunkInput.value = '';
    }
    applyDisabledTitles();
  };

  const parseBase64Chunks = (raw) => {
    if (!raw) {
      return [];
    }
    const sanitized = String(raw).replace(/\r/g, '');
    const chunks = [];
    sanitized.split('\n').forEach((line) => {
      const match = line.match(/\|\s*data\[\d+\]:\s*(\S+)/i);
      if (match && match[1]) {
        chunks.push(match[1].trim());
      }
    });
    return chunks;
  };

  const decodeBase64Chunks = (chunks) => {
    if (!chunks || !chunks.length || typeof atob !== 'function') {
      return null;
    }
    const merged = chunks.join('');
    const binary = atob(merged);
    const bytes = new Uint8Array(binary.length);
    for (let i = 0; i < binary.length; i += 1) {
      bytes[i] = binary.charCodeAt(i);
    }
    return bytes;
  };

  const isImagePath = (path) => /\.(png|jpe?g|gif|bmp|webp|ico|svg)$/i.test(path || '');

  const getImageMimeType = (path) => {
    const lower = (path || '').toLowerCase();
    if (lower.endsWith('.png')) {
      return 'image/png';
    }
    if (lower.endsWith('.jpg') || lower.endsWith('.jpeg')) {
      return 'image/jpeg';
    }
    if (lower.endsWith('.gif')) {
      return 'image/gif';
    }
    if (lower.endsWith('.bmp')) {
      return 'image/bmp';
    }
    if (lower.endsWith('.webp')) {
      return 'image/webp';
    }
    if (lower.endsWith('.ico')) {
      return 'image/x-icon';
    }
    if (lower.endsWith('.svg')) {
      return 'image/svg+xml';
    }
    return 'application/octet-stream';
  };

  const extractCatPayload = (raw) => {
    if (!raw) {
      return null;
    }
    const sanitized = String(raw).replace(/\r/g, '');
    const lines = sanitized.split('\n');
    const payload = [];
    let started = false;
    lines.forEach((line) => {
      const trimmed = line.trimEnd();
      if (!trimmed.trimStart().startsWith('|')) {
        return;
      }
      const contentRaw = trimmed.trimStart().slice(1);
      const content = contentRaw.startsWith(' ') ? contentRaw.slice(1) : contentRaw;
      if (!started) {
        const looksLikeMeta = /^[A-Za-z0-9_-]+\s*:\s*/.test(content);
        if (looksLikeMeta) {
          return;
        }
        started = true;
      }
      payload.push(content);
    });
    if (!payload.length) {
      return null;
    }
    return payload.join('\n');
  };

  const isBinaryBuffer = (bytes) => {
    if (!bytes || !bytes.length) {
      return false;
    }
    let controlCount = 0;
    for (let i = 0; i < bytes.length; i += 1) {
      const value = bytes[i];
      if (value === 0) {
        return true;
      }
      if (value < 32 && value !== 9 && value !== 10 && value !== 13) {
        controlCount += 1;
      }
    }
    return controlCount / bytes.length > 0.2;
  };

  const decodeUtf8String = (bytes) => {
    if (!bytes) {
      return null;
    }
    try {
      const decoder = new TextDecoder('utf-8', { fatal: false });
      return decoder.decode(bytes);
    } catch {
      try {
        const fallbackDecoder = new TextDecoder();
        return fallbackDecoder.decode(bytes);
      } catch {
        return null;
      }
    }
  };

  const updateFsPreview = (node) => {
    if (!fsElements.previewSection) {
      return;
    }
    if (!node || node.type !== 'file') {
      fsElements.previewSection.hidden = true;
      if (fsElements.previewText) {
        fsElements.previewText.hidden = true;
        fsElements.previewText.textContent = '';
      }
      if (fsElements.previewImage) {
        fsElements.previewImage.hidden = true;
        fsElements.previewImage.removeAttribute('src');
        fsElements.previewImage.removeAttribute('alt');
      }
      if (fsElements.previewEmpty) {
        fsElements.previewEmpty.hidden = true;
      }
      return;
    }

    const base64Chunks = parseBase64Chunks(node.b64read);
    let previewKind = 'none';
    let previewText = '';
    let previewSrc = '';

    if (base64Chunks.length) {
      const bytes = decodeBase64Chunks(base64Chunks);
      if (bytes) {
        if (isImagePath(node.path)) {
          previewKind = 'image';
          previewSrc = `data:${getImageMimeType(node.path)};base64,${base64Chunks.join('')}`;
        } else if (!isBinaryBuffer(bytes)) {
          const decoded = decodeUtf8String(bytes);
          if (decoded !== null) {
            previewKind = 'text';
            previewText = decoded;
          }
        }
        if (previewKind === 'none') {
          previewKind = 'binary';
        }
      } else {
        previewKind = 'binary';
      }
    } else {
      const catPayload = extractCatPayload(node.cat);
      if (catPayload !== null) {
        previewKind = 'text';
        previewText = catPayload;
      }
    }

    fsElements.previewSection.hidden = false;

    if (previewKind === 'text') {
      if (fsElements.previewText) {
        fsElements.previewText.hidden = false;
        fsElements.previewText.textContent = previewText;
      }
      if (fsElements.previewImage) {
        fsElements.previewImage.hidden = true;
        fsElements.previewImage.removeAttribute('src');
        fsElements.previewImage.removeAttribute('alt');
      }
      if (fsElements.previewEmpty) {
        fsElements.previewEmpty.hidden = true;
      }
    } else if (previewKind === 'image') {
      if (fsElements.previewImage) {
        fsElements.previewImage.hidden = false;
        fsElements.previewImage.src = previewSrc;
        fsElements.previewImage.alt = node.path || 'preview';
      }
      if (fsElements.previewText) {
        fsElements.previewText.hidden = true;
        fsElements.previewText.textContent = '';
      }
      if (fsElements.previewEmpty) {
        fsElements.previewEmpty.hidden = true;
      }
    } else if (previewKind === 'binary') {
      if (fsElements.previewEmpty) {
        fsElements.previewEmpty.hidden = false;
        fsElements.previewEmpty.textContent = translate('filesystem.messages.previewBinary');
      }
      if (fsElements.previewText) {
        fsElements.previewText.hidden = true;
        fsElements.previewText.textContent = '';
      }
      if (fsElements.previewImage) {
        fsElements.previewImage.hidden = true;
        fsElements.previewImage.removeAttribute('src');
        fsElements.previewImage.removeAttribute('alt');
      }
    } else {
      if (fsElements.previewEmpty) {
        fsElements.previewEmpty.hidden = false;
        fsElements.previewEmpty.textContent = translate('filesystem.messages.previewPlaceholder');
      }
      if (fsElements.previewText) {
        fsElements.previewText.hidden = true;
        fsElements.previewText.textContent = '';
      }
      if (fsElements.previewImage) {
        fsElements.previewImage.hidden = true;
        fsElements.previewImage.removeAttribute('src');
        fsElements.previewImage.removeAttribute('alt');
      }
    }
  };

  const updateFsDetail = (node) => {
    if (!node) {
      resetFsDetails();
      return;
    }

    if (fsElements.selectedPath) {
      fsElements.selectedPath.textContent = node.path;
    }

    if (fsElements.detailTableBody) {
      const rows = [];
      rows.push({
        key: translate('filesystem.table.type'),
        value:
          node.type === 'dir'
            ? translate('filesystem.details.directory')
            : translate('filesystem.details.file')
      });
      rows.push({
        key: translate('filesystem.table.size'),
        value: node.size || '--'
      });
      rows.push({
        key: translate('filesystem.table.storage'),
        value: currentStorageId || '--'
      });
      if (node.type === 'dir' && node.children) {
        rows.push({
          key: translate('filesystem.table.children'),
          value: `${node.children.length}${translate('filesystem.details.childCountSuffix')}`
        });
      }
      if (fsElements.detailTable) {
        fsElements.detailTable.hidden = false;
      }
      fsElements.detailTableBody.innerHTML = '';
      rows.forEach(({ key, value }) => {
        const tr = document.createElement('tr');
        const th = document.createElement('th');
        th.textContent = key;
        const td = document.createElement('td');
        td.textContent = value;
        tr.append(th, td);
        fsElements.detailTableBody.append(tr);
      });
    }

    updateFsActionsForNode(node);

    const isDir = node.type === 'dir';

    if (isDir) {
      if (fsElements.dirSection) {
        fsElements.dirSection.hidden = true;
      }
      if (fsElements.dirRaw) {
        fsElements.dirRaw.textContent = translate('filesystem.messages.dirPlaceholder');
      }
      if (fsElements.catSection) {
        fsElements.catSection.hidden = true;
      }
      if (fsElements.catRaw) {
        fsElements.catRaw.textContent = translate('filesystem.messages.catPlaceholder');
      }
      if (fsElements.b64Section) {
        fsElements.b64Section.hidden = true;
      }
      if (fsElements.b64Raw) {
        fsElements.b64Raw.textContent = translate('filesystem.messages.b64Placeholder');
      }
      if (fsElements.hashSection) {
        fsElements.hashSection.hidden = true;
      }
      if (fsElements.hashRaw) {
        fsElements.hashRaw.textContent = translate('filesystem.messages.hashPlaceholder');
      }
      updateFsPreview(node);
      return;
    }

    if (fsElements.dirSection) {
      fsElements.dirSection.hidden = true;
    }

    if (fsElements.catSection) {
      fsElements.catSection.hidden = !node.cat;
    }
    if (fsElements.catRaw) {
      fsElements.catRaw.textContent = node.cat || translate('filesystem.messages.catPlaceholder');
    }

    if (fsElements.b64Section) {
      fsElements.b64Section.hidden = !node.b64read;
    }
    if (fsElements.b64Raw) {
      fsElements.b64Raw.textContent = node.b64read || translate('filesystem.messages.b64Placeholder');
    }

    if (fsElements.hashSection) {
      fsElements.hashSection.hidden = !node.hash;
    }
    if (fsElements.hashRaw) {
      fsElements.hashRaw.textContent = node.hash || translate('filesystem.messages.hashPlaceholder');
    }

    updateFsPreview(node);
  };

  const runFsAutoFetch = async ({ silent = false, userInitiated = false } = {}) => {
    if (!fsElements.tree) {
      return;
    }
    if (!currentStorageId) {
      clearFsView(translate('filesystem.messages.selectStorage'));
      updateFsRefreshButtonState();
      return;
    }

    if (!isSerialReady()) {
      if (userInitiated) {
        appendLogEntry('error', translate('connection.info.connectFirst'));
      }
      const sample = fsSamples[currentStorageId];
      if (sample) {
        const data = {
          storageId: currentStorageId,
          tree: sample.tree || [],
          listRaw: sample.listRaw || '',
          rootRaw: sample.listRaw || '',
          timestamp: formatTimeStamp(),
          infoKey: sample.noteKey || getFsHintKey(currentStorageId),
          isSample: true
        };
        lastFsData = data;
        renderFsView(data, { preserveSelection: false });
      } else {
        clearFsView(translate('filesystem.messages.selectStorage'));
      }
      updateFsRefreshButtonState();
      return;
    }

    if (fsFetching) {
      if (userInitiated) {
        appendLogEntry('info', translate('filesystem.list.refreshing'));
      }
      return;
    }

    const previousSelection = currentFsSelection;
    const previousData = lastFsData;
    fsFetching = true;
    updateFsRefreshButtonState();
    setFsPending();

    const targetStorage = currentStorageId;
    try {
      const result = await fetchFsTree(targetStorage);
      if (targetStorage !== currentStorageId) {
        return;
      }
      const data = {
        storageId: targetStorage,
        tree: result.tree,
        listRaw: result.listRaw,
        rootRaw: result.rootRaw,
        timestamp: formatTimeStamp(),
        infoKey: getFsHintKey(targetStorage),
        isSample: false
      };
      lastFsData = data;
      renderFsView(data, {
        preserveSelection: Boolean(previousSelection),
        preferredPath: previousSelection
      });
      if (!silent) {
        appendLogEntry('info', `UI: fs list refreshed (${targetStorage})`);
      }
    } catch (error) {
      if (targetStorage === currentStorageId) {
        appendLogEntry('error', error?.message || 'fs ls failed');
        if (previousData) {
          lastFsData = previousData;
          renderFsView(previousData, {
            preserveSelection: Boolean(previousSelection),
            preferredPath: previousSelection
          });
        } else {
          renderFsError('filesystem.messages.fetchFailed');
        }
      }
    } finally {
      fsFetching = false;
      updateFsRefreshButtonState();
    }
  };

  const refreshLanguageSensitiveUI = () => {
    renderStorageList(lastStorageListRaw || getStorageListRaw(currentStorageId || ''), currentStorageId || '');
    renderStorageStatus(lastStorageStatusRaw);
    renderConfigTable(configEntries);
    if (currentStorageId) {
      if (lastFsData && lastFsData.storageId === currentStorageId) {
        renderFsView(lastFsData);
      } else if (fsSamples[currentStorageId]) {
        const sample = fsSamples[currentStorageId];
        const data = {
          storageId: currentStorageId,
          tree: sample.tree || [],
          listRaw: sample.listRaw || '',
          rootRaw: sample.listRaw || '',
          timestamp: '--:--',
          infoKey: sample.noteKey || getFsHintKey(currentStorageId),
          isSample: true
        };
        lastFsData = data;
        renderFsView(data, { preserveSelection: false });
      } else {
        clearFsView(translate('filesystem.messages.selectStorage'));
      }
    } else {
      clearFsView(translate('filesystem.messages.selectStorage'));
    }

    if (helpElements.output) {
      if (helpOutputState === 'content') {
        applyHelpOutput(lastHelpOutputRaw);
      } else if (helpOutputState === 'pending') {
        applyHelpOutput(translate('results.pending'));
      } else if (helpOutputState === 'error') {
        if (helpOutputErrorKey) {
          applyHelpOutput(translate(helpOutputErrorKey));
        } else {
          applyHelpOutput(lastHelpOutputRaw || translate('results.placeholder'));
        }
      } else {
        applyHelpOutput(translate('commands.help.help.placeholder'));
      }
    }
    updateFsRefreshButtonState();
  };

  const COMMAND_TIMEOUT_MS = 8000;
  const statusClassMap = {
    disconnected: 'status-pill--disconnected',
    connecting: 'status-pill--connecting',
    disconnecting: 'status-pill--disconnecting',
    connected: 'status-pill--connected'
  };
  const connectButtonClasses = [
    'btn-state-idle',
    'btn-state-connecting',
    'btn-state-disconnecting',
    'btn-state-connected',
    'btn-state-unsupported'
  ];
  const textEncoder = new TextEncoder();
  const textDecoder = new TextDecoder();
  let serialPort = null;
  let serialWriter = null;
  let serialReader = null;
  let activeCommand = null;
  let logLineBuffer = '';
  let isDisconnecting = false;
  let pendingCleanupPromise = null;
  let readLoopPromise = null;
  let pendingPortClosePromise = null;

  const isSerialReady = () => connectionState === 'connected' && Boolean(serialWriter);

  const updateFsRefreshButtonState = () => {
    if (!fsElements.listRefreshButton) {
      return;
    }
    const labelKey = fsFetching ? 'filesystem.list.refreshing' : 'filesystem.list.refresh';
    fsElements.listRefreshButton.textContent = translate(labelKey);
    const shouldDisable = !isSerialReady() || !currentStorageId || fsFetching;
    if (shouldDisable) {
      fsElements.listRefreshButton.disabled = true;
      fsElements.listRefreshButton.setAttribute('disabled', '');
    } else {
      fsElements.listRefreshButton.disabled = false;
      fsElements.listRefreshButton.removeAttribute('disabled');
    }
    applyDisabledTitles();
  };

  refreshConnectionLabel = () => {
    if (!statusLabel) {
      return;
    }
    const key =
      connectionState === 'unsupported'
        ? 'connection.status.unsupported'
        : `connection.status.${connectionState}`;
    const fallbackKey = 'connection.status.disconnected';
    statusLabel.dataset.i18n = key;
    const message =
      getTranslationValue(currentLanguage, key) ||
      getTranslationValue(currentLanguage, fallbackKey) ||
      connectionState;
    statusLabel.textContent = message;
    if (connectionState === 'unsupported' && connectButton) {
      connectButton.setAttribute('title', translate('connection.info.unsupportedHint'));
    }
  };

  const updateCommandButtonsState = () => {
    const shouldDisable = connectionState !== 'connected' || Boolean(activeCommand);
    commandButtons.forEach((button) => {
      if (shouldDisable) {
        button.classList.add('btn--inactive');
        button.setAttribute('aria-disabled', 'true');
        const titleKey =
          connectionState === 'disconnecting'
            ? 'connection.info.waitDisconnect'
            : 'connection.info.connectFirst';
        button.setAttribute('title', translate(titleKey));
      } else {
        button.classList.remove('btn--inactive');
        button.removeAttribute('aria-disabled');
        button.removeAttribute('title');
      }
    });
  };

  const setConnectionState = (state) => {
    if (connectionState === state) {
      refreshConnectionLabel();
      updateCommandButtonsState();
      applyDisabledTitles();
      return;
    }
    connectionState = state;
    if (statusPill) {
      Object.values(statusClassMap).forEach((cls) => statusPill.classList.remove(cls));
      const pillarClass = statusClassMap[state] || statusClassMap.disconnected;
      statusPill.classList.add(pillarClass);
    }
    if (connectButton) {
      connectButton.classList.remove(...connectButtonClasses);
      let classToApply = 'btn-state-idle';
      if (state === 'connecting') {
        classToApply = 'btn-state-connecting';
      } else if (state === 'disconnecting') {
        classToApply = 'btn-state-disconnecting';
      } else if (state === 'connected') {
        classToApply = 'btn-state-connected';
      } else if (state === 'unsupported') {
        classToApply = 'btn-state-unsupported';
      }
      connectButton.classList.add(classToApply);

      if (state === 'unsupported') {
        connectButton.disabled = true;
        connectButton.setAttribute('disabled', '');
        connectButton.setAttribute('title', translate('connection.info.unsupportedHint'));
      } else if (state === 'connected' || state === 'connecting' || state === 'disconnecting') {
        connectButton.disabled = true;
        connectButton.setAttribute('disabled', '');
        if (state === 'connecting') {
          connectButton.setAttribute('title', translate('connection.status.connecting'));
        } else if (state === 'disconnecting') {
          connectButton.setAttribute('title', translate('connection.info.waitDisconnect'));
        } else {
          connectButton.setAttribute('title', translate('connection.info.disconnectFirst'));
        }
      } else {
        connectButton.disabled = false;
        connectButton.removeAttribute('disabled');
        connectButton.removeAttribute('title');
      }
    }
    if (disconnectButton) {
      if (state !== 'connected') {
        disconnectButton.disabled = true;
        disconnectButton.setAttribute('disabled', '');
      } else {
        disconnectButton.disabled = false;
        disconnectButton.removeAttribute('disabled');
      }
    }
    updateCommandButtonsState();
    updateConfigControlsState();
    refreshConnectionLabel();
    applyDisabledTitles();
    updateFsRefreshButtonState();
    if (state === 'connected') {
      fetchHelpCommandList().catch(() => {
        /* handled via log */
      });
      if (isTabSupported('config')) {
        ensureConfigInitialized();
        if (configNeedsInitialFetch) {
          refreshConfigList({ silent: true }).catch(() => {
            /* handled via log */
          });
        } else {
          renderConfigTable(configEntries);
        }
      } else {
        configEntries = [];
        configNeedsInitialFetch = true;
        renderConfigTable([]);
      }
      if (currentTab === 'system' || currentTab === 'wifi') {
        triggerActiveAutoCommand(currentTab);
      }
      if (currentTab === 'help') {
        const activeHelpPanel = document.querySelector('#tab-help .command-panel.is-active');
        const commandId = activeHelpPanel?.dataset.command;
        if (commandId && helpAutoIds.has(commandId)) {
          sendSystemCommand(commandId, { panel: activeHelpPanel });
        }
      }
      if (currentTab === 'storage') {
        runStorageAutoFetch().catch(() => {
          /* handled via log */
        });
      }
      if (currentTab === 'config') {
        ensureConfigInitialized();
        refreshConfigList({ silent: true }).catch(() => {
          /* handled via log */
        });
      }
    } else {
      resetAutoCommandQueue();
    }
  };

  const clearLogPlaceholder = () => {
    if (!logOutput) {
      return;
    }
    if (logOutput.dataset.hasPlaceholder === 'false') {
      return;
    }
    const placeholder = logOutput.querySelector('[data-i18n="commands.help.log.placeholder"]');
    if (placeholder) {
      placeholder.remove();
    }
    logOutput.dataset.hasPlaceholder = 'false';
  };

  const appendLogEntry = (type, message) => {
    if (!logOutput || !message) {
      return;
    }
    clearLogPlaceholder();
    const entry = document.createElement('div');
    entry.className = `log-entry log-entry--${type}`;
    entry.textContent = `[${formatTimeStamp()}] ${message}`;
    logOutput.append(entry);
    logOutput.scrollTop = logOutput.scrollHeight;
  };

  const processLogChunk = (chunk) => {
    if (!logOutput) {
      return;
    }
    logLineBuffer += chunk;
    let newlineIndex = logLineBuffer.indexOf('\n');
    while (newlineIndex !== -1) {
      let line = logLineBuffer.slice(0, newlineIndex);
      logLineBuffer = logLineBuffer.slice(newlineIndex + 1);
      line = line.replace(/\r$/, '');
      if (line.trim().length) {
        appendLogEntry('rx', `>> ${line}`);
      }
      newlineIndex = logLineBuffer.indexOf('\n');
    }
  };

  const sanitizeSerialText = (value) => (value || '').replace(/\r/g, '');

  const hasPromptLine = (buffer) => {
    const sanitized = sanitizeSerialText(buffer);
    if (!sanitized) {
      return false;
    }
    const lines = sanitized.split('\n');
    for (let i = lines.length - 1; i >= 0; i -= 1) {
      const candidate = lines[i].trim();
      if (candidate === '') {
        continue;
      }
      return candidate.startsWith('>');
    }
    return false;
  };

  const updateCommandPanelRaw = (command) => {
    if (!command) {
      return;
    }
    const sanitized = sanitizeSerialText(command.buffer);
    if (command.panel) {
      const rawEl = command.panel.querySelector('[data-result-raw]');
      if (rawEl) {
        rawEl.textContent = sanitized;
      }
    }
    if (typeof command.onUpdate === 'function') {
      command.onUpdate(sanitized);
    }
  };

  const renderSystemResponse = (panel, rawText) => {
    if (!panel) {
      return;
    }
    const normalized = sanitizeSerialText(rawText || '').trim();
    const timestampEl = panel.querySelector('[data-result-timestamp]');
    if (timestampEl) {
      timestampEl.textContent = formatTimeStamp();
    }
    const rawEl = panel.querySelector('[data-result-raw]');
    if (rawEl) {
      rawEl.textContent = normalized || translate('results.placeholder');
    }
    const tableEl = panel.querySelector('[data-result-table]');
    const tableBody = tableEl ? tableEl.querySelector('tbody') : null;
    if (tableEl && tableBody) {
      tableBody.innerHTML = '';
      const rows = parsePipeTable(normalized);
      if (rows.length) {
        rows.forEach(({ key, value }) => {
          const row = document.createElement('tr');
          const keyCell = document.createElement('th');
          keyCell.textContent = key;
          const valueCell = document.createElement('td');
          valueCell.textContent = value;
          row.append(keyCell, valueCell);
          tableBody.append(row);
        });
        tableEl.hidden = false;
      } else {
        tableEl.hidden = true;
      }
    }
  };

  const finalizeActiveCommand = ({ error = false, fallbackMessage = '' } = {}) => {
    if (!activeCommand) {
      return;
    }
    const {
      timeoutId,
      panel,
      commandText,
      resolve: resolveCommand,
      reject: rejectCommand,
      onFinalize
    } = activeCommand;
    if (timeoutId) {
      clearTimeout(timeoutId);
    }
    const buffer = sanitizeSerialText(activeCommand.buffer).trim();
    const output = buffer || fallbackMessage || (error ? 'Command aborted.' : '');
    if (error) {
      appendLogEntry('error', `Command failed: ${commandText}`);
    } else {
      appendLogEntry('info', `Command completed: ${commandText}`);
    }
    if (panel) {
      renderSystemResponse(panel, output);
    }
    if (typeof onFinalize === 'function') {
      try {
        onFinalize({ output, error, command: activeCommand });
      } catch (callbackError) {
        appendLogEntry('error', `Finalize handler error: ${callbackError.message}`);
      }
    }
    activeCommand = null;
    updateCommandButtonsState();
    if (error) {
      resetAutoCommandQueue();
      if (typeof rejectCommand === 'function') {
        rejectCommand(new Error(output));
      }
    } else {
      processAutoCommandQueue();
      if (typeof resolveCommand === 'function') {
        resolveCommand(output);
      }
    }
  };

  const preparePanelForCommand = (panel) => {
    if (!panel) {
      return;
    }
    const timestampEl = panel.querySelector('[data-result-timestamp]');
    if (timestampEl) {
      timestampEl.textContent = formatTimeStamp();
    }
    const rawEl = panel.querySelector('[data-result-raw]');
    if (rawEl) {
      rawEl.textContent = translate('results.pending');
    }
    const tableEl = panel.querySelector('[data-result-table]');
    const tableBody = tableEl ? tableEl.querySelector('tbody') : null;
    if (tableBody) {
      tableBody.innerHTML = '';
    }
    if (tableEl) {
      tableEl.hidden = true;
    }
  };

  const handleSerialChunk = (chunk) => {
    if (!chunk) {
      return;
    }
    processLogChunk(chunk);
    if (activeCommand) {
      activeCommand.buffer += chunk;
      updateCommandPanelRaw(activeCommand);
      if (hasPromptLine(activeCommand.buffer)) {
        finalizeActiveCommand();
      }
    }
  };

  const startReadLoop = () => {
    if (!serialPort?.readable || serialReader) {
      return;
    }
    const reader = serialPort.readable.getReader();
    serialReader = reader;
    readLoopPromise = (async () => {
      try {
        while (true) {
          if (serialReader !== reader) {
            break;
          }
          const { value, done } = await reader.read();
          if (done) {
            const remaining = textDecoder.decode();
            if (remaining) {
              handleSerialChunk(remaining);
            }
            break;
          }
          if (value) {
            const chunk = textDecoder.decode(value, { stream: true });
            handleSerialChunk(chunk);
          }
        }
      } catch (error) {
        appendLogEntry('error', `Read error: ${error.message}`);
      } finally {
        try {
          reader.releaseLock();
        } catch {
          /* ignore */
        }
        if (serialReader === reader) {
          serialReader = null;
        }
        readLoopPromise = null;
        if (!isDisconnecting && connectionState === 'connected') {
          appendLogEntry('error', 'Serial connection closed unexpectedly.');
          disconnectSerial();
        }
      }
    })();
  };

  const cleanupSerial = () => {
    if (!pendingCleanupPromise) {
      pendingCleanupPromise = (async () => {
        const reader = serialReader;
        if (reader) {
          appendLogEntry('debug', 'Cancelling serial reader...');
          reader
            .cancel()
            .then(() => appendLogEntry('debug', 'Reader cancel resolved.'))
            .catch((error) => appendLogEntry('error', `Reader cancel error: ${error.message}`))
            .finally(() => appendLogEntry('debug', 'Reader cancel promise settled.'));
          try {
            reader.releaseLock();
            appendLogEntry('debug', 'Reader lock released (cleanup).');
          } catch {
            appendLogEntry('debug', 'Reader release in cleanup skipped.');
          }
        } else {
          appendLogEntry('debug', 'No active reader to cancel.');
        }

        if (readLoopPromise) {
          appendLogEntry('debug', 'Awaiting read loop termination...');
          try {
            await readLoopPromise;
            appendLogEntry('debug', 'Read loop terminated.');
          } catch (error) {
            appendLogEntry('error', `Read loop error: ${error.message}`);
          }
        } else {
          appendLogEntry('debug', 'No pending read loop promise.');
        }
        readLoopPromise = null;
        serialReader = null;

        const writer = serialWriter;
        serialWriter = null;
        if (writer) {
          appendLogEntry('debug', 'Closing writer...');
          try {
            await writer.close();
            appendLogEntry('debug', 'Writer close resolved.');
          } catch (error) {
            appendLogEntry('error', `Writer close error: ${error.message}`);
          }
          try {
            writer.releaseLock();
            appendLogEntry('debug', 'Writer lock released.');
          } catch {
            /* ignore */
          }
        } else {
          appendLogEntry('debug', 'No active writer to release.');
        }

        const port = serialPort;
        serialPort = null;
        if (port) {
          appendLogEntry('debug', 'Preparing to close serial port...');
          if (typeof port.setSignals === 'function') {
            appendLogEntry('debug', 'Lowering control signals (DTR/RTS)...');
            try {
              await port.setSignals({ dataTerminalReady: false, requestToSend: false });
              appendLogEntry('debug', 'Signals lowered.');
            } catch (error) {
              appendLogEntry('error', `setSignals error: ${error.message}`);
            }
            try {
              await port.setSignals({ break: false });
            } catch (error) {
              appendLogEntry('error', `setSignals (break) error: ${error.message}`);
            }
            await new Promise((resolve) => setTimeout(resolve, 200));
          }

          appendLogEntry('debug', 'Closing serial port...');
          let closeTaskWrapper = null;
          try {
            const closeTask = port.close();
            appendLogEntry('debug', 'Serial port close promise created.');
            closeTaskWrapper = closeTask
              .then(() => appendLogEntry('debug', 'Serial port closed.'))
              .catch((error) => {
                appendLogEntry('error', `Port close error: ${error.message}`);
                throw error;
              })
              .finally(() => {
                appendLogEntry('debug', 'Serial port close promise settled.');
                if (pendingPortClosePromise === closeTaskWrapper) {
                  pendingPortClosePromise = null;
                }
              });
            pendingPortClosePromise = closeTaskWrapper;
            appendLogEntry('debug', 'Waiting for serial port close to resolve...');
            await closeTaskWrapper;
            appendLogEntry('debug', 'Serial port close awaited.');
          } catch (error) {
            appendLogEntry('error', `Port close exception: ${error.message}`);
            pendingPortClosePromise = null;
          }
        } else {
          appendLogEntry('debug', 'No serial port instance to close.');
        }
      })().finally(() => {
        appendLogEntry('debug', 'Cleanup promise resolved.');
        pendingCleanupPromise = null;
      });
    }
    return pendingCleanupPromise;
  };

  const disconnectSerial = async () => {
    if (connectionState === 'unsupported') {
      return;
    }
    if (connectionState === 'disconnected') {
      appendLogEntry('info', 'Serial device already disconnected.');
      return;
    }
    if (connectionState === 'disconnecting' && isDisconnecting) {
      appendLogEntry('info', translate('connection.info.waitDisconnect'));
      return;
    }
    isDisconnecting = true;
    appendLogEntry('info', 'Disconnecting from serial device...');
    setConnectionState('disconnecting');
    try {
      if (activeCommand) {
        finalizeActiveCommand({ error: true, fallbackMessage: 'Connection closed.' });
      }
      await cleanupSerial();
    } catch (error) {
      appendLogEntry('error', `Disconnect error: ${error.message}`);
    } finally {
      setConnectionState('disconnected');
      appendLogEntry('info', 'Serial device disconnected.');
      isDisconnecting = false;
    }
  };

  const connectSerial = async () => {
    if (!('serial' in navigator)) {
      setConnectionState('unsupported');
      appendLogEntry('error', translate('connection.info.unsupportedHint'));
      return;
    }
    if (isDisconnecting || connectionState === 'disconnecting') {
      appendLogEntry('info', translate('connection.info.waitDisconnect'));
      return;
    }
    if (pendingCleanupPromise) {
      appendLogEntry('info', translate('connection.info.waitDisconnect'));
      try {
        await pendingCleanupPromise;
      } catch (error) {
        appendLogEntry('error', `Cleanup error: ${error.message}`);
      }
    }
    if (pendingPortClosePromise) {
      appendLogEntry('debug', 'Waiting for serial port close to finish...');
      try {
        await pendingPortClosePromise;
      } catch (error) {
        appendLogEntry('error', `Pending port close error: ${error.message}`);
      } finally {
        pendingPortClosePromise = null;
      }
    }
    if (connectionState === 'connected' || connectionState === 'connecting') {
      return;
    }
    try {
      setConnectionState('connecting');
      appendLogEntry('info', 'Requesting serial device...');
      serialPort = await navigator.serial.requestPort();
      await serialPort.open({ baudRate: 115200 });
      if (serialPort.writable) {
        serialWriter = serialPort.writable.getWriter();
      }
      setConnectionState('connected');
      appendLogEntry('info', 'Serial device connected.');
      updateCommandButtonsState();
      startReadLoop();
    } catch (error) {
      appendLogEntry('error', `Connection failed: ${error.message}`);
      await cleanupSerial();
      setConnectionState('disconnected');
    }
  };

  const runSerialCommand = (commandText, options = {}) => {
    if (!isSerialReady()) {
      const message =
        connectionState === 'connected'
          ? translate('connection.info.connectFirst')
          : translate('connection.info.connectFirst');
      appendLogEntry('error', message);
      return Promise.reject(new Error(message));
    }
    if (activeCommand) {
      const error = new Error('Another command is already in progress.');
      appendLogEntry('info', error.message);
      return Promise.reject(error);
    }

    const {
      id = commandText,
      panel = null,
      button = null,
      onUpdate = null,
      onFinalize = null,
      onStart = null,
      timeoutMs = COMMAND_TIMEOUT_MS
    } = options;

    if (panel) {
      preparePanelForCommand(panel);
    }
    if (typeof onStart === 'function') {
      try {
        onStart();
      } catch (error) {
        appendLogEntry('error', `Command setup error: ${error.message}`);
      }
    }

    return new Promise((resolve, reject) => {
      activeCommand = {
        id,
        commandText,
        panel,
        button,
        buffer: '',
        timeoutId: null,
        onUpdate,
        onFinalize,
        resolve,
        reject
      };
      updateCommandButtonsState();
      activeCommand.timeoutId = window.setTimeout(() => {
        if (!activeCommand) {
          return;
        }
        appendLogEntry('error', 'Timeout waiting for device prompt.');
        finalizeActiveCommand({
          error: true,
          fallbackMessage: 'Timeout waiting for device prompt.'
        });
      }, timeoutMs);
      appendLogEntry('tx', `<< ${commandText}`);
      serialWriter
        .write(textEncoder.encode(`${commandText}\n`))
        .catch((error) => {
          appendLogEntry('error', `Write failed: ${error.message}`);
          finalizeActiveCommand({ error: true, fallbackMessage: error.message });
        });
    });
  };

  const sendSystemCommand = (commandId, options = {}) => {
    const entry = commandPanels.get(commandId);
    if (!entry) {
      return;
    }
    const { commandText: overrideText, button: overrideButton, onFinalize } = options;
    const commandText = overrideText || commandId.replace(/-/g, ' ');
    const targetButton = overrideButton || entry.button;
    runSerialCommand(commandText, {
      id: commandId,
      panel: entry.panel,
      button: targetButton,
      onFinalize: (result) => {
        if (!result?.error) {
          if (commandId === 'sys-time') {
            updateSysTimeInputFromOutput(result.output);
          } else if (commandId === 'sys-timezone') {
            updateSysTimezoneInputFromOutput(result.output);
          }
        }
        if (typeof onFinalize === 'function') {
          onFinalize(result);
        }
      }
    }).catch(() => {
      /* handled via log */
    });
  };

  const quoteArgument = (value) => {
    if (value == null || value === '') {
      return '""';
    }
    const needsQuotes = /[\s"\\]/.test(value);
    if (!needsQuotes) {
      return value;
    }
    return `"${value.replace(/(["\\])/g, '\\$1')}"`;
  };

  const handleFsWriteRun = () => {
    if (connectionState !== 'connected') {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    if (!currentStorageId) {
      appendLogEntry('error', translate('filesystem.messages.selectStorage'));
      return;
    }
    const nameInput = fsElements.writePathInput;
    const contentInput = fsElements.writeContentInput;
    if (!nameInput || !contentInput) {
      return;
    }
    const selectionNode = currentFsSelection ? fsPathMap.get(currentFsSelection) : null;
    const basePath = selectionNode?.type === 'dir' ? selectionNode.path : nameInput.dataset.basePath;
    if (!basePath || selectionNode?.type !== 'dir') {
      appendLogEntry('error', translate('filesystem.messages.noDetail'));
      return;
    }
    const rawName = nameInput.value.trim();
    const sanitizedName = rawName.replace(/^\/+/, '');
    if (!sanitizedName) {
      appendLogEntry('error', translate('filesystem.actions.write.errors.nameRequired'));
      nameInput.focus();
      return;
    }
    const targetPath = joinFsPath(basePath, sanitizedName);
    const payload = contentInput.value || '';
    appendLogEntry('debug', `UI: fs write -> ${targetPath}`);
    runSerialCommand(`fs write ${quoteArgument(targetPath)} ${quoteArgument(payload)}`, {
      id: `fs-write-${targetPath}`,
      onFinalize: ({ error }) => {
        if (!error) {
          nameInput.value = '';
          nameInput.focus();
          Promise.resolve(runFsAutoFetch({ silent: true })).catch(() => {
            /* handled via log */
          });
        }
      }
    })
      .catch(() => {
        /* handled via log */
      });
  };

  const handleFsListRefresh = () => {
    runFsAutoFetch({ userInitiated: true }).catch(() => {
      /* handled via log */
    });
  };

  const fillSysTimeWithBrowserNow = () => {
    if (!sysTimeInput) {
      return;
    }
    sysTimeInput.value = formatDateTimeLocalForInput(new Date());
    sysTimeInput.focus();
  };

  const fillSysTimezoneWithBrowser = () => {
    if (!sysTimezoneInput) {
      return;
    }
    sysTimezoneInput.value = computeBrowserTimezoneString();
    sysTimezoneInput.focus();
  };

  const handleSysTimeSet = () => {
    if (connectionState !== 'connected') {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    const value = (sysTimeInput?.value || '').trim();
    if (!value) {
      appendLogEntry('error', translate('commands.system.sys-time.errors.required'));
      return;
    }
    if (!/^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}$/.test(value)) {
      appendLogEntry('error', translate('commands.system.sys-time.errors.invalid'));
      return;
    }
    appendLogEntry('debug', `UI: sys time set -> ${value}`);
    sendSystemCommand('sys-time', {
      commandText: `sys time ${value}`,
      button: sysTimeSetButton
    });
  };

  const handleSysTimezoneSet = () => {
    if (connectionState !== 'connected') {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    const value = (sysTimezoneInput?.value || '').trim();
    if (!value) {
      appendLogEntry('error', translate('commands.system.sys-timezone.errors.required'));
      return;
    }
    appendLogEntry('debug', `UI: sys timezone set -> ${value}`);
    sendSystemCommand('sys-timezone', {
      commandText: `sys timezone ${value}`,
      button: sysTimezoneSetButton
    });
  };

  const handleWifiAutoRun = () => {
    if (connectionState !== 'connected') {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    const mode = wifiAutoSelect?.value === 'off' ? 'off' : 'on';
    appendLogEntry('debug', `UI: wifi auto -> ${mode}`);
    sendSystemCommand('wifi-auto', {
      commandText: `wifi auto ${mode}`,
      button: wifiAutoRunButton
    });
  };

  const handleWifiAddRun = () => {
    if (connectionState !== 'connected') {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    const ssid = (wifiAddSsidInput?.value || '').trim();
    if (!ssid) {
      appendLogEntry('error', translate('commands.wifi.add.errors.ssidRequired'));
      return;
    }
    const keyRaw = wifiAddKeyInput ? wifiAddKeyInput.value : '';
    appendLogEntry('debug', `UI: wifi add -> ${ssid}`);
    sendSystemCommand('wifi-add', {
      commandText: `wifi add ${quoteArgument(ssid)} ${quoteArgument(keyRaw)}`,
      button: wifiAddRunButton
    });
  };

  const handleWifiDelRun = () => {
    if (connectionState !== 'connected') {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    const raw = wifiDelIndexInput ? wifiDelIndexInput.value : '';
    if (!raw || Number.isNaN(Number(raw))) {
      appendLogEntry('error', translate('commands.wifi.del.errors.indexRequired'));
      return;
    }
    const index = Number(raw);
    if (!Number.isInteger(index) || index < 0) {
      appendLogEntry('error', translate('commands.wifi.del.errors.indexRequired'));
      return;
    }
    appendLogEntry('debug', `UI: wifi del -> ${index}`);
    sendSystemCommand('wifi-del', {
      commandText: `wifi del ${index}`,
      button: wifiDelRunButton
    });
  };

  const handleWifiConnectRun = () => {
    if (connectionState !== 'connected') {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    const ssid = (wifiConnectSsidInput?.value || '').trim();
    const keyRaw = wifiConnectKeyInput ? wifiConnectKeyInput.value : '';
    if (!ssid && keyRaw) {
      appendLogEntry('error', translate('commands.wifi.connect.errors.keyWithoutSsid'));
      return;
    }
    let commandText = 'wifi connect';
    if (ssid) {
      const keyArg = keyRaw ? quoteArgument(keyRaw) : '""';
      commandText += ` ${quoteArgument(ssid)} ${keyArg}`;
    }
    appendLogEntry('debug', `UI: wifi connect -> ${ssid || '(stored list)'}`);
    sendSystemCommand('wifi-connect', {
      commandText,
      button: wifiConnectRunButton
    });
  };

  const handleNtpAutoRun = () => {
    if (connectionState !== 'connected') {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    const mode = ntpAutoSelect?.value === 'off' ? 'off' : 'on';
    appendLogEntry('debug', `UI: ntp auto -> ${mode}`);
    sendSystemCommand('ntp-auto', {
      commandText: `ntp auto ${mode}`,
      button: ntpAutoRunButton
    });
  };

  const handleNtpSetRun = () => {
    if (connectionState !== 'connected') {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    const servers = ntpSetServerInputs.map((input) => (input ? input.value.trim() : ''));
    if (!servers[0]) {
      appendLogEntry('error', translate('commands.wifi.ntp.set.errors.serverRequired'));
      return;
    }
    const args = servers.filter((value, index) => value || index === 0).map((value) => quoteArgument(value));
    appendLogEntry('debug', `UI: ntp set -> ${args.join(' ')}`);
    sendSystemCommand('ntp-set', {
      commandText: `ntp set ${args.join(' ')}`,
      button: ntpSetRunButton
    });
  };

  const handleNtpEnableRun = () => {
    if (connectionState !== 'connected') {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    appendLogEntry('debug', 'UI: ntp enable');
    sendSystemCommand('ntp-enable', {
      commandText: 'ntp enable',
      button: ntpEnableRunButton
    });
  };

  const handleNtpDisableRun = () => {
    if (connectionState !== 'connected') {
      appendLogEntry('error', translate('connection.info.connectFirst'));
      return;
    }
    appendLogEntry('debug', 'UI: ntp disable');
    sendSystemCommand('ntp-disable', {
      commandText: 'ntp disable',
      button: ntpDisableRunButton
    });
  };

  const attachCommandButtonHandler = (button, handler) => {
    if (!button || typeof handler !== 'function') {
      return;
    }
    button.addEventListener(
      'click',
      (event) => {
        event.preventDefault();
        event.stopPropagation();
        event.stopImmediatePropagation();
        handler();
      },
      { capture: true }
    );
  };

  attachCommandButtonHandler(sysTimeUseBrowserButton, fillSysTimeWithBrowserNow);
  attachCommandButtonHandler(sysTimeSetButton, handleSysTimeSet);
  attachCommandButtonHandler(sysTimezoneUseBrowserButton, fillSysTimezoneWithBrowser);
  attachCommandButtonHandler(sysTimezoneSetButton, handleSysTimezoneSet);
  attachCommandButtonHandler(wifiAutoRunButton, handleWifiAutoRun);
  attachCommandButtonHandler(wifiAddRunButton, handleWifiAddRun);
  attachCommandButtonHandler(wifiDelRunButton, handleWifiDelRun);
  attachCommandButtonHandler(wifiConnectRunButton, handleWifiConnectRun);
  attachCommandButtonHandler(ntpAutoRunButton, handleNtpAutoRun);
  attachCommandButtonHandler(ntpSetRunButton, handleNtpSetRun);
  attachCommandButtonHandler(ntpEnableRunButton, handleNtpEnableRun);
  attachCommandButtonHandler(ntpDisableRunButton, handleNtpDisableRun);
  attachCommandButtonHandler(fsElements.listRefreshButton, handleFsListRefresh);
  attachCommandButtonHandler(fsElements.writeRunButton, handleFsWriteRun);

  commandPanels.forEach(({ button }, commandId) => {
    if (!button) {
      return;
    }
    button.addEventListener('click', () => {
      if (connectionState !== 'connected') {
        appendLogEntry('error', translate('connection.info.connectFirst'));
        return;
      }
      sendSystemCommand(commandId);
    });
  });

  if (connectButton) {
    connectButton.addEventListener('click', connectSerial);
  }
  if (disconnectButton) {
    disconnectButton.addEventListener('click', disconnectSerial);
  }

  if (languageSelect) {
    languageSelect.addEventListener('change', (event) => {
      setLanguage(event.target.value);
    });
  }

  const initialLanguage = resolveInitialLanguage();
  setLanguage(initialLanguage, { persist: false });
  if (languageSelect) {
    languageSelect.value = currentLanguage;
  }

  if (!('serial' in navigator)) {
    setConnectionState('unsupported');
    appendLogEntry('error', translate('connection.info.unsupportedHint'));
  } else {
    if (connectButton) {
      connectButton.disabled = false;
      connectButton.removeAttribute('disabled');
    }
    setConnectionState('disconnected');
  }

  updateCommandButtonsState();
});
