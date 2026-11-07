# NoxAeApi C SDK

Official Pure C (C99) SDK for the [NoxAeApi](https://github.com/NoxlyDev/NOXAPI-HUB) Minecraft REST API suite and Velocity Network Hub.

## Features

- **Pure C99** with minimal dependencies (powered by standard `libcurl`)
- Modular or static library build support
- Full module coverage:
  - Players, Server, Economy, Leaderboards, LuckPerms, NoxAuth
  - Plugins, Worlds, Skills, Identity, Integrations (RankForge, etc.), Misc
- Velocity Hub Node routing support via `noxaeapi_client_set_node(client, "server-1")`

## Quick Start

```c
#include <stdio.h>
#include <noxaeapi.h>

int main() {
    noxaeapi_client_t* client = noxaeapi_client_create("http://localhost:8080", "your-api-key");
    
    // Fetch online players
    noxaeapi_response_t resp = noxaeapi_players_get_online(client);
    if (resp.status_code == 200) {
        printf("Online players: %s\n", resp.body);
    }
    noxaeapi_response_free(&resp);

    // Velocity Hub Node routing
    noxaeapi_client_set_node(client, "survival-1");
    resp = noxaeapi_server_get_info(client);
    if (resp.status_code == 200) {
        printf("Survival server: %s\n", resp.body);
    }
    noxaeapi_response_free(&resp);

    noxaeapi_client_free(client);
    return 0;
}
```

## Compilation

```bash
# Using Makefile
make

# Using CMake
cmake -B build
cmake --build build
```

## License

MIT © Wumx Labs & NoxAeApi Contributors
