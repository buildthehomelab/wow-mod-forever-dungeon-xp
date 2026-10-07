# Forever Dungeon XP

An [AzerothCore](https://www.azerothcore.org/) (WotLK 3.3.5a) module that brings WoW Forever's
dungeon XP rework to a 3.3.5 server:

- **Kills inside a dungeon give much less XP**: a quarter of normal by default.
- **Dungeon quests give much more XP**: three times normal by default.

Quests only pay out once, so a first run through a dungeon, with its quests, still levels you well.
Running the same dungeon again and again doesn't. No client patch, no database changes.

## What counts as a dungeon

- **Kills:** any kill while you're on a dungeon map (normal or heroic). Raids only count if
  `IncludeRaids` is on. Battlegrounds and the open world are never affected. Your pet's XP from the
  kill is cut the same way, and so is the rested bonus on it.
- **Quests:** a quest counts when the game data marks it as a Dungeon or Heroic quest, like
  "Red Silk Bandanas" or "The Defias Brotherhood", or when it's sorted under a dungeon's zone in the
  quest log, like "Oh Brother. . ." (Deadmines) or "Serpentbloom" (Wailing Caverns). With
  `IncludeRaids` on, Raid quests count too.

These quests are never boosted, because they pay out every time:

- daily, weekly and monthly quests
- repeatable quests
- Dungeon Finder reward quests

The quest giver shows the boosted XP when you pick up the quest and when you turn it in.

## Requirements

- [AzerothCore](https://github.com/azerothcore/azerothcore-wotlk) `master` (WotLK 3.3.5a)
- A WoW 3.3.5a (12340) client
- No client patch and no database changes
- Optional: [mod-individual-progression](https://github.com/ZhengPeiRu21/mod-individual-progression) and mod-playerbots work alongside it (see "With other modules")

## Install

Clone it into your AzerothCore `modules` folder **as `mod-forever-dungeon-xp`**, without the
repo's `wow-` prefix. AzerothCore finds the module's entry point from the folder name.

```bash
cd <azerothcore>/modules
git clone https://github.com/buildthehomelab/wow-mod-forever-dungeon-xp.git mod-forever-dungeon-xp
```

Rebuild the worldserver, then copy `conf/mod_forever_dungeon_xp.conf.dist` to your config folder
as `mod_forever_dungeon_xp.conf`.

## Settings

| Setting | Default | What it does |
|---------|---------|--------------|
| `ForeverDungeonXP.Enable` | `1` | Master switch. With `0`, dungeon kills and quests give normal XP. |
| `ForeverDungeonXP.KillMultiplier` | `0.25` | Multiplies XP from kills inside dungeons. `0` for none. |
| `ForeverDungeonXP.QuestMultiplier` | `3.0` | Multiplies XP from one-time dungeon quests. |
| `ForeverDungeonXP.IncludeRaids` | `0` | With `1`, raids and raid quests count as dungeons too. |

Both multipliers apply on top of the server's XP rates. `.reload config` picks up changes without a
restart. To remove the module, delete it and rebuild; it doesn't store anything.

## With other modules

- **mod-individual-progression:** works alongside it. Its vanilla quest XP fix and this module's
  multiplier stack, and its level cap for players still in vanilla content still applies.
- **mod-playerbots:** bots follow the same rules as players.

## Limits

- The dungeon-quest check relies on the game data. A dungeon quest that's neither marked as one nor
  sorted under the dungeon's zone isn't boosted. A breadcrumb quest that's sorted under a dungeon's
  zone but done outside, like "Hamuul Runetotem", is boosted.
- Upper Blackrock Spire shares its map with Lower Blackrock Spire, so its kills and its quests
  (sorted under Blackrock Spire) count as dungeon content even with `IncludeRaids` off.
- Only kill XP inside dungeons is cut. Exploration XP is unchanged.

## Troubleshooting

- **Raid kills or raid quests give normal XP.** Raids only count as dungeons with
  `ForeverDungeonXP.IncludeRaids = 1`.
- **A dungeon quest isn't boosted.** Daily, weekly, monthly, repeatable and Dungeon Finder quests
  never are, and a quest that the game data neither marks as a dungeon quest nor sorts under a
  dungeon's zone isn't either.
- **A multiplier change doesn't show.** Run `.reload config` or restart the worldserver.

## Credits

Author: [buildthehomelab](https://github.com/buildthehomelab)

The design follows the WoW Forever private server ruleset. The code is original.

## License

MIT. See [LICENSE](LICENSE).
