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
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(ESP32SERIALCTL_ENABLE_I2C) || defined(Wire_h) || defined(_WIRE_H_) || \
    defined(_WIRE_H) || defined(_TWOWIRE_H_) || defined(TwoWire_h) ||             \
    defined(_WIRELIB_H_)
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
          emitHelpEntry(cmd);
        }
        return true;
      }

      const char *topicText = topic;
      bool groupMatch = false;
      for (size_t i = 0; i < commandCount_; ++i)
      {
        const Command &cmd = commands_[i];
        if (cmd.group && equalsIgnoreCase(cmd.group, topicText))
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
          if (cmd.group && equalsIgnoreCase(cmd.group, topicText))
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
        if (isTopLevel && equalsIgnoreCase(cmd.name, topicText))
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

    static void setDefaultRgbPin(int pin) { rgbDefaultPin_ = pin; }
    static int defaultRgbPin() { return rgbDefaultPin_; }

  private:
    static bool parsePinArgument(Context &ctx, const typename Base::Argument &arg,
                                 uint8_t &pin, const char *field)
    {
      ParsedNumber number;
      if (!arg.toNumber(number) || number.unit != NumberUnit::None || number.value < 0 ||
          number.value > 255)
      {
        ctx.printError(400, field);
        return false;
      }
      pin = static_cast<uint8_t>(number.value);
      return true;
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
      if (pinOpt && pinOpt->value)
      {
        ParsedNumber number;
        if (!Base::parseNumber(pinOpt->value, number) ||
            number.unit != NumberUnit::None || number.value < 0 || number.value > 255)
        {
          ctx.printError(400, "Invalid pin");
          return -1;
        }
        return static_cast<int>(number.value);
      }
      if (rgbDefaultPin_ < 0)
      {
        ctx.printError(404, "RGB pin not set");
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
        ctx.printError(400, "Usage: pwm set <pin> <freq> <duty> ");
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
      const uint8_t bits = kPwmResolutionBits;
      if (ctx.argc() >= 4)
      {
        ctx.printError(400, "Usage: pwm set <pin> <freq> <duty>");
        return;
      }
      uint32_t dutyValue = 0;
      if (!parseDutyArgument(ctx, ctx.arg(2), bits, dutyValue))
      {
        return;
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
      snprintf(line, sizeof(line), "bits: %u", kPwmResolutionBits);
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

    static int rgbDefaultPin_;

    Base cli_;
  };

  template <size_t MaxLineLength, size_t MaxTokens>
  const typename ESP32SerialCtl<MaxLineLength, MaxTokens>::Command
      ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands[] = {
          {"sys", "info", &ESP32SerialCtl::handleSysInfo,
           ": Show board and firmware information"},
          {"sys", "uptime", &ESP32SerialCtl::handleSysUptime,
           ": Show system uptime"},
          {"sys", "time", &ESP32SerialCtl::handleSysTime,
           ": Show local time (ISO 8601)"},
          {"sys", "mem", &ESP32SerialCtl::handleSysMem,
           ": Show heap and PSRAM usage"},
          {"sys", "reset", &ESP32SerialCtl::handleSysReset,
           ": Software reset"},
#if defined(ESP32SERIALCTL_HAS_WIRE)
          {"i2c", "scan", &ESP32SerialCtl::handleI2cScan,
           "[--bus name|index] : Scan for I2C devices"},
          {"i2c", "read", &ESP32SerialCtl::handleI2cRead,
           "[--bus name|index] <addr> <reg> [len] : Read bytes from device"},
          {"i2c", "write", &ESP32SerialCtl::handleI2cWrite,
           "[--bus name|index] <addr> <reg> <bytes...> : Write bytes to device"},
#endif
          {"gpio", "mode", &ESP32SerialCtl::handleGpioMode,
           "<pin> <in|out|pullup|pulldown|opendrain> : Set pin mode"},
          {"gpio", "read", &ESP32SerialCtl::handleGpioRead,
           "<pin> : Read GPIO input value"},
          {"gpio", "write", &ESP32SerialCtl::handleGpioWrite,
           "<pin> <0|1|on|off|low|high> : Write GPIO output"},
          {"gpio", "toggle", &ESP32SerialCtl::handleGpioToggle,
           "<pin> : Toggle output"},
          {"adc", "read", &ESP32SerialCtl::handleAdcRead,
           "<pin> [samples N] : Read ADC value (average)"},
          {"pwm", "set", &ESP32SerialCtl::handlePwmSet,
           "<pin> <freq> <duty> : Start PWM output (12-bit fixed)"},
          {"pwm", "stop", &ESP32SerialCtl::handlePwmStop,
           "<pin> : Stop PWM output"},
          {"rgb", "pin", &ESP32SerialCtl::handleRgbPin,
           "<pin> : Set default RGB output pin"},
          {"rgb", "set", &ESP32SerialCtl::handleRgbSet,
           "[--pin pin] [--count N] [--wait us] <r> <g> <b> : Set RGB LED color"},
          {"rgb", "stream", &ESP32SerialCtl::handleRgbStream,
           "[--pin pin] [--wait us] <r> <g> <b> [...] : Stream RGB colors sequentially"},
          {nullptr, "help", &ESP32SerialCtl::handleHelp,
           ": Show help for commands"},
          {nullptr, "?", &ESP32SerialCtl::handleHelp,
           ": Shortcut for help"},
  };

  template <size_t MaxLineLength, size_t MaxTokens>
  const size_t ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommandCount =
      sizeof(ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands) /
      sizeof(ESP32SerialCtl<MaxLineLength, MaxTokens>::kCommands[0]);

  template <size_t MaxLineLength, size_t MaxTokens>
  int ESP32SerialCtl<MaxLineLength, MaxTokens>::rgbDefaultPin_ =
#ifdef RGB_BUILTIN
      RGB_BUILTIN;
#else
      -1;
#endif

} // namespace esp32serialctl
