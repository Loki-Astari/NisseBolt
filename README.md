[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/G2G216KZR3)

# NisseBolt

![NisseBolt](img/NisseBolt.jpg)

A C++ library for building Slack bots, modeled after the [Slack Bolt for Python](https://slack.dev/bolt-python/) API. If you've used Bolt in Python, NisseBolt should feel familiar — register handlers for messages, commands, actions, and views with the same mental model, but in modern C++20.

## Why?

The Slack Bolt API is a well-designed interface for building bots. But if your infrastructure is C++ and you want type-safe, zero-boilerplate serialization with async IO, rewriting in Python isn't ideal. NisseBolt brings the Bolt programming model to C++ while leveraging the [ThorsAnvil](https://github.com/Loki-Astari/ThorsAnvil) ecosystem — automatic JSON serialization, coroutine-based async networking, and hot-reloadable plugin architecture.

## Quick Example

```cpp
#include "NisseBolt/App.h"

namespace Bolt = ThorsAnvil::Nisse::Bolt;

struct MyBot: Bolt::App
{
    MyBot(Bolt::AppConfig const& config, std::string const& slot)
        : App(config, slot)
    {
        // Respond to messages containing "hello"
        message("hello", [](auto const& event, auto const& say) {
            say("Hey there!");
        });

        // Handle a slash command
        command("/greet", [](auto const& ack, auto const& response, auto const& cmd) {
            ack("Greetings!");
        });

        // Handle a button click
        action("approve_button", [](auto const& ack, auto const& response, auto const& action, auto const& value) {
            ack();
        });
    }
};

THORS_ANVIL_NISSE_BOLT_SERVER_INIT(Bolt::AppConfig, MyBot);
```

## Core Concepts

| Concept | Description |
|---|---|
| **App** | Your bot. Subclass it and register handlers in the constructor. |
| **Say** | Send messages back to a channel. Supports plain text and Block Kit. |
| **Ack** | Acknowledge an incoming request from Slack (slash commands, actions, views). |
| **View** | Open, push, or update Slack modals with submit/close handlers. |

### Handler Types

- **`message(filter, handler)`** — React to messages. Filter by string, regex, or custom predicate.
- **`event<T>(handler)`** — Handle any Slack event type (app mentions, reactions, file uploads, etc.).
- **`command(name, handler)`** — Respond to slash commands like `/greet`.
- **`action(actionId, handler)`** — Handle interactive components (buttons, menus, etc.).
- **`viewOpen` / `viewPush` / `viewUpdate`** — Manage Slack modals.

## Part of ThorsAnvil

NisseBolt is a component of the [ThorsAnvil](https://github.com/Loki-Astari/ThorsAnvil) monorepo. It builds on:

- **[ThorsSerializer](https://github.com/Loki-Astari/ThorsSerializer)** — Automatic JSON/YAML/BSON serialization via compile-time traits. All Slack API types are serialized without hand-written code.
- **[Nisse](https://github.com/Loki-Astari/ThorsAnvil)** — Coroutine-based async HTTP server. Your handlers read like synchronous code while running on an event loop.
- **[Mug](https://github.com/Loki-Astari/ThorsAnvil)** — Hot-reloadable plugin server. Your bot compiles to a shared library that Mug loads, monitors, and reloads on change — no server restart needed.

## Documentation

- **API Reference:** [loki-astari.github.io/ThorsAnvil/NisseBolt.html](https://loki-astari.github.io/ThorsAnvil/NisseBolt.html)

### Article Series

A detailed walkthrough covering setup, configuration, and building a complete Slack bot:

1. [Mug: The Plugin Server](https://lokiastari.com/posts/Mug) — How the hot-reloadable plugin architecture works.
2. [SlackMug: Connecting to Slack](https://lokiastari.com/posts/SlackMug) — Setting up a Slack app and wiring it to NisseBolt.
3. [SlackHandlers: Messages & Commands](https://lokiastari.com/posts/SlackHandlers) — Handling messages and slash commands.
4. [SlackAction: Interactive Components](https://lokiastari.com/posts/SlackAction) — Buttons, menus, and user actions.
5. [SlackView: Modals](https://lokiastari.com/posts/SlackView) — Opening and managing Slack modals.

## Installing

The simplest way to install is via [Homebrew](https://formulae.brew.sh/formula/thors-anvil):

```bash
brew install thors-anvil
```

Or build from source:

```bash
git clone --recurse-submodules https://github.com/Loki-Astari/ThorsAnvil.git
cd ThorsAnvil
./configure
make
make install
```

### Requirements

- C++20 compiler
- Boost (>= 1.70, for Coroutine2)
- OpenSSL
- libevent

See the [ThorsAnvil README](https://github.com/Loki-Astari/ThorsAnvil) for full build instructions and platform notes.

## Using in Your Project

Once ThorsAnvil is installed, CMake config files are available automatically.

### CMake

```cmake
find_package(ThorsAnvil REQUIRED)

add_library(mybot MODULE mybot.cpp)
target_link_libraries(mybot ThorsAnvil::NisseBolt)
```

All transitive dependencies (`ThorsAnvil::ThorsSlack`, `ThorsAnvil::Nisse`, `ThorsAnvil::ThorSerialize`, etc.) are pulled in automatically.

### Make

```makefile
CXXFLAGS += -std=c++20 -I$(shell brew --prefix thors-anvil)/include
LDFLAGS  += -L$(shell brew --prefix thors-anvil)/lib
LDLIBS   += -lNisseBolt -lNisse -lThorSerialize -lThorsSocket -lThorsLogging

mybot.so: mybot.cpp
	$(CXX) -shared $(CXXFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@
```
