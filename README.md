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

> **v0.4.0 — Terje player-state actions added, not yet run on a server.** v0.3.0 was
> packed with Addon Builder and started on a DayZ 1.29 test server alongside GameLabs
> and DayZ Expansion: the Mission module compiles, the five vehicle actions register
> with GameLabs before it posts its action list to CFCloud, and the mod initialises
> without warnings.
>
> ```
> 01:30:22 [CFCloud_Bridge] [INFO] Registered 5 actions, 30 known to GameLabs in total.
> 01:30:26 [CFCloud_Bridge] [INFO] Initialized - version 0.3.0
> ```
>
> The three Terje actions of v0.4.0 (status, set stat, heal) have been written against
> the TerjeCore source but have not been compiled on a server yet.
>
> What is **not** yet proven is the in-game behaviour of each action — see *Acceptance*.
> The design document and the underlying audits are kept outside this repository.

---

## Repository Layout

```text
CFCloud_Cars_Addon/                 ← repository root (this README)
├── CFCloud_Bridge/                 ← the DayZ mod — PBO source folder
│   ├── $PBOPREFIX$                 ← must equal the folder name
│   ├── config.cpp                  ← CfgPatches + CfgMods, script layer registration
│   ├── stringtable.csv             ← localisation (full 15-column layout)
│   └── scripts/
│       ├── 3_Game/                 ← logger, settings, constants, manager
│       ├── 4_World/GameLabs/       ← aircraft tracking hook, action base classes
│       └── 5_Mission/              ← MissionServer hook, the actions
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

## Functionality

Eight fixed Dynamic Actions plus one generated per Terje stat, all server-side. The vehicle group needs DayZ Expansion
Vehicles, the Terje group needs TerjeCore; each group is compiled only when its mod is
present.

| Action | Context | What it does |
|---|---|---|
| **Unlock vehicle** | vehicle | Opens an Expansion-locked vehicle, key pairing left intact |
| **Lock vehicle (admin)** | vehicle | Force-locks a vehicle; the owner's key cannot undo it |
| **Vehicle status** | vehicle | Logs lock state, key pairing, persistent IDs and owner |
| **Set vehicle owner** | vehicle | Reassigns ownership (target player must be online), handles the keychain case |
| **Player's vehicles** | player | Lists every vehicle owned by that player |
| **Terje status** | player | Lists every Terje stat with current value, maximum and the ID the set action takes |
| **Terje set stat** | player | Sets one Terje stat by ID to an absolute value, clamped to Terje's own range. Generic path, mainly for the Data API |
| **Terje heal** | player | Resets every Terje condition, the same full heal Terje offers to VPP/COT. Vanilla health, blood and shock are left alone |
| **Terje: *&lt;stat&gt;* setzen** | player | **One action per registered Terje stat**, generated at startup from Terje's registry — the same list the COT compatibility module shows as sliders. Sleep, mind, every disease and wound counter, radiation buffer and sickness, every skill, souls. Only a value field |

The per-stat actions follow Terje, not this mod: a Terje update that registers a new
stat produces a new action on the next server start.

### Terje stat IDs

The set action takes the ID Terje registers in its admin-tool bridge
(`TerjeCore/Scripts/4_World/Compatibility/TerjeAdmintoolSupport.c` and the module
overrides). Which IDs exist depends on the Terje modules the server runs; the status
action prints the live list. The common ones:

| Module | ID | Meaning |
|---|---|---|
| Medicine | `terjeSleep` | Sleep (0 = exhausted, max = fully rested) |
| Medicine | `terjeMind` | Mental state (0 = insane, max = stable) |
| Medicine | `terjePain`, `terjeInfluenza`, `terjeSepsis`, `terjeZVirus`, `terjeRabies`, `terjePoison`, `terjeBiohazard`, `terjeOverdose`, `terjeContusion` | Diseases and conditions, 0 = healthy |
| Medicine | `terjeBC`, `terjeBD`, `terjeBCS`, `terjeBDS`, `terjeCS`, `terjeDS`, `terjeHematomas`, `terjeStubs`, `terjeBullets`, `terjeViscera` | Wounds, bandages and sutures |
| Radiation | `terjeRadiationAccum` | Absorbed dose buffer |
| Radiation | `terjeRadiationSick` | Radiation sickness |
| Skills | skill ID (e.g. `athletic`) | Skill level |
| StartScreen | `terjeSouls` | Souls |

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
| DayZ Expansion — **Vehicles** module | optional — enables the five vehicle actions; the lock system lives there, not in Expansion Core |
| [TerjeCore](https://github.com/TerjeBruoygard/TerjeMods) | optional — enables the three Terje actions; Medicine, Radiation, Skills and StartScreen each add their own stats |
| CFTools Cloud | an active application grant for the server |

Script references to the optional mods are guarded by preprocessor defines
(`GAMELABS`, `EXPANSIONMODVEHICLE`, `TERJE_CORE_MOD`), so a missing mod never breaks
compilation of this one — the matching action group simply does not register.

`requiredAddons` is `{"DZ_Data", "GameLabs_Scripts"}`. The GameLabs entry is not
decoration: it pins the order in which `modded class MissionServer` is merged. Without
it this mod's class is applied before GameLabs', `GLActionRegisterHook()` does not exist
yet, and the server refuses to compile the Mission module. GameLabs anchors its own
`Z_Dependencies` addon the same way.

The side effect is a hard dependency on GameLabs at config level. That matches reality —
the mod does nothing without it — but it means "install it and it stays inert" is no
longer a claim we can make for a server without GameLabs.

---

## Installation

The mod is pure server-side logic — no items, no layouts, no sounds, no keybinds — so
it belongs in **`-serverMod`**, next to GameLabs itself:

```bat
-serverMod=@GameLabs;@CFCloud_Bridge -mod=@DayZExpansion...
```

Putting it in `-mod` also works, but has two drawbacks: clients then load this mod while
GameLabs (a server-side mod) is absent on their side, and the PBO has to be signed to
pass `verifySignatures=2`. Neither applies in `-serverMod`.

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
| `m_AllowTerjeSetStat` | `true` | Terje set-stat action enabled |
| `m_AllowTerjeHeal` | `true` | Terje full-heal action enabled |
| `m_DiscordWebhookUrl` | `""` | Where action results are reported. Empty = server log only |

The file is rewritten on every load, so it always matches the current schema: options
added by an update appear with their defaults, options that no longer exist disappear.

---

## Acceptance

Enforce Script has no test framework, so these are checked on a running server.
**Done:** the mod compiles, loads and registers its actions (see *Status*).
**Open:**

1. `GET /v1/server/{id}/GameLabs/actions` lists all five `CFCloudBridge_*` actions
2. Unlock a locked car → open, lock sound, key still paired
3. Sell that car on the P2P market → works (proves no `FORCEDUNLOCKED` was set)
3b. With `m_DiscordWebhookUrl` set, each action posts an embed to that channel
4. Admin-lock a car → `READY_TO_FORCELOCK`, then `FORCEDLOCKED` a tick later
5. Try to unlock that car in game with the matching key → not possible
6. Restart the server → unlocked state survives
7. Spawn an Expansion helicopter → appears in `GET .../GameLabs/entities/vehicles`
8. Start the server **without** Expansion Vehicles → the mod loads, compiles and stays
   inert (`#ifdef EXPANSIONMODVEHICLE` covers this)
9. Start the server **without** GameLabs → behaviour unverified. `requiredAddons` now
   names `GameLabs_Scripts`, and whether DayZ treats a missing requiredAddon as a
   warning or as a load failure has not been tested here

Checks 3, 5 and 8 matter most: they cover the failures that would otherwise surface
weeks later as a player complaint.

---

## Good to know

**The CFCloud interface cannot show action results — set a webhook instead.** This is
not a limitation of this mod. `GameLabsActionResponse.Execute()` implements exactly one
response type, `webhook`; the `output` type its own comment mentions has no
implementation at all, and the Data API answers an action with `204` (*queued*, not
*executed*). So clicking an action in CFCloud will always look like nothing happened.

Put a Discord webhook URL in `m_DiscordWebhookUrl` and every action posts its result
there as an embed — what it did or why it refused, plus the vehicle's state. Leave it
empty and the server log stays the only output, which makes the two read-only actions
(status, player's vehicles) effectively useless from the interface.

**The admin lock takes effect one server tick later.** Expansion's own path sets
`READY_TO_FORCELOCK` first and converts it in `UpdateLock()`. A status check fired
immediately afterwards may still report the intermediate state — that is correct
behaviour, not a bug.

**Setting an owner needs the player online.** Expansion stores the owner as the DayZ
UID (`PlayerIdentity.GetId()`), which cannot be derived from a SteamID64 by script —
only a connected identity carries both. Writing a SteamID64 into that field would
create an owner the game never recognises, so the action resolves a live identity or
refuses.

**The generic set-stat action takes the ID as text, not from a dropdown.** GameLabs'
`options` data type is used by exactly one shipped action, and only with vector
options; how the backend returns a selected string option is undocumented. For
clicking in the interface, use the per-stat actions instead — they need no ID.

**Reading Terje's registry in `GLActionRegisterHook()` is safe.** The vanilla
`MissionBase` constructor runs `PluginManagerInit()`, which creates and initialises
every plugin including Terje's settings plugin, before GameLabs' `MissionServer`
constructor calls the hook. That is what makes the generated per-stat actions
possible.

**Unlocking does not mark the vehicle as lockpicked.** Expansion uses the
`FORCEDUNLOCKED` state as its "broken into" marker, and the P2P market refuses to sell
a vehicle in that state. This mod always passes the target state explicitly and never
relies on the default parameter.

---

## License

[AGPL-3.0-only](LICENSE) · Copyright (c) 2026 Psyern / Deadmans Echo

No third-party source code is bundled or redistributed. See [NOTICE](NOTICE) for the
mods this addon interoperates with and the APIs it calls.
