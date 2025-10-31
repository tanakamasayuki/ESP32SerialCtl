#pragma once

#include <Arduino.h>
#include <esp_system.h>
#include <esp_chip_info.h>
#include <esp_heap_caps.h>
#include <driver/gpio.h>
#include <hal/gpio_types.h>
#include <esp32-hal-ledc.h>
#include <core_version.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <FS.h>
#include <mbedtls/base64.h>
#include <mbedtls/md5.h>
#include <mbedtls/sha256.h>
#if !defined(mbedtls_sha256_starts_ret)
#define mbedtls_sha256_starts_ret mbedtls_sha256_starts
#endif
#if !defined(mbedtls_sha256_update_ret)
#define mbedtls_sha256_update_ret mbedtls_sha256_update
#endif
#if !defined(mbedtls_sha256_finish_ret)
#define mbedtls_sha256_finish_ret mbedtls_sha256_finish
#endif
#if !defined(mbedtls_md5_starts_ret)
#define mbedtls_md5_starts_ret mbedtls_md5_starts
#endif
#if !defined(mbedtls_md5_update_ret)
#define mbedtls_md5_update_ret mbedtls_md5_update
#endif
#if !defined(mbedtls_md5_finish_ret)
#define mbedtls_md5_finish_ret mbedtls_md5_finish
#endif
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if !defined(_WIN32)
#include <sys/time.h>
#endif
#include <new>
#include <soc/soc.h>
#include <soc/gpio_reg.h>

#if defined(ESP32SERIALCTL_ENABLE_WIFI) || defined(WiFi_h) || defined(_WIFI_H_) || \
    defined(_WIFI_H) || defined(WiFiClient_h) || defined(_WIFICLIENT_H_) ||        \
    defined(WiFiServer_h) || defined(_WIFISERVER_H_) || defined(WIFI_STA)
#include <WiFi.h>
#include <WiFiMulti.h>
#include <esp_sntp.h>
#define ESP32SERIALCTL_HAS_WIFI 1
#endif

#if !defined(ESP32SERIALCTL_WIFI_MAX_NETWORKS)
#define ESP32SERIALCTL_WIFI_MAX_NETWORKS 8
#endif

#if !defined(ESP32SERIALCTL_WIFI_CONNECT_TIMEOUT_MS)
#define ESP32SERIALCTL_WIFI_CONNECT_TIMEOUT_MS 10000UL
#endif

#if !defined(ESP32SERIALCTL_NTP_MAX_SERVERS)
#define ESP32SERIALCTL_NTP_MAX_SERVERS 3
#endif

#if !defined(ESP32SERIALCTL_DEFAULT_NTP_SERVER)
#define ESP32SERIALCTL_DEFAULT_NTP_SERVER "pool.ntp.org"
#endif

#if !defined(ESP32SERIALCTL_DEFAULT_TIMEZONE)
#define ESP32SERIALCTL_DEFAULT_TIMEZONE ""
#endif

#if !defined(ESP32SERIALCTL_DISABLE_PREFERENCES)
#include <Preferences.h>
#define ESP32SERIALCTL_HAS_PREFERENCES 1
#endif

#if defined(ESP32SERIALCTL_ENABLE_I2C) || defined(Wire_h) || defined(_WIRE_H_) || \
    defined(_WIRE_H) || defined(_TWOWIRE_H_) || defined(TwoWire_h) ||             \
    defined(_WIRELIB_H_)
#include <Wire.h>
#define ESP32SERIALCTL_HAS_WIRE 1
#endif

#if defined(ESP32SERIALCTL_HAS_WIRE)
#if !defined(ESP32SERIALCTL_HAS_WIRE1) &&           \
    ((defined(SOC_I2C_NUM) && (SOC_I2C_NUM > 1)) || \
     (defined(SOC_HP_I2C_NUM) && (SOC_HP_I2C_NUM > 1)))
#define ESP32SERIALCTL_HAS_WIRE1 1
#endif
#if !defined(ESP32SERIALCTL_HAS_WIRE2) && (defined(SOC_I2C_NUM) && (SOC_I2C_NUM > 2))
#define ESP32SERIALCTL_HAS_WIRE2 1
#endif
#endif

#if defined(ESP32SERIALCTL_ENABLE_SD)
#include <SD.h>
#define ESP32SERIALCTL_HAS_SD 1
#endif
#if !defined(ESP32SERIALCTL_HAS_SD)
#if defined(SD_H) || defined(SD_h) || defined(_SD_H_) || defined(_SD_H)
#define ESP32SERIALCTL_HAS_SD 1
#endif
#endif

#if defined(ESP32SERIALCTL_ENABLE_SPIFFS)
#include <SPIFFS.h>
#define ESP32SERIALCTL_HAS_SPIFFS 1
#endif
#if !defined(ESP32SERIALCTL_HAS_SPIFFS)
#if defined(SPIFFS_H) || defined(SPIFFS_h) || defined(_SPIFFS_H_) || \
    defined(_SPIFFS_H)
#define ESP32SERIALCTL_HAS_SPIFFS 1
#endif
#endif

#if defined(ESP32SERIALCTL_ENABLE_LITTLEFS)
#include <LittleFS.h>
#define ESP32SERIALCTL_HAS_LITTLEFS 1
#endif
#if !defined(ESP32SERIALCTL_HAS_LITTLEFS)
#if defined(LITTLEFS_H) || defined(LITTLEFS_h) || defined(_LITTLEFS_H_) || \
    defined(_LITTLEFS_H)
#define ESP32SERIALCTL_HAS_LITTLEFS 1
#endif
#endif

#if defined(ESP32SERIALCTL_ENABLE_FFAT) || defined(ESP32SERIALCTL_ENABLE_FATFS)
#include <FFat.h>
#define ESP32SERIALCTL_HAS_FFAT 1
#endif
#if !defined(ESP32SERIALCTL_HAS_FFAT)
#if defined(FFATFS_H) || defined(_FFATFS_H_) || defined(FFatFS_H) || \
    defined(FFatFS_h) || defined(FFAT_H) || defined(_FFAT_H_) ||     \
    defined(FFat_h)
#define ESP32SERIALCTL_HAS_FFAT 1
#endif
#endif

#if defined(ESP32SERIALCTL_HAS_SD) || defined(ESP32SERIALCTL_HAS_SPIFFS) || \
    defined(ESP32SERIALCTL_HAS_LITTLEFS) || defined(ESP32SERIALCTL_HAS_FFAT)
#define ESP32SERIALCTL_HAS_STORAGE 1
#endif

namespace esp32serialctl
{

  inline constexpr const char kPrefsNamespace[] = "serial_ctl";
  inline constexpr const char kPrefsTimezoneKey[] = "tz";
  inline constexpr const char kPrefsDefaultTimezone[] = ESP32SERIALCTL_DEFAULT_TIMEZONE;
#if defined(ESP32SERIALCTL_HAS_WIFI) && defined(ESP32SERIALCTL_HAS_PREFERENCES)
  inline constexpr const char kPrefsWifiAutoKey[] = "wifi_auto";
  inline constexpr const char kPrefsWifiSsidFmt[] = "wifi%u_ssid";
  inline constexpr const char kPrefsWifiKeyFmt[] = "wifi%u_key";
  inline constexpr const char kPrefsWifiLegacySsidFmt[] = "ap%u_ssid";
  inline constexpr const char kPrefsWifiLegacyKeyFmt[] = "ap%u_pass";
  inline constexpr const char kPrefsNtpAutoKey[] = "ntp_auto";
  inline constexpr const char kPrefsNtpEnabledKey[] = "ntp_enabled";
  inline constexpr const char kPrefsNtpServerFmt[] = "ntp%u_srv";
#endif

  enum class NumberUnit : uint8_t
  {
    None = 0,
    TimeMilliseconds,
    FrequencyHz,
    Percent
  };

  struct ParsedNumber
  {
    int64_t value;
    NumberUnit unit;
  };

#if !defined(ESP32SERIALCTL_CONFIG_MAX_LOCALES)
#define ESP32SERIALCTL_CONFIG_MAX_LOCALES 8
#endif

  // 汎用の多言語説明型（Config や Command 両方で使えるように名称を変更）
  struct LocalizedText
  {
    const char *lang;
    const char *description;
  };

#if !defined(ESP32SERIALCTL_CMD_ARG_MAX)
#define ESP32SERIALCTL_CMD_ARG_MAX 8
#endif

  // 引数仕様
  struct CmdArgSpec
  {
    const char *name; // 引数名
    const char *type; // 表示用型名（"int"/"string" 等）
    bool required;    // 必須か
    const char *hint; // 補助説明
  };

  // ユーザー定義ハンドラ型（CommandEntry が保持する）
  using CmdHandlerFn = int (*)(const char **argv, size_t argc, void *ctx);

  // CommandEntry: 設定と同様に静的配列で定義できることを想定
  struct CommandEntry
  {
    const char *name;
    LocalizedText descriptions[ESP32SERIALCTL_CONFIG_MAX_LOCALES];
    CmdArgSpec args[ESP32SERIALCTL_CMD_ARG_MAX];
    CmdHandlerFn handler;
  };

  struct ConfigEntry
  {
    const char *name;
    const char *defaultValue;
    LocalizedText descriptions[ESP32SERIALCTL_CONFIG_MAX_LOCALES];
  };

  inline constexpr const char kPrefsConfigDefaultNamespace[] = "serial_ctl_cfg";

  template <size_t MaxLineLength = 128, size_t MaxTokens = 16>
  class SerialCtl
  {
  public:
    class Context;

    using Handler = void (*)(Context &);

    struct Command
    {
      const char *const group;
      const char *const name;
      Handler handler;
      const char *const help;
    };

    struct Argument
    {
      const char *value;
      size_t length;
      bool quoted;

      const char *c_str() const { return value ? value : ""; }
      size_t size() const { return length; }
      bool empty() const { return length == 0; }
      bool isQuoted() const { return quoted; }
      bool equals(const char *text) const
      {
        return SerialCtl::equalsIgnoreCase(c_str(), text);
      }
      bool toBool(bool &out) const { return SerialCtl::parseBoolean(c_str(), out); }
      bool toNumber(ParsedNumber &out) const
      {
        return SerialCtl::parseNumber(c_str(), out);
      }
    };

    struct Option
    {
      const char *name;
      const char *value;

      bool equals(const char *text) const
      {
        return SerialCtl::equalsIgnoreCase(name, text);
      }
      const char *c_str() const { return value ? value : ""; }
      bool hasValue() const { return value && *value; }
      bool valueEquals(const char *text) const
      {
        return value && SerialCtl::equalsIgnoreCase(value, text);
      }
      bool toBool(bool &out) const
      {
        return value ? SerialCtl::parseBoolean(value, out) : false;
      }
      bool toNumber(ParsedNumber &out) const
      {
        return value ? SerialCtl::parseNumber(value, out) : false;
      }
    };

    class Context
    {
    public:
      SerialCtl &controller() const { return *owner_; }
      Stream &input() const { return owner_->input_; }
      Print &output() const { return owner_->output_; }
      const Command &command() const { return *command_; }
      size_t argc() const { return argc_; }
      const Argument &arg(size_t index) const
      {
        static const Argument empty_arg = {"", 0u, false};
        return index < argc_ ? args_[index] : empty_arg;
      }
      size_t optionCount() const { return optionCount_; }
      const Option &option(size_t index) const
      {
        static const Option empty_opt = {"", nullptr};
        return index < optionCount_ ? options_[index] : empty_opt;
      }
      const Option *findOption(const char *name) const
      {
        if (!name)
        {
          return nullptr;
        }
        for (size_t i = 0; i < optionCount_; ++i)
        {
          if (SerialCtl::equalsIgnoreCase(options_[i].name, name))
          {
            return &options_[i];
          }
        }
        return nullptr;
      }
      bool hasOption(const char *name) const
      {
        return findOption(name) != nullptr;
      }
      const char *optionValue(const char *name,
                              const char *fallback = nullptr) const
      {
        const Option *opt = findOption(name);
        return (opt && opt->value) ? opt->value : fallback;
      }
      void printOK(const char *message = nullptr) const
      {
        owner_->emitOK(message);
      }
      void printError(uint16_t code, const char *reason) const
      {
        owner_->emitError(code, reason);
      }
      void printList(const char *text) const { owner_->emitList(text); }
      void printBody(const char *text) const { owner_->emitBody(text); }

    private:
      friend class SerialCtl;
      SerialCtl *owner_ = nullptr;
      const Command *command_ = nullptr;
      const Argument *args_ = nullptr;
      size_t argc_ = 0;
      const Option *options_ = nullptr;
      size_t optionCount_ = 0;
    };

    SerialCtl(Stream &io, const Command *commands, size_t commandCount)
        : input_(io), output_(io), commands_(commands),
          commandCount_(commandCount)
    {
      resetLine();
      promptPending_ = true;
    }

    SerialCtl(Stream &input, Print &output, const Command *commands,
              size_t commandCount)
        : input_(input), output_(output), commands_(commands),
          commandCount_(commandCount)
    {
      resetLine();
      promptPending_ = true;
    }

    void service()
    {
      ensureInitialized();
      while (input_.available() > 0)
      {
        int raw = input_.read();
        if (raw < 0)
        {
          break;
        }
        feed(static_cast<char>(raw));
      }
      flushPromptIfNeeded();
    }

    void ensureInitialized()
    {
      if (initialized_)
      {
        return;
      }
      initialized_ = true;
      runInitialSetup();
    }

    void runInitialSetup()
    {
#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
      applyStoredTimezone();
#endif
#if defined(ESP32SERIALCTL_HAS_WIFI) && defined(ESP32SERIALCTL_HAS_PREFERENCES)
      wifiMaybeAutoConnect();
      ntpMaybeAutoStart();
#endif
    }

#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
    void applyStoredTimezone()
    {
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, true))
      {
        return;
      }
      String timezone = prefs.getString(kPrefsTimezoneKey, kPrefsDefaultTimezone);
      prefs.end();
      timezoneCache_ = timezone;
      if (timezoneCache_.length() == 0 && strlen(kPrefsDefaultTimezone) > 0)
      {
        timezoneCache_ = kPrefsDefaultTimezone;
      }
      if (timezoneCache_.length() > 0)
      {
        applyTimezoneEnv(timezoneCache_.c_str());
      }
    }

    bool saveTimezone(const char *tz)
    {
      if (!tz || !*tz)
      {
        return false;
      }
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, false))
      {
        return false;
      }
      const size_t expected = strlen(tz);
      const size_t written = prefs.putString(kPrefsTimezoneKey, tz);
      prefs.end();
      const bool ok = written == expected;
      if (ok)
      {
        timezoneCache_ = tz;
#if defined(ESP32SERIALCTL_HAS_WIFI) && defined(ESP32SERIALCTL_HAS_PREFERENCES)
        ntpReconfigureIfNeeded();
#endif
      }
      return ok;
    }

    static bool applyTimezoneEnv(const char *tz)
    {
      if (!tz || !*tz)
      {
        return false;
      }
      if (setenv("TZ", tz, 1) != 0)
      {
        return false;
      }
      tzset();
      return true;
    }
#endif

#if defined(ESP32SERIALCTL_HAS_WIFI)
    struct WifiStoredNetwork
    {
      String ssid;
      String key;
      size_t slot;
    };

    WiFiMulti wifiMulti;

    static const char *wifiStatusToString(wl_status_t status)
    {
      switch (status)
      {
      case WL_NO_SHIELD:
        return "no_shield";
      case WL_IDLE_STATUS:
        return "idle";
      case WL_NO_SSID_AVAIL:
        return "no_ssid";
      case WL_SCAN_COMPLETED:
        return "scan_completed";
      case WL_CONNECTED:
        return "connected";
      case WL_CONNECT_FAILED:
        return "connect_failed";
      case WL_CONNECTION_LOST:
        return "connection_lost";
      case WL_DISCONNECTED:
        return "disconnected";
#if defined(WL_CONNECTION_LOST_SILENT)
      case WL_CONNECTION_LOST_SILENT:
        return "connection_lost_silent";
#endif
      default:
        return "unknown";
      }
    }

    void wifiConfigureStationMode(bool autoReconnect = true)
    {
      WiFi.mode(WIFI_STA);
      WiFi.setAutoReconnect(autoReconnect);
    }

    void wifiResetMulti()
    {
      wifiMulti = WiFiMulti();
    }

    bool wifiAttemptConnection(uint32_t timeoutMs)
    {
      const unsigned long deadline = millis() + timeoutMs;
      wl_status_t status = WiFi.status();
      while (status != WL_CONNECTED && (timeoutMs == 0UL || (long)(deadline - millis()) > 0))
      {
        wifiMulti.run();
        delay(100);
        status = WiFi.status();
      }
      return status == WL_CONNECTED;
    }

#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
    static void wifiFormatKey(char *buffer, size_t size, size_t slot, const char *fmt)
    {
      if (!buffer || size == 0 || !fmt)
      {
        return;
      }
      snprintf(buffer, size, fmt, static_cast<unsigned>(slot));
    }

    bool wifiReadNetworkSlot(size_t slot, String &ssid, String &key) const
    {
      ssid = "";
      key = "";
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, true))
      {
        return false;
      }
      char keyBuffer[16];
      wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiSsidFmt);
      ssid = prefs.getString(keyBuffer, "");
      if (ssid.length() == 0)
      {
        wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiLegacySsidFmt);
        ssid = prefs.getString(keyBuffer, "");
      }
      wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiKeyFmt);
      key = prefs.getString(keyBuffer, "");
      if (key.length() == 0)
      {
        wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiLegacyKeyFmt);
        key = prefs.getString(keyBuffer, "");
      }
      prefs.end();
      return ssid.length() > 0;
    }

    size_t wifiLoadStoredNetworks(WifiStoredNetwork (&out)[ESP32SERIALCTL_WIFI_MAX_NETWORKS]) const
    {
      size_t count = 0;
      for (size_t slot = 0; slot < ESP32SERIALCTL_WIFI_MAX_NETWORKS; ++slot)
      {
        if (count >= ESP32SERIALCTL_WIFI_MAX_NETWORKS)
        {
          break;
        }
        String ssid;
        String key;
        if (!wifiReadNetworkSlot(slot, ssid, key))
        {
          continue;
        }
        out[count].ssid = ssid;
        out[count].key = key;
        out[count].slot = slot;
        ++count;
      }
      return count;
    }

    bool wifiStoreNetwork(size_t slot, const char *ssid, const char *key)
    {
      if (!ssid)
      {
        return false;
      }
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, false))
      {
        return false;
      }
      char keyBuffer[16];
      wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiSsidFmt);
      const size_t ssidLen = strlen(ssid);
      bool ok = prefs.putString(keyBuffer, ssid) == ssidLen;
      if (ok)
      {
        wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiKeyFmt);
        const size_t keyLen = key ? strlen(key) : 0;
        ok = prefs.putString(keyBuffer, key ? key : "") == keyLen;
      }
      if (ok)
      {
        wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiLegacySsidFmt);
        prefs.putString(keyBuffer, "");
        wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiLegacyKeyFmt);
        prefs.putString(keyBuffer, "");
      }
      prefs.end();
      return ok;
    }

    bool wifiClearNetwork(size_t slot)
    {
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, false))
      {
        return false;
      }
      char keyBuffer[16];
      wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiSsidFmt);
      bool ok = prefs.putString(keyBuffer, "") == 0;
      wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiKeyFmt);
      ok = ok && prefs.putString(keyBuffer, "") == 0;
      wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiLegacySsidFmt);
      prefs.putString(keyBuffer, "");
      wifiFormatKey(keyBuffer, sizeof(keyBuffer), slot, kPrefsWifiLegacyKeyFmt);
      prefs.putString(keyBuffer, "");
      prefs.end();
      return ok;
    }

    bool wifiFindFreeSlot(size_t &slot) const
    {
      for (size_t i = 0; i < ESP32SERIALCTL_WIFI_MAX_NETWORKS; ++i)
      {
        String ssid;
        String key;
        if (!wifiReadNetworkSlot(i, ssid, key))
        {
          slot = i;
          return true;
        }
      }
      return false;
    }

    bool wifiAutoPreference() const
    {
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, true))
      {
        return true;
      }
      const uint8_t value = prefs.getUChar(kPrefsWifiAutoKey, 1U);
      prefs.end();
      return value != 0U;
    }

    bool wifiSetAutoPreference(bool enabled)
    {
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, false))
      {
        return false;
      }
      const bool ok = prefs.putUChar(kPrefsWifiAutoKey, enabled ? 1U : 0U) == 1;
      prefs.end();
      return ok;
    }

    const String &currentTimezone() const { return timezoneCache_; }

    static void ntpFormatKey(char *buffer, size_t size, size_t index)
    {
      if (!buffer || size == 0)
      {
        return;
      }
      snprintf(buffer, size, kPrefsNtpServerFmt, static_cast<unsigned>(index));
    }

    size_t ntpLoadServers(String (&out)[ESP32SERIALCTL_NTP_MAX_SERVERS], bool ensureDefault = false)
    {
      Preferences prefs;
      bool opened = prefs.begin(kPrefsNamespace, !ensureDefault);
      for (size_t i = 0; i < ESP32SERIALCTL_NTP_MAX_SERVERS; ++i)
      {
        out[i] = "";
      }
      if (opened)
      {
        char keyBuffer[16];
        for (size_t i = 0; i < ESP32SERIALCTL_NTP_MAX_SERVERS; ++i)
        {
          ntpFormatKey(keyBuffer, sizeof(keyBuffer), i);
          if (prefs.isKey(keyBuffer))
          {
            out[i] = prefs.getString(keyBuffer, "");
          }
        }
        if (ensureDefault && out[0].length() == 0)
        {
          ntpFormatKey(keyBuffer, sizeof(keyBuffer), 0);
          out[0] = ESP32SERIALCTL_DEFAULT_NTP_SERVER;
          prefs.putString(keyBuffer, out[0].c_str());
        }
        prefs.end();
      }
      else if (ensureDefault)
      {
        out[0] = ESP32SERIALCTL_DEFAULT_NTP_SERVER;
      }
      else if (out[0].length() == 0)
      {
        out[0] = ESP32SERIALCTL_DEFAULT_NTP_SERVER;
      }
      size_t count = 0;
      for (size_t i = 0; i < ESP32SERIALCTL_NTP_MAX_SERVERS; ++i)
      {
        if (out[i].length() > 0)
        {
          ++count;
        }
      }
      return count;
    }

    bool ntpStoreServers(const String (&servers)[ESP32SERIALCTL_NTP_MAX_SERVERS])
    {
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, false))
      {
        return false;
      }
      char keyBuffer[16];
      bool ok = true;
      for (size_t i = 0; i < ESP32SERIALCTL_NTP_MAX_SERVERS; ++i)
      {
        ntpFormatKey(keyBuffer, sizeof(keyBuffer), i);
        const size_t len = servers[i].length();
        ok = ok && (prefs.putString(keyBuffer, servers[i].c_str()) == len);
      }
      prefs.end();
      return ok;
    }

    bool ntpAutoPreference()
    {
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, true))
      {
        return ntpAutoCache_;
      }
      const uint8_t value = prefs.getUChar(kPrefsNtpAutoKey, 1U);
      prefs.end();
      ntpAutoCache_ = value != 0U;
      return ntpAutoCache_;
    }

    bool ntpSetAutoPreference(bool enabled)
    {
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, false))
      {
        return false;
      }
      const bool ok = prefs.putUChar(kPrefsNtpAutoKey, enabled ? 1U : 0U) == 1;
      prefs.end();
      if (ok)
      {
        ntpAutoCache_ = enabled;
      }
      return ok;
    }

    bool ntpEnabledPreference()
    {
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, true))
      {
        return ntpEnabledCache_;
      }
      const uint8_t value = prefs.getUChar(kPrefsNtpEnabledKey, 0U);
      prefs.end();
      ntpEnabledCache_ = value != 0U;
      return ntpEnabledCache_;
    }

    bool ntpSetEnabledPreference(bool enabled)
    {
      Preferences prefs;
      if (!prefs.begin(kPrefsNamespace, false))
      {
        return false;
      }
      const bool ok = prefs.putUChar(kPrefsNtpEnabledKey, enabled ? 1U : 0U) == 1;
      prefs.end();
      if (ok)
      {
        ntpEnabledCache_ = enabled;
      }
      return ok;
    }

    bool ntpConfigure(bool start)
    {
      if (timezoneCache_.length() == 0)
      {
        return false;
      }
      String servers[ESP32SERIALCTL_NTP_MAX_SERVERS];
      ntpLoadServers(servers, true);
      for (size_t i = 0; i < ESP32SERIALCTL_NTP_MAX_SERVERS; ++i)
      {
        const char *source = nullptr;
        if (servers[i].length() > 0)
        {
          source = servers[i].c_str();
        }
        else if (i == 0)
        {
          source = ESP32SERIALCTL_DEFAULT_NTP_SERVER;
        }
        else
        {
          source = "";
        }
        strncpy(ntpServerBuffer_[i], source, sizeof(ntpServerBuffer_[i]) - 1);
        ntpServerBuffer_[i][sizeof(ntpServerBuffer_[i]) - 1] = '\0';
      }
      const char *server1 = ntpServerBuffer_[0];
      const char *server2 = ntpServerBuffer_[1];
      const char *server3 = ntpServerBuffer_[2];
      if (!server1 || strlen(server1) == 0)
      {
        server1 = ESP32SERIALCTL_DEFAULT_NTP_SERVER;
      }
      if (start)
      {
        esp_sntp_stop();
        esp_sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
        configTzTime(timezoneCache_.c_str(), server1, server2, server3);
        return true;
      }
      esp_sntp_stop();
      return true;
    }

    bool ntpWaitForSync(uint32_t timeoutMs)
    {
      const uint32_t start = millis();
      while (true)
      {
        if (sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED)
        {
          return true;
        }
        struct tm nowLocal;
        if (getLocalTime(&nowLocal, 500))
        {
          return true;
        }
        if (timeoutMs > 0 && millis() - start >= timeoutMs)
        {
          return false;
        }
        delay(200);
      }
    }

    void ntpReconfigureIfNeeded()
    {
      if (!ntpEnabledPreference())
      {
        return;
      }
      if (timezoneCache_.length() == 0)
      {
        return;
      }
      ntpConfigure(true);
    }

    void ntpMaybeAutoStart()
    {
      const bool autoMode = ntpAutoPreference();
      if (!autoMode)
      {
        return;
      }
      if (!ntpEnabledPreference())
      {
        return;
      }
      if (timezoneCache_.length() == 0)
      {
        return;
      }
      ntpConfigure(true);
    }

    void wifiPopulateFromStored(const WifiStoredNetwork *networks, size_t count)
    {
      for (size_t i = 0; i < count; ++i)
      {
        const WifiStoredNetwork &entry = networks[i];
        if (entry.ssid.length() == 0)
        {
          continue;
        }
        wifiMulti.addAP(entry.ssid.c_str(), entry.key.c_str());
      }
    }

    void wifiMaybeAutoConnect()
    {
      WifiStoredNetwork networks[ESP32SERIALCTL_WIFI_MAX_NETWORKS];
      const size_t count = wifiLoadStoredNetworks(networks);
      if (count == 0 || !wifiAutoPreference())
      {
        return;
      }
      wifiConfigureStationMode(true);
      wifiResetMulti();
      wifiPopulateFromStored(networks, count);
      wifiAttemptConnection(static_cast<uint32_t>(ESP32SERIALCTL_WIFI_CONNECT_TIMEOUT_MS));
    }
#endif
#endif
    void feed(char c)
    {
      if (c == '\r' || c == '\n')
      {
        finalizeLine();
        return;
      }
      if (overflow_)
      {
        return;
      }
      if (linePos_ >= MaxLineLength)
      {
        overflow_ = true;
        return;
      }
      lineBuffer_[linePos_++] = c;
    }

    void resetLine()
    {
      linePos_ = 0;
      overflow_ = false;
    }

    void execute(const char *line)
    {
      ensureInitialized();
      if (!line)
      {
        return;
      }
      resetLine();
      const char *cursor = line;
      while (*cursor && !overflow_)
      {
        if (linePos_ >= MaxLineLength)
        {
          overflow_ = true;
          break;
        }
        lineBuffer_[linePos_++] = *cursor++;
      }
      if (overflow_)
      {
        emitError(510, "Line too long");
      }
      else
      {
        lineBuffer_[linePos_] = '\0';
        processLine(lineBuffer_);
      }
      resetLine();
      flushPromptIfNeeded();
    }

    Print &output() { return output_; }
    const Print &output() const { return output_; }
    Stream &input() { return input_; }
    const Stream &input() const { return input_; }

    const Command *commands() const { return commands_; }
    size_t commandCount() const { return commandCount_; }

    static bool equalsIgnoreCase(const char *a, const char *b)
    {
      if (a == b)
      {
        return true;
      }
      if (!a || !b)
      {
        return false;
      }
      while (*a && *b)
      {
        const unsigned char ca = static_cast<unsigned char>(*a);
        const unsigned char cb = static_cast<unsigned char>(*b);
        if (tolower(ca) != tolower(cb))
        {
          return false;
        }
        ++a;
        ++b;
      }
      return *a == '\0' && *b == '\0';
    }

    static bool parseBoolean(const char *text, bool &out)
    {
      if (!text || !*text)
      {
        return false;
      }
      if (equalsIgnoreCase(text, "on") || equalsIgnoreCase(text, "true") ||
          strcmp(text, "1") == 0)
      {
        out = true;
        return true;
      }
      if (equalsIgnoreCase(text, "off") || equalsIgnoreCase(text, "false") ||
          strcmp(text, "0") == 0)
      {
        out = false;
        return true;
      }
      return false;
    }

    static bool parseNumber(const char *text, ParsedNumber &out)
    {
      if (!text || !*text)
      {
        return false;
      }
      char *end = nullptr;
      const long long raw = strtoll(text, &end, 0);
      if (end == text)
      {
        return false;
      }

      NumberUnit unit = NumberUnit::None;
      long long value = raw;

      if (*end != '\0')
      {
        if (equalsIgnoreCase(end, "ms"))
        {
          unit = NumberUnit::TimeMilliseconds;
        }
        else if (equalsIgnoreCase(end, "s"))
        {
          unit = NumberUnit::TimeMilliseconds;
          value = raw * 1000LL;
        }
        else if (equalsIgnoreCase(end, "hz"))
        {
          unit = NumberUnit::FrequencyHz;
        }
        else if (equalsIgnoreCase(end, "khz"))
        {
          unit = NumberUnit::FrequencyHz;
          value = raw * 1000LL;
        }
        else if (end[0] == '%' && end[1] == '\0')
        {
          unit = NumberUnit::Percent;
        }
        else
        {
          return false;
        }
      }

      out.value = value;
      out.unit = unit;
      return true;
    }

    template <typename Visitor>
    void forEachCommand(Visitor visitor) const
    {
      for (size_t i = 0; i < commandCount_; ++i)
      {
        visitor(commands_[i]);
      }
    }

    using CommandSupportFilter = bool (*)(const Command &cmd);

    static void setCommandSupportFilter(CommandSupportFilter filter)
    {
      commandSupportFilter_ = filter;
    }

    static bool commandSupported(const Command &cmd)
    {
      return !commandSupportFilter_ || commandSupportFilter_(cmd);
    }

    bool printHelp(const char *topic = nullptr)
    {
      const bool hasTopic = topic && *topic;
      if (!hasTopic)
      {
        emitOK("help");
        for (size_t i = 0; i < commandCount_; ++i)
        {
          const Command &cmd = commands_[i];
          if (!cmd.name)
          {
            continue;
          }
          if (!commandSupported(cmd))
          {
            continue;
          }
          emitHelpEntry(cmd);
        }
        return true;
      }

      const char *topicText = topic;
      bool groupMatch = false;
      for (size_t i = 0; i < commandCount_; ++i)
      {
        const Command &cmd = commands_[i];
        if (cmd.group && equalsIgnoreCase(cmd.group, topicText) && commandSupported(cmd))
        {
          groupMatch = true;
          break;
        }
      }

      if (groupMatch)
      {
        char header[64];
        snprintf(header, sizeof(header), "help %s", topicText);
        emitOK(header);
        for (size_t i = 0; i < commandCount_; ++i)
        {
          const Command &cmd = commands_[i];
          if (cmd.group && equalsIgnoreCase(cmd.group, topicText) && commandSupported(cmd))
          {
            emitHelpEntry(cmd);
          }
        }
        return true;
      }

      for (size_t i = 0; i < commandCount_; ++i)
      {
        const Command &cmd = commands_[i];
        if (!cmd.name)
        {
          continue;
        }
        const bool isTopLevel = !cmd.group || !*cmd.group;
        if (isTopLevel && equalsIgnoreCase(cmd.name, topicText) && commandSupported(cmd))
        {
          char header[64];
          snprintf(header, sizeof(header), "help %s", topicText);
          emitOK(header);
          emitHelpEntry(cmd);
          return true;
        }
      }

      return false;
    }

  private:
    struct Token
    {
      char *value;
      size_t length;
      bool quoted;
    };

    void finalizeLine()
    {
      if (overflow_)
      {
        emitError(510, "Line too long");
        resetLine();
        return;
      }
      if (linePos_ == 0)
      {
        queuePrompt();
        resetLine();
        return;
      }
      lineBuffer_[linePos_] = '\0';
      processLine(lineBuffer_);
      resetLine();
    }

    void processLine(char *line)
    {
      if (!line)
      {
        return;
      }
      char *segment = line;
      bool inQuote = false;
      bool escape = false;
      for (char *cursor = line;; ++cursor)
      {
        const char ch = *cursor;
        if (ch == '\0')
        {
          handleSegment(segment);
          break;
        }
        if (inQuote)
        {
          if (escape)
          {
            escape = false;
          }
          else if (ch == '\\')
          {
            escape = true;
          }
          else if (ch == '"')
          {
            inQuote = false;
          }
          continue;
        }
        if (ch == '"')
        {
          inQuote = true;
          continue;
        }
        if (ch == ';')
        {
          *cursor = '\0';
          handleSegment(segment);
          segment = cursor + 1;
        }
      }
    }

    void handleSegment(char *segment)
    {
      if (!segment)
      {
        return;
      }
      char *trimmed = ltrim(segment);
      rtrim(trimmed);
      if (*trimmed == '\0')
      {
        return;
      }

      stripComment(trimmed);
      rtrim(trimmed);
      if (*trimmed == '\0')
      {
        return;
      }

      Token tokens[MaxTokens];
      size_t tokenCount = 0;
      if (!tokenize(trimmed, tokens, tokenCount))
      {
        return;
      }
      dispatch(tokens, tokenCount);
    }

    bool tokenize(char *text, Token *tokens, size_t &tokenCount)
    {
      tokenCount = 0;
      char *cursor = text;
      while (true)
      {
        cursor = ltrim(cursor);
        if (*cursor == '\0')
        {
          break;
        }
        if (tokenCount >= MaxTokens)
        {
          emitError(413, "Too many tokens");
          return false;
        }
        if (*cursor == '"')
        {
          ++cursor;
          char *start = cursor;
          char *write = cursor;
          bool escape = false;
          bool closed = false;
          while (*cursor)
          {
            char ch = *cursor;
            if (escape)
            {
              *write++ = translateEscape(ch);
              escape = false;
            }
            else if (ch == '\\')
            {
              escape = true;
            }
            else if (ch == '"')
            {
              closed = true;
              ++cursor;
              break;
            }
            else
            {
              *write++ = ch;
            }
            ++cursor;
          }
          if (!closed)
          {
            emitError(508, "Missing closing quote");
            return false;
          }
          *write = '\0';
          tokens[tokenCount++] = Token{start, static_cast<size_t>(write - start),
                                       true};
        }
        else
        {
          char *start = cursor;
          while (*cursor && !isspace(static_cast<unsigned char>(*cursor)))
          {
            ++cursor;
          }
          char *end = cursor;
          if (*cursor)
          {
            *cursor = '\0';
            ++cursor;
          }
          tokens[tokenCount++] =
              Token{start, static_cast<size_t>(end - start), false};
        }
      }
      return true;
    }

    void dispatch(Token *tokens, size_t tokenCount)
    {
      if (tokenCount == 0)
      {
        return;
      }

      bool consumed[MaxTokens];
      for (size_t i = 0; i < MaxTokens; ++i)
      {
        consumed[i] = false;
      }

      optionCount_ = 0;

      for (size_t i = 0; i < tokenCount; ++i)
      {
        if (consumed[i])
        {
          continue;
        }
        Token &token = tokens[i];
        if (token.length >= 2 && token.value[0] == '-' && token.value[1] == '-')
        {
          char *name = token.value + 2;
          char *value = nullptr;
          char *eq = strchr(name, '=');
          if (eq)
          {
            *eq = '\0';
            value = eq + 1;
          }
          else if ((i + 1) < tokenCount && !consumed[i + 1])
          {
            Token &next = tokens[i + 1];
            if (!(next.length >= 2 && next.value[0] == '-' &&
                  next.value[1] == '-'))
            {
              value = next.value;
              consumed[i + 1] = true;
            }
          }
          if (optionCount_ >= MaxTokens)
          {
            emitError(431, "Too many options");
            return;
          }
          options_[optionCount_++] = Option{name, value};
          consumed[i] = true;
        }
      }

      Argument positional[MaxTokens];
      size_t positionalCount = 0;
      for (size_t i = 0; i < tokenCount; ++i)
      {
        if (consumed[i])
        {
          continue;
        }
        if (positionalCount >= MaxTokens)
        {
          emitError(413, "Too many tokens");
          return;
        }
        positional[positionalCount++] =
            Argument{tokens[i].value, tokens[i].length, tokens[i].quoted};
      }

      if (positionalCount == 0)
      {
        return;
      }

      const Command *selected = nullptr;
      size_t argBase = 0;

      for (size_t i = 0; i < commandCount_; ++i)
      {
        const Command &cmd = commands_[i];
        if (!cmd.name)
        {
          continue;
        }
        if (!cmd.group)
        {
          if (equalsIgnoreCase(positional[0].c_str(), cmd.name))
          {
            selected = &cmd;
            argBase = 1;
            break;
          }
        }
        else if (positionalCount >= 2 &&
                 equalsIgnoreCase(positional[0].c_str(), cmd.group) &&
                 equalsIgnoreCase(positional[1].c_str(), cmd.name))
        {
          selected = &cmd;
          argBase = 2;
          break;
        }
      }

      if (!selected)
      {
        if (printHelp(positional[0].c_str()))
        {
          return;
        }
        emitError(404, "Unknown command");
        return;
      }

      if (!commandSupported(*selected))
      {
        emitError(404, "Unknown command");
        return;
      }

      if (!selected->handler)
      {
        emitError(500, "No handler");
        return;
      }

      size_t argc = 0;
      if (positionalCount > argBase)
      {
        argc = positionalCount - argBase;
        for (size_t i = 0; i < argc; ++i)
        {
          arguments_[i] = positional[argBase + i];
        }
      }

      context_.owner_ = this;
      context_.command_ = selected;
      context_.args_ = arguments_;
      context_.argc_ = argc;
      context_.options_ = options_;
      context_.optionCount_ = optionCount_;

      selected->handler(context_);
      queuePrompt();
    }

    void emitHelpEntry(const Command &cmd)
    {
      if (!cmd.name)
      {
        return;
      }
      if (!commandSupported(cmd))
      {
        return;
      }

      char commandText[48];
      if (cmd.group && *cmd.group)
      {
        snprintf(commandText, sizeof(commandText), "%s %s", cmd.group, cmd.name);
      }
      else
      {
        snprintf(commandText, sizeof(commandText), "%s", cmd.name);
      }

      const char *help = cmd.help ? cmd.help : "";
      if (help && *help)
      {
        char line[128];
        snprintf(line, sizeof(line), "%s %s", commandText, help);
        emitList(line);
      }
      else
      {
        emitList(commandText);
      }
    }

    static char *ltrim(char *text)
    {
      while (*text && isspace(static_cast<unsigned char>(*text)))
      {
        ++text;
      }
      return text;
    }

    static void rtrim(char *text)
    {
      size_t len = strlen(text);
      while (len > 0)
      {
        unsigned char ch = static_cast<unsigned char>(text[len - 1]);
        if (!isspace(ch))
        {
          break;
        }
        text[--len] = '\0';
      }
    }

    static void stripComment(char *text)
    {
      bool inQuote = false;
      bool escape = false;
      for (char *cursor = text; *cursor; ++cursor)
      {
        const char ch = *cursor;
        if (inQuote)
        {
          if (escape)
          {
            escape = false;
          }
          else if (ch == '\\')
          {
            escape = true;
          }
          else if (ch == '"')
          {
            inQuote = false;
          }
          continue;
        }
        if (ch == '"')
        {
          inQuote = true;
        }
        else if (ch == '#')
        {
          *cursor = '\0';
          break;
        }
      }
    }

    static char translateEscape(char ch)
    {
      switch (ch)
      {
      case 'n':
        return '\n';
      case 'r':
        return '\r';
      case 't':
        return '\t';
      case '"':
        return '"';
      case '\\':
        return '\\';
      default:
        return ch;
      }
    }

    void emitOK(const char *message)
    {
      output_.print("OK");
      if (message && *message)
      {
        output_.print(' ');
        output_.print(message);
      }
      output_.print("\r\n");
      queuePrompt();
    }

    void emitError(uint16_t code, const char *reason)
    {
      output_.print("ERR ");
      output_.print(code);
      output_.print(' ');
      if (reason && *reason)
      {
        output_.print(reason);
      }
      output_.print("\r\n");
      queuePrompt();
    }

    void emitList(const char *text)
    {
      output_.print(" - ");
      if (text)
      {
        output_.print(text);
      }
      output_.print("\r\n");
    }

    void emitBody(const char *text)
    {
      output_.print("| ");
      if (text)
      {
        output_.print(text);
      }
      output_.print("\r\n");
    }

    void flushPromptIfNeeded()
    {
      if (!promptPending_)
      {
        return;
      }
      promptPending_ = false;
      output_.print("> ");
    }

    void queuePrompt() { promptPending_ = true; }

    Stream &input_;
    Print &output_;
    const Command *const commands_;
    const size_t commandCount_;
    static CommandSupportFilter commandSupportFilter_;
    char lineBuffer_[MaxLineLength + 1];
    size_t linePos_ = 0;
    bool overflow_ = false;

    Context context_;
    Argument arguments_[MaxTokens];
    Option options_[MaxTokens];
    size_t optionCount_ = 0;
    bool promptPending_ = true;
    bool initialized_ = false;
#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
    String timezoneCache_;
#endif
#if defined(ESP32SERIALCTL_HAS_WIFI) && defined(ESP32SERIALCTL_HAS_PREFERENCES)
    bool ntpAutoCache_ = true;
    bool ntpEnabledCache_ = false;
    char ntpServerBuffer_[ESP32SERIALCTL_NTP_MAX_SERVERS][64] = {{0}};
#endif
  };

  template <size_t MaxLineLength, size_t MaxTokens>
  typename SerialCtl<MaxLineLength, MaxTokens>::CommandSupportFilter
      SerialCtl<MaxLineLength, MaxTokens>::commandSupportFilter_ = nullptr;

  template <size_t MaxLineLength = 128, size_t MaxTokens = 16>
  class ESP32SerialCtl
  {
  public:
    using Base = SerialCtl<MaxLineLength, MaxTokens>;
    using Command = typename Base::Command;
    using Context = typename Base::Context;

    ESP32SerialCtl()
        : cli_(Serial, activeCommands_, activeCommandCount_)
    {
      configureConfig(nullptr, 0, nullptr);
    }

    template <size_t EntryCount>
    explicit ESP32SerialCtl(const ConfigEntry (&entries)[EntryCount],
                            const char *configNamespace = nullptr)
        : cli_(Serial, activeCommands_, activeCommandCount_)
    {
      configureConfig(entries, EntryCount, configNamespace);
    }

    // Constructor that accepts both config entries and command entries as
    // static arrays. It activates the provided commands before constructing
    // the CLI internals so they are used from the start.
    template <size_t EntryCount, size_t CmdCount>
    explicit ESP32SerialCtl(const ConfigEntry (&entries)[EntryCount],
                            const CommandEntry (&commands)[CmdCount],
                            const char *configNamespace = nullptr)
        : cli_(Serial, ESP32SerialCtl::allocateAndActivateCommands(commands, CmdCount),
               activeCommandCount_)
    {
      configureConfig(entries, EntryCount, configNamespace);
    }

    ESP32SerialCtl(const ConfigEntry *entries, size_t count,
                   const char *configNamespace = nullptr)
        : cli_(Serial, activeCommands_, activeCommandCount_)
    {
      configureConfig(entries, count, configNamespace);
    }

    explicit ESP32SerialCtl(Stream &io)
        : cli_(io, activeCommands_, activeCommandCount_)
    {
      configureConfig(nullptr, 0, nullptr);
    }

    template <size_t EntryCount>
    ESP32SerialCtl(Stream &io, const ConfigEntry (&entries)[EntryCount],
                   const char *configNamespace = nullptr)
        : cli_(io, activeCommands_, activeCommandCount_)
    {
      configureConfig(entries, EntryCount, configNamespace);
    }

    ESP32SerialCtl(Stream &io, const ConfigEntry *entries, size_t count,
                   const char *configNamespace = nullptr)
        : cli_(io, activeCommands_, activeCommandCount_)
    {
      configureConfig(entries, count, configNamespace);
    }

    ESP32SerialCtl(Stream &input, Print &output)
        : cli_(input, output, activeCommands_, activeCommandCount_)
    {
      configureConfig(nullptr, 0, nullptr);
    }

    template <size_t EntryCount>
    ESP32SerialCtl(Stream &input, Print &output,
                   const ConfigEntry (&entries)[EntryCount],
                   const char *configNamespace = nullptr)
        : cli_(input, output, activeCommands_, activeCommandCount_)
    {
      configureConfig(entries, EntryCount, configNamespace);
    }

    ESP32SerialCtl(Stream &input, Print &output, const ConfigEntry *entries,
                   size_t count, const char *configNamespace = nullptr)
        : cli_(input, output, activeCommands_, activeCommandCount_)
    {
      configureConfig(entries, count, configNamespace);
    }

    Base &raw() { return cli_; }
    const Base &raw() const { return cli_; }

    void service() { cli_.service(); }
    void feed(char c) { cli_.feed(c); }
    void execute(const char *line) { cli_.execute(line); }

    Print &output() { return cli_.output(); }
    Stream &input() { return cli_.input(); }

    void setPinAllAccess(bool enable) { pinAllAccess_ = enable; }
    bool pinAllAccess() const { return pinAllAccess_; }

    bool setPinName(int pinNumber, const char *name)
    {
      return setPinNameInternal(pinNumber, name);
    }

    bool setPinName(int pinNumber, const String &name)
    {
      return setPinNameInternal(pinNumber, name.c_str());
    }

    bool setPinName(gpio_num_t pin, const char *name)
    {
      return setPinName(static_cast<int>(pin), name);
    }

    bool setPinName(gpio_num_t pin, const String &name)
    {
      return setPinName(static_cast<int>(pin), name);
    }

    bool setPinAllowed(int pinNumber, bool allowed)
    {
      return setPinAllowedInternal(pinNumber, allowed);
    }

    bool setPinAllowed(gpio_num_t pin, bool allowed)
    {
      return setPinAllowed(static_cast<int>(pin), allowed);
    }

    bool isPinAllowed(int pinNumber) const
    {
      if (!validPinIndex(pinNumber))
      {
        return false;
      }
      return pinAllowedEffective(static_cast<uint8_t>(pinNumber));
    }

    bool isPinAllowed(gpio_num_t pin) const
    {
      return isPinAllowed(static_cast<int>(pin));
    }

    String pinName(int pinNumber) const
    {
      if (!validPinIndex(pinNumber))
      {
        return String();
      }
      return pinNames_[static_cast<uint8_t>(pinNumber)];
    }

    String pinName(gpio_num_t pin) const
    {
      return pinName(static_cast<int>(pin));
    }

    static void setDefaultRgbPin(int pin) { rgbDefaultPin_ = pin; }
    static int defaultRgbPin() { return rgbDefaultPin_; }

    static void setConfigNamespace(const char *ns)
    {
      if (ns && *ns)
      {
        configNamespace_ = ns;
      }
      else
      {
        configNamespace_ = kPrefsConfigDefaultNamespace;
      }
    }

    template <size_t EntryCount>
    static void setConfigEntries(const ConfigEntry (&entries)[EntryCount])
    {
      setConfigEntries(entries, EntryCount);
    }

    static void setConfigEntries(const ConfigEntry *entries, size_t count)
    {
      if (!entries || count == 0)
      {
        configEntries_ = nullptr;
        configEntryCount_ = 0;
        Base::setCommandSupportFilter(&ESP32SerialCtl::filterCommand);
        return;
      }
      configEntries_ = entries;
      configEntryCount_ = count;
      Base::setCommandSupportFilter(&ESP32SerialCtl::filterCommand);
    }

    // Generic dispatcher invoked for user-registered commands. It looks up
    // the CommandEntry associated with the invoked Command and calls the
    // user handler with argv/argc.
    static void dispatchToUserHandler(Context &ctx)
    {
      const Command &cmd = ctx.command();
      const Command *commands = ESP32SerialCtl::activeCommands_;
      const size_t total = ESP32SerialCtl::activeCommandCount_;
      size_t index = SIZE_MAX;
      for (size_t i = 0; i < total; ++i)
      {
        if (&commands[i] == &cmd)
        {
          index = i;
          break;
        }
      }
      if (index == SIZE_MAX)
      {
        ctx.printError(500, "Handler mapping not found");
        return;
      }
      const CommandEntry *entry = ESP32SerialCtl::commandEntryMap_[index];
      if (!entry || !entry->handler)
      {
        ctx.printError(500, "No handler for command");
        return;
      }

      const size_t argc = ctx.argc();
      const char *argvBuf[ESP32SERIALCTL_CMD_ARG_MAX > 16 ? ESP32SERIALCTL_CMD_ARG_MAX : 16];
      for (size_t i = 0; i < argc; ++i)
      {
        argvBuf[i] = ctx.arg(i).c_str();
      }

      // call user handler
      int rc = entry->handler(argvBuf, argc, nullptr);
      if (rc != 0)
      {
        ctx.printError(500, "Command handler error");
      }
    }

    // Register user CommandEntry array. This will build an internal
    // combined Command array (built-ins followed by user commands) and
    // set activeCommands_ to point to it. The registered CommandEntry
    // pointers are recorded in commandEntryMap_.
    //
    // Note: SerialCtl::Command contains const members and is not
    // assignable. To avoid assignment (which fails to compile), we
    // allocate raw storage and placement-new each Command element
    // (copy-construct built-ins, aggregate-construct user entries).
    static void registerCommands(const CommandEntry *entries, size_t count)
    {
      // base built-in commands
      const Command *base = ESP32SerialCtl::kCommands;
      const size_t baseCount = ESP32SerialCtl::kCommandCount;
      const size_t total = baseCount + count;

      // allocate raw storage for Commands and the mapping array
      void *raw = ::operator new[](sizeof(Command) * total);
      Command *combined = static_cast<Command *>(raw);
      const CommandEntry **map = new const CommandEntry *[total];

      // copy-construct built-ins into the raw storage
      for (size_t i = 0; i < baseCount; ++i)
      {
        new (&combined[i]) Command(base[i]);
        map[i] = nullptr;
      }

      // construct user entries via aggregate initialization
      for (size_t i = 0; i < count; ++i)
      {
        const CommandEntry &e = entries[i];
        size_t dst = baseCount + i;

        // pick first non-empty localized description as help
        const char *help = nullptr;
        for (size_t j = 0; j < ESP32SERIALCTL_CONFIG_MAX_LOCALES; ++j)
        {
          const LocalizedText &lt = e.descriptions[j];
          if (lt.description && *lt.description)
          {
            help = lt.description;
            break;
          }
        }
        const char *helpStr = help ? help : "";

        // Command layout: group, name, handler, help
        new (&combined[dst]) Command{nullptr,
                                     (e.name ? e.name : nullptr),
                                     &ESP32SerialCtl::dispatchToUserHandler,
                                     helpStr};
        map[dst] = &entries[i];
      }

      // set active pointers
      activeCommands_ = combined;
      activeCommandCount_ = total;
      commandEntryMap_ = map;
    }

    // Helper used by constructors to allocate+activate commands and return
    // the pointer suitable for SerialCtl initialization.
    static Command *allocateAndActivateCommands(const CommandEntry *entries, size_t count)
    {
      registerCommands(entries, count);
      // activeCommands_ now points to newly allocated combined array
      return const_cast<Command *>(activeCommands_);
    }

    static const ConfigEntry *configEntry(size_t index)
    {
      if (!configEntries_ || index >= configEntryCount_)
      {
        return nullptr;
      }
      return &configEntries_[index];
    }

    static size_t configEntryCount()
    {
      return configEntryCount_;
    }

    String configGet(const char *name) const
    {
      if (!name || !*name)
      {
        return String();
      }
      const ConfigEntry *entry = findConfigEntry(name);
      if (!entry)
      {
        return String();
      }
      String value;
      if (configLoadStoredValue(*entry, value))
      {
        return value;
      }
      return entry->defaultValue ? String(entry->defaultValue) : String();
    }

    bool configSet(const char *name, const String &value)
    {
      if (!name || !*name)
      {
        return false;
      }
      const ConfigEntry *entry = findConfigEntry(name);
      if (!entry)
      {
        return false;
      }
      return configStoreValue(*entry, value);
    }

    bool configSet(const char *name, const char *value)
    {
      return configSet(name, String(value ? value : ""));
    }

    bool configClear(const char *name)
    {
      if (!name || !*name)
      {
        return false;
      }
      const ConfigEntry *entry = findConfigEntry(name);
      if (!entry)
      {
        return false;
      }
      return configRemoveValue(*entry);
    }

  private:
    static bool validPinIndex(int pin)
    {
#if defined(GPIO_PIN_COUNT)
      return pin >= 0 && pin < GPIO_PIN_COUNT;
#else
      return pin >= 0 && pin < 256;
#endif
    }

    static bool setPinNameInternal(int pinNumber, const char *name)
    {
      if (!validPinIndex(pinNumber))
      {
        return false;
      }
      const uint8_t pin = static_cast<uint8_t>(pinNumber);
      if (name && *name)
      {
        pinNames_[pin] = name;
      }
      else
      {
        pinNames_[pin] = "";
      }
      pinAllowed_[pin] = true;
      return true;
    }

    static bool setPinAllowedInternal(int pinNumber, bool allowed)
    {
      if (!validPinIndex(pinNumber))
      {
        return false;
      }
      pinAllowed_[static_cast<uint8_t>(pinNumber)] = allowed;
      return true;
    }

    static bool pinAllowedEffective(uint8_t pin)
    {
      if (!validPinIndex(static_cast<int>(pin)))
      {
        return false;
      }
      if (pinAllAccess_)
      {
        return true;
      }
      return pinAllowed_[pin];
    }

    static bool ensurePinAllowed(Context &ctx, uint8_t pin)
    {
      if (!pinAllowedEffective(pin))
      {
        char message[48];
        snprintf(message, sizeof(message), "Pin %u not allowed",
                 static_cast<unsigned>(pin));
        ctx.printError(403, message);
        return false;
      }
      return true;
    }

    static bool resolvePinName(const char *text, uint8_t &pin)
    {
      if (!text || !*text)
      {
        return false;
      }
      for (int i = 0; i < GPIO_PIN_COUNT; ++i)
      {
        const String &alias = pinNames_[i];
        if (alias.length() == 0)
        {
          continue;
        }
        if (Base::equalsIgnoreCase(alias.c_str(), text))
        {
          pin = static_cast<uint8_t>(i);
          return true;
        }
      }
      return false;
    }

    static bool resolvePinText(const char *text, uint8_t &pin)
    {
      if (!text)
      {
        return false;
      }
      ParsedNumber number;
      if (Base::parseNumber(text, number) && number.unit == NumberUnit::None &&
          validPinIndex(static_cast<int>(number.value)))
      {
        pin = static_cast<uint8_t>(number.value);
        return true;
      }
      return resolvePinName(text, pin);
    }

    static bool resolvePinArgument(const typename Base::Argument &arg, uint8_t &pin)
    {
      if (arg.empty())
      {
        return false;
      }
      ParsedNumber number;
      if (arg.toNumber(number) && number.unit == NumberUnit::None &&
          validPinIndex(static_cast<int>(number.value)))
      {
        pin = static_cast<uint8_t>(number.value);
        return true;
      }
      return resolvePinName(arg.c_str(), pin);
    }

    static void configureConfig(const ConfigEntry *entries, size_t count,
                                const char *configNamespace)
    {
      if (configNamespace)
      {
        setConfigNamespace(configNamespace);
      }
      if (entries && count > 0)
      {
        setConfigEntries(entries, count);
      }
      else if (!entries || count == 0)
      {
        setConfigEntries(nullptr, 0);
      }
      Base::setCommandSupportFilter(&ESP32SerialCtl::filterCommand);
    }

    static const char *configNamespaceCStr()
    {
      return configNamespace_.length() > 0 ? configNamespace_.c_str()
                                           : kPrefsConfigDefaultNamespace;
    }

    static bool configSupportEnabled()
    {
      return configEntries_ && configEntryCount_ > 0;
    }

    static bool filterCommand(const Command &cmd)
    {
      if (cmd.group && Base::equalsIgnoreCase(cmd.group, "conf"))
      {
        return configSupportEnabled();
      }
      return true;
    }

    static const ConfigEntry *findConfigEntry(const char *name, size_t *indexOut = nullptr)
    {
      if (!configSupportEnabled() || !name || !*name)
      {
        return nullptr;
      }
      for (size_t i = 0; i < configEntryCount_; ++i)
      {
        const ConfigEntry &entry = configEntries_[i];
        if (!entry.name)
        {
          continue;
        }
        if (Base::equalsIgnoreCase(entry.name, name))
        {
          if (indexOut)
          {
            *indexOut = i;
          }
          return &entry;
        }
      }
      return nullptr;
    }

    static bool configLoadStoredValue(const ConfigEntry &entry, String &out)
    {
#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
      if (!entry.name || !*entry.name)
      {
        out = "";
        return false;
      }
      Preferences prefs;
      if (!prefs.begin(configNamespaceCStr(), true))
      {
        out = "";
        return false;
      }
      const bool exists = prefs.isKey(entry.name);
      if (exists)
      {
        out = prefs.getString(entry.name, "");
      }
      else
      {
        out = "";
      }
      prefs.end();
      return exists;
#else
      (void)entry;
      out = "";
      return false;
#endif
    }

    static bool configStoreValue(const ConfigEntry &entry, const String &value)
    {
#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
      if (!entry.name || !*entry.name)
      {
        return false;
      }
      Preferences prefs;
      if (!prefs.begin(configNamespaceCStr(), false))
      {
        return false;
      }
      const size_t written = prefs.putString(entry.name, value);
      const size_t expected = static_cast<size_t>(value.length());
      bool ok = false;
      if (expected > 0)
      {
        ok = written >= expected;
      }
      else
      {
        ok = prefs.isKey(entry.name);
      }
      prefs.end();
      return ok;
#else
      (void)entry;
      (void)value;
      return false;
#endif
    }

    static bool configRemoveValue(const ConfigEntry &entry)
    {
#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
      if (!entry.name || !*entry.name)
      {
        return false;
      }
      Preferences prefs;
      if (!prefs.begin(configNamespaceCStr(), false))
      {
        return false;
      }
      const bool existed = prefs.isKey(entry.name);
      const bool removed = prefs.remove(entry.name);
      prefs.end();
      return existed ? removed : true;
#else
      (void)entry;
      return false;
#endif
    }

    static bool configHasStoredValue(const ConfigEntry &entry)
    {
#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
      if (!entry.name || !*entry.name)
      {
        return false;
      }
      Preferences prefs;
      if (!prefs.begin(configNamespaceCStr(), true))
      {
        return false;
      }
      const bool exists = prefs.isKey(entry.name);
      prefs.end();
      return exists;
#else
      (void)entry;
      return false;
#endif
    }

    static const LocalizedText *
    findConfigDescriptionForLang(const ConfigEntry &entry, const char *lang)
    {
      if (!lang || !*lang)
      {
        return nullptr;
      }
      for (size_t i = 0; i < ESP32SERIALCTL_CONFIG_MAX_LOCALES; ++i)
      {
        const LocalizedText &text = entry.descriptions[i];
        if (text.lang && text.description && *text.description &&
            Base::equalsIgnoreCase(text.lang, lang))
        {
          return &text;
        }
        if (!text.lang && !text.description)
        {
          break;
        }
      }
      return nullptr;
    }

    static const LocalizedText *
    firstConfigDescription(const ConfigEntry &entry)
    {
      for (size_t i = 0; i < ESP32SERIALCTL_CONFIG_MAX_LOCALES; ++i)
      {
        const LocalizedText &text = entry.descriptions[i];
        if (text.description && *text.description)
        {
          return &text;
        }
        if (!text.lang && !text.description)
        {
          break;
        }
      }
      return nullptr;
    }

    static void emitConfigDescriptions(Context &ctx, const ConfigEntry &entry,
                                       const char *lang)
    {
      char line[192];
      if (lang && *lang)
      {
        const LocalizedText *match =
            findConfigDescriptionForLang(entry, lang);
        if (!match)
        {
          match = firstConfigDescription(entry);
        }
        snprintf(line, sizeof(line), "desc_count: %u",
                 match ? 1u : 0u);
        ctx.printBody(line);
        if (match && match->description && *match->description)
        {
          const char *langCode =
              (match->lang && *match->lang) ? match->lang
                                            : (lang && *lang ? lang : "");
          snprintf(line, sizeof(line), "desc.lang: %s", langCode);
          ctx.printBody(line);
          snprintf(line, sizeof(line), "desc.text: %s", match->description);
          ctx.printBody(line);
        }
        return;
      }

      unsigned printed = 0;
      for (size_t i = 0; i < ESP32SERIALCTL_CONFIG_MAX_LOCALES; ++i)
      {
        const LocalizedText &text = entry.descriptions[i];
        if (!text.description || !*text.description)
        {
          if (!text.lang && !text.description)
          {
            break;
          }
          continue;
        }
        ++printed;
      }
      snprintf(line, sizeof(line), "desc_count: %u", printed);
      ctx.printBody(line);
      if (printed == 0)
      {
        return;
      }
      for (size_t i = 0; i < ESP32SERIALCTL_CONFIG_MAX_LOCALES; ++i)
      {
        const LocalizedText &text = entry.descriptions[i];
        if (!text.description || !*text.description)
        {
          if (!text.lang && !text.description)
          {
            break;
          }
          continue;
        }
        const char *langCode =
            (text.lang && *text.lang) ? text.lang : "";
        snprintf(line, sizeof(line), "desc[%u].lang: %s",
                 static_cast<unsigned>(i), langCode);
        ctx.printBody(line);
        snprintf(line, sizeof(line), "desc[%u].text: %s",
                 static_cast<unsigned>(i), text.description);
        ctx.printBody(line);
      }
    }

    static void handleConfList(Context &ctx)
    {
      if (!configSupportEnabled())
      {
        ctx.printError(404, "Config not supported");
        return;
      }
      ctx.printOK("conf list");
      const char *lang = ctx.optionValue("lang");
      if (!configEntries_ || configEntryCount_ == 0)
      {
        ctx.printBody("config_count: 0");
        return;
      }
      char line[192];
      snprintf(line, sizeof(line), "config_count: %u",
               static_cast<unsigned>(configEntryCount_));
      ctx.printBody(line);
      for (size_t i = 0; i < configEntryCount_; ++i)
      {
        const ConfigEntry &entry = configEntries_[i];
        if (!entry.name)
        {
          continue;
        }
        snprintf(line, sizeof(line), "index: %u", static_cast<unsigned>(i));
        ctx.printBody(line);
        snprintf(line, sizeof(line), "name: %s",
                 entry.name ? entry.name : "");
        ctx.printBody(line);
        String storedValue;
        const bool stored = configLoadStoredValue(entry, storedValue);
        snprintf(line, sizeof(line), "stored: %s", stored ? "true" : "false");
        ctx.printBody(line);
        snprintf(line, sizeof(line), "value: %s",
                 stored ? storedValue.c_str()
                        : ((entry.defaultValue && *entry.defaultValue) ? entry.defaultValue : ""));
        ctx.printBody(line);
        snprintf(line, sizeof(line), "default: %s",
                 (entry.defaultValue && *entry.defaultValue) ? entry.defaultValue : "");
        ctx.printBody(line);
        emitConfigDescriptions(ctx, entry, lang);
      }
    }

    static void handleConfGet(Context &ctx)
    {
      if (!configSupportEnabled())
      {
        ctx.printError(404, "Config not supported");
        return;
      }
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: conf get <name>");
        return;
      }
      const char *name = ctx.arg(0).c_str();
      if (!name || !*name)
      {
        ctx.printError(400, "Invalid config name");
        return;
      }
      size_t index = 0;
      const ConfigEntry *entry = findConfigEntry(name, &index);
      if (!entry)
      {
        ctx.printError(404, "Config not found");
        return;
      }
      String value;
      const bool stored = configLoadStoredValue(*entry, value);
      if (!stored)
      {
        value = entry->defaultValue ? entry->defaultValue : "";
      }
      ctx.printOK("conf get");
      char line[160];
      snprintf(line, sizeof(line), "index: %u",
               static_cast<unsigned>(index));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "name: %s",
               entry->name ? entry->name : "");
      ctx.printBody(line);
      snprintf(line, sizeof(line), "stored: %s", stored ? "true" : "false");
      ctx.printBody(line);
      snprintf(line, sizeof(line), "value: %s",
               value.length() > 0 ? value.c_str() : "");
      ctx.printBody(line);
      if (!stored && entry->defaultValue)
      {
        snprintf(line, sizeof(line), "default: %s",
                 entry->defaultValue ? entry->defaultValue : "");
        ctx.printBody(line);
      }
      else if (stored && entry->defaultValue)
      {
        snprintf(line, sizeof(line), "default: %s",
                 entry->defaultValue ? entry->defaultValue : "");
        ctx.printBody(line);
      }
    }

    static void handleConfSet(Context &ctx)
    {
      if (!configSupportEnabled())
      {
        ctx.printError(404, "Config not supported");
        return;
      }
      if (ctx.argc() != 2)
      {
        ctx.printError(400, "Usage: conf set <name> <value>");
        return;
      }
      const char *name = ctx.arg(0).c_str();
      if (!name || !*name)
      {
        ctx.printError(400, "Invalid config name");
        return;
      }
      size_t index = 0;
      const ConfigEntry *entry = findConfigEntry(name, &index);
      if (!entry)
      {
        ctx.printError(404, "Config not found");
        return;
      }
      const char *valueText = ctx.arg(1).c_str();
#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
      if (!configStoreValue(*entry, String(valueText)))
      {
        ctx.printError(500, "Failed to store config");
        return;
      }
      ctx.printOK("conf set");
      char line[160];
      snprintf(line, sizeof(line), "index: %u",
               static_cast<unsigned>(index));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "name: %s",
               entry->name ? entry->name : "");
      ctx.printBody(line);
      snprintf(line, sizeof(line), "stored: true");
      ctx.printBody(line);
      snprintf(line, sizeof(line), "value: %s",
               (valueText && *valueText) ? valueText : "");
      ctx.printBody(line);
      if (entry->defaultValue)
      {
        snprintf(line, sizeof(line), "default: %s",
                 entry->defaultValue ? entry->defaultValue : "");
        ctx.printBody(line);
      }
#else
      (void)valueText;
      ctx.printError(501, "Preferences disabled");
#endif
    }

    static void handleConfDel(Context &ctx)
    {
      if (!configSupportEnabled())
      {
        ctx.printError(404, "Config not supported");
        return;
      }
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: conf del <name>");
        return;
      }
      const char *name = ctx.arg(0).c_str();
      if (!name || !*name)
      {
        ctx.printError(400, "Invalid config name");
        return;
      }
      size_t index = 0;
      const ConfigEntry *entry = findConfigEntry(name, &index);
      if (!entry)
      {
        ctx.printError(404, "Config not found");
        return;
      }
#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
      if (!configRemoveValue(*entry))
      {
        ctx.printError(500, "Failed to delete config");
        return;
      }
      ctx.printOK("conf del");
      char line[160];
      snprintf(line, sizeof(line), "index: %u",
               static_cast<unsigned>(index));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "name: %s",
               entry->name ? entry->name : "");
      ctx.printBody(line);
      snprintf(line, sizeof(line), "stored: false");
      ctx.printBody(line);
      if (entry->defaultValue)
      {
        snprintf(line, sizeof(line), "default: %s",
                 entry->defaultValue ? entry->defaultValue : "");
        ctx.printBody(line);
      }
#else
      ctx.printError(501, "Preferences disabled");
#endif
    }

    static bool parsePinArgument(Context &ctx, const typename Base::Argument &arg,
                                 uint8_t &pin, const char *field)
    {
      if (!resolvePinArgument(arg, pin))
      {
        ctx.printError(400, field);
        return false;
      }
      return ensurePinAllowed(ctx, pin);
    }

    static bool parseUint8Component(Context &ctx, const typename Base::Argument &arg,
                                    uint8_t &value, const char *field)
    {
      ParsedNumber number;
      if (!arg.toNumber(number) || number.unit != NumberUnit::None ||
          number.value < 0 || number.value > 255)
      {
        ctx.printError(400, field);
        return false;
      }
      value = static_cast<uint8_t>(number.value);
      return true;
    }

    static bool parseBooleanToken(const typename Base::Argument &arg, bool &value)
    {
      if (arg.equals("on") || arg.equals("high") || arg.equals("1") || arg.equals("true"))
      {
        value = true;
        return true;
      }
      if (arg.equals("off") || arg.equals("low") || arg.equals("0") || arg.equals("false"))
      {
        value = false;
        return true;
      }
      return false;
    }

    static bool parseFrequencyArgument(Context &ctx, const typename Base::Argument &arg,
                                       uint32_t &freqHz)
    {
      ParsedNumber number;
      if (!arg.toNumber(number) || number.value <= 0)
      {
        ctx.printError(400, "Invalid frequency");
        return false;
      }
      if (number.unit != NumberUnit::None && number.unit != NumberUnit::FrequencyHz)
      {
        ctx.printError(400, "Invalid frequency unit");
        return false;
      }
      freqHz = static_cast<uint32_t>(number.value);
      return true;
    }

    static bool parseDutyArgument(Context &ctx, const typename Base::Argument &arg,
                                  uint8_t bits, uint32_t &dutyValue)
    {
      ParsedNumber number;
      if (!arg.toNumber(number))
      {
        ctx.printError(400, "Invalid duty");
        return false;
      }
      const uint32_t dutyMax = (1u << bits);
      int64_t value = number.value;
      if (number.unit == NumberUnit::Percent)
      {
        if (value < 0)
        {
          value = 0;
        }
        if (value > 100)
        {
          value = 100;
        }
        dutyValue = static_cast<uint32_t>((static_cast<uint64_t>(dutyMax) * value + 50) / 100);
        return true;
      }
      if (number.unit != NumberUnit::None)
      {
        ctx.printError(400, "Invalid duty unit");
        return false;
      }
      if (value < 0)
      {
        ctx.printError(400, "Duty out of range");
        return false;
      }
      const uint32_t rawMax = (1u << bits);
      if (value > static_cast<int64_t>(rawMax))
      {
        ctx.printError(400, "Duty out of range");
        return false;
      }
      if (value == static_cast<int64_t>(rawMax))
      {
        dutyValue = dutyMax;
        return true;
      }
      dutyValue = static_cast<uint32_t>(value);
      return true;
    }

    static uint32_t parseCountOption(Context &ctx, const char *name, uint32_t defaultValue)
    {
      const typename Base::Option *opt = ctx.findOption(name);
      if (!opt)
      {
        return defaultValue;
      }
      ParsedNumber number;
      if (!opt->toNumber(number) || number.unit != NumberUnit::None || number.value <= 0)
      {
        ctx.printError(400, "Invalid option");
        return 0;
      }
      return static_cast<uint32_t>(number.value);
    }

    static int resolveRgbPin(Context &ctx)
    {
      const typename Base::Option *pinOpt = ctx.findOption("pin");
      uint8_t resolved = 0;
      if (pinOpt && pinOpt->value)
      {
        if (!resolvePinText(pinOpt->value, resolved))
        {
          ctx.printError(400, "Invalid pin");
          return -1;
        }
        if (!ensurePinAllowed(ctx, resolved))
        {
          return -1;
        }
        return static_cast<int>(resolved);
      }
      if (rgbDefaultPin_ < 0)
      {
        ctx.printError(404, "RGB pin not set");
        return -1;
      }
      if (!validPinIndex(rgbDefaultPin_))
      {
        ctx.printError(500, "RGB pin invalid");
        return -1;
      }
      resolved = static_cast<uint8_t>(rgbDefaultPin_);
      if (!ensurePinAllowed(ctx, resolved))
      {
        return -1;
      }
      return rgbDefaultPin_;
    }

    static bool resolveWaitOption(Context &ctx, uint32_t defaultWait, uint32_t &out)
    {
      const typename Base::Option *waitOpt = ctx.findOption("wait");
      if (!waitOpt)
      {
        out = defaultWait;
        return true;
      }
      ParsedNumber number;
      if (!Base::parseNumber(waitOpt->value, number) ||
          (number.unit != NumberUnit::None && number.unit != NumberUnit::TimeMilliseconds))
      {
        ctx.printError(400, "Invalid wait");
        return false;
      }
      int64_t value = number.value;
      if (number.unit == NumberUnit::TimeMilliseconds)
      {
        value *= 1000;
      }
      if (value < 0)
      {
        value = 0;
      }
      out = static_cast<uint32_t>(value);
      return true;
    }

#if defined(ESP32SERIALCTL_HAS_STORAGE)
    struct StorageEntry
    {
      const char *name;
      const char *description;
      fs::FS *fs;
      uint64_t (*totalBytes)();
      uint64_t (*usedBytes)();
    };

    static const StorageEntry kStorageEntries[];
    static const size_t kStorageCount;
    static int currentStorageIndex_;

    static int findStorageIndex(const char *name)
    {
      if (!name || !*name)
      {
        return -1;
      }
      for (size_t i = 0; i < kStorageCount; ++i)
      {
        if (Base::equalsIgnoreCase(name, kStorageEntries[i].name))
        {
          return static_cast<int>(i);
        }
      }
      return -1;
    }

    static bool checkStorageMounted(fs::FS &fs)
    {
      File root = fs.open("/");
      if (!root)
      {
        return false;
      }
      root.close();
      return true;
    }

    static bool getStorageEntry(Context &ctx, int index,
                                const StorageEntry *&entryOut, fs::FS *&fsOut,
                                bool requireMounted)
    {
      if (kStorageCount == 0)
      {
        ctx.printError(404, "No storage backends");
        return false;
      }
      if (index < 0)
      {
        ctx.printError(412, "No storage selected");
        return false;
      }
      if (static_cast<size_t>(index) >= kStorageCount)
      {
        ctx.printError(404, "Unknown storage");
        return false;
      }
      entryOut = &kStorageEntries[index];
      fsOut = entryOut->fs;
      if (!fsOut)
      {
        ctx.printError(500, "Storage backend unavailable");
        return false;
      }
      if (requireMounted && !checkStorageMounted(*fsOut))
      {
        char message[96];
        snprintf(message, sizeof(message), "%s not mounted", entryOut->name);
        ctx.printError(503, message);
        return false;
      }
      return true;
    }

    static const typename Base::Option *findStorageOption(Context &ctx)
    {
      const typename Base::Option *opt = ctx.findOption("storage");
      if (opt)
      {
        return opt;
      }
      return ctx.findOption("store");
    }

    static bool resolveStorageForFs(Context &ctx, const StorageEntry *&entryOut,
                                    fs::FS *&fsOut)
    {
      const typename Base::Option *opt = findStorageOption(ctx);
      if (opt && (!opt->value || !*opt->value))
      {
        ctx.printError(400, "Storage name required");
        return false;
      }

      int index = -1;
      if (opt && opt->value)
      {
        index = findStorageIndex(opt->value);
        if (index < 0)
        {
          ctx.printError(404, "Unknown storage");
          return false;
        }
      }
      else
      {
        index = currentStorageIndex_;
      }

      if (!getStorageEntry(ctx, index, entryOut, fsOut, true))
      {
        return false;
      }
      return true;
    }

    static bool parseSizeArgument(Context &ctx, const typename Base::Argument &arg,
                                  uint64_t &out, const char *message)
    {
      ParsedNumber number;
      if (!arg.toNumber(number) || number.unit != NumberUnit::None || number.value < 0)
      {
        ctx.printError(400, message);
        return false;
      }
      out = static_cast<uint64_t>(number.value);
      return true;
    }

    static const char *resolvePathArgument(const typename Base::Argument &arg,
                                           const char *fallback)
    {
      return arg.empty() ? fallback : arg.c_str();
    }

    static bool parseSizeOption(Context &ctx, const typename Base::Option *opt,
                                uint64_t &out, const char *message, bool &present)
    {
      if (!opt || !opt->value)
      {
        present = false;
        return true;
      }
      present = true;
      ParsedNumber number;
      if (!Base::parseNumber(opt->value, number) || number.unit != NumberUnit::None ||
          number.value < 0)
      {
        ctx.printError(400, message);
        return false;
      }
      out = static_cast<uint64_t>(number.value);
      return true;
    }

    template <typename T>
    struct RemoveReference
    {
      using type = T;
    };
    template <typename T>
    struct RemoveReference<T &>
    {
      using type = T;
    };
    template <typename T>
    struct RemoveReference<T &&>
    {
      using type = T;
    };

    template <typename FS>
    static constexpr bool storageHasTotalBytes(decltype(&FS::totalBytes))
    {
      return true;
    }

    template <typename FS>
    static constexpr bool storageHasTotalBytes(...)
    {
      return false;
    }

    template <typename FS>
    static constexpr bool storageHasUsedBytes(decltype(&FS::usedBytes))
    {
      return true;
    }

    template <typename FS>
    static constexpr bool storageHasUsedBytes(...)
    {
      return false;
    }

    template <typename FS>
    static auto detectTotalBytes(FS &fs, int)
        -> decltype(static_cast<uint64_t>(fs.totalBytes()))
    {
      return static_cast<uint64_t>(fs.totalBytes());
    }

    template <typename FS>
    static uint64_t detectTotalBytes(FS &, ...)
    {
      return 0;
    }

    template <typename FS>
    static auto detectUsedBytes(FS &fs, int)
        -> decltype(static_cast<uint64_t>(fs.usedBytes()))
    {
      return static_cast<uint64_t>(fs.usedBytes());
    }

    template <typename FS>
    static uint64_t detectUsedBytes(FS &, ...)
    {
      return 0;
    }

#if defined(ESP32SERIALCTL_HAS_SD)
    using SdStorageType = typename RemoveReference<decltype(SD)>::type;
    static constexpr bool kSdHasTotalBytes = storageHasTotalBytes<SdStorageType>(nullptr);
    static constexpr bool kSdHasUsedBytes = storageHasUsedBytes<SdStorageType>(nullptr);

    static uint64_t storageSdTotalBytes()
    {
      return detectTotalBytes(SD, 0);
    }

    static uint64_t storageSdUsedBytes()
    {
      return detectUsedBytes(SD, 0);
    }
#endif

#if defined(ESP32SERIALCTL_HAS_SPIFFS)
    using SpiffsStorageType = typename RemoveReference<decltype(SPIFFS)>::type;
    static constexpr bool kSpiffsHasTotalBytes =
        storageHasTotalBytes<SpiffsStorageType>(nullptr);
    static constexpr bool kSpiffsHasUsedBytes =
        storageHasUsedBytes<SpiffsStorageType>(nullptr);

    static uint64_t storageSpiffsTotalBytes()
    {
      return detectTotalBytes(SPIFFS, 0);
    }

    static uint64_t storageSpiffsUsedBytes()
    {
      return detectUsedBytes(SPIFFS, 0);
    }
#endif

#if defined(ESP32SERIALCTL_HAS_LITTLEFS)
    using LittleFsStorageType = typename RemoveReference<decltype(LittleFS)>::type;
    static constexpr bool kLittleFsHasTotalBytes =
        storageHasTotalBytes<LittleFsStorageType>(nullptr);
    static constexpr bool kLittleFsHasUsedBytes =
        storageHasUsedBytes<LittleFsStorageType>(nullptr);

    static uint64_t storageLittleFsTotalBytes()
    {
      return detectTotalBytes(LittleFS, 0);
    }

    static uint64_t storageLittleFsUsedBytes()
    {
      return detectUsedBytes(LittleFS, 0);
    }
#endif

#if defined(ESP32SERIALCTL_HAS_FFAT)
    using FatFsStorageType = typename RemoveReference<decltype(FFat)>::type;
    static constexpr bool kFatFsHasTotalBytes =
        storageHasTotalBytes<FatFsStorageType>(nullptr);
    static constexpr bool kFatFsHasUsedBytes =
        storageHasUsedBytes<FatFsStorageType>(nullptr);

    static uint64_t storageFatFsTotalBytes()
    {
      return detectTotalBytes(FFat, 0);
    }

    static uint64_t storageFatFsUsedBytes()
    {
      return detectUsedBytes(FFat, 0);
    }
#endif

    static void handleStorageList(Context &ctx)
    {
      ctx.printOK("storage list");
      if (kStorageCount == 0)
      {
        ctx.printBody("No storage backends");
        return;
      }
      for (size_t i = 0; i < kStorageCount; ++i)
      {
        const StorageEntry &entry = kStorageEntries[i];
        fs::FS *fs = entry.fs;
        const bool mounted = fs && checkStorageMounted(*fs);
        char line[128];
        if (entry.description && *entry.description)
        {
          snprintf(line, sizeof(line), "%s (%s) [%s]", entry.name, entry.description, mounted ? "mounted" : "not mounted");
        }
        else
        {
          snprintf(line, sizeof(line), "%s [%s]", entry.name, mounted ? "mounted" : "not mounted");
        }
        ctx.printBody(line);
      }
    }

    static void handleStorageUse(Context &ctx)
    {
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: storage use <name>");
        return;
      }
      const char *name = ctx.arg(0).c_str();
      const int index = findStorageIndex(name);
      if (index < 0)
      {
        ctx.printError(404, "Unknown storage");
        return;
      }
      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!getStorageEntry(ctx, index, entry, fs, true))
      {
        return;
      }
      currentStorageIndex_ = index;
      ctx.printOK("storage use");
      char line[96];
      snprintf(line, sizeof(line), "current: %s", entry->name);
      ctx.printBody(line);
    }

    static bool resolveStorageForStatus(Context &ctx, const StorageEntry *&entryOut,
                                        fs::FS *&fsOut)
    {
      if (ctx.argc() > 1)
      {
        ctx.printError(400, "Usage: storage status [name]");
        return false;
      }
      int index = currentStorageIndex_;
      if (ctx.argc() == 1)
      {
        index = findStorageIndex(ctx.arg(0).c_str());
        if (index < 0)
        {
          ctx.printError(404, "Unknown storage");
          return false;
        }
      }
      if (!getStorageEntry(ctx, index, entryOut, fsOut, false))
      {
        return false;
      }
      return true;
    }

    static void handleStorageStatus(Context &ctx)
    {
      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!resolveStorageForStatus(ctx, entry, fs))
      {
        return;
      }
      const bool mounted = fs && checkStorageMounted(*fs);
      ctx.printOK("storage status");
      char line[128];
      snprintf(line, sizeof(line), "storage: %s", entry->name);
      ctx.printBody(line);
      ctx.printBody(mounted ? "mounted: yes" : "mounted: no");
      if (!mounted)
      {
        return;
      }

      if (!entry->totalBytes || !entry->usedBytes)
      {
        ctx.printBody("capacity: unavailable");
        return;
      }
      const uint64_t total = entry->totalBytes();
      const uint64_t used = entry->usedBytes();
      if (total == 0 && used == 0)
      {
        ctx.printBody("capacity: unavailable");
        return;
      }
      const uint64_t freeBytes = (total >= used) ? (total - used) : 0;

      snprintf(line, sizeof(line), "total: %llu bytes",
               static_cast<unsigned long long>(total));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "used: %llu bytes",
               static_cast<unsigned long long>(used));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "free: %llu bytes",
               static_cast<unsigned long long>(freeBytes));
      ctx.printBody(line);
    }

    static void handleFsLs(Context &ctx)
    {
      if (ctx.argc() > 1)
      {
        ctx.printError(400, "Usage: fs ls [path]");
        return;
      }
      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!resolveStorageForFs(ctx, entry, fs))
      {
        return;
      }

      const char *path = (ctx.argc() == 0) ? "/" : resolvePathArgument(ctx.arg(0), "/");
      File dir = fs->open(path);
      if (!dir)
      {
        ctx.printError(404, "Path not found");
        return;
      }

      ctx.printOK("fs ls");
      char header[160];
      snprintf(header, sizeof(header), "storage: %s path: %s", entry->name, path);
      ctx.printBody(header);

      if (!dir.isDirectory())
      {
        char line[160];
        snprintf(line, sizeof(line), "file %s (%llu bytes)", dir.name(),
                 static_cast<unsigned long long>(dir.size()));
        ctx.printBody(line);
        dir.close();
        return;
      }

      bool any = false;
      while (true)
      {
        File child = dir.openNextFile();
        if (!child)
        {
          break;
        }
        char line[160];
        if (child.isDirectory())
        {
          snprintf(line, sizeof(line), "<DIR> %s", child.name());
        }
        else
        {
          snprintf(line, sizeof(line), "%10llu %s",
                   static_cast<unsigned long long>(child.size()), child.name());
        }
        ctx.printBody(line);
        child.close();
        any = true;
      }
      if (!any)
      {
        ctx.printBody("(empty)");
      }
      dir.close();
    }

    static void handleFsCat(Context &ctx)
    {
      if (ctx.argc() < 1 || ctx.argc() > 3)
      {
        ctx.printError(400, "Usage: fs cat <path> [offset] [len]");
        return;
      }
      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!resolveStorageForFs(ctx, entry, fs))
      {
        return;
      }

      const char *path = ctx.arg(0).c_str();
      uint64_t offset = 0;
      uint64_t length = 0;
      bool limit = false;

      if (ctx.argc() >= 2 &&
          !parseSizeArgument(ctx, ctx.arg(1), offset, "Invalid offset"))
      {
        return;
      }
      if (ctx.argc() == 3 &&
          !parseSizeArgument(ctx, ctx.arg(2), length, "Invalid length"))
      {
        return;
      }
      limit = (ctx.argc() == 3);

      File file = fs->open(path, FILE_READ);
      if (!file)
      {
        ctx.printError(404, "File not found");
        return;
      }
      if (offset > 0 && !file.seek(offset))
      {
        file.close();
        ctx.printError(400, "Seek failed");
        return;
      }

      ctx.printOK("fs cat");
      char info[160];
      snprintf(info, sizeof(info), "storage: %s path: %s offset: %llu",
               entry->name, path, static_cast<unsigned long long>(offset));
      ctx.printBody(info);
      if (limit)
      {
        snprintf(info, sizeof(info), "length: %llu bytes",
                 static_cast<unsigned long long>(length));
        ctx.printBody(info);
      }
      else
      {
        ctx.printBody("length: until EOF");
      }

      static const size_t kChunkSize = 96;
      uint8_t buffer[kChunkSize];
      uint64_t remaining = length;
      const bool limited = limit;
      bool lineOpen = false;
      bool hasData = false;
      Print &out = ctx.controller().output();

      auto openLine = [&]()
      {
        if (!lineOpen)
        {
          out.print("| ");
          lineOpen = true;
        }
      };
      auto closeLine = [&]()
      {
        if (lineOpen)
        {
          out.print("\r\n");
          lineOpen = false;
        }
      };

      while (file.available())
      {
        if (limited && remaining == 0)
        {
          break;
        }
        size_t toRead = kChunkSize;
        if (limited && remaining < toRead)
        {
          toRead = static_cast<size_t>(remaining);
        }
        size_t readBytes = file.read(buffer, toRead);
        if (readBytes == 0)
        {
          break;
        }
        if (limited)
        {
          remaining -= readBytes;
        }
        for (size_t i = 0; i < readBytes; ++i)
        {
          const uint8_t ch = buffer[i];
          if (ch == '\r')
          {
            continue;
          }
          if (ch == '\n')
          {
            closeLine();
            hasData = true;
            continue;
          }
          openLine();
          hasData = true;
          if (ch == '\t')
          {
            out.print("\\t");
          }
          else if (isprint(ch))
          {
            out.write(ch);
          }
          else
          {
            char hex[5];
            snprintf(hex, sizeof(hex), "\\x%02X",
                     static_cast<unsigned int>(ch));
            out.print(hex);
          }
        }
      }
      closeLine();
      file.close();
      if (!hasData)
      {
        ctx.printBody("(no data)");
      }
    }

    static void handleFsB64Read(Context &ctx)
    {
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: fs b64read <path> [--offset N] [--len N] [--chunk N]");
        return;
      }
      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!resolveStorageForFs(ctx, entry, fs))
      {
        return;
      }

      uint64_t offset = 0;
      uint64_t length = 0;
      bool limit = false;
      bool hasOffset = false;
      bool hasLength = false;

      if (!parseSizeOption(ctx, ctx.findOption("offset"), offset, "Invalid offset",
                           hasOffset))
      {
        return;
      }
      if (!parseSizeOption(ctx, ctx.findOption("len"), length, "Invalid length",
                           hasLength))
      {
        return;
      }
      limit = hasLength;

      uint64_t chunkOpt = 0;
      bool hasChunk = false;
      if (!parseSizeOption(ctx, ctx.findOption("chunk"), chunkOpt, "Invalid chunk size",
                           hasChunk))
      {
        return;
      }
      size_t chunkSize = hasChunk ? static_cast<size_t>(chunkOpt) : 48U;
      if (chunkSize == 0U || chunkSize > 192U)
      {
        ctx.printError(400, "Chunk must be 1..192");
        return;
      }

      const char *path = ctx.arg(0).c_str();
      File file = fs->open(path, FILE_READ);
      if (!file)
      {
        ctx.printError(404, "File not found");
        return;
      }
      if (hasOffset && offset > 0 && !file.seek(offset))
      {
        file.close();
        ctx.printError(400, "Seek failed");
        return;
      }

      ctx.printOK("fs b64read");
      char info[280];
      snprintf(info, sizeof(info), "storage: %s path: %s", entry->name, path);
      ctx.printBody(info);
      if (hasOffset)
      {
        snprintf(info, sizeof(info), "offset: %llu",
                 static_cast<unsigned long long>(offset));
        ctx.printBody(info);
      }
      if (limit)
      {
        snprintf(info, sizeof(info), "length: %llu",
                 static_cast<unsigned long long>(length));
        ctx.printBody(info);
      }
      snprintf(info, sizeof(info), "chunk: %u",
               static_cast<unsigned int>(chunkSize));
      ctx.printBody(info);

      uint8_t buffer[192];
      char encoded[260];
      size_t chunkIndex = 0;
      uint64_t remaining = length;
      const bool limited = limit;

      while (file.available())
      {
        if (limited && remaining == 0)
        {
          break;
        }
        size_t toRead = chunkSize;
        if (limited && remaining < toRead)
        {
          toRead = static_cast<size_t>(remaining);
        }
        size_t readBytes = file.read(buffer, toRead);
        if (readBytes == 0)
        {
          break;
        }

        size_t encodedLen = 0;
        if (mbedtls_base64_encode(reinterpret_cast<unsigned char *>(encoded),
                                  sizeof(encoded) - 1, &encodedLen, buffer, readBytes) !=
            0)
        {
          file.close();
          ctx.printError(500, "Base64 encode failed");
          return;
        }
        encoded[encodedLen] = '\0';

        snprintf(info, sizeof(info), "data[%u]: %s",
                 static_cast<unsigned int>(chunkIndex), encoded);
        ctx.printBody(info);

        if (limited)
        {
          remaining -= readBytes;
        }
        ++chunkIndex;
      }

      file.close();
      snprintf(info, sizeof(info), "chunks: %u", static_cast<unsigned int>(chunkIndex));
      ctx.printBody(info);
    }

    static void handleFsB64Write(Context &ctx)
    {
      if (ctx.argc() < 2)
      {
        ctx.printError(400, "Usage: fs b64write <path> <base64...> [--append]");
        return;
      }
      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!resolveStorageForFs(ctx, entry, fs))
      {
        return;
      }

      const char *path = ctx.arg(0).c_str();
      const bool append = ctx.hasOption("append");

      char base64Data[MaxLineLength + 1];
      size_t dataLen = 0;
      base64Data[0] = '\0';

      for (size_t i = 1; i < ctx.argc(); ++i)
      {
        const typename Base::Argument &arg = ctx.arg(i);
        for (size_t j = 0; j < arg.size(); ++j)
        {
          const char ch = arg.c_str()[j];
          if (isspace(static_cast<unsigned char>(ch)))
          {
            continue;
          }
          if (dataLen + 1 >= sizeof(base64Data))
          {
            ctx.printError(413, "Input too long");
            return;
          }
          base64Data[dataLen++] = ch;
        }
      }
      base64Data[dataLen] = '\0';

      if (dataLen == 0)
      {
        ctx.printError(400, "No data");
        return;
      }

      const size_t decodedCapacity =
          static_cast<size_t>(((dataLen + 3U) / 4U) * 3U + 4U);
      uint8_t decoded[((MaxLineLength + 3U) / 4U) * 3U + 4U];
      if (decodedCapacity > sizeof(decoded))
      {
        ctx.printError(413, "Data too long");
        return;
      }
      size_t decodedLen = 0;
      int rc = mbedtls_base64_decode(decoded, sizeof(decoded), &decodedLen,
                                     reinterpret_cast<const unsigned char *>(base64Data),
                                     dataLen);
      if (rc == MBEDTLS_ERR_BASE64_INVALID_CHARACTER)
      {
        ctx.printError(400, "Invalid base64");
        return;
      }
      if (rc != 0)
      {
        ctx.printError(500, "Base64 decode failed");
        return;
      }

      File file = fs->open(path, append ? "a" : "w");
      if (!file)
      {
        ctx.printError(500, "Open failed");
        return;
      }
      const size_t written = file.write(decoded, decodedLen);
      file.close();
      if (written != decodedLen)
      {
        ctx.printError(500, "Write failed");
        return;
      }

      ctx.printOK("fs b64write");
      char line[160];
      snprintf(line, sizeof(line), "storage: %s path: %s", entry->name, path);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "bytes: %u append: %s",
               static_cast<unsigned int>(written), append ? "yes" : "no");
      ctx.printBody(line);
    }

    static void handleFsWrite(Context &ctx)
    {
      if (ctx.argc() != 2)
      {
        ctx.printError(400, "Usage: fs write <path> \"<data>\"");
        return;
      }
      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!resolveStorageForFs(ctx, entry, fs))
      {
        return;
      }
      const char *path = ctx.arg(0).c_str();
      const char *data = ctx.arg(1).c_str();
      File file = fs->open(path, "w");
      if (!file)
      {
        ctx.printError(500, "Write failed");
        return;
      }
      const size_t length = strlen(data);
      const size_t written = file.write(reinterpret_cast<const uint8_t *>(data), length);
      file.close();
      if (written != length)
      {
        ctx.printError(500, "Write failed");
        return;
      }
      ctx.printOK("fs write");
      char line[160];
      snprintf(line, sizeof(line), "storage: %s path: %s bytes: %u",
               entry->name, path, static_cast<unsigned int>(written));
      ctx.printBody(line);
    }

    static void handleFsRm(Context &ctx)
    {
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: fs rm <path>");
        return;
      }
      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!resolveStorageForFs(ctx, entry, fs))
      {
        return;
      }
      const char *path = ctx.arg(0).c_str();
      File target = fs->open(path);
      if (!target)
      {
        ctx.printError(404, "Path not found");
        return;
      }
      const bool isDir = target.isDirectory();
      target.close();

      bool ok = false;
      if (isDir)
      {
        ok = fs->rmdir(path);
      }
      else
      {
        ok = fs->remove(path);
      }
      if (!ok)
      {
        ctx.printError(500, "Delete failed");
        return;
      }

      ctx.printOK("fs rm");
      char line[160];
      snprintf(line, sizeof(line), "storage: %s path: %s", entry->name, path);
      ctx.printBody(line);
      ctx.printBody(isDir ? "type: directory" : "type: file");
    }

    static void handleFsStat(Context &ctx)
    {
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: fs stat <path>");
        return;
      }
      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!resolveStorageForFs(ctx, entry, fs))
      {
        return;
      }
      const char *path = ctx.arg(0).c_str();
      File file = fs->open(path);
      if (!file)
      {
        ctx.printError(404, "Path not found");
        return;
      }

      ctx.printOK("fs stat");
      char line[160];
      snprintf(line, sizeof(line), "storage: %s path: %s", entry->name, path);
      ctx.printBody(line);
      ctx.printBody(file.isDirectory() ? "type: directory" : "type: file");
      snprintf(line, sizeof(line), "size: %llu bytes",
               static_cast<unsigned long long>(file.size()));
      ctx.printBody(line);
      const uint64_t modified = file.getLastWrite();
      if (modified > 0)
      {
        snprintf(line, sizeof(line), "modified: %llu",
                 static_cast<unsigned long long>(modified));
        ctx.printBody(line);
      }
      file.close();
    }

    static void handleFsMkdir(Context &ctx)
    {
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: fs mkdir <path>");
        return;
      }
      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!resolveStorageForFs(ctx, entry, fs))
      {
        return;
      }
      const char *path = ctx.arg(0).c_str();
      if (!fs->mkdir(path))
      {
        ctx.printError(500, "mkdir failed");
        return;
      }
      ctx.printOK("fs mkdir");
      char line[160];
      snprintf(line, sizeof(line), "storage: %s path: %s", entry->name, path);
      ctx.printBody(line);
    }

    static void handleFsMv(Context &ctx)
    {
      if (ctx.argc() != 2)
      {
        ctx.printError(400, "Usage: fs mv <src> <dst>");
        return;
      }
      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!resolveStorageForFs(ctx, entry, fs))
      {
        return;
      }
      const char *src = ctx.arg(0).c_str();
      const char *dst = ctx.arg(1).c_str();
      if (!fs->rename(src, dst))
      {
        ctx.printError(500, "rename failed");
        return;
      }
      ctx.printOK("fs mv");
      char line[160];
      snprintf(line, sizeof(line), "storage: %s", entry->name);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "src: %s", src);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "dst: %s", dst);
      ctx.printBody(line);
    }

    enum class HashAlgorithm : uint8_t
    {
      Sha256 = 0,
      Md5
    };

    static bool parseHashAlgorithmOption(Context &ctx, HashAlgorithm &algo)
    {
      const typename Base::Option *opt = ctx.findOption("algo");
      if (!opt || !opt->value)
      {
        algo = HashAlgorithm::Sha256;
        return true;
      }
      if (Base::equalsIgnoreCase(opt->value, "sha256"))
      {
        algo = HashAlgorithm::Sha256;
        return true;
      }
      if (Base::equalsIgnoreCase(opt->value, "md5"))
      {
        algo = HashAlgorithm::Md5;
        return true;
      }
      ctx.printError(400, "Unknown algorithm");
      return false;
    }

    static void digestToHex(const unsigned char *digest, size_t length, char *out,
                            size_t outSize)
    {
      static const char hexDigits[] = "0123456789abcdef";
      size_t pos = 0;
      for (size_t i = 0; i < length && pos + 2 < outSize; ++i)
      {
        const unsigned char value = digest[i];
        out[pos++] = hexDigits[(value >> 4) & 0x0F];
        out[pos++] = hexDigits[value & 0x0F];
      }
      out[pos] = '\0';
    }

    static void handleFsHash(Context &ctx)
    {
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: fs hash <path> [--algo sha256|md5]");
        return;
      }

      HashAlgorithm algo;
      if (!parseHashAlgorithmOption(ctx, algo))
      {
        return;
      }

      const StorageEntry *entry = nullptr;
      fs::FS *fs = nullptr;
      if (!resolveStorageForFs(ctx, entry, fs))
      {
        return;
      }

      const char *path = ctx.arg(0).c_str();
      File file = fs->open(path, FILE_READ);
      if (!file)
      {
        ctx.printError(404, "Path not found");
        return;
      }
      if (file.isDirectory())
      {
        file.close();
        ctx.printError(400, "Path is directory");
        return;
      }

      const uint64_t fileSize = file.size();

      unsigned char digest[32];
      size_t digestLen = 0;
      uint8_t buffer[256];
      int rc = 0;

      if (algo == HashAlgorithm::Sha256)
      {
        mbedtls_sha256_context sha;
        mbedtls_sha256_init(&sha);
        rc = mbedtls_sha256_starts_ret(&sha, 0);
        while (rc == 0 && file.available())
        {
          const size_t readBytes = file.read(buffer, sizeof(buffer));
          if (readBytes == 0)
          {
            break;
          }
          rc = mbedtls_sha256_update_ret(&sha, buffer, readBytes);
        }
        if (rc == 0)
        {
          rc = mbedtls_sha256_finish_ret(&sha, digest);
        }
        mbedtls_sha256_free(&sha);
        digestLen = 32;
      }
      else
      {
        mbedtls_md5_context md5;
        mbedtls_md5_init(&md5);
        rc = mbedtls_md5_starts_ret(&md5);
        while (rc == 0 && file.available())
        {
          const size_t readBytes = file.read(buffer, sizeof(buffer));
          if (readBytes == 0)
          {
            break;
          }
          rc = mbedtls_md5_update_ret(&md5, buffer, readBytes);
        }
        if (rc == 0)
        {
          rc = mbedtls_md5_finish_ret(&md5, digest);
        }
        mbedtls_md5_free(&md5);
        digestLen = 16;
      }

      file.close();

      if (rc != 0)
      {
        ctx.printError(500, "Hash failed");
        return;
      }

      char hex[65];
      digestToHex(digest, digestLen, hex, sizeof(hex));

      ctx.printOK("fs hash");
      char line[160];
      snprintf(line, sizeof(line), "storage: %s path: %s", entry->name, path);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "algo: %s",
               (algo == HashAlgorithm::Sha256) ? "sha256" : "md5");
      ctx.printBody(line);
      snprintf(line, sizeof(line), "size: %llu bytes",
               static_cast<unsigned long long>(fileSize));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "digest: %s", hex);
      ctx.printBody(line);
    }
#endif // defined(ESP32SERIALCTL_HAS_STORAGE)

    static bool isGpioOutputEnabled(uint8_t pin)
    {
#if defined(SOC_GPIO_PIN_COUNT)
      if (pin >= SOC_GPIO_PIN_COUNT)
      {
        return false;
      }
#endif

      if (pin < 32)
      {
#if defined(GPIO_ENABLE_REG)
        uint32_t enable = REG_READ(GPIO_ENABLE_REG);
        return ((enable >> pin) & 0x1u) != 0;
#else
        return false;
#endif
      }

#if defined(GPIO_ENABLE1_REG)
      if (pin < 64)
      {
        uint32_t enable = REG_READ(GPIO_ENABLE1_REG);
        return ((enable >> (pin - 32)) & 0x1u) != 0;
      }
#endif

#if defined(GPIO_ENABLE2_REG)
      if (pin < 96)
      {
        uint32_t enable = REG_READ(GPIO_ENABLE2_REG);
        return ((enable >> (pin - 64)) & 0x1u) != 0;
      }
#endif

#if defined(GPIO_ENABLE3_REG)
      if (pin < 128)
      {
        uint32_t enable = REG_READ(GPIO_ENABLE3_REG);
        return ((enable >> (pin - 96)) & 0x1u) != 0;
      }
#endif

      return false;
    }

#if defined(ESP32SERIALCTL_HAS_WIRE)
    struct I2cBusEntry
    {
      TwoWire *wire;
      const char *name;
      uint8_t index;
    };

    static const I2cBusEntry *availableI2cBuses(size_t &count)
    {
      static const I2cBusEntry entries[] = {
          {&Wire, "Wire", 0},
#if defined(ESP32SERIALCTL_HAS_WIRE1)
          {&Wire1, "Wire1", 1},
#endif
#if defined(ESP32SERIALCTL_HAS_WIRE2)
          {&Wire2, "Wire2", 2},
#endif
      };
      count = sizeof(entries) / sizeof(entries[0]);
      count = count > 0 ? count : 0;
      return entries;
    }

    static const I2cBusEntry *resolveI2cBus(Context &ctx)
    {
      size_t busCount = 0;
      const I2cBusEntry *buses = availableI2cBuses(busCount);
      if (busCount == 0 || !buses)
      {
        ctx.printError(500, "I2C unavailable");
        return nullptr;
      }

      const typename Base::Option *busOpt = ctx.findOption("bus");
      if (!busOpt || !busOpt->value || !*busOpt->value)
      {
        return &buses[0];
      }

      const char *value = busOpt->value;
      for (size_t i = 0; i < busCount; ++i)
      {
        if (Base::equalsIgnoreCase(value, buses[i].name))
        {
          return &buses[i];
        }
        char alias[8];
        if (buses[i].index == 0)
        {
          if (Base::equalsIgnoreCase(value, "wire0") ||
              Base::equalsIgnoreCase(value, "default") ||
              Base::equalsIgnoreCase(value, "0"))
          {
            return &buses[i];
          }
        }
        else
        {
          snprintf(alias, sizeof(alias), "wire%u",
                   static_cast<unsigned int>(buses[i].index));
          if (Base::equalsIgnoreCase(value, alias))
          {
            return &buses[i];
          }
        }
      }

      ParsedNumber number;
      if (Base::parseNumber(value, number) &&
          number.unit == NumberUnit::None)
      {
        for (size_t i = 0; i < busCount; ++i)
        {
          if (number.value == static_cast<int64_t>(buses[i].index))
          {
            return &buses[i];
          }
        }
      }

      ctx.printError(404, "Unknown bus");
      return nullptr;
    }

    static bool parseI2cAddress(Context &ctx, const typename Base::Argument &arg, uint8_t &address)
    {
      ParsedNumber number;
      if (!arg.toNumber(number) || number.unit != NumberUnit::None ||
          number.value < 0 || number.value > 0x7F)
      {
        ctx.printError(400, "Invalid address");
        return false;
      }
      address = static_cast<uint8_t>(number.value);
      return true;
    }

    static bool parseI2cRegister(Context &ctx, const typename Base::Argument &arg, uint8_t &reg)
    {
      return parseUint8Component(ctx, arg, reg, "Invalid register");
    }

    static bool parseI2cLength(Context &ctx, const typename Base::Argument &arg, size_t &length)
    {
      ParsedNumber number;
      if (!arg.toNumber(number) || number.unit != NumberUnit::None ||
          number.value <= 0 || number.value > 32)
      {
        ctx.printError(400, "Invalid length");
        return false;
      }
      length = static_cast<size_t>(number.value);
      return true;
    }

    static void printI2cTransmissionError(Context &ctx, uint8_t code, const char *stage)
    {
      const char *reason = "unknown error";
      switch (code)
      {
      case 1:
        reason = "data too long";
        break;
      case 2:
        reason = "nack on address";
        break;
      case 3:
        reason = "nack on data";
        break;
      case 4:
        reason = "bus error";
        break;
      case 5:
        reason = "timeout";
        break;
      default:
        break;
      }
      char line[80];
      if (stage && *stage)
      {
        snprintf(line, sizeof(line), "%s failed: %s (%u)", stage, reason, static_cast<unsigned int>(code));
      }
      else
      {
        snprintf(line, sizeof(line), "I2C error: %s (%u)", reason, static_cast<unsigned int>(code));
      }
      ctx.printError(502, line);
    }

    static void handleI2cScan(Context &ctx)
    {
      size_t busCount = 0;
      const I2cBusEntry *buses = availableI2cBuses(busCount);
      if (busCount == 0)
      {
        ctx.printError(500, "I2C unavailable");
        return;
      }

      ctx.printOK("i2c scan");
      auto scanBus = [&](const I2cBusEntry &bus)
      {
        char line[48];
        snprintf(line, sizeof(line), "bus: %s (%u)", bus.name,
                 static_cast<unsigned int>(bus.index));
        ctx.printBody(line);

        bool any = false;
        for (uint8_t addr = 0x03; addr <= 0x77; ++addr)
        {
          bus.wire->beginTransmission(addr);
          uint8_t error = bus.wire->endTransmission();
          if (error == 0)
          {
            snprintf(line, sizeof(line), "addr: 0x%02X",
                     static_cast<unsigned int>(addr));
            ctx.printList(line);
            any = true;
          }
        }

        if (!any)
        {
          ctx.printBody("no devices");
        }
      };

      const typename Base::Option *busOpt = ctx.findOption("bus");
      if (busOpt && busOpt->value && *busOpt->value)
      {
        const I2cBusEntry *selected = resolveI2cBus(ctx);
        if (!selected)
        {
          return;
        }
        scanBus(*selected);
        return;
      }

      for (size_t i = 0; i < busCount; ++i)
      {
        scanBus(buses[i]);
      }
    }

    static void handleI2cRead(Context &ctx)
    {
      if (ctx.argc() < 2 || ctx.argc() > 3)
      {
        ctx.printError(400, "Usage: i2c read [--bus name|index] <addr> <reg> [len]");
        return;
      }

      const I2cBusEntry *bus = resolveI2cBus(ctx);
      if (!bus)
      {
        return;
      }
      TwoWire &i2c = *bus->wire;

      uint8_t address;
      if (!parseI2cAddress(ctx, ctx.arg(0), address))
      {
        return;
      }

      uint8_t reg;
      if (!parseI2cRegister(ctx, ctx.arg(1), reg))
      {
        return;
      }

      size_t length = 1;
      if (ctx.argc() == 3 && !parseI2cLength(ctx, ctx.arg(2), length))
      {
        return;
      }

      i2c.beginTransmission(address);
      i2c.write(reg);
      uint8_t error = i2c.endTransmission(false);
      if (error != 0)
      {
        char stage[48];
        snprintf(stage, sizeof(stage), "register write (bus %s)", bus->name);
        printI2cTransmissionError(ctx, error, stage);
        return;
      }

      size_t received =
          i2c.requestFrom(static_cast<uint8_t>(address),
                          static_cast<uint8_t>(length), true);
      if (received != length)
      {
        ctx.printError(504, "Incomplete read");
        while (i2c.available() > 0)
        {
          i2c.read();
        }
        return;
      }

      uint8_t buffer[32];
      for (size_t i = 0; i < length; ++i)
      {
        int value = i2c.read();
        if (value < 0)
        {
          ctx.printError(504, "Read failed");
          return;
        }
        buffer[i] = static_cast<uint8_t>(value);
      }

      char dataLine[3 * 32 + 1];
      size_t pos = 0;
      const char *hex = "0123456789ABCDEF";
      for (size_t i = 0; i < length && pos + 2 < sizeof(dataLine); ++i)
      {
        if (i > 0 && pos < sizeof(dataLine) - 1)
        {
          dataLine[pos++] = ' ';
        }
        if (pos + 2 >= sizeof(dataLine))
        {
          break;
        }
        dataLine[pos++] = hex[buffer[i] >> 4];
        dataLine[pos++] = hex[buffer[i] & 0x0F];
      }
      dataLine[pos] = '\0';

      ctx.printOK("i2c read");
      char line[128];
      snprintf(line, sizeof(line), "bus: %s (%u)", bus->name,
               static_cast<unsigned int>(bus->index));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "addr: 0x%02X", static_cast<unsigned int>(address));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "reg: 0x%02X", static_cast<unsigned int>(reg));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "len: %u", static_cast<unsigned int>(length));
      ctx.printBody(line);
      if (length > 0)
      {
        snprintf(line, sizeof(line), "data: %s", dataLine);
        ctx.printBody(line);
      }
    }

    static void handleI2cWrite(Context &ctx)
    {
      if (ctx.argc() < 3)
      {
        ctx.printError(400, "Usage: i2c write [--bus name|index] <addr> <reg> <bytes...>");
        return;
      }

      const I2cBusEntry *bus = resolveI2cBus(ctx);
      if (!bus)
      {
        return;
      }
      TwoWire &i2c = *bus->wire;

      uint8_t address;
      if (!parseI2cAddress(ctx, ctx.arg(0), address))
      {
        return;
      }

      uint8_t reg;
      if (!parseI2cRegister(ctx, ctx.arg(1), reg))
      {
        return;
      }

      const size_t dataCount = ctx.argc() - 2;
      if (dataCount > 32)
      {
        ctx.printError(413, "Too many bytes");
        return;
      }
      uint8_t data[32];
      for (size_t i = 0; i < dataCount; ++i)
      {
        if (!parseUint8Component(ctx, ctx.arg(i + 2), data[i], "Invalid data byte"))
        {
          return;
        }
      }

      i2c.beginTransmission(address);
      i2c.write(reg);
      for (size_t i = 0; i < dataCount; ++i)
      {
        i2c.write(data[i]);
      }
      uint8_t error = i2c.endTransmission(true);
      if (error != 0)
      {
        char stage[40];
        snprintf(stage, sizeof(stage), "write (bus %s)", bus->name);
        printI2cTransmissionError(ctx, error, stage);
        return;
      }

      char bytesLine[3 * 32 + 1];
      size_t pos = 0;
      const char *hex = "0123456789ABCDEF";
      for (size_t i = 0; i < dataCount && pos + 2 < sizeof(bytesLine); ++i)
      {
        if (i > 0 && pos < sizeof(bytesLine) - 1)
        {
          bytesLine[pos++] = ' ';
        }
        if (pos + 2 >= sizeof(bytesLine))
        {
          break;
        }
        bytesLine[pos++] = hex[data[i] >> 4];
        bytesLine[pos++] = hex[data[i] & 0x0F];
      }
      bytesLine[pos] = '\0';

      ctx.printOK("i2c write");
      char line[128];
      snprintf(line, sizeof(line), "bus: %s (%u)", bus->name,
               static_cast<unsigned int>(bus->index));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "addr: 0x%02X", static_cast<unsigned int>(address));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "reg: 0x%02X", static_cast<unsigned int>(reg));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "bytes: %s", bytesLine);
      ctx.printBody(line);
    }
#endif

    static constexpr uint8_t kPwmResolutionBits = 8;

    static void handleSysInfo(Context &ctx)
    {
      char buffer[128];

      ctx.printOK("sys info");

#ifdef ARDUINO_BOARD
      ctx.printBody("Board: " ARDUINO_BOARD);
#endif
#ifdef ARDUINO_VARIANT
      ctx.printBody("Variant: " ARDUINO_VARIANT);
#endif
#ifdef ARDUINO_ARCH
      ctx.printBody("Architecture: " ARDUINO_ARCH);
#endif
#ifdef ARDUINO_FQBN
      ctx.printBody("FQBN: " ARDUINO_FQBN);
#endif

      const char *model = ESP.getChipModel();
      if (!model)
      {
        model = "ESP32";
      }
      snprintf(buffer, sizeof(buffer), "Chip: %s rev %d", model,
               ESP.getChipRevision());
      ctx.printBody(buffer);

      snprintf(buffer, sizeof(buffer), "Cores: %d", ESP.getChipCores());
      ctx.printBody(buffer);
      snprintf(buffer, sizeof(buffer), "CPU Frequency: %ld MHz",
               static_cast<long>(ESP.getCpuFreqMHz()));
      ctx.printBody(buffer);

      esp_chip_info_t chipInfo;
      esp_chip_info(&chipInfo);
      int featureLen = snprintf(buffer, sizeof(buffer), "Chip Features: WiFi");
      if (featureLen < 0)
      {
        featureLen = 0;
      }
#if defined(CHIP_FEATURE_BT) || defined(CHIP_FEATURE_BLE) || defined(CHIP_FEATURE_IEEE802154)
      auto appendFeatureSuffix = [&](const char *suffix)
      {
        if (!suffix)
        {
          return;
        }
        if (featureLen >= static_cast<int>(sizeof(buffer)))
        {
          return;
        }
        size_t remaining = sizeof(buffer) - static_cast<size_t>(featureLen);
        int written = snprintf(buffer + featureLen, remaining, "%s", suffix);
        if (written > 0)
        {
          featureLen += written;
        }
      };
#endif
#if defined(CHIP_FEATURE_BT)
      if (chipInfo.features & CHIP_FEATURE_BT)
      {
        appendFeatureSuffix("/BT");
      }
#endif
#if defined(CHIP_FEATURE_BLE)
      if (chipInfo.features & CHIP_FEATURE_BLE)
      {
        appendFeatureSuffix("/BLE");
      }
#endif
#if defined(CHIP_FEATURE_IEEE802154)
      if (chipInfo.features & CHIP_FEATURE_IEEE802154)
      {
        appendFeatureSuffix("/802.15.4");
      }
#endif
      ctx.printBody(buffer);

      snprintf(buffer, sizeof(buffer), "GPIO Count: %d",
               static_cast<uint8_t>(GPIO_PIN_COUNT));
      ctx.printBody(buffer);

#ifdef LED_BUILTIN
      snprintf(buffer, sizeof(buffer), "LED BUILTIN: %d",
               static_cast<uint8_t>(LED_BUILTIN));
      ctx.printBody(buffer);
#endif
#ifdef RGB_BUILTIN
      snprintf(buffer, sizeof(buffer), "RGB BUILTIN: %d",
               static_cast<uint8_t>(RGB_BUILTIN));
      ctx.printBody(buffer);
#endif

      snprintf(buffer, sizeof(buffer), "Flash: %lu bytes @ %lu Hz",
               static_cast<unsigned long>(ESP.getFlashChipSize()),
               static_cast<unsigned long>(ESP.getFlashChipSpeed()));
      ctx.printBody(buffer);

      const uint64_t mac = ESP.getEfuseMac();
      snprintf(buffer, sizeof(buffer), "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
               static_cast<unsigned int>((mac >> 40) & 0xFF),
               static_cast<unsigned int>((mac >> 32) & 0xFF),
               static_cast<unsigned int>((mac >> 24) & 0xFF),
               static_cast<unsigned int>((mac >> 16) & 0xFF),
               static_cast<unsigned int>((mac >> 8) & 0xFF),
               static_cast<unsigned int>(mac & 0xFF));
      ctx.printBody(buffer);

      const char *idf = esp_get_idf_version();
      snprintf(buffer, sizeof(buffer), "IDF: %s", idf ? idf : "unknown");
      ctx.printBody(buffer);

#ifdef ARDUINO_ESP32_RELEASE
      ctx.printBody("Arduino Core: " ARDUINO_ESP32_RELEASE);
#endif

      snprintf(buffer, sizeof(buffer), "Build: %s %s", __DATE__, __TIME__);
      ctx.printBody(buffer);
    }

    static void handleSysUptime(Context &ctx)
    {
      char buffer[48];
      const unsigned long ms = millis();
      const unsigned long seconds = ms / 1000UL;
      const unsigned long minutes = seconds / 60UL;
      const unsigned long hours = minutes / 60UL;
      ctx.printOK("sys uptime");
      snprintf(buffer, sizeof(buffer), "Uptime: %lu:%02lu:%02lu (%lu ms)", hours,
               minutes % 60UL, seconds % 60UL, ms);
      ctx.printBody(buffer);
    }

    static bool formatLocalTimeIso(time_t now, char *iso, size_t size)
    {
      if (!iso || size == 0)
      {
        return false;
      }

      struct tm localTm;
#if defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP32)
      if (!localtime_r(&now, &localTm))
      {
        return false;
      }
#else
      struct tm *tmp = localtime(&now);
      if (!tmp)
      {
        return false;
      }
      localTm = *tmp;
#endif

      char datePart[32];
      if (strftime(datePart, sizeof(datePart), "%Y-%m-%dT%H:%M:%S", &localTm) == 0)
      {
        return false;
      }

      char zone[8] = {0};
      const size_t zoneLen = strftime(zone, sizeof(zone), "%z", &localTm);

      if (zoneLen == 5)
      {
        char zoneFormatted[8];
        zoneFormatted[0] = zone[0];
        zoneFormatted[1] = zone[1];
        zoneFormatted[2] = zone[2];
        zoneFormatted[3] = ':';
        zoneFormatted[4] = zone[3];
        zoneFormatted[5] = zone[4];
        zoneFormatted[6] = '\0';
        snprintf(iso, size, "%s%s", datePart, zoneFormatted);
      }
      else if (zoneLen > 0)
      {
        snprintf(iso, size, "%s%s", datePart, zone);
      }
      else
      {
        snprintf(iso, size, "%sZ", datePart);
      }
      return true;
    }

    static bool parseLocalDateTime(const char *text, struct tm &out)
    {
      if (!text)
      {
        return false;
      }

      const size_t len = strlen(text);
      if (len != 19)
      {
        return false;
      }

      const char sepDate = text[10];
      if (text[4] != '-' || text[7] != '-' ||
          (sepDate != 'T' && sepDate != ' ') ||
          text[13] != ':' || text[16] != ':')
      {
        return false;
      }

      auto parseDigits = [](const char *ptr, size_t length) -> int
      {
        int value = 0;
        for (size_t i = 0; i < length; ++i)
        {
          char ch = ptr[i];
          if (ch < '0' || ch > '9')
          {
            return -1;
          }
          value = value * 10 + (ch - '0');
        }
        return value;
      };

      const int year = parseDigits(text, 4);
      const int month = parseDigits(text + 5, 2);
      const int day = parseDigits(text + 8, 2);
      const int hour = parseDigits(text + 11, 2);
      const int minute = parseDigits(text + 14, 2);
      const int second = parseDigits(text + 17, 2);

      if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31 ||
          hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59)
      {
        return false;
      }

      struct tm tmValue;
      memset(&tmValue, 0, sizeof(tmValue));
      tmValue.tm_year = year - 1900;
      tmValue.tm_mon = month - 1;
      tmValue.tm_mday = day;
      tmValue.tm_hour = hour;
      tmValue.tm_min = minute;
      tmValue.tm_sec = second;
      tmValue.tm_isdst = -1;
      out = tmValue;
      return true;
    }

    static bool setSystemTime(time_t epoch)
    {
#if defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP32) || defined(__unix__)
      struct timeval tv;
      tv.tv_sec = epoch;
      tv.tv_usec = 0;
      return settimeofday(&tv, nullptr) == 0;
#elif defined(CLOCK_REALTIME)
      struct timespec ts;
      ts.tv_sec = epoch;
      ts.tv_nsec = 0;
      return clock_settime(CLOCK_REALTIME, &ts) == 0;
#else
      (void)epoch;
      return false;
#endif
    }

    static void handleSysTime(Context &ctx)
    {
      if (ctx.argc() > 1)
      {
        ctx.printError(400, "Usage: sys time [YYYY-MM-DDTHH:MM:SS]");
        return;
      }

      if (ctx.argc() == 1)
      {
        struct tm parsed;
        if (!parseLocalDateTime(ctx.arg(0).c_str(), parsed))
        {
          ctx.printError(400, "Invalid datetime (use YYYY-MM-DDTHH:MM:SS)");
          return;
        }

        const time_t epoch = mktime(&parsed);
        if (epoch == static_cast<time_t>(-1))
        {
          ctx.printError(400, "Datetime out of range");
          return;
        }

        if (!setSystemTime(epoch))
        {
          ctx.printError(500, "Failed to set time");
          return;
        }
      }

      time_t now = time(nullptr);
      if (now == static_cast<time_t>(-1))
      {
        ctx.printError(500, "time unavailable");
        return;
      }

      char iso[48];
      if (!formatLocalTimeIso(now, iso, sizeof(iso)))
      {
        ctx.printError(500, "format failed");
        return;
      }

      ctx.printOK("sys time");
      char line[64];
      snprintf(line, sizeof(line), "localtime: %s", iso);
      ctx.printBody(line);
    }

#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
    static void printTimezone(Context &ctx)
    {
      const char *current = getenv("TZ");
      if (!current || !*current)
      {
        current = kPrefsDefaultTimezone;
      }
      ctx.printOK("sys timezone");
      char line[64];
      snprintf(line, sizeof(line), "tz: %s", (*current) ? current : "(none)");
      ctx.printBody(line);
    }

    static void handleSysTimezone(Context &ctx)
    {
      if (ctx.argc() == 0)
      {
        printTimezone(ctx);
        return;
      }
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: sys timezone [tz]");
        return;
      }

      const auto &arg = ctx.arg(0);
      const char *tz = arg.c_str();
      if (!tz || !*tz)
      {
        ctx.printError(400, "Timezone required");
        return;
      }

      if (!Base::applyTimezoneEnv(tz))
      {
        ctx.printError(500, "Failed to apply timezone");
        return;
      }

      if (!ctx.controller().saveTimezone(tz))
      {
        ctx.printError(507, "Failed to save timezone");
        return;
      }

      ctx.printOK("sys timezone");
      char line[64];
      snprintf(line, sizeof(line), "tz: %s", tz);
      ctx.printBody(line);
    }
#endif

#if defined(ESP32SERIALCTL_HAS_WIFI) && defined(ESP32SERIALCTL_HAS_PREFERENCES)
    static bool parseWifiIndex(Context &ctx, const typename Base::Argument &arg, size_t &indexOut)
    {
      ParsedNumber number;
      if (!arg.toNumber(number) || number.unit != NumberUnit::None || number.value < 0 ||
          number.value > static_cast<int64_t>(ESP32SERIALCTL_WIFI_MAX_NETWORKS))
      {
        ctx.printError(400, "Invalid index");
        return false;
      }
      indexOut = static_cast<size_t>(number.value);
      return true;
    }

    static void handleWifiAuto(Context &ctx)
    {
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: wifi auto <on|off>");
        return;
      }
      bool enable;
      const auto &arg = ctx.arg(0);
      if (arg.equals("on"))
      {
        enable = true;
      }
      else if (arg.equals("off"))
      {
        enable = false;
      }
      else if (!arg.toBool(enable))
      {
        ctx.printError(400, "Expected on/off");
        return;
      }

      if (!ctx.controller().wifiSetAutoPreference(enable))
      {
        ctx.printError(507, "Failed to update preference");
        return;
      }

      ctx.printOK("wifi auto");
      char line[48];
      snprintf(line, sizeof(line), "auto: %s", enable ? "on" : "off");
      ctx.printBody(line);
    }

    static void handleWifiList(Context &ctx)
    {
      typename Base::WifiStoredNetwork networks[ESP32SERIALCTL_WIFI_MAX_NETWORKS];
      const size_t count = ctx.controller().wifiLoadStoredNetworks(networks);
      ctx.printOK("wifi list");
      if (count == 0)
      {
        ctx.printBody("entries: 0");
        ctx.printBody("note: no networks stored");
        return;
      }
      char line[64];
      snprintf(line, sizeof(line), "entries: %u", static_cast<unsigned>(count));
      ctx.printBody(line);
      for (size_t i = 0; i < count; ++i)
      {
        String entry;
        entry.reserve(32 + networks[i].ssid.length());
        entry += '#';
        entry += static_cast<unsigned>(i);
        entry += " slot:";
        entry += static_cast<unsigned>(networks[i].slot);
        entry += " ssid:";
        entry += networks[i].ssid;
        ctx.printBody(entry.c_str());
      }
    }

    static void handleWifiAdd(Context &ctx)
    {
      if (ctx.argc() < 2)
      {
        ctx.printError(400, "Usage: wifi add <ssid> <key>");
        return;
      }
      const char *ssid = ctx.arg(0).c_str();
      const char *key = ctx.arg(1).c_str();
      if (!ssid || !*ssid)
      {
        ctx.printError(400, "SSID required");
        return;
      }

      size_t slot = 0;
      if (!ctx.controller().wifiFindFreeSlot(slot))
      {
        ctx.printError(409, "Storage full");
        return;
      }
      if (!ctx.controller().wifiStoreNetwork(slot, ssid, key))
      {
        ctx.printError(507, "Failed to store network");
        return;
      }

      typename Base::WifiStoredNetwork networks[ESP32SERIALCTL_WIFI_MAX_NETWORKS];
      const size_t count = ctx.controller().wifiLoadStoredNetworks(networks);
      size_t listIndex = count > 0 ? count - 1 : 0;
      for (size_t i = 0; i < count; ++i)
      {
        if (networks[i].slot == slot)
        {
          listIndex = i;
          break;
        }
      }

      ctx.printOK("wifi add");
      char line[64];
      snprintf(line, sizeof(line), "index: %u slot: %u",
               static_cast<unsigned>(listIndex),
               static_cast<unsigned>(slot));
      ctx.printBody(line);
    }

    static void handleWifiDel(Context &ctx)
    {
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: wifi del <index>");
        return;
      }
      typename Base::WifiStoredNetwork networks[ESP32SERIALCTL_WIFI_MAX_NETWORKS];
      size_t count = ctx.controller().wifiLoadStoredNetworks(networks);
      if (count == 0)
      {
        ctx.printError(404, "No networks stored");
        return;
      }
      size_t index = 0;
      if (!parseWifiIndex(ctx, ctx.arg(0), index) || index >= count)
      {
        ctx.printError(404, "Index not found");
        return;
      }
      const size_t slot = networks[index].slot;
      if (!ctx.controller().wifiClearNetwork(slot))
      {
        ctx.printError(507, "Failed to delete entry");
        return;
      }

      ctx.printOK("wifi del");
      char line[64];
      snprintf(line, sizeof(line), "removed: %u", static_cast<unsigned>(index));
      ctx.printBody(line);
    }

    static void handleWifiConnect(Context &ctx)
    {
      if (ctx.argc() > 2)
      {
        ctx.printError(400, "Usage: wifi connect [ssid] [key]");
        return;
      }

      typename Base::WifiStoredNetwork networks[ESP32SERIALCTL_WIFI_MAX_NETWORKS];
      const size_t storedCount = ctx.controller().wifiLoadStoredNetworks(networks);

      String tempSsid;
      String tempKey;
      if (ctx.argc() >= 1)
      {
        tempSsid = ctx.arg(0).c_str();
        if (tempSsid.length() == 0)
        {
          ctx.printError(400, "SSID required");
          return;
        }
        tempKey = ctx.argc() == 2 ? ctx.arg(1).c_str() : "";
      }
      else if (storedCount == 0)
      {
        ctx.printError(404, "No networks stored");
        return;
      }

      ctx.controller().wifiConfigureStationMode(true);
      WiFi.disconnect(true);
      delay(20);
      ctx.controller().wifiResetMulti();

      if (tempSsid.length() > 0)
      {
        ctx.controller().wifiMulti.addAP(tempSsid.c_str(), tempKey.c_str());
      }
      ctx.controller().wifiPopulateFromStored(networks, storedCount);

      const bool connected =
          ctx.controller().wifiAttemptConnection(static_cast<uint32_t>(ESP32SERIALCTL_WIFI_CONNECT_TIMEOUT_MS));
      wl_status_t status = WiFi.status();
      if (!connected)
      {
        ctx.printError(504, "Connect timeout");
        char line[96];
        snprintf(line, sizeof(line), "status: %s", Base::wifiStatusToString(status));
        ctx.printBody(line);
        return;
      }

      ctx.printOK("wifi connect");
      char line[96];
      snprintf(line, sizeof(line), "status: %s", Base::wifiStatusToString(status));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "ssid: %s", WiFi.SSID().c_str());
      ctx.printBody(line);
      snprintf(line, sizeof(line), "ip: %s", WiFi.localIP().toString().c_str());
      ctx.printBody(line);
      snprintf(line, sizeof(line), "bssid: %s", WiFi.BSSIDstr().c_str());
      ctx.printBody(line);
      snprintf(line, sizeof(line), "rssi: %d dBm", WiFi.RSSI());
      ctx.printBody(line);
      ctx.controller().ntpReconfigureIfNeeded();
    }

    static void handleWifiDisconnect(Context &ctx)
    {
      if (ctx.argc() != 0)
      {
        ctx.printError(400, "Usage: wifi disconnect");
        return;
      }
      WiFi.disconnect(true);
      ctx.controller().wifiResetMulti();
      ctx.printOK("wifi disconnect");
      ctx.printBody("status: disconnected");
    }

    static void handleWifiStatus(Context &ctx)
    {
      if (ctx.argc() != 0)
      {
        ctx.printError(400, "Usage: wifi status");
        return;
      }

      wl_status_t status = WiFi.status();

      ctx.printOK("wifi status");
      char line[96];
      snprintf(line, sizeof(line), "status: %s", Base::wifiStatusToString(status));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "auto: %s", ctx.controller().wifiAutoPreference() ? "on" : "off");
      ctx.printBody(line);
      snprintf(line, sizeof(line), "ssid: %s", WiFi.SSID().c_str());
      ctx.printBody(line);
      snprintf(line, sizeof(line), "ip: %s", WiFi.localIP().toString().c_str());
      ctx.printBody(line);
      snprintf(line, sizeof(line), "mac: %s", WiFi.macAddress().c_str());
      ctx.printBody(line);
      snprintf(line, sizeof(line), "bssid: %s", WiFi.BSSIDstr().c_str());
      ctx.printBody(line);
      const long channel = static_cast<long>(WiFi.channel());
      snprintf(line, sizeof(line), "channel: %ld", channel);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "rssi: %d dBm", WiFi.RSSI());
      ctx.printBody(line);
    }

    static const char *ntpSyncStatusToString(sntp_sync_status_t status)
    {
      switch (status)
      {
      case SNTP_SYNC_STATUS_RESET:
        return "reset";
      case SNTP_SYNC_STATUS_COMPLETED:
        return "completed";
      case SNTP_SYNC_STATUS_IN_PROGRESS:
        return "in_progress";
      default:
        return "unknown";
      }
    }

    static void handleNtpStatus(Context &ctx)
    {
      if (ctx.argc() != 0)
      {
        ctx.printError(400, "Usage: ntp status");
        return;
      }

      bool autoMode = ctx.controller().ntpAutoPreference();
      bool enabled = ctx.controller().ntpEnabledPreference();
      String servers[ESP32SERIALCTL_NTP_MAX_SERVERS];
      size_t serverCount = ctx.controller().ntpLoadServers(servers);

      ctx.printOK("ntp status");
      char line[128];
      snprintf(line, sizeof(line), "auto: %s", autoMode ? "on" : "off");
      ctx.printBody(line);
      snprintf(line, sizeof(line), "enabled: %s", enabled ? "on" : "off");
      ctx.printBody(line);
      const String &tz = ctx.controller().currentTimezone();
      snprintf(line, sizeof(line), "timezone: %s",
               tz.length() > 0 ? tz.c_str() : "(unset - use sys timezone)");
      ctx.printBody(line);
      for (size_t i = 0; i < ESP32SERIALCTL_NTP_MAX_SERVERS; ++i)
      {
        if (servers[i].length() == 0)
        {
          continue;
        }
        snprintf(line, sizeof(line), "server%u: %s",
                 static_cast<unsigned>(i), servers[i].c_str());
        ctx.printBody(line);
      }
      if (serverCount == 0)
      {
        snprintf(line, sizeof(line), "server0: %s", ESP32SERIALCTL_DEFAULT_NTP_SERVER);
        ctx.printBody(line);
      }
      const bool running = esp_sntp_enabled();
      snprintf(line, sizeof(line), "running: %s", running ? "yes" : "no");
      ctx.printBody(line);
      snprintf(line, sizeof(line), "sync: %s",
               ntpSyncStatusToString(sntp_get_sync_status()));
      ctx.printBody(line);
      struct tm nowLocal = {};
      if (getLocalTime(&nowLocal, 1000))
      {
        time_t nowEpoch = mktime(&nowLocal);
        char iso[48];
        if (formatLocalTimeIso(nowEpoch, iso, sizeof(iso)))
        {
          char syncLine[64];
          snprintf(syncLine, sizeof(syncLine), "last_sync: %s", iso);
          ctx.printBody(syncLine);
        }
      }
    }

    static void handleNtpSet(Context &ctx)
    {
      if (ctx.argc() < 1 || ctx.argc() > static_cast<int>(ESP32SERIALCTL_NTP_MAX_SERVERS))
      {
        ctx.printError(400, "Usage: ntp set <server> [server2] [server3]");
        return;
      }
      String servers[ESP32SERIALCTL_NTP_MAX_SERVERS];
      const size_t provided = ctx.argc();
      for (size_t i = 0; i < ESP32SERIALCTL_NTP_MAX_SERVERS; ++i)
      {
        if (i < provided)
        {
          servers[i] = ctx.arg(i).c_str();
        }
        else
        {
          servers[i] = "";
        }
      }
      if (servers[0].length() == 0)
      {
        ctx.printError(400, "Primary server required");
        return;
      }
      if (!ctx.controller().ntpStoreServers(servers))
      {
        ctx.printError(507, "Failed to store servers");
        return;
      }
      ctx.controller().ntpReconfigureIfNeeded();

      ctx.printOK("ntp set");
      char line[128];
      for (size_t i = 0; i < ESP32SERIALCTL_NTP_MAX_SERVERS; ++i)
      {
        if (servers[i].length() == 0)
        {
          continue;
        }
        snprintf(line, sizeof(line), "server%u: %s",
                 static_cast<unsigned>(i), servers[i].c_str());
        ctx.printBody(line);
      }
    }

    static void handleNtpEnable(Context &ctx)
    {
      if (ctx.argc() != 0)
      {
        ctx.printError(400, "Usage: ntp enable");
        return;
      }
      const String &tz = ctx.controller().currentTimezone();
      if (tz.length() == 0)
      {
        ctx.printError(412, "Set timezone first via sys timezone");
        return;
      }
      if (!ctx.controller().ntpConfigure(true))
      {
        ctx.printError(500, "Failed to configure NTP");
        return;
      }
      if (!ctx.controller().ntpSetEnabledPreference(true))
      {
        ctx.controller().ntpConfigure(false);
        ctx.printError(507, "Failed to enable NTP");
        return;
      }
      ctx.printOK("ntp enable");
      ctx.printBody("status: enabled");
      if (ctx.controller().ntpWaitForSync(30000))
      {
        struct tm nowLocal = {};
        if (getLocalTime(&nowLocal, 1000))
        {
          time_t nowEpoch = mktime(&nowLocal);
          char iso[48];
          if (formatLocalTimeIso(nowEpoch, iso, sizeof(iso)))
          {
            char syncLine[64];
            snprintf(syncLine, sizeof(syncLine), "synced: %s", iso);
            ctx.printBody(syncLine);
          }
        }
      }
      else
      {
        ctx.printBody("synced: pending");
      }
    }

    static void handleNtpDisable(Context &ctx)
    {
      if (ctx.argc() != 0)
      {
        ctx.printError(400, "Usage: ntp disable");
        return;
      }
      if (!ctx.controller().ntpSetEnabledPreference(false))
      {
        ctx.printError(507, "Failed to disable NTP");
        return;
      }
      ctx.controller().ntpConfigure(false);
      ctx.printOK("ntp disable");
      ctx.printBody("status: disabled");
    }

    static void handleNtpAuto(Context &ctx)
    {
      if (ctx.argc() != 1)
      {
        ctx.printError(400, "Usage: ntp auto <on|off>");
        return;
      }
      bool enable;
      const auto &arg = ctx.arg(0);
      if (arg.equals("on"))
      {
        enable = true;
      }
      else if (arg.equals("off"))
      {
        enable = false;
      }
      else if (!arg.toBool(enable))
      {
        ctx.printError(400, "Expected on/off");
        return;
      }
      if (!ctx.controller().ntpSetAutoPreference(enable))
      {
        ctx.printError(507, "Failed to update preference");
        return;
      }
      if (enable)
      {
        ctx.controller().ntpReconfigureIfNeeded();
      }
      ctx.printOK("ntp auto");
      char line[48];
      snprintf(line, sizeof(line), "auto: %s", enable ? "on" : "off");
      ctx.printBody(line);
    }
#endif

    static void handleSysMem(Context &ctx)
    {
      char lines[24][96];
      size_t lineCount = 0;

      auto addLine = [&](const char *fmt, ...)
      {
        if (lineCount >= (sizeof(lines) / sizeof(lines[0])))
        {
          return;
        }
        va_list args;
        va_start(args, fmt);
        vsnprintf(lines[lineCount], sizeof(lines[lineCount]), fmt, args);
        va_end(args);
        ++lineCount;
      };

      const size_t heapTotal = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
      if (heapTotal > 0U)
      {
        addLine("Heap Total: %lu bytes", static_cast<unsigned long>(heapTotal));
      }

      const size_t heapFree = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
      addLine("Heap Free: %lu bytes", static_cast<unsigned long>(heapFree));

      const size_t heapMinFree = heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);
      addLine("Heap Min Free: %lu bytes", static_cast<unsigned long>(heapMinFree));

      const size_t heapLargest = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT);
      addLine("Heap Largest Block: %lu bytes", static_cast<unsigned long>(heapLargest));

      const size_t internalTotal = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
      if (internalTotal > 0U)
      {
        addLine("Internal Total: %lu bytes", static_cast<unsigned long>(internalTotal));

        const size_t internalFree = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        addLine("Internal Free: %lu bytes", static_cast<unsigned long>(internalFree));

        const size_t internalMin = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);
        addLine("Internal Min Free: %lu bytes", static_cast<unsigned long>(internalMin));

        const size_t internalLargest = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
        addLine("Internal Largest Block: %lu bytes",
                static_cast<unsigned long>(internalLargest));
      }

      const size_t psramTotal = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
      if (psramTotal > 0U)
      {
        addLine("PSRAM Total: %lu bytes", static_cast<unsigned long>(psramTotal));

        const size_t psramFree = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
        addLine("PSRAM Free: %lu bytes", static_cast<unsigned long>(psramFree));

        const size_t psramMin = heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM);
        addLine("PSRAM Min Free: %lu bytes", static_cast<unsigned long>(psramMin));

        const size_t psramLargest = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
        addLine("PSRAM Largest Block: %lu bytes",
                static_cast<unsigned long>(psramLargest));
      }

      const size_t rtosHeapFree = static_cast<size_t>(xPortGetFreeHeapSize());
      addLine("RTOS Heap Free: %lu bytes", static_cast<unsigned long>(rtosHeapFree));

      const size_t rtosHeapMin = static_cast<size_t>(xPortGetMinimumEverFreeHeapSize());
      addLine("RTOS Heap Min Free: %lu bytes", static_cast<unsigned long>(rtosHeapMin));

      const UBaseType_t stackHighWater = uxTaskGetStackHighWaterMark(nullptr);
      addLine("Task Stack High Water: %lu words", static_cast<unsigned long>(stackHighWater));

      ctx.printOK("sys mem");
      for (size_t i = 0; i < lineCount; ++i)
      {
        ctx.printBody(lines[i]);
      }
    }

    static void handleSysReset(Context &ctx)
    {
      ctx.printOK("sys reset");
      ctx.printBody("Restarting...");
      ctx.controller().output().flush();
      delay(50);

      if (rgbDefaultPin_ >= 0)
      {
        rgbLedWrite(rgbDefaultPin_, 0, 0, 0);
      }
      for (int i = 0; i < GPIO_PIN_COUNT; i++)
      {
        pinMode(i, INPUT);
      }

      ESP.restart();
    }

    static void handleHelp(Context &ctx)
    {
      if (ctx.argc() > 1)
      {
        ctx.printError(400, "Usage: help [topic]");
        return;
      }

      if (!ctx.arg(0).empty())
      {
        if (!ctx.controller().printHelp(ctx.arg(0).c_str()))
        {
          ctx.printError(404, "Unknown help topic");
        }
        return;
      }

      ctx.controller().printHelp();
    }

    static void handleGpioMode(Context &ctx)
    {
      if (ctx.argc() < 2)
      {
        ctx.printError(400, "Usage: gpio mode <pin> <mode>");
        return;
      }
      uint8_t pin;
      if (!parsePinArgument(ctx, ctx.arg(0), pin, "Invalid pin"))
      {
        return;
      }
      const typename Base::Argument &modeArg = ctx.arg(1);
      int mode = -1;
      const char *modeName = nullptr;
      if (modeArg.equals("in"))
      {
        mode = INPUT;
        modeName = "in";
      }
      else if (modeArg.equals("out"))
      {
        mode = OUTPUT;
        modeName = "out";
      }
      else if (modeArg.equals("pullup"))
      {
        mode = INPUT_PULLUP;
        modeName = "pullup";
      }
      else if (modeArg.equals("pulldown"))
      {
#ifdef INPUT_PULLDOWN
        mode = INPUT_PULLDOWN;
        modeName = "pulldown";
#else
        ctx.printError(501, "pulldown unsupported");
        return;
#endif
      }
      else if (modeArg.equals("opendrain"))
      {
#ifdef OUTPUT_OPEN_DRAIN
        mode = OUTPUT_OPEN_DRAIN;
        modeName = "opendrain";
#else
        ctx.printError(501, "opendrain unsupported");
        return;
#endif
      }

      if (mode < 0)
      {
        ctx.printError(400, "Invalid mode");
        return;
      }

      pinMode(pin, mode);
      ctx.printOK("gpio mode");
      char line[48];
      snprintf(line, sizeof(line), "pin: %u", pin);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "mode: %s", modeName);
      ctx.printBody(line);
    }

    static void handleGpioRead(Context &ctx)
    {
      if (ctx.argc() < 1)
      {
        ctx.printError(400, "Usage: gpio read <pin>");
        return;
      }
      uint8_t pin;
      if (!parsePinArgument(ctx, ctx.arg(0), pin, "Invalid pin"))
      {
        return;
      }
      int value = digitalRead(pin);
      ctx.printOK("gpio read");
      char line[48];
      snprintf(line, sizeof(line), "pin: %u", pin);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "value: %d", value);
      ctx.printBody(line);
    }

    static void handleGpioWrite(Context &ctx)
    {
      if (ctx.argc() < 2)
      {
        ctx.printError(400, "Usage: gpio write <pin> <value>");
        return;
      }
      uint8_t pin;
      if (!parsePinArgument(ctx, ctx.arg(0), pin, "Invalid pin"))
      {
        return;
      }
      bool state;
      if (!parseBooleanToken(ctx.arg(1), state))
      {
        ctx.printError(400, "Invalid value");
        return;
      }

      if (!isGpioOutputEnabled(pin))
      {
        pinMode(pin, OUTPUT);
        if (!isGpioOutputEnabled(pin))
        {
          ctx.printError(501, "Output unsupported");
          return;
        }
      }

      digitalWrite(pin, state ? HIGH : LOW);
      ctx.printOK("gpio write");
      char line[48];
      snprintf(line, sizeof(line), "pin: %u", pin);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "value: %s", state ? "high" : "low");
      ctx.printBody(line);
    }

    static void handleGpioToggle(Context &ctx)
    {
      if (ctx.argc() < 1)
      {
        ctx.printError(400, "Usage: gpio toggle <pin>");
        return;
      }
      uint8_t pin;
      if (!parsePinArgument(ctx, ctx.arg(0), pin, "Invalid pin"))
      {
        return;
      }

      if (!isGpioOutputEnabled(pin))
      {
        pinMode(pin, OUTPUT);
        if (!isGpioOutputEnabled(pin))
        {
          ctx.printError(501, "Output unsupported");
          return;
        }
      }

      int current = digitalRead(pin);
      int next = current ? LOW : HIGH;
      digitalWrite(pin, next);
      ctx.printOK("gpio toggle");
      char line[48];
      snprintf(line, sizeof(line), "pin: %u", pin);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "value: %s", next ? "high" : "low");
      ctx.printBody(line);
    }

    static void handleGpioPins(Context &ctx)
    {
      if (ctx.argc() > 0)
      {
        ctx.printError(400, "Usage: gpio pins");
        return;
      }

      ctx.printOK("gpio pins");

      char header[64];
      snprintf(header, sizeof(header), "mode: %s",
               pinAllAccess_ ? "all" : "restricted");
      ctx.printBody(header);

      if (pinAllAccess_)
      {
        ctx.printBody("allowed pins: all");
      }
      else
      {
        String allowedList;
        for (int i = 0; i < GPIO_PIN_COUNT; ++i)
        {
          if (!pinAllowed_[i])
          {
            continue;
          }
          if (allowedList.length() > 0)
          {
            allowedList += ", ";
          }
          allowedList += i;
        }
        char allowedLine[192];
        snprintf(allowedLine, sizeof(allowedLine), "allowed pins: %s",
                 allowedList.length() > 0 ? allowedList.c_str() : "(none)");
        ctx.printBody(allowedLine);
      }

      if (pinAllAccess_)
      {
        for (int i = 0; i < GPIO_PIN_COUNT; ++i)
        {
          const bool explicitAllowed = pinAllowed_[i];
          const String &name = pinNames_[i];
          if (!explicitAllowed && name.length() == 0)
          {
            continue;
          }
          char info[192];
          if (name.length() > 0)
          {
            snprintf(info, sizeof(info), "pin %2d: allow%s name: %.100s",
                     i,
                     explicitAllowed ? " (explicit)" : "",
                     name.c_str());
          }
          else
          {
            snprintf(info, sizeof(info), "pin %02d: allow (explicit)", i);
          }
          ctx.printBody(info);
        }
        return;
      }

      for (int i = 0; i < GPIO_PIN_COUNT; ++i)
      {
        const bool explicitAllowed = pinAllowed_[i];
        const bool allowed = pinAllAccess_ || explicitAllowed;
        const String &name = pinNames_[i];
        char info[192];
        if (name.length() > 0)
        {
          snprintf(info, sizeof(info), "pin %2d: %s%s name: %.100s",
                   i,
                   allowed ? "allow" : "deny",
                   explicitAllowed ? " (explicit)" : "",
                   name.c_str());
        }
        else
        {
          snprintf(info, sizeof(info), "pin %2d: %s%s",
                   i,
                   allowed ? "allow" : "deny",
                   explicitAllowed ? " (explicit)" : "");
        }
        ctx.printBody(info);
      }
    }

    static void handleAdcRead(Context &ctx)
    {
      if (ctx.argc() < 1)
      {
        ctx.printError(400, "Usage: adc read <pin> [samples N]");
        return;
      }
      uint8_t pin;
      if (!parsePinArgument(ctx, ctx.arg(0), pin, "Invalid pin"))
      {
        return;
      }
      uint32_t samples = 1;
      if (ctx.argc() >= 3 && ctx.arg(1).equals("samples"))
      {
        ParsedNumber number;
        if (!ctx.arg(2).toNumber(number) || number.unit != NumberUnit::None || number.value <= 0)
        {
          ctx.printError(400, "Invalid samples");
          return;
        }
        samples = static_cast<uint32_t>(number.value);
      }
      else if (ctx.argc() == 2 && !ctx.arg(1).equals("samples"))
      {
        ParsedNumber number;
        if (!ctx.arg(1).toNumber(number) || number.unit != NumberUnit::None || number.value <= 0)
        {
          ctx.printError(400, "Invalid samples");
          return;
        }
        samples = static_cast<uint32_t>(number.value);
      }

      uint64_t total = 0;
      for (uint32_t i = 0; i < samples; ++i)
      {
        total += static_cast<uint32_t>(analogRead(pin));
      }
      uint32_t average = static_cast<uint32_t>(total / samples);
      ctx.printOK("adc read");
      char line[64];
      snprintf(line, sizeof(line), "pin: %u", pin);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "samples: %lu", static_cast<unsigned long>(samples));
      ctx.printBody(line);
      snprintf(line, sizeof(line), "value: %lu", static_cast<unsigned long>(average));
      ctx.printBody(line);
    }

    static void handlePwmSet(Context &ctx)
    {
      if (ctx.argc() < 3)
      {
        ctx.printError(400, "Usage: pwm set <pin> <freq> <duty> [bits]");
        return;
      }
      uint8_t pin;
      if (!parsePinArgument(ctx, ctx.arg(0), pin, "Invalid pin"))
      {
        return;
      }
      uint32_t freqHz;
      if (!parseFrequencyArgument(ctx, ctx.arg(1), freqHz))
      {
        return;
      }
      uint8_t bits;
      if (!parseUint8Component(ctx, ctx.arg(3), bits, "Invalid bits"))
      {
        // Use default bits if not provided
        bits = kPwmResolutionBits;
      }
      uint32_t dutyValue = 0;
      if (!parseDutyArgument(ctx, ctx.arg(2), bits, dutyValue))
      {
        return;
      }

      uint32_t apb_hz = APB_CLK_FREQ;
#if __has_include("esp_clk.h")
      extern "C" uint32_t esp_clk_apb_freq(void);
      apb_hz = esp_clk_apb_freq();
#endif
      uint32_t max_freq = apb_hz / (1u << bits);
      uint32_t min_freq = ceil(apb_hz / ((1u << bits) * 1024.0));
      if (freqHz < min_freq || freqHz > max_freq)
      {
        ctx.printError(400, "Frequency out of range");
        char line[80];
        snprintf(line, sizeof(line), "Valid range: %lu Hz - %lu Hz",
                 static_cast<unsigned long>(min_freq),
                 static_cast<unsigned long>(max_freq));
        ctx.printBody(line);
        return;
      }

      if (ledcReadFreq(pin) != 0)
      {
        ledcDetach(pin);
      }
      ledcAttach(pin, freqHz, bits);
      if (!ledcWrite(pin, dutyValue))
      {
        ctx.printError(500, "pwm write failed");
        return;
      }

      ctx.printOK("pwm set");
      char line[64];
      snprintf(line, sizeof(line), "pin: %u", pin);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "freq: %lu Hz", freqHz);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "bits: %u", bits);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "duty: %lu / %lu",
               static_cast<unsigned long>(dutyValue),
               static_cast<unsigned long>(1u << bits));
      ctx.printBody(line);
    }

    static void handlePwmStop(Context &ctx)
    {
      if (ctx.argc() < 1)
      {
        ctx.printError(400, "Usage: pwm stop <pin>");
        return;
      }
      uint8_t pin;
      if (!parsePinArgument(ctx, ctx.arg(0), pin, "Invalid pin"))
      {
        return;
      }

      if (ledcReadFreq(pin) != 0)
      {
        ledcDetach(pin);
        digitalWrite(pin, LOW);
      }
      ctx.printOK("pwm stop");
      char line[48];
      snprintf(line, sizeof(line), "pin: %u", pin);
      ctx.printBody(line);
    }

    static void handleRgbPin(Context &ctx)
    {
      if (ctx.argc() < 1)
      {
        ctx.printError(400, "Usage: rgb pin <pin>");
        return;
      }
      uint8_t pin;
      if (!parsePinArgument(ctx, ctx.arg(0), pin, "Invalid pin"))
      {
        return;
      }
      rgbDefaultPin_ = pin;
      ctx.printOK("rgb pin");
      char line[48];
      snprintf(line, sizeof(line), "default: %u", pin);
      ctx.printBody(line);
    }

    static void rgbWriteRepeated(int pin, uint8_t r, uint8_t g, uint8_t b,
                                 uint32_t count, uint32_t waitUs)
    {
      for (uint32_t i = 0; i < count; ++i)
      {
        rgbLedWrite(pin, r, g, b);
        if (waitUs > 0 && i + 1 < count)
        {
          delayMicroseconds(waitUs);
        }
      }
    }

    static void handleRgbSet(Context &ctx)
    {
      if (ctx.argc() < 3)
      {
        ctx.printError(400, "Usage: rgb set [--pin pin] [--count N] [--wait us] <r> <g> <b>");
        return;
      }
      int pin = resolveRgbPin(ctx);
      if (pin < 0)
      {
        return;
      }

      uint8_t r, g, b;
      if (!parseUint8Component(ctx, ctx.arg(0), r, "Invalid red") ||
          !parseUint8Component(ctx, ctx.arg(1), g, "Invalid green") ||
          !parseUint8Component(ctx, ctx.arg(2), b, "Invalid blue"))
      {
        return;
      }

      uint32_t count = parseCountOption(ctx, "count", 1);
      if (count == 0)
      {
        return;
      }
      uint32_t waitUs = 0;
      if (!resolveWaitOption(ctx, 100, waitUs))
      {
        return;
      }

      rgbWriteRepeated(pin, r, g, b, count, waitUs);

      ctx.printOK("rgb set");
      char line[64];
      snprintf(line, sizeof(line), "pin: %d", pin);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "rgb: %u %u %u", r, g, b);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "count: %lu wait: %lu us",
               static_cast<unsigned long>(count),
               static_cast<unsigned long>(waitUs));
      ctx.printBody(line);
    }

    static void handleRgbStream(Context &ctx)
    {
      if (ctx.argc() < 3 || (ctx.argc() % 3) != 0)
      {
        ctx.printError(400, "Usage: rgb stream [--pin pin] [--wait us] <r g b>...");
        return;
      }
      int pin = resolveRgbPin(ctx);
      if (pin < 0)
      {
        return;
      }
      uint32_t waitUs = 0;
      if (!resolveWaitOption(ctx, 100, waitUs))
      {
        return;
      }

      const size_t colorCount = ctx.argc() / 3;
      uint8_t colors[64][3];
      if (colorCount > 64)
      {
        ctx.printError(413, "Too many colors");
        return;
      }
      for (size_t i = 0; i < colorCount; ++i)
      {
        if (!parseUint8Component(ctx, ctx.arg(i * 3 + 0), colors[i][0], "Invalid red") ||
            !parseUint8Component(ctx, ctx.arg(i * 3 + 1), colors[i][1], "Invalid green") ||
            !parseUint8Component(ctx, ctx.arg(i * 3 + 2), colors[i][2], "Invalid blue"))
        {
          return;
        }
      }

      for (size_t i = 0; i < colorCount; ++i)
      {
        rgbLedWrite(pin, colors[i][0], colors[i][1], colors[i][2]);
        if (waitUs > 0 && i + 1 < colorCount)
        {
          delayMicroseconds(waitUs);
        }
      }

      ctx.printOK("rgb stream");
      char line[64];
      snprintf(line, sizeof(line), "pin: %d", pin);
      ctx.printBody(line);
      snprintf(line, sizeof(line), "colors: %u wait: %lu us",
               static_cast<unsigned int>(colorCount),
               static_cast<unsigned long>(waitUs));
      ctx.printBody(line);
    }

    static const Command kCommands[];
    static const size_t kCommandCount;

    // Active commands pointer (may point to built-in kCommands or to a
    // combined array created by registerCommands). Managed per-template
    // instantiation.
    static const Command *activeCommands_;
    static size_t activeCommandCount_;

    // Map from activeCommands_ index to CommandEntry (nullptr for built-ins)
    static const CommandEntry **commandEntryMap_;

    static bool pinAllAccess_;
    static bool pinAllowed_[GPIO_PIN_COUNT];
    static String pinNames_[GPIO_PIN_COUNT];

    static int rgbDefaultPin_;
    static const ConfigEntry *configEntries_;
    static size_t configEntryCount_;
    static String configNamespace_;

    Base cli_;
  };

  // Static storage for activeCommands_ and map will be initialized after
  // kCommands array is defined.

  template <size_t MaxLineLength, size_t MaxTokens>
  const typename ESP32SerialCtl<MaxLineLength, MaxTokens>::Command
      ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands[] = {
          {"sys", "info", &ESP32SerialCtl::handleSysInfo,
           ": Show board and firmware information"},
          {"sys", "uptime", &ESP32SerialCtl::handleSysUptime,
           ": Show system uptime"},
          {"sys", "time", &ESP32SerialCtl::handleSysTime,
           "[datetime] : Show or set local time (ISO 8601)"},
          {"sys", "mem", &ESP32SerialCtl::handleSysMem,
           ": Show heap and PSRAM usage"},
#if defined(ESP32SERIALCTL_HAS_PREFERENCES)
          {"sys", "timezone", &ESP32SerialCtl::handleSysTimezone,
           "[tz] : Show or set timezone"},
#endif
          {"sys", "reset", &ESP32SerialCtl::handleSysReset,
           ": Software reset"},
          {"conf", "list", &ESP32SerialCtl::handleConfList,
           "[--lang code] : List configurable entries"},
          {"conf", "get", &ESP32SerialCtl::handleConfGet,
           "<name> : Show config value and source"},
          {"conf", "set", &ESP32SerialCtl::handleConfSet,
           "<name> <value> : Store config value"},
          {"conf", "del", &ESP32SerialCtl::handleConfDel,
           "<name> : Remove stored config value"},
#if defined(ESP32SERIALCTL_HAS_WIFI) && defined(ESP32SERIALCTL_HAS_PREFERENCES)
          {"wifi", "auto", &ESP32SerialCtl::handleWifiAuto,
           "<on|off> : Enable or disable automatic Wi-Fi connect"},
          {"wifi", "list", &ESP32SerialCtl::handleWifiList,
           ": List stored Wi-Fi networks"},
          {"wifi", "add", &ESP32SerialCtl::handleWifiAdd,
           "<ssid> <key> : Store Wi-Fi credentials"},
          {"wifi", "del", &ESP32SerialCtl::handleWifiDel,
           "<index> : Remove stored Wi-Fi credentials"},
          {"wifi", "connect", &ESP32SerialCtl::handleWifiConnect,
           "[ssid] [key] : Connect using stored credentials or overrides"},
          {"wifi", "disconnect", &ESP32SerialCtl::handleWifiDisconnect,
           ": Disconnect from Wi-Fi"},
          {"wifi", "status", &ESP32SerialCtl::handleWifiStatus,
           ": Show Wi-Fi connection status"},
          {"ntp", "status", &ESP32SerialCtl::handleNtpStatus,
           ": Show NTP configuration"},
          {"ntp", "set", &ESP32SerialCtl::handleNtpSet,
           "<server> [server2] [server3] : Configure NTP servers"},
          {"ntp", "enable", &ESP32SerialCtl::handleNtpEnable,
           ": Enable NTP synchronization"},
          {"ntp", "disable", &ESP32SerialCtl::handleNtpDisable,
           ": Disable NTP synchronization"},
          {"ntp", "auto", &ESP32SerialCtl::handleNtpAuto,
           "<on|off> : Toggle NTP auto-start"},
#endif
#if defined(ESP32SERIALCTL_HAS_STORAGE)
          {"storage", "list", &ESP32SerialCtl::handleStorageList,
           ": List available storage devices"},
          {"storage", "use", &ESP32SerialCtl::handleStorageUse,
           "<name> : Set current storage backend"},
          {"storage", "status", &ESP32SerialCtl::handleStorageStatus,
           "[name] : Show storage status and capacity"},
          {"fs", "ls", &ESP32SerialCtl::handleFsLs,
           "[path] [--storage name] : List files and directories"},
          {"fs", "cat", &ESP32SerialCtl::handleFsCat,
           "<path> [offset] [len] [--storage name] : Show file contents"},
          {"fs", "b64read", &ESP32SerialCtl::handleFsB64Read,
           "<path> [--offset N] [--len N] [--chunk N] [--storage name] : Read file as Base64"},
          {"fs", "b64write", &ESP32SerialCtl::handleFsB64Write,
           "<path> <base64...> [--append] [--storage name] : Write Base64 data to file"},
          {"fs", "write", &ESP32SerialCtl::handleFsWrite,
           "<path> \"data\" [--storage name] : Write text to file"},
          {"fs", "rm", &ESP32SerialCtl::handleFsRm,
           "<path> [--storage name] : Delete file or directory"},
          {"fs", "stat", &ESP32SerialCtl::handleFsStat,
           "<path> [--storage name] : Show file information"},
          {"fs", "mkdir", &ESP32SerialCtl::handleFsMkdir,
           "<path> [--storage name] : Create directory"},
          {"fs", "mv", &ESP32SerialCtl::handleFsMv,
           "<src> <dst> [--storage name] : Move or rename entry"},
          {"fs", "hash", &ESP32SerialCtl::handleFsHash,
           "<path> [--algo sha256|md5] [--storage name] : Show file hash"},
#endif
          {"gpio", "mode", &ESP32SerialCtl::handleGpioMode,
           "<pin> <in|out|pullup|pulldown|opendrain> : Set pin mode"},
          {"gpio", "read", &ESP32SerialCtl::handleGpioRead,
           "<pin> : Read GPIO input value"},
          {"gpio", "write", &ESP32SerialCtl::handleGpioWrite,
           "<pin> <0|1|on|off|low|high> : Write GPIO output"},
          {"gpio", "toggle", &ESP32SerialCtl::handleGpioToggle,
           "<pin> : Toggle output"},
          {"gpio", "pins", &ESP32SerialCtl::handleGpioPins,
           ": Show GPIO pin access state"},
          {"adc", "read", &ESP32SerialCtl::handleAdcRead,
           "<pin> [samples N] : Read ADC value (average)"},
          {"pwm", "set", &ESP32SerialCtl::handlePwmSet,
           "<pin> <freq> <duty> [bits]: Start PWM output"},
          {"pwm", "stop", &ESP32SerialCtl::handlePwmStop,
           "<pin> : Stop PWM output"},
          {"rgb", "pin", &ESP32SerialCtl::handleRgbPin,
           "<pin> : Set default RGB output pin"},
          {"rgb", "set", &ESP32SerialCtl::handleRgbSet,
           "[--pin pin] [--count N] [--wait us] <r> <g> <b> : Set RGB LED color"},
          {"rgb", "stream", &ESP32SerialCtl::handleRgbStream,
           "[--pin pin] [--wait us] <r> <g> <b> [...] : Stream RGB colors sequentially"},
#if defined(ESP32SERIALCTL_HAS_WIRE)
          {"i2c", "scan", &ESP32SerialCtl::handleI2cScan,
           "[--bus name|index] : Scan for I2C devices"},
          {"i2c", "read", &ESP32SerialCtl::handleI2cRead,
           "[--bus name|index] <addr> <reg> [len] : Read bytes from device"},
          {"i2c", "write", &ESP32SerialCtl::handleI2cWrite,
           "[--bus name|index] <addr> <reg> <bytes...> : Write bytes to device"},
#endif
          {nullptr, "help", &ESP32SerialCtl::handleHelp,
           ": Show help for commands"},
          {nullptr, "?", &ESP32SerialCtl::handleHelp,
           ": Shortcut for help"},
  };

  // Initialize activeCommands_ to point to built-in kCommands by default.
  template <size_t MaxLineLength, size_t MaxTokens>
  const typename ESP32SerialCtl<MaxLineLength, MaxTokens>::Command *
      ESP32SerialCtl<MaxLineLength, MaxTokens>::activeCommands_ =
          ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands;

  template <size_t MaxLineLength, size_t MaxTokens>
  size_t ESP32SerialCtl<MaxLineLength, MaxTokens>::activeCommandCount_ =
      sizeof(ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands) /
      sizeof(ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands[0]);

  template <size_t MaxLineLength, size_t MaxTokens>
  const CommandEntry **ESP32SerialCtl<MaxLineLength, MaxTokens>::commandEntryMap_ = nullptr;

#if defined(ESP32SERIALCTL_HAS_STORAGE)
  template <size_t MaxLineLength, size_t MaxTokens>
  const typename ESP32SerialCtl<MaxLineLength, MaxTokens>::StorageEntry
      ESP32SerialCtl<MaxLineLength, MaxTokens>::kStorageEntries[] = {
#if defined(ESP32SERIALCTL_HAS_SD)
          {"sd", "SD card", &SD,
           ESP32SerialCtl<MaxLineLength, MaxTokens>::kSdHasTotalBytes
               ? &ESP32SerialCtl<MaxLineLength, MaxTokens>::storageSdTotalBytes
               : nullptr,
           ESP32SerialCtl<MaxLineLength, MaxTokens>::kSdHasUsedBytes
               ? &ESP32SerialCtl<MaxLineLength, MaxTokens>::storageSdUsedBytes
               : nullptr},
#endif
#if defined(ESP32SERIALCTL_HAS_SPIFFS)
          {"spiffs", "SPIFFS", &SPIFFS,
           ESP32SerialCtl<MaxLineLength, MaxTokens>::kSpiffsHasTotalBytes
               ? &ESP32SerialCtl<MaxLineLength, MaxTokens>::storageSpiffsTotalBytes
               : nullptr,
           ESP32SerialCtl<MaxLineLength, MaxTokens>::kSpiffsHasUsedBytes
               ? &ESP32SerialCtl<MaxLineLength, MaxTokens>::storageSpiffsUsedBytes
               : nullptr},
#endif
#if defined(ESP32SERIALCTL_HAS_LITTLEFS)
          {"littlefs", "LittleFS", &LittleFS,
           ESP32SerialCtl<MaxLineLength, MaxTokens>::kLittleFsHasTotalBytes
               ? &ESP32SerialCtl<MaxLineLength, MaxTokens>::storageLittleFsTotalBytes
               : nullptr,
           ESP32SerialCtl<MaxLineLength, MaxTokens>::kLittleFsHasUsedBytes
               ? &ESP32SerialCtl<MaxLineLength, MaxTokens>::storageLittleFsUsedBytes
               : nullptr},
#endif
#if defined(ESP32SERIALCTL_HAS_FFAT)
          {"fatfs", "FAT FS", &FFat,
           ESP32SerialCtl<MaxLineLength, MaxTokens>::kFatFsHasTotalBytes
               ? &ESP32SerialCtl<MaxLineLength, MaxTokens>::storageFatFsTotalBytes
               : nullptr,
           ESP32SerialCtl<MaxLineLength, MaxTokens>::kFatFsHasUsedBytes
               ? &ESP32SerialCtl<MaxLineLength, MaxTokens>::storageFatFsUsedBytes
               : nullptr},
#endif
  };

  template <size_t MaxLineLength, size_t MaxTokens>
  const size_t ESP32SerialCtl<MaxLineLength, MaxTokens>::kStorageCount =
      sizeof(ESP32SerialCtl<MaxLineLength, MaxTokens>::kStorageEntries) /
      sizeof(ESP32SerialCtl<MaxLineLength, MaxTokens>::kStorageEntries[0]);

  template <size_t MaxLineLength, size_t MaxTokens>
  int ESP32SerialCtl<MaxLineLength, MaxTokens>::currentStorageIndex_ = -1;
#endif

  template <size_t MaxLineLength, size_t MaxTokens>
  const size_t ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommandCount =
      sizeof(ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands) /
      sizeof(ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands[0]);

  template <size_t MaxLineLength, size_t MaxTokens>
  bool ESP32SerialCtl<MaxLineLength, MaxTokens>::pinAllAccess_ = true;

  template <size_t MaxLineLength, size_t MaxTokens>
  bool ESP32SerialCtl<MaxLineLength, MaxTokens>::pinAllowed_[GPIO_PIN_COUNT] = {false};

  template <size_t MaxLineLength, size_t MaxTokens>
  String ESP32SerialCtl<MaxLineLength, MaxTokens>::pinNames_[GPIO_PIN_COUNT];

  template <size_t MaxLineLength, size_t MaxTokens>
  const ConfigEntry *ESP32SerialCtl<MaxLineLength, MaxTokens>::configEntries_ = nullptr;

  template <size_t MaxLineLength, size_t MaxTokens>
  size_t ESP32SerialCtl<MaxLineLength, MaxTokens>::configEntryCount_ = 0;

  template <size_t MaxLineLength, size_t MaxTokens>
  String ESP32SerialCtl<MaxLineLength, MaxTokens>::configNamespace_ =
      kPrefsConfigDefaultNamespace;

  template <size_t MaxLineLength, size_t MaxTokens>
  int ESP32SerialCtl<MaxLineLength, MaxTokens>::rgbDefaultPin_ =
#ifdef RGB_BUILTIN
      RGB_BUILTIN;
#else
      -1;
#endif

} // namespace esp32serialctl
