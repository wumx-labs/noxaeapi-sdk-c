# Changelog

All notable changes to the `noxaeapi-sdk-c` will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.7.1] - 2026-11-07

### Added
- Official documentation link (`https://noxapi.noxlydev.xyz`) in `README.md`.

## [0.7.0] - 2026-11-07

### Added
- Initial official release of the Pure C (C99) SDK for NoxAeApi ecosystem.
- Single-header library (`include/noxaeapi.h`) with STB-style implementation support.
- Libcurl transport integration with automatic connection reuse and header management.
- Complete support for all NoxAeApi core modules:
  - `players`: Online/offline players, health/food, teleportation, kicks, bans, whitelist.
  - `server`: Health metrics, TPS, command execution, broadcasts, server info.
  - `economy`: Balances, deposit, withdraw, multi-currency support, top rankings.
  - `leaderboard`: Dynamic leaderboard retrieval.
  - `luckperms`: Permission checks, group assignments, permission nodes.
  - `noxauth`: In-game authentication, password checks.
  - `plugins`: Plugins listing, enable, disable.
  - `worlds`: Worlds listing, time and weather mutation.
  - `skills`: Player skill querying and experience modification.
  - `identity`: Cross-server identity resolution and server listing.
  - `rankforge`: PvP stats, seasonal tiers, points, top rankings.
  - `integrations`: Direct access to 16+ supported Spigot/Paper plugins.
  - `misc`: Ping, health check.
- Velocity network node client via `noxaeapi_client_set_node(client, "node-id")`.
