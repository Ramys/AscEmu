/*
 * Moon++ Scripts for Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team
 * Copyright (C) 2007-2015 Moon++ Team <http://www.moonplusplus.info/>
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

#ifndef _INSTANCE_BASE_H
#define _INSTANCE_BASE_H

#include "Map/WorldCreatorDefines.hpp"

const int32 INVALIDATE_TIMER = -1;
const uint32 DEFAULT_UPDATE_FREQUENCY = 1000;    //milliseconds
const uint32 DEFAULT_DESPAWN_TIMER = 2000;      //milliseconds

#define MOONSCRIPT_INSTANCE_FACTORY_FUNCTION(ClassName, ParentClassName) \
public:\
    ADD_INSTANCE_FACTORY_FUNCTION(ClassName);\
    typedef ParentClassName ParentClass;

enum EncounterState
{
    State_NotStarted    = 0,
    State_InProgress    = 1,
    State_Finished      = 2,
    State_Performed     = 3,
    State_PreProgress   = 4,        // for example: violet hold
    State_InvalidState  = 0xff
};

enum InstanceType
{
    Type_Default    = INSTANCE_NULL,
    Type_Raid       = INSTANCE_RAID,
    Type_NonRaid    = INSTANCE_NONRAID,
    Type_PvP        = INSTANCE_BATTLEGROUND,    //pvp
    Type_MultiMode  = INSTANCE_MULTIMODE
};

enum InstanceMode
{
    Mode_Normal     = MODE_NORMAL_10MEN,
    Mode_Heroic     = MODE_NORMAL_25MEN,
    Mode_Normal_10  = MODE_NORMAL_10MEN,
    Mode_Normal_25  = MODE_NORMAL_25MEN,
    Mode_Heroic_10  = MODE_HEROIC_10MEN,
    Mode_Heroic_25  = MODE_HEROIC_25MEN
};

enum DataType
{
    Data_EncounterState     = 0,
    Data_EncounterProgress  = 1,
    Data_UnspecifiedType    = 2
};

enum GameObjectState
{
    State_Active    = 0,    // Door: open
    State_Inactive  = 1
};

struct BossData
{
    BossData(EncounterState pState)
    {
        mSqlId = 0;
        mGuid = 0;
        mState = pState;
    };

    BossData(uint32 pId = 0, uint64 pGuid = 0, EncounterState pState = State_NotStarted)
    {
        mSqlId = pId;
        mGuid = pGuid;
        mState = pState;
    };

    ~BossData()
    {
    };

    uint32 mSqlId;
    uint64 mGuid;
    EncounterState mState;
};

class MoonInstanceScript;

typedef std::map<uint32, BossData> EncounterMap;
typedef std::map<uint32, GameObjectState> GameObjectEntryMap;

typedef std::vector<uint32> IdVector;
typedef std::set<uint32> IdSet;
typedef std::map<uint32, IdVector> EntryIdMap;

typedef std::pair<int32, int32> TimerPair;
typedef std::vector<TimerPair> TimerArray;

typedef std::unordered_map<uint32, GameObject*> GameObjectMap;

typedef std::set<Unit*> UnitSet;
typedef std::set<Player*> PlayerSet;
typedef std::set<Creature*> CreatureSet;
typedef std::set<GameObject*> GameObjectSet;

typedef std::map<uint32, uint32> WorldStateMap;

//////////////////////////////////////////////////////////////////////////////////////////
//Class MoonInstanceScript
class MoonInstanceScript : public InstanceScript
{
    public:
        MoonInstanceScript(MapMgr* pMapMgr);
        virtual ~MoonInstanceScript();

        // Creature
        Creature* GetCreatureBySqlId(uint32 pSqlId);
        Creature* GetCreatureByGuid(uint32 pGuid);
        Creature* FindClosestCreatureOnMap(uint32 pEntry, float pX, float pY, float pZ);
        Creature* SpawnCreature(uint32 pEntry, Movement::Location pLocation);
        Creature* SpawnCreature(uint32 pEntry, float pX, float pY, float pZ, float pO);
        Creature* SpawnCreature(uint32 pEntry, float pX, float pY, float pZ, float pO, uint32 pFactionId);
        Creature* PushCreature(uint32 pEntry, float pX, float pY, float pZ, float pO, uint32 pFaction = 0);
        CreatureSet FindCreaturesOnMap(uint32 pEntry);
        CreatureSet FindCreaturesOnMap(std::vector<uint32> pEntries);

        // GameObject
        GameObject* GetGameObjectBySqlId(uint32 pSqlId);
        GameObject* GetGameObjectByGuid(uint32 pGuid);
        GameObject* FindClosestGameObjectOnMap(uint32 pEntry, float pX, float pY, float pZ);
        GameObject* SpawnGameObject(uint32 pEntry, float pX, float pY, float pZ, float pO);
        GameObjectSet FindGameObjectsOnMap(uint32 pEntry);
        void AddGameObjectStateByEntry(uint32 pEntry, GameObjectState pState, bool pUseQuery = false);
        void AddGameObjectStateById(uint32 pId, GameObjectState pState);

        // Distance calculation
        float GetRangeToObject(Object* pObjectA, Object* pObjectB);
        float GetRangeToObject(Object* pObject, float pX, float pY, float pZ);
        float GetRangeToObject(float pX1, float pY1, float pZ1, float pX2, float pY2, float pZ2);

        // Player and instance - reimplementation for easier calling
        bool HasPlayers();
        size_t GetPlayerCount();
        Player* GetPlayerByGuid(uint32 pGuid);
        bool IsCombatInProgress();
        PlayerTeam GetInstanceTeam() { return mInstanceTeam; }

        // Timers - reimplementation from MoonScriptCreatureAI
        int32 AddTimer(int32 pDurationMillisec);
        int32 GetTimer(int32 pTimerId);
        void RemoveTimer(int32 & pTimerId);
        void ResetTimer(int32 pTimerId, int32 pDurationMillisec);
        bool IsTimerFinished(int32 pTimerId);
        void CancelAllTimers();

        // Update Event
        void RegisterScriptUpdateEvent();
        void SetUpdateEventFreq(uint32 pUpdateFreq);
        uint32 GetUpdateEventFreq();

        // Cells
        void SetCellForcedStates(float pMinX, float pMaxX, float pMinY, float pMaxY, bool pActivate = true);

        virtual void UpdateEvent();
        virtual void Destroy();

        void SendUnitEncounter(uint32_t type, Unit* unit = nullptr, uint8_t value_a = 0, uint8_t value_b = 0);

    protected:

        // Encounter generators
        void BuildEncounterMap();
        void BuildEncounterMapWithEntries(IdVector pEntries);
        void BuildEncounterMapWithIds(IdVector pIds);
        IdVector BuildIdVector(uint32 pCount, ...);
        IdSet BuildIdSet(uint32 pCount, ...);

        EncounterMap mEncounters;
        WorldStateMap mWorldStates;
        GameObjectEntryMap mGameObjects;

        uint32 mUpdateFrequency;
        TimerArray mTimers;
        int32 mTimerIdCounter;
        bool mSpawnsCreated;
        bool mInstanceTeamSet;
        PlayerTeam mInstanceTeam;
};

#endif      // _INSTANCE_BASE_H
