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
          "disconnected": "未接続"
        },
        "actions": {
          "connect": "デバイスに接続",
          "disconnect": "切断"
        },
        "info": {
          "disabledTitle": "UI プレビューのため現在は操作できません",
          "placeholderStatus": "準備完了 - 接続待機"
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
            "desc": "sys info / uptime / reset"
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
            "desc": "i2c / gpio / adc / pwm / rgb"
          },
          "help": {
            "label": "ヘルプ",
            "desc": "help / ?"
          }
        }
      },
      "sections": {
        "system": {
          "title": "システムコマンド",
          "description": "ESP32 の動作状態の確認やシステム制御に関する標準コマンドとユーザー定義コマンドをまとめています。",
          "ariaTablist": "システムコマンド"
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
          "description": "I2C / GPIO / ADC / PWM / RGB LED の制御を行います。"
        },
        "help": {
          "title": "ヘルプとログ",
          "description": "コマンド一覧の参照や応答ログの確認に利用します。"
        }
      },
      "results": {
        "latest": "最新結果 (自動取得)",
        "latestManual": "最新結果",
        "placeholder": "結果がここに表示されます"
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
            "bullets": [
              "チップ情報の即時確認に利用",
              "サポート問い合わせ時の環境共有",
              "ビルドメタデータの検証"
            ],
            "action": "sys info を送信"
          },
          "sys-uptime": {
            "description": "起動からの経過時間を hh:mm:ss とミリ秒で表示します。",
            "action": "sys uptime を送信"
          },
          "sys-time": {
            "description": "現在時刻を ISO 8601 形式で取得します。タイムゾーンや RTC の同期確認に使用します。",
            "action": "sys time を送信"
          },
          "sys-mem": {
            "description": "ヒープおよび PSRAM の総量・空き・最小値・最大ブロックを確認します。",
            "action": "sys mem を送信"
          },
          "sys-reset": {
            "description": "ESP32 をソフトリセットします。実行前に確認ステップを挟む想定です。",
            "bullets": [
              "確認ダイアログでオペレーターの意図を確認",
              "必要であればログを保存",
              "ESP.restart() を呼び出して再起動"
            ],
            "action": "sys reset を送信"
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
        "hints": {
          "spiffs": "SPIFFS ではディレクトリが利用できないため、すべてのファイルは <code>/</code> 直下に作成されます。",
          "littlefs": "LittleFS はディレクトリ階層を利用できます。必要に応じてルート以外のパスも取得します。"
        },
        "messages": {
          "selectStorage": "ストレージタブで対象を選択してください。",
          "noSample": "このストレージのサンプルデータはまだ用意されていません。",
          "loading": "ストレージを選択...",
          "noDetail": "ファイルまたはディレクトリを選択すると詳細が表示されます。",
          "dirPlaceholder": "ディレクトリの内容がここに表示されます",
          "catPlaceholder": "ファイル内容がここに表示されます",
          "b64Placeholder": "Base64 形式の読み出し結果がここに表示されます",
          "hashPlaceholder": "ハッシュ結果がここに表示されます"
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
            "pathLabel": "パス",
            "pathPlaceholder": "/notes.txt",
            "contentLabel": "内容 (短いテキスト向け)",
            "contentPlaceholder": "fs write /notes.txt \"hello\"",
            "action": "fs write を送信"
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
        "labels": {
          "address": "アドレス (0x--)",
          "length": "読み取りバイト数 (省略可)",
          "payload": "送信データ (スペース区切り 0x)",
          "mode": "モード",
          "value": "値",
          "duty": "デューティ (0-4095 または %)",
          "frequency": "周波数 (Hz)",
          "channel": "チャネル",
          "atten": "減衰モード",
          "sampleRate": "サンプル (ms/BPS)",
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
          "disconnected": "Disconnected"
        },
        "actions": {
          "connect": "Connect Device",
          "disconnect": "Disconnect"
        },
        "info": {
          "disabledTitle": "Preview only – controls are disabled",
          "placeholderStatus": "Ready – waiting for connection"
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
            "desc": "sys info / uptime / reset"
          },
          "storage": {
            "label": "Storage",
            "desc": "storage list / use / status"
          },
          "filesystem": {
            "label": "Filesystem",
            "desc": "fs ls / write / hash"
          },
          "peripherals": {
            "label": "Peripherals",
            "desc": "i2c / gpio / adc / pwm / rgb"
          },
          "help": {
            "label": "Help",
            "desc": "help / ?"
          }
        }
      },
      "sections": {
        "system": {
          "title": "System Commands",
          "description": "Collection of standard and user-defined commands for monitoring and controlling ESP32 system state.",
          "ariaTablist": "System Commands"
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
          "description": "Control I2C, GPIO, ADC, PWM, and RGB LED peripherals."
        },
        "help": {
          "title": "Help & Logs",
          "description": "Review command references and inspect command output logs."
        }
      },
      "results": {
        "latest": "Latest Result (auto)",
        "latestManual": "Latest Result",
        "placeholder": "Results will appear here"
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
            "bullets": [
              "Quickly confirm chip information",
              "Share environment details for support requests",
              "Validate build metadata"
            ],
            "action": "Send sys info"
          },
          "sys-uptime": {
            "description": "Show uptime in hh:mm:ss and milliseconds.",
            "action": "Send sys uptime"
          },
          "sys-time": {
            "description": "Fetch current time in ISO 8601 format to verify timezone and RTC sync.",
            "action": "Send sys time"
          },
          "sys-mem": {
            "description": "Inspect heap and PSRAM totals, free space, minimum, and largest block.",
            "action": "Send sys mem"
          },
          "sys-reset": {
            "description": "Soft reset the ESP32. Add a confirmation step before executing.",
            "bullets": [
              "Confirm operator intent with a dialog",
              "Save logs if necessary",
              "Call ESP.restart() to reboot"
            ],
            "action": "Send sys reset"
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
        "hints": {
          "spiffs": "SPIFFS does not support directories; all files are created under <code>/</code>.",
          "littlefs": "LittleFS supports directory hierarchies and can fetch non-root paths when needed."
        },
        "messages": {
          "selectStorage": "Select a storage target from the storage tab.",
          "noSample": "Sample data for this storage has not been prepared yet.",
          "loading": "Select a storage...",
          "noDetail": "Select a file or directory to view details.",
          "dirPlaceholder": "Directory contents will appear here.",
          "catPlaceholder": "File contents will appear here.",
          "b64Placeholder": "Base64 read results will appear here.",
          "hashPlaceholder": "Hash results will appear here."
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
            "pathLabel": "Path",
            "pathPlaceholder": "/notes.txt",
            "contentLabel": "Content (for short text)",
            "contentPlaceholder": "fs write /notes.txt \"hello\"",
            "action": "Send fs write"
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
        "labels": {
          "address": "Address (0x--)",
          "length": "Read length (optional)",
          "payload": "Payload (space-separated 0x)",
          "mode": "Mode",
          "value": "Value",
          "duty": "Duty (0-4095 or %)",
          "frequency": "Frequency (Hz)",
          "channel": "Channel",
          "atten": "Attenuation",
          "sampleRate": "Sample interval (ms/BPS)",
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
          "disconnected": "未连接"
        },
        "actions": {
          "connect": "连接设备",
          "disconnect": "断开连接"
        },
        "info": {
          "disabledTitle": "预览模式，控件已禁用",
          "placeholderStatus": "已就绪 - 等待连接"
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
            "desc": "sys info / uptime / reset"
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
            "desc": "i2c / gpio / adc / pwm / rgb"
          },
          "help": {
            "label": "帮助",
            "desc": "help / ?"
          }
        }
      },
      "sections": {
        "system": {
          "title": "系统指令",
          "description": "汇集用于监控 ESP32 状态与执行系统控制的标准指令和用户自定义指令。",
          "ariaTablist": "系统指令"
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
          "description": "控制 I2C、GPIO、ADC、PWM 与 RGB LED 等外设。"
        },
        "help": {
          "title": "帮助与日志",
          "description": "用于查看指令列表和响应日志。"
        }
      },
      "results": {
        "latest": "最新结果（自动）",
        "latestManual": "最新结果",
        "placeholder": "结果将在此显示"
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
            "bullets": [
              "快速确认芯片信息",
              "在支持请求时共享环境信息",
              "校验构建元数据"
            ],
            "action": "发送 sys info"
          },
          "sys-uptime": {
            "description": "以 hh:mm:ss 和毫秒显示开机后经过的时间。",
            "action": "发送 sys uptime"
          },
          "sys-time": {
            "description": "以 ISO 8601 格式获取当前时间，用于确认时区与 RTC 同步。",
            "action": "发送 sys time"
          },
          "sys-mem": {
            "description": "查看堆与 PSRAM 的总量、空闲、最小值以及最大块。",
            "action": "发送 sys mem"
          },
          "sys-reset": {
            "description": "对 ESP32 执行软重启，执行前请添加确认步骤。",
            "bullets": [
              "通过确认对话框核对操作意图",
              "必要时保存日志",
              "调用 ESP.restart() 重启"
            ],
            "action": "发送 sys reset"
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
        "hints": {
          "spiffs": "SPIFFS 不支持目录，所有文件都会创建在 <code>/</code> 根目录下。",
          "littlefs": "LittleFS 支持目录层级，可根据需要获取非根路径。"
        },
        "messages": {
          "selectStorage": "请在存储选项卡中选择目标。",
          "noSample": "该存储的示例数据尚未准备。",
          "loading": "请选择存储...",
          "noDetail": "选择文件或目录以查看详细信息。",
          "dirPlaceholder": "目录内容将在此显示。",
          "catPlaceholder": "文件内容将在此显示。",
          "b64Placeholder": "Base64 读取结果将在此显示。",
          "hashPlaceholder": "哈希结果将在此显示。"
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
            "pathLabel": "路径",
            "pathPlaceholder": "/notes.txt",
            "contentLabel": "内容（适用于短文本）",
            "contentPlaceholder": "fs write /notes.txt \"hello\"",
            "action": "发送 fs write"
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
        "labels": {
          "address": "地址 (0x--)",
          "length": "读取字节数（可选）",
          "payload": "发送数据（空格分隔 0x）",
          "mode": "模式",
          "value": "值",
          "duty": "占空比 (0-4095 或 %)",
          "frequency": "频率 (Hz)",
          "channel": "通道",
          "atten": "衰减模式",
          "sampleRate": "采样间隔 (ms/BPS)",
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
    hashRaw: document.querySelector('[data-fs-hash-raw]')
  };

  const tabButtons = document.querySelectorAll('.tab-button');
  const tabPanels = document.querySelectorAll('.tab-content');

  let storageInitialized = false;
  let currentStorageId = null;
  let currentTab = 'system';
  let currentFsSelection = null;
  let fsPathMap = new Map();
  let lastStorageStatusRaw = storageSamples.statusNone;

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
    currentTab = targetId;
    tabButtons.forEach((button) => {
      const isActive = button.dataset.tab === targetId;
      button.classList.toggle('is-active', isActive);
      button.setAttribute('aria-selected', isActive ? 'true' : 'false');
    });

    tabPanels.forEach((panel) => {
      const isActive = panel.id === `tab-${targetId}`;
      panel.classList.toggle('is-active', isActive);
    });

    if (targetId === 'storage') {
      runStorageAutoFetch();
    }
    if (targetId === 'filesystem') {
      runFsAutoFetch();
    }
  };

  tabButtons.forEach((button) => {
    button.addEventListener('click', () => {
      activateTab(button.dataset.tab);
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
| MAC: 88:AE:6F:34:E3:EC
| IDF: v5.5.1-255-g07e9bf4970
| Arduino Core: 3_3_2
| Build: Oct 21 2025 15:43:05`,
    'sys-uptime': `OK sys uptime
| Uptime: 0:23:28 (1408444 ms)`,
    'sys-time': `OK sys time
| localtime: 1970-01-01T00:23:32+00:00`,
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
| Task Stack High Water: 3952 words`
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

  const runStorageAutoFetch = () => {
    renderStorageList(getStorageListRaw(currentStorageId), currentStorageId || '');
    renderStorageStatus(lastStorageStatusRaw);

    if (!storageInitialized && storageElements.select) {
      storageElements.select.addEventListener('change', (event) => {
        if (!event.target.value) {
          return;
        }
        handleStorageSelection(event.target.value);
      });
      storageInitialized = true;
    }
  };

  const handleStorageSelection = (storageId) => {
    if (!storageId) {
      return;
    }
    currentStorageId = storageId;
    currentFsSelection = null;
    renderStorageList(getStorageListRaw(storageId), storageId);
    const useRaw = `OK storage use\n| current: ${storageId}`;
    const statusRaw = storageSamples.statusById[storageId] || storageSamples.statusNone;
    const combinedStatusRaw = `${useRaw}\n${statusRaw}`;
    lastStorageStatusRaw = combinedStatusRaw;
    renderStorageStatus(combinedStatusRaw);
    runFsAutoFetch();
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
    currentFsSelection = null;
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
      button.textContent = item.type === 'dir' ? `${item.name}/` : item.name;
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

    if (fsElements.dirSection) {
      fsElements.dirSection.hidden = !(node.type === 'dir' && node.lsRaw);
    }
    if (fsElements.dirRaw && node.lsRaw) {
      fsElements.dirRaw.textContent = node.lsRaw;
    }

    if (fsElements.catSection) {
      fsElements.catSection.hidden = !(node.type === 'file' && node.cat);
    }
    if (fsElements.catRaw && node.cat) {
      fsElements.catRaw.textContent = node.cat;
    }

    if (fsElements.b64Section) {
      fsElements.b64Section.hidden = !(node.type === 'file' && node.b64read);
    }
    if (fsElements.b64Raw && node.b64read) {
      fsElements.b64Raw.textContent = node.b64read;
    }

    if (fsElements.hashSection) {
      fsElements.hashSection.hidden = !(node.type === 'file' && node.hash);
    }
    if (fsElements.hashRaw && node.hash) {
      fsElements.hashRaw.textContent = node.hash;
    }
  };

  const runFsAutoFetch = () => {
    if (!fsElements.tree) {
      return;
    }
    if (!currentStorageId) {
      clearFsView(translate('filesystem.messages.selectStorage'));
      return;
    }
    const fsData = fsSamples[currentStorageId];
    if (!fsData) {
      clearFsView(translate('filesystem.messages.noSample'));
      return;
    }

    if (fsElements.infoMessage) {
      fsElements.infoMessage.innerHTML = translate(fsData.noteKey, { html: true });
    }
    if (fsElements.listTimestamp) {
      fsElements.listTimestamp.textContent = formatTimeStamp();
    }
    if (fsElements.listRaw) {
      fsElements.listRaw.textContent = fsData.listRaw;
    }

    fsPathMap = new Map();
    fsElements.tree.innerHTML = '';
    fsElements.tree.append(buildFsTree(fsData.tree));
    currentFsSelection = null;

    const defaultNode = findFirstSelectableNode(fsData.tree);
    if (defaultNode) {
      selectFsPath(defaultNode.path);
    } else {
      resetFsDetails();
    }
  };

  const refreshLanguageSensitiveUI = () => {
    renderStorageList(getStorageListRaw(currentStorageId || ''), currentStorageId || '');
    renderStorageStatus(lastStorageStatusRaw);
    if (currentStorageId && fsSamples[currentStorageId]) {
      const fsData = fsSamples[currentStorageId];
      if (fsElements.infoMessage) {
        fsElements.infoMessage.innerHTML = translate(fsData.noteKey, { html: true });
      }
      if (currentFsSelection && fsPathMap.size) {
        const node = fsPathMap.get(currentFsSelection);
        if (node) {
          updateFsDetail(node);
        } else {
          resetFsDetails();
        }
      } else {
        resetFsDetails();
      }
    } else {
      clearFsView(translate('filesystem.messages.selectStorage'));
    }
  };

  const connectButton = document.querySelector('#connect-button');
  const disconnectButton = document.querySelector('#disconnect-button');
  const statusLabel = document.querySelector('#connection-status-label');

  const disabledElements = [
    connectButton,
    disconnectButton,
    ...document.querySelectorAll(
      'button[disabled], select[disabled], input[disabled], textarea[disabled]'
    )
  ].filter(Boolean);

  const applyDisabledTitles = () => {
    disabledElements.forEach((element) => {
      element.setAttribute('title', translate('connection.info.disabledTitle'));
    });
  };

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

  if (statusLabel) {
    statusLabel.textContent = translate('connection.info.placeholderStatus');
  }
});
