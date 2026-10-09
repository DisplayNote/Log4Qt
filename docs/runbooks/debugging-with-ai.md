# Debugging with AI — log sanitisation (control 4.7)

> Generated from the `log-sanitise` template (displaynote-engineering plugin) by
> `docs-update`. The "Mandatory step" and "Rules" sections are kept verbatim.

## Where this repo's logs live

| Source | Location / command | Typical sensitive content |
|---|---|---|
| Application log | Log4Qt is the logging framework itself and has **no default log file or path**: files exist only where the consuming app's configuration puts them. The file sinks are `FileAppender` (`file` property, one file; missing parent directories are created), `RollingFileAppender` (`<file>.1` … `<file>.N` by size), `DailyRollingFileAppender` (rolled by date pattern), `DailyFileAppender` (`<dir>/<base><date>.<ext>` per day, deleting files older than `keepDays`) and `BinaryFileAppender` / `RollingBinaryFileAppender`. The configuration comes, in this order (`LogManager::doStartup`, `src/log4qt/logmanager.cpp`), from the file named by `LOG4QT_CONFIGURATION` (environment, not read on iOS) or `QSettings` `[Log4Qt] Configuration`, the `QSettings` group `[Log4Qt/Properties]`, `<executable path>.log4qt.properties`, `log4qt.properties` next to the executable, `log4qt.properties` in the current working directory — or from the app's own code. When Log4Qt runs inside Montage, Montage's configuration chooses the appenders and file paths, so everything below ends up in Montage's log files. In this repo only the examples write files (`examples/basic/main.cpp`: `basic.log` next to the executable; `examples/propertyconfigurator/propertyconfigurator.exe.log4qt.properties`: `./propertyconfigurator_<yyyy_MM_dd>.log`), and the tests write synthetic logs to temp directories (`tests/log4qttest/log4qttest.cpp`, `tests/dailyfileappendertest/`). Log4Qt's own internal messages (the `Log4Qt` logger, "LogLog") go to stdout/stderr and, because that logger is additive, also to the app's root appenders | every message the app logs, unfiltered (Log4Qt has no redaction of its own); MDC/NDC values (`%X`/`%x`), thread and logger names; source file paths, function names and line numbers when the pattern uses `%F`/`%M`/`%L`/`%l`; from Log4Qt's own messages: log and configuration file paths (user-home folders), appender names and, at DEBUG/TRACE, every configuration value (see the repo-specific note) |
| Live stream | Depends on the appenders the app configures: `ConsoleAppender` → stdout (default) or stderr; `ColorConsoleAppender` (Windows) → console; `DebugAppender` → `OutputDebugStringW` on Windows, `std::cerr` elsewhere; `WDCAppender` → `OutputDebugString` (Windows only, a no-op stub elsewhere) — capture with the debugger or DebugView; `SystemLogAppender` → `syslog()` with facility `LOG_DAEMON` on non-Windows builds (ident = `serviceName`, default the application name, reduced to letters and digits), or the Windows Event Log via `ReportEventW` (event source = `serviceName`) — read with the platform's system-log tools (e.g. Event Viewer, the macOS Console/`log` tool, `journalctl`); `TelnetAppender` → a TCP listener (default port 23 on all interfaces) that streams every formatted event to any connected client; `SignalAppender` → the in-process Qt signal `appended(int level, QString message)`. Log4Qt's internal logger always writes to stdout (≤ INFO) and stderr (≥ WARN), level `ERROR` unless `LOG4QT_DEBUG`, `[Log4Qt] Debug` or `log4j.Debug` raise it. There is **no** Android logcat or Apple `os_log` appender in this repo: on Android, Log4Qt output reaches `adb logcat -d` only through whatever the app adds; and when the app enables `log4j.handleQtMessages` / `LogManager::setHandleQtMessages(true)`, Log4Qt replaces the Qt message handler, so `qDebug`/`qWarning`/… from the app and every Qt library in the process go to the Log4Qt appenders (logger `Qt`) instead of Qt's default output | same as the application log; the telnet stream is raw and unauthenticated on the network |
| Crash reports | No crash reporter (Sentry or similar) is wired into this repo. A `QtFatalMsg` routed through Log4Qt is logged and then `std::abort()` is called (`qt_message_fatal`, `src/log4qt/logmanager.cpp`; MSVC debug builds show the CRT report dialog first). Android native crashes surface as `tombstone_*` files and in `adb logcat -d -b crash`; inside Montage they are reported through Montage's own crash reporting | the fatal message text, source paths, memory addresses |
| CI logs | Azure Pipelines run logs (`displaynote-devops`) | internal paths, hostnames |
| Customer bundles | Zendesk ticket attachments — download them to a file and sanitise before reading; the Zendesk MCP bypasses the Claude Code guard, so never read attachments through it (see `.agents/skills/log-sanitise/SKILL.md`) | **Protected**: end-user identifiers (1.3 §4.2) |

## Mandatory step — sanitise before any AI tool sees the log

Before a log file, a log excerpt or a live log stream reaches Claude Code, Codex,
Copilot, Cursor, ChatGPT, Claude (Cowork) or any custom OpenAI-API integration,
run it through `dn_logscrub`:

```bash
# Work OUTSIDE the repository: a raw log inside it blocks the searches that would read it
mkdir -p ~/dn-tickets/22416 && cd ~/dn-tickets/22416

# Claude Code (plugin installed) — one call, all files, shared placeholders
/log-sanitise montage.log launcher.log --map ticket-22416.dnmap

# Any agent / shell — the portable copy synced into the repo
python3 <repo>/.agents/skills/log-sanitise/scripts/dn_logscrub.py montage.log launcher.log --map ticket-22416.dnmap

# Inputs in a read-only folder, or elsewhere: write the copies into one directory
python3 <repo>/.agents/skills/log-sanitise/scripts/dn_logscrub.py /var/log/omni/*.log --out-dir ~/dn-tickets/22416

# Live streams — pipe, never paste; use a command that ends (`-d`), not a live tail
adb logcat -d | python3 <repo>/.agents/skills/log-sanitise/scripts/dn_logscrub.py - > logcat.scrubbed.txt

# Customer logs — add the customer's domain(s) so their hostnames are pseudonymised too
python3 <repo>/.agents/skills/log-sanitise/scripts/dn_logscrub.py bundle/*.log --domain acme-school.org
```

A run that is cut short (Ctrl-C, a tool timeout) still writes a trailer marked
`interrupted`: the copy is clean but incomplete. The sanitiser processes a few
MB per second; run very large bundles from your own terminal.

The tool writes `<name>.scrubbed.<ext>` next to each input, prints one summary
line per file (`dn_logscrub: montage.log: 41 redactions (EMAIL=3, ID=12, …)`) and
starts every output with a `# dn_logscrub v…` marker header and ends it with a
`# dn_logscrub end | redactions=…` trailer carrying the per-category counts
(output is written as it is produced, so a live stream appears immediately
instead of waiting for the end). Only files that start with that header and
end with that trailer may be opened by, pasted into, or attached to an AI tool.
A file with the header but no trailer as its last non-empty line is raw:
something was appended after scrubbing (`cat raw >> x.scrubbed.log`,
concatenated files, a process still writing). A trailer ending in
`| interrupted` is accepted — the copy is clean, only incomplete. A
`# dn_logscrub-partial` header does not count: it means rules were switched off
for that run.

## Rules

1. Only `*.scrubbed.*` files (marker header on the first line and the
   `# dn_logscrub end` trailer as the last non-empty line) go to an AI tool.
   Raw logs never do — the Claude Code hook blocks them; for other tools the
   rule is on you.
2. Placeholders are stable within a run: `<EMAIL_1>` is the same person in every
   file of that run. Keep placeholders in PR descriptions, ticket comments and
   Slack messages; never expand them there.
3. The `--map` file (`*.dnmap`) holds the originals for your own reverse lookup.
   It stays on your machine; never attach it, commit it or paste from it.
4. Customer logs are **Protected** data by default (AI Governance Policy 1.3 §4.2).
   Sanitised customer logs may go to Green-List tools with a corporate account.
   Raw customer logs may go to an AI tool only with AI Lead + CEO approval
   (1.3 §4.1) — signal an approved exception with `DN_LOGSCRUB_ALLOW_RAW=1`.
5. Residual check: skim the scrubbed file for anything the patterns missed
   (a person's name in free text, a customer hostname without `--domain`, an
   unusual token format). Fix with `--domain`, or open an issue on
   `displaynote-engineering` with the *category and shape* of the miss — never
   with the value itself.
6. Automated flows (Sentry fixer, Release Management Agent, n8n) call the same
   library (`Scrubber().scrub_text(...)`) before every external model call. A
   flow that writes its result to a file uses `Scrubber().finalize(text, name)`,
   which adds the header and trailer the Claude Code hook and `--check` require.

## What is redacted vs. kept

Redacted irreversibly: private keys, JWTs, auth headers, URL credentials,
cloud/API keys, any `password= / token= / secret= / *_key=` value, OAuth
`code=`/`state=`/`nonce=` in URLs.
Pseudonymised consistently: e-mails, quoted/keyed names (people, devices,
computers), keyed ids (serial, deviceId, session, meetingId, roomPin, tenantId,
licence…), Android `getprop` serials, Wi-Fi SSIDs, IPv4/IPv6, MACs, UUIDs, phone
numbers, the user-home part of any path (Windows with either slash, JSON-escaped,
`file:///`, WSL), Windows `DOMAIN\user` accounts and UNC servers, `DESKTOP-…`
machine names, `.local/.lan/.internal` hosts, `--domain` domains, long hex and
high-entropy strings.
Kept: timestamps, version numbers, stack traces, package/class/method names and
JNI symbols, the rest of the path, file hashes and git commit ids, loopback
addresses, and keys that only describe a secret (`token_expires_in=3600`).
Not caught: a person's name in free text, a customer hostname without `--domain`.

Repo-specific note: Log4Qt writes whatever the consuming app logs, unchanged —
it has no redaction, so the content risk of a Log4Qt log is the app's (in
Montage: Montage's). Levels are decided at runtime by configuration: the
`l4qTrace` … `l4qFatal` macros (`src/log4qt/logger.h`, lines 182-199) only check
the logger level before logging, and none of this repo's build files define
`QT_NO_DEBUG_OUTPUT`, `QT_NO_DEBUG` or `NDEBUG` for the library, so nothing is
compiled out: a release build logs DEBUG/TRACE lines whenever the configuration
enables them. The `l4q*` macros always attach `__FILE__`, `__LINE__` and
`Q_FUNC_INFO`, so patterns using `%F`/`%L`/`%M`/`%l` print source paths (as
passed to the compiler, which can include the build machine's user folder) in
release builds too; for `qDebug`-family messages routed through
`LogManager::qtMessageHandler` the file/line/function are only those Qt puts in
the message context (typically empty in Qt release builds unless
`QT_MESSAGELOGCONTEXT` is defined). Log4Qt's own lines that carry data:

- Internal logger at its default `ERROR` level, so in every build, to stdout/stderr
  and (the `Log4Qt` logger is additive) to the app's root appenders: full log
  file paths when a file cannot be opened, written, removed or renamed
  (`src/log4qt/fileappender.cpp` lines 157-161, 197-201, 215-219, 230-234;
  `src/log4qt/binaryfileappender.cpp` lines 160-164, 207-211, 225-229, 240-244);
  the properties file path when it cannot be opened or read
  (`src/log4qt/propertyconfigurator.cpp` lines 110-116, 122-128); in builds with
  `QT += sql` only (not the DisplayNote package), the full failed `INSERT`
  statement — i.e. the logged message itself — plus the database error text
  (`src/log4qt/databaseappender.cpp` lines 121-125). `ConfiguratorHelper` reports a
  failed file watch with the configuration file path through `qWarning`
  (`src/log4qt/helpers/configuratorhelper.cpp` line 92).
- Internal logger at DEBUG (opt-in through `LOG4QT_DEBUG`, `[Log4Qt] Debug` in
  `QSettings` or `log4j.Debug`): configuration file paths
  (`src/log4qt/logmanager.cpp` lines 285, 303, 329), every property the factory
  sets, with its value (`src/log4qt/helpers/factory.cpp` line 351), opened,
  closed and renamed log file names (`fileappender.cpp` lines 143, 206, 226;
  `binaryfileappender.cpp` lines 146, 217, 236), Qt filter rules and message
  pattern (`propertyconfigurator.cpp` lines 241, 248).
- Internal logger at TRACE: the `LOG4QT_*` environment settings and every key and
  value under the `QSettings` groups `[Log4Qt]` and `[Log4Qt/Properties]`
  (`logmanager.cpp` lines 376-398), every key and value loaded from a properties
  file (`src/log4qt/helpers/properties.cpp` line 276), and created parent
  directories (`fileappender.cpp` line 174, `binaryfileappender.cpp` line 184).
  Anything an app puts in its logging configuration (paths under the user's home,
  a telnet welcome message, a database connection name) is printed verbatim.
- `TelnetAppender` is compiled into the DisplayNote package (`QT += network` in
  `src/log4qt/log4qt.pro`; `src/log4qt/log4qt.pri` adds it when `QT` contains
  `network`) and can be enabled from any of the configuration sources listed above,
  including a `log4qt.properties` in the current working directory. It listens on
  `QHostAddress::Any`, port 23 by default (`src/log4qt/telnetappender.cpp` lines
  41-42, 50-51, 163), without authentication or encryption, and writes every
  formatted event and the welcome message to every connected client (lines 139,
  207). The bind address cannot be set from a properties file (the factory only
  converts `bool`, `int`, `QString` and `Log4Qt::Level`,
  `src/log4qt/helpers/factory.cpp` lines 356-376). Treat a captured telnet stream
  as a raw log: save it to a file and sanitise it before any AI tool sees it.
- `DatabaseAppender` (not in the DisplayNote package) never handles credentials: it
  uses an existing `QSqlDatabase` connection by name, opened by the app.
- `SystemLogAppender` copies events into the OS log (syslog / Windows Event Log),
  where they outlive the app's own log files; `SignalAppender` hands the
  formatted message to app code, which may forward it anywhere.

CI (`azure-pipelines.yml`) only builds and packages the library; it runs no tests
and no script in this repo writes transcripts or log files. Locally, the test
suites write synthetic test messages to temp directories (`tests/log4qttest/log4qttest.cpp`,
`tests/dailyfileappendertest/dailyfileappendertest.cpp`, `tests/filewatcher/tst_filewatchertest.cpp`).
User-home path parts, IPs and `password=`/`token=` values are handled by the
default rules; free-text names in app messages and folder names below the home
directory are not, so check them in the residual pass. No repo-specific sanitiser patterns have been shipped yet; raise any needed as an issue on `displaynote-engineering` and list them here once shipped.
