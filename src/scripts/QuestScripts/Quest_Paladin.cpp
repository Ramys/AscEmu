/*
 * Copyright (c) 2014-2020 AscEmu Team <http://www.ascemu.org>
 * Copyright (c) 2008-2015 Sun++ Team <http://www.sunplusplus.info>
 * Copyright (c) 2007-2015 Moon++ Team <http://www.moonplusplus.info>
 * Copyright (C) 2008-2012 ArcEmu Team <http://www.ArcEmu.org/>
 * Copyright (C) 2005-2007 Ascent Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Setup.h"

class PaladinDeadNPC : public CreatureAIScript
{
    ADD_CREATURE_FACTORY_FUNCTION(PaladinDeadNPC);
    explicit PaladinDeadNPC(Creature* pCreature) : CreatureAIScript(pCreature) {}

    void OnLoad()
    {
        getCreature()->setStandState(STANDSTATE_DEAD);
        getCreature()->setDeathState(CORPSE);
        getCreature()->GetAIInterface()->m_canMove = false;
    }
};

class GildedBrazier : public GameObjectAIScript
{
public:

    explicit GildedBrazier(GameObject* goinstance) : GameObjectAIScript(goinstance) {}
    static GameObjectAIScript* Create(GameObject* GO) { return new GildedBrazier(GO); }

    void OnActivate(Player* pPlayer)
    {
        if (pPlayer->HasQuest(9678))
        {
            float SSX = pPlayer->GetPositionX();
            float SSY = pPlayer->GetPositionY();
            float SSZ = pPlayer->GetPositionZ();
            float SSO = pPlayer->GetOrientation();

            GameObject* Brazier = pPlayer->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(SSX, SSY, SSZ, 181956);
            if (Brazier)
            {
                Brazier->setState(GO_STATE_OPEN);
                pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(17716, SSX, SSY, SSZ, SSO, true, false, 0, 0)->Despawn(600000, 0);
            }
        }
        else
        {
            pPlayer->BroadcastMessage("Missing required quest : The First Trial");
        }
    }
};

class stillbladeQAI : public CreatureAIScript
{
    ADD_CREATURE_FACTORY_FUNCTION(stillbladeQAI);
    explicit stillbladeQAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
    }

    void OnDied(Unit* mKiller)
    {
        float SSX = mKiller->GetPositionX();
        float SSY = mKiller->GetPositionY();
        float SSZ = mKiller->GetPositionZ();

        GameObject* Brazier = mKiller->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(SSX, SSY, SSZ, 181956);
        if (Brazier)
        {
            Brazier->setState(GO_STATE_CLOSED);
        }
    }
};

void SetupPaladin(ScriptMgr* mgr)
{
    mgr->register_creature_script(17768, &PaladinDeadNPC::Create);
    mgr->register_creature_script(17542, &PaladinDeadNPC::Create);
    mgr->register_creature_script(6177, &PaladinDeadNPC::Create);
    mgr->register_creature_script(6172, &PaladinDeadNPC::Create);
    mgr->register_gameobject_script(181956, &GildedBrazier::Create);
    mgr->register_creature_script(17716, &stillbladeQAI::Create);
}
