/*
 * mod-heartstone-no-cd
 *
 * Cuando un jugador castea la Piedra de Hogar (spell 8690) se le devuelve el
 * enfriamiento, o se le aplica el configurado en el .conf (en segundos).
 *
 *   HeartstoneNoCd.Enable      = 1
 *   HeartstoneNoCd.Cooldown    = 0   (segundos; 0 = sin enfriamiento)
 *   HeartstoneNoCd.Bots.Enable = 0   (playerbots: 0 = los bots NO reciben el efecto)
 */

#include "Config.h"
#include "GameTime.h"
#include "Item.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellInfo.h"

// Dependencia opcional de mod-playerbots: si no esta instalado, se ignora.
#if __has_include("Playerbots.h")
    #include "Playerbots.h"
    #define HSNC_HAS_PLAYERBOTS 1
#else
    #define HSNC_HAS_PLAYERBOTS 0
#endif

using namespace std::chrono_literals;

namespace
{
    constexpr uint32 SPELL_HEARTHSTONE = 8690;
    constexpr uint32 ITEM_HEARTHSTONE  = 6948;

    bool   g_enabled         = true;
    bool   g_botsEnabled     = false;
    uint32 g_cooldownSeconds = 0;

    bool IsPlayerbot(Player* player)
    {
#if HSNC_HAS_PLAYERBOTS
        return GET_PLAYERBOT_AI(player) != nullptr;
#else
        (void)player;
        return false;
#endif
    }
}

class HeartstoneNoCdWorldScript : public WorldScript
{
public:
    HeartstoneNoCdWorldScript() : WorldScript("HeartstoneNoCdWorldScript", { WORLDHOOK_ON_AFTER_CONFIG_LOAD }) { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        g_enabled         = sConfigMgr->GetOption<bool>("HeartstoneNoCd.Enable", true);
        g_botsEnabled     = sConfigMgr->GetOption<bool>("HeartstoneNoCd.Bots.Enable", false);
        g_cooldownSeconds = sConfigMgr->GetOption<uint32>("HeartstoneNoCd.Cooldown", 0);
    }
};

class HeartstoneNoCdPlayerScript : public PlayerScript
{
public:
    HeartstoneNoCdPlayerScript() : PlayerScript("HeartstoneNoCdPlayerScript", { PLAYERHOOK_ON_SPELL_CAST }) { }

    void OnPlayerSpellCast(Player* player, Spell* spell, bool /*skipCheck*/) override
    {
        if (!g_enabled || !player || !spell)
            return;

        if (spell->GetSpellInfo()->Id != SPELL_HEARTHSTONE)
            return;

        if (!g_botsEnabled && IsPlayerbot(player))
            return;

        uint32 itemId = spell->m_CastItem ? spell->m_CastItem->GetEntry() : ITEM_HEARTHSTONE;

        // Este hook corre ANTES de que el core aplique el cooldown, asi que
        // se reajusta apenas termina el cast (el evento muere junto al Player).
        player->m_Events.AddEventAtOffset([player, itemId]()
        {
            player->RemoveSpellCooldown(SPELL_HEARTHSTONE, true);

            if (g_cooldownSeconds > 0)
            {
                uint32 endMs = static_cast<uint32>(GameTime::GetGameTimeMS().count()) + g_cooldownSeconds * 1000;
                player->AddSpellCooldown(SPELL_HEARTHSTONE, itemId, endMs, true);
            }
        }, 100ms);
    }
};

void AddHeartstoneNoCdScripts()
{
    new HeartstoneNoCdWorldScript();
    new HeartstoneNoCdPlayerScript();
}
