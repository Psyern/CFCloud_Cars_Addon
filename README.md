# CFCloud Cars Addon

<p align="center">
  <img src="https://img.shields.io/badge/DayZ-1.29+-0074D9?style=for-the-badge&logo=steam&logoColor=white" alt="DayZ 1.29+">
  <img src="https://img.shields.io/badge/Enforce_Script-Enfusion-FF851B?style=for-the-badge" alt="Enforce Script">
  <img src="https://img.shields.io/badge/Server--Side-Only-8E44AD?style=for-the-badge" alt="Server-side only">
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-AGPL--3.0-green?style=for-the-badge" alt="License AGPL-3.0"></a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Requires-GameLabs-1B2838?style=flat-square" alt="Requires GameLabs">
  <img src="https://img.shields.io/badge/Requires-DayZ_Expansion_Vehicles-2ECC40?style=flat-square" alt="Requires DayZ Expansion Vehicles">
  <img src="https://img.shields.io/badge/CFTools-Dynamic_Actions-E74C3C?style=flat-square" alt="CFTools Dynamic Actions">
</p>

<p align="center">
  <b>Unlock Expansion-locked vehicles straight from CFTools Cloud</b><br>
  Adds GameLabs Dynamic Actions so admins can open, lock and inspect vehicles that were
  locked with DayZ Expansion — no RCon, no server restart, no need to be in game.
</p>

<p align="center">
  <a href="https://deadmans-echo.de">
    <img src="https://img.shields.io/badge/Community-Deadmans_Echo-F0C040?style=for-the-badge" alt="Deadmans Echo">
  </a>
</p>

---

## Status

> **Work in progress.** This repository currently contains the mod skeleton — config,
> script layers, logger and settings. The GameLabs actions described under *Planned
> functionality* are specified and approved but **not yet implemented**. The design
> document and the underlying audits are kept outside this repository.

---

## Repository Layout

```text
CFCloud_Cars_Addon/                 ← repository root (this README)
├── CFCloud_Bridge/                 ← the DayZ mod — PBO source folder
│   ├── $PBOPREFIX$                 ← must equal the folder name
│   ├── config.cpp                  ← CfgPatches + CfgMods, script layer registration
│   ├── stringtable.csv             ← localisation (full 15-column layout)
│   └── scripts/
│       ├── Common/                 ← compiled into every layer
│       ├── 3_Game/                 ← logger, settings, constants, manager
│       └── 5_Mission/              ← MissionServer hook, GameLabs actions
├── LICENSE
└── NOTICE                          ← required mods and why nothing is redistributed
```

---

## Why this exists

GameLabs ships around 25 Dynamic Actions out of the box — repair, refuel, unstuck,
delete, explode. **None of them opens a vehicle lock**, for neither vanilla nor
Expansion. On a server running DayZ Expansion, "I locked myself out" or "I lost my only
car key" therefore has no answer from the CFCloud interface at all.

This mod closes that gap.

---

## Planned functionality

Five Dynamic Actions, all server-side:

| Action | Context | What it does |
|---|---|---|
| **Unlock vehicle** | vehicle | Opens an Expansion-locked vehicle, key pairing left intact |
| **Lock vehicle (admin)** | vehicle | Force-locks a vehicle; the owner's key cannot undo it |
| **Vehicle status** | vehicle | Logs lock state, key pairing, persistent IDs and owner |
| **Set vehicle owner** | vehicle | Reassigns ownership, handles the keychain case |
| **Player's vehicles** | player | Lists every vehicle owned by that player |

Plus **entity tracking for Expansion aircraft**: GameLabs only tracks `CarScript` and
`BoatScript`, so Expansion helicopters and planes never show up in CFCloud. This mod
registers them, which makes them visible on the CFCloud map as well.

### Deliberately not included

* **Issuing a replacement key.** Expansion's `PairToVehicle()` overwrites the vehicle's
  master key ID with the new key's ID, which silently kills every key that already
  exists for that vehicle. Calling that a "spare key" would be misleading.
* **Expansion base code locks.** Different subsystem; belongs in its own pass.
* **Repair / refuel / unstuck / delete.** GameLabs already ships these.

---

## Requirements

| | |
|---|---|
| DayZ Server | 1.29 or newer |
| [GameLabs](https://github.com/CFToolsCloud/gamelabs-plugin-dayz) | required — the mod registers nothing without it |
| DayZ Expansion — **Vehicles** module | required — the lock system lives there, not in Expansion Core |
| CFTools Cloud | an active application grant for the server |

Both dependencies are guarded by preprocessor defines (`GAMELABS`,
`EXPANSIONMODVEHICLE`). Missing either one does **not** break compilation — the mod
loads and stays inert.

`requiredAddons` is deliberately `{"DZ_Data"}` only. It orders configs; it does not
guard script references, so it is the wrong tool for the job.

---

## Installation

The mod is pure server-side logic — no items, no layouts, no sounds, no keybinds.

```bat
-mod=@CFCloud_Bridge;@GameLabs;@DayZExpansion...
```

It also runs as `-serverMod`, since clients need none of its content.

Pack `CFCloud_Bridge/` with the DayZ Tools Addon Builder. `$PBOPREFIX$` must keep
matching the folder name.

---

## Configuration

Written on first start to `$profile:CFCloud_Bridge\Config\Settings.json`.

| Key | Default | Meaning |
|---|---|---|
| `m_LogLevel` | `3` | 0 none, 1 error, 2 warning, 3 info, 4 debug |
| `m_AllowUnlock` | `true` | Unlock action enabled |
| `m_AllowLock` | `true` | Admin lock action enabled |
| `m_AllowSetOwner` | `false` | Owner reassignment — off by default, it reaches deep |

---

## Good to know

**Results go to the server log, not to the interface.** The Data API answers a Dynamic
Action with `204` — *queued*, not *executed* — and GameLabs marks the `output` response
type as unavailable for general use. Unlock and lock are visible in game anyway; the
two read-only actions are only useful with server log access.

**The admin lock takes effect one server tick later.** Expansion's own path sets
`READY_TO_FORCELOCK` first and converts it in `UpdateLock()`. A status check fired
immediately afterwards may still report the intermediate state — that is correct
behaviour, not a bug.

**Unlocking does not mark the vehicle as lockpicked.** Expansion uses the
`FORCEDUNLOCKED` state as its "broken into" marker, and the P2P market refuses to sell
a vehicle in that state. This mod always passes the target state explicitly and never
relies on the default parameter.

---

## License

[AGPL-3.0-only](LICENSE) · Copyright (c) 2026 Psyern / Deadmans Echo

No third-party source code is bundled or redistributed. See [NOTICE](NOTICE) for the
mods this addon interoperates with and the APIs it calls.
