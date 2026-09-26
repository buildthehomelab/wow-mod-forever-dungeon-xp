/*
 * mod-forever-dungeon-xp
 *
 * WoW Forever's dungeon XP rework: kills inside a dungeon give much less XP, and dungeon quests
 * give much more. A first run through a dungeon, with its quests, still levels you well, but running
 * the same dungeon again and again doesn't, because the quests only pay out once.
 *
 * Kill XP is cut in OnPlayerGiveXP when the player is on a dungeon map, before the core hands it
 * out, so pets and rested XP scale with it. Quest XP is raised in OnPlayerQuestComputeXP, which the
 * core also runs for the quest giver's windows, so players see the raised number before they
 * accept and when they turn in.
 *
 * A quest counts as a dungeon quest when the game data marks it as one (Dungeon or Heroic quest
 * type), or when it's sorted under a dungeon's zone. Daily, weekly, monthly, repeatable and Dungeon
 * Finder reward quests are left alone.
 *
 * Released under the MIT License.
 */

#include "Config.h"
#include "DBCStores.h"
#include "Map.h"
#include "Player.h"
#include "QuestDef.h"
#include "ScriptMgr.h"

#include <cmath>

namespace
{
    struct Config
    {
        bool enabled = true;
        float killMultiplier = 0.25f;
        float questMultiplier = 3.0f;
        bool includeRaids = false;
    };

    Config config;

    bool CountsAsDungeon(MapEntry const* map)
    {
        return map && (map->IsNonRaidDungeon() || (config.includeRaids && map->IsRaid()));
    }

    bool IsDungeonQuest(Quest const* quest)
    {
        switch (quest->GetType())
        {
            case QUEST_TYPE_DUNGEON:
            case QUEST_TYPE_HEROIC:
                return true;
            case QUEST_TYPE_RAID:
            case QUEST_TYPE_RAID_10:
            case QUEST_TYPE_RAID_25:
                if (config.includeRaids)
                    return true;
                break;
            default:
                break;
        }

        // Many dungeon quests have no quest type but are sorted under the dungeon's zone, like
        // "Oh Brother. . ." (Deadmines) or "Serpentbloom" (Wailing Caverns). A negative value is a
        // quest category (class, profession, holiday), not a zone.
        if (quest->GetZoneOrSort() > 0)
            if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(quest->GetZoneOrSort()))
                return CountsAsDungeon(sMapStore.LookupEntry(area->mapid));

        return false;
    }

    // Only quests that pay out once. The core already gives no XP for turning a plain quest in
    // twice, but dailies and the like pay every time, so boosting them would bring the farming back.
    bool IsOneTimeQuest(Quest const* quest)
    {
        return !quest->IsDaily() && !quest->IsWeekly() && !quest->IsMonthly() && !quest->IsRepeatable()
            && !quest->IsDFQuest();
    }

    uint32 Scale(uint32 xp, float multiplier)
    {
        if (!xp || multiplier <= 0.0f)
            return 0;

        // Keep at least 1 XP, so a small kill still shows up as a kill that gave XP.
        return std::max<uint32>(1, uint32(std::lround(double(xp) * multiplier)));
    }
}

class ForeverDungeonXPWorldScript : public WorldScript
{
public:
    ForeverDungeonXPWorldScript() : WorldScript("ForeverDungeonXPWorldScript") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        config.enabled         = sConfigMgr->GetOption<bool>("ForeverDungeonXP.Enable", true);
        config.killMultiplier  = sConfigMgr->GetOption<float>("ForeverDungeonXP.KillMultiplier", 0.25f);
        config.questMultiplier = sConfigMgr->GetOption<float>("ForeverDungeonXP.QuestMultiplier", 3.0f);
        config.includeRaids    = sConfigMgr->GetOption<bool>("ForeverDungeonXP.IncludeRaids", false);
    }
};

class ForeverDungeonXPPlayerScript : public PlayerScript
{
public:
    ForeverDungeonXPPlayerScript() : PlayerScript("ForeverDungeonXPPlayerScript",
        { PLAYERHOOK_ON_GIVE_EXP, PLAYERHOOK_ON_QUEST_COMPUTE_EXP }) { }

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* /*victim*/, uint8 xpSource) override
    {
        if (!config.enabled || xpSource != XPSOURCE_KILL || !amount || !player || !player->IsInWorld())
            return;

        if (CountsAsDungeon(player->GetMap()->GetEntry()))
            amount = Scale(amount, config.killMultiplier);
    }

    // Also runs for the quest giver's windows, where player can be null.
    void OnPlayerQuestComputeXP(Player* player, Quest const* quest, uint32& xpValue) override
    {
        if (!config.enabled || !xpValue || !player || !quest)
            return;

        if (IsOneTimeQuest(quest) && IsDungeonQuest(quest))
            xpValue = Scale(xpValue, config.questMultiplier);
    }
};

void AddForeverDungeonXPScripts()
{
    new ForeverDungeonXPWorldScript();
    new ForeverDungeonXPPlayerScript();
}
