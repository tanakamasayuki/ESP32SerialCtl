#pragma once

#include <Arduino.h>
#include <esp_system.h>
#include <esp_chip_info.h>
#include <esp_heap_caps.h>
#ifdef ARDUINO_ARCH_ESP32
#include <core_version.h>
#endif
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace esp32serialctl
{

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
    char lineBuffer_[MaxLineLength + 1];
    size_t linePos_ = 0;
    bool overflow_ = false;

    Context context_;
    Argument arguments_[MaxTokens];
    Option options_[MaxTokens];
    size_t optionCount_ = 0;
    bool promptPending_ = true;
  };

  template <size_t MaxLineLength = 128, size_t MaxTokens = 16>
  class ESP32SerialCtl
  {
  public:
    using Base = SerialCtl<MaxLineLength, MaxTokens>;
    using Command = typename Base::Command;
    using Context = typename Base::Context;

    ESP32SerialCtl()
        : cli_(Serial, kCommands, kCommandCount) {}

    explicit ESP32SerialCtl(Stream &io)
        : cli_(io, kCommands, kCommandCount) {}

    ESP32SerialCtl(Stream &input, Print &output)
        : cli_(input, output, kCommands, kCommandCount) {}

    Base &raw() { return cli_; }
    const Base &raw() const { return cli_; }

    void service() { cli_.service(); }
    void feed(char c) { cli_.feed(c); }
    void execute(const char *line) { cli_.execute(line); }

    Print &output() { return cli_.output(); }
    Stream &input() { return cli_.input(); }

  private:
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
      auto appendFeatureSuffix = [&](const char *suffix) {
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

    static void handleSysTime(Context &ctx)
    {
      time_t now = time(nullptr);
      if (now == static_cast<time_t>(-1))
      {
        ctx.printError(500, "time unavailable");
        return;
      }

      struct tm localTm;
#if defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP32)
      if (!localtime_r(&now, &localTm))
      {
        ctx.printError(500, "localtime failed");
        return;
      }
#else
      struct tm *tmp = localtime(&now);
      if (!tmp)
      {
        ctx.printError(500, "localtime failed");
        return;
      }
      localTm = *tmp;
#endif

      char datePart[32];
      if (strftime(datePart, sizeof(datePart), "%Y-%m-%dT%H:%M:%S", &localTm) == 0)
      {
        ctx.printError(500, "format failed");
        return;
      }

      char zone[8] = {0};
      const size_t zoneLen = strftime(zone, sizeof(zone), "%z", &localTm);

      char iso[48];
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
        snprintf(iso, sizeof(iso), "%s%s", datePart, zoneFormatted);
      }
      else if (zoneLen > 0)
      {
        snprintf(iso, sizeof(iso), "%s%s", datePart, zone);
      }
      else
      {
        snprintf(iso, sizeof(iso), "%sZ", datePart);
      }

      ctx.printOK("sys time");
      char line[64];
      snprintf(line, sizeof(line), "localtime: %s", iso);
      ctx.printBody(line);
    }

    static void handleSysMem(Context &ctx)
    {
      char lines[24][96];
      size_t lineCount = 0;

      auto addLine = [&](const char *fmt, ...) {
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
      ESP.restart();
    }

    static void handleHelp(Context &ctx)
    {
      const char *topic = ctx.arg(0).c_str();
      if (topic && *topic)
      {
        ctx.printError(404, "Unknown help topic");
        return;
      }
      ctx.printOK("help");
      ctx.controller().forEachCommand([&ctx](const Command &cmd)
                                       {
      char commandText[48];
      if (cmd.group && *cmd.group) {
        snprintf(commandText, sizeof(commandText), "%s %s", cmd.group, cmd.name);
      } else {
        snprintf(commandText, sizeof(commandText), "%s", cmd.name);
      }
      const char *help = cmd.help ? cmd.help : "";
      if (help && *help) {
        char line[96];
        snprintf(line, sizeof(line), "%s : %s", commandText, help);
        ctx.printList(line);
      } else {
        ctx.printList(commandText);
      } });
    }

    static const Command kCommands[];
    static const size_t kCommandCount;

    Base cli_;
  };

  template <size_t MaxLineLength, size_t MaxTokens>
  const typename ESP32SerialCtl<MaxLineLength, MaxTokens>::Command
      ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands[] = {
          {"sys", "info", &ESP32SerialCtl::handleSysInfo,
           "Show board and firmware information"},
          {"sys", "uptime", &ESP32SerialCtl::handleSysUptime,
           "Show system uptime"},
          {"sys", "time", &ESP32SerialCtl::handleSysTime,
           "Show local time (ISO 8601)"},
          {"sys", "mem", &ESP32SerialCtl::handleSysMem,
           "Show heap and PSRAM usage"},
          {"sys", "reset", &ESP32SerialCtl::handleSysReset,
           "Software reset"},
          {nullptr, "help", &ESP32SerialCtl::handleHelp,
           "Show help for commands"},
          {nullptr, "?", &ESP32SerialCtl::handleHelp,
           "Shortcut for help"},
  };

  template <size_t MaxLineLength, size_t MaxTokens>
  const size_t ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommandCount =
      sizeof(ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands) /
      sizeof(ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands[0]);

} // namespace esp32serialctl
