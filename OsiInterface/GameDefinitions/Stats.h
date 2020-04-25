#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "Wrappers.h"

namespace dse
{

#pragma pack(push, 1)
	struct CRPGStats_Requirement
	{
		RequirementType RequirementId;
		int32_t IntParam;
		FixedString StringParam;
		bool Negate;
		uint8_t _Pad[7];
	};

	template <class T>
	struct CNamedElementManager : public Noncopyable<CNamedElementManager<T>>
	{
		void* VMT{ nullptr };
		ObjectSet<T *> Primitives;
		Map<FixedString, uint32_t> NameHashMap;
		uint32_t Unused{ 0 };
		uint32_t NumItems{ 0 };
		uint32_t NumSomeItems{ 0 };

		void Add(FixedString const& name, T* elem)
		{
			NameHashMap.Insert(name, Primitives.Set.Size);
			Primitives.Set.Add(elem);
		}

		int FindIndex(char const * str) const
		{
			auto fs = ToFixedString(str);
			if (fs) {
				return FindIndex(fs);
			} else {
				return -1;
			}
		}

		int FindIndex(FixedString const& str) const
		{
			auto ptr = NameHashMap.Find(str);
			if (ptr != nullptr) {
				return (int)*ptr;
			} else {
				return -1;
			}
		}

		T * Find(int index) const
		{
			if (index < 0 || index >= (int)Primitives.Set.Size) {
				return nullptr;
			} else {
				return Primitives.Set.Buf[index];
			}
		}

		T * Find(char const * str) const
		{
			auto fs = ToFixedString(str);
			if (fs) {
				return Find(fs);
			} else {
				return nullptr;
			}
		}

		T * Find(FixedString const& str) const
		{
			auto ptr = NameHashMap.Find(str);
			if (ptr != nullptr) {
				return Primitives.Set.Buf[*ptr];
			} else {
				return nullptr;
			}
		}
	};

	struct CRPGStats_Object_Property : public Noncopyable<CRPGStats_Object_Property>
	{
		virtual ~CRPGStats_Object_Property() {}
		virtual CRPGStats_Object_Property * Clone() = 0;

		FixedString Name;
		CRPGStats_Object_Property_Type TypeId;
	};

	struct CDivinityStats_Condition;

	struct CDivinityStats_Object_Property_Data : public CRPGStats_Object_Property
	{
		virtual ~CDivinityStats_Object_Property_Data() {}
		virtual CRPGStats_Object_Property* Clone() { return nullptr; }
		virtual bool GetDescription(STDWString* Line1) { return false; }
		virtual bool GetDescription(TranslatedString * Line1, TranslatedString * Line2) { return false; }
		virtual bool GetDescription(TranslatedString * Line1) { return false; }
		virtual uint64_t Unknown() { return 0; }

		CRPGStats_Object_PropertyContext PropertyContext;
		uint8_t _Pad1[3];
		CDivinityStats_Condition* Conditions;
	};

	struct CDivinityStats_Object_Property_Custom : public CDivinityStats_Object_Property_Data
	{
		int CustomProperties; // "Custom Properties" from ValueLists
	};

	struct CDivinityStats_Object_Property_Status : public CDivinityStats_Object_Property_Data
	{
		FixedString Status;
		float StatusChance;
		float Duration;
		FixedString Argument3;
		int Argument4;
		int Argument5;
		bool HasBoost;
		uint8_t _Pad1[7];
		ObjectSet<int> SurfaceBoosts; // ESurfaceType
	};

	struct CDivinityStats_Object_Property_SurfaceChange : public CDivinityStats_Object_Property_Data
	{
		int SurfaceChange; // "Surface Change" from ValueLists
		float Arg1;
		float Arg2;
		float Arg3;
		float Arg4;
	};
	
	struct CDivinityStats_Object_Property_Sabotage : public CDivinityStats_Object_Property_Data
	{
		int Amount;
	};
	
	struct CDivinityStats_Object_Property_Summon : public CDivinityStats_Object_Property_Data
	{
		FixedString Template;
		float Duration;
		bool IsTotem;
		char _Pad[3];
		FixedString Skill;
	};

	struct CDivinityStats_Object_Property_Force : public CDivinityStats_Object_Property_Data
	{
		int Distance;
		char _Pad[4];
		FixedString Unknown;
	};

	struct CDivinityStats_Object_Property_GameAction : public CDivinityStats_Object_Property_Data
	{
		int GameAction; // "Game Action" from ValueLists
		float Arg1;
		float Arg2;
		uint8_t _Pad[4];
		FixedString Arg3;
		float Arg4;
		float Arg5;
		int StatusHealType; // "StatusHealType" from ValueLists
	};

	struct CDivinityStats_Object_Property_OsirisTask : public CDivinityStats_Object_Property_Data
	{
		int OsirisTask; // "Osiris Task" from ValueLists
		float Chance;
		int VitalityOnRevive;
	};



	struct CRPGStats_Object_Property_CustomDescription : public CDivinityStats_Object_Property_Data
	{
		CRPGStats_Object_Property_CustomDescription()
		{}

		virtual ~CRPGStats_Object_Property_CustomDescription() {}

		inline static void operator delete (void * ptr, size_t sz)
		{
			GameFree(ptr);
		}

		virtual CRPGStats_Object_Property * Clone()
		{
			auto cl = GameAlloc<CRPGStats_Object_Property_CustomDescription>();
			cl->Name = Name;
			cl->TypeId = TypeId;
			cl->PropertyContext = PropertyContext;
			cl->Conditions = Conditions;
			cl->TextLine1 = TextLine1;
			return cl;
		}

		virtual bool GetDescription(STDWString * Line1)
		{
			*Line1 = TextLine1;
			return true;

		}

		virtual bool GetDescription(TranslatedString * Line1, TranslatedString * Line2)
		{
			GetDescription(Line1);
			return true;
		}

		virtual bool GetDescription(TranslatedString * Line1)
		{
			GetDescription(&Line1->Str1.WStr);
			return true;
		}

		virtual uint64_t Unknown()
		{
			return 0;
		}

		STDWString TextLine1;
	};


	struct CRPGStats_Object_Property_List
	{
		CNamedElementManager<CRPGStats_Object_Property> Properties;
		FixedString Name;
		CRPGStats_Object_PropertyContext AllPropertyContexts{ 0 };
		uint8_t _Pad[7];
	};

	struct CRPGStats_LevelMap : public Noncopyable<CRPGStats_LevelMap>
	{
		virtual ~CRPGStats_LevelMap() {}
		virtual void SetModifierList(int modifierListIndex, int modifierIndex) = 0;
		virtual void SetModifierList(FixedString const& modifierListName, FixedString const& modifierName) = 0;
		virtual int64_t GetScaledValue(int difficultyScale, int level) = 0;

		int ModifierListIndex{ -1 };
		int ModifierIndex{ -1 };
		int RPGEnumerationIndex{ -1 };
		uint8_t _Pad[4];
		FixedString Name;
	};

	struct CDivinityStats_Condition;

	struct CRPGStats_Object : public ProtectedGameObject<CRPGStats_Object>
	{
		void * VMT;
		uint32_t Handle;
		uint32_t Level;
		uint32_t ModifierListIndex;
		uint32_t Unused2;
		int32_t * IndexedProperties;
		uint64_t Unused3;
		uint64_t Unused4;
		char * Name;
		TranslatedString TranslatedStringX;
		FixedString FS2;
		struct CDivinityStats * DivStats;
		Map<FixedString, CRPGStats_Object_Property_List *> PropertyList;
		uint32_t Unused5;
		Map<FixedString, CDivinityStats_Condition *> ConditionList;
		uint32_t Unused6;
		uint64_t AIFlags;
		ObjectSet<CRPGStats_Requirement, GameMemoryAllocator, true> Requirements;
		ObjectSet<CRPGStats_Requirement, GameMemoryAllocator, true> MemorizationRequirements;
		ObjectSet<void *> CrimeReactionPriorities; // Set<CrimeReactionPriority>
		ObjectSet<FixedString, GameMemoryAllocator, true> StringProperties1;
		ObjectSet<FixedString, GameMemoryAllocator, true> ComboCategories;
#if !defined(OSI_EOCAPP)
		STDWString SomeSTDWSTRING;
#endif
		int32_t Using;
		uint32_t D;
	};

	struct CRPGStats_ObjectInstance : public CRPGStats_Object
	{
		uint32_t InstanceId;
	};

	struct TalentArray : public BitArray<4>
	{
		inline bool Toggle(TalentType talent, bool enabled)
		{
			if (enabled) {
				return Set((uint32_t)talent);
			} else {
				return Clear((uint32_t)talent);
			}
		}

		inline bool HasTalent(TalentType talent) const
		{
			return IsSet((uint32_t)talent);
		}
	};

	struct CDivinityStats_Equipment_Attributes : public ProtectedGameObject<CDivinityStats_Equipment_Attributes>
	{
		void * VMT;
		uint32_t InstanceId;
		uint32_t Unkn0;
		uint64_t Unkn1;
		int32_t Durability;
		int32_t DurabilityDegradeSpeed;
		int32_t StrengthBoost;
		int32_t FinesseBoost;
		int32_t IntelligenceBoost;
		int32_t ConstitutionBoost;
		int32_t MemoryBoost;
		int32_t WitsBoost;
		int32_t SightBoost;
		int32_t HearingBoost;
		int32_t VitalityBoost;
		int32_t SourcePointsBoost;
		int32_t MaxAP;
		int32_t StartAP;
		int32_t APRecovery;
		int32_t AccuracyBoost;
		int32_t DodgeBoost;
		int32_t LifeSteal;
		int32_t CriticalChance;
		int32_t ChanceToHitBoost;
		int32_t MovementSpeedBoost;
		int32_t RuneSlots;
		int32_t RuneSlots_V1;
		int32_t FireResistance;
		int32_t AirResistance;
		int32_t WaterResistance;
		int32_t EarthResistance;
		int32_t PoisonResistance;
		int32_t ShadowResistance;
		int32_t PiercingResistance;
		int32_t CorrosiveResistance;
		int32_t PhysicalResistance;
		int32_t MagicResistance;
		int32_t CustomResistance;
		int32_t Movement;
		int32_t Initiative;
		int32_t Willpower;
		int32_t Bodybuilding;
		int32_t MaxSummons;
		int32_t AbilityModifiers[40];
		int32_t Value;
		int32_t Weight;
		uint32_t Unkn2;
		void * Reflection;
		FixedString Skills;
		FixedString ItemColor;
		uint32_t ModifierType;
		uint32_t Unkn3;
		FixedString ObjectInstanceName;
		FixedString BoostName;
		EquipmentStatsType StatsType;
		TalentArray Talents;
		uint32_t Unkn4;
		uint64_t AttributeFlagsObjectId;

		struct PropertyMapBase & GetPropertyMap() const;
	};

	struct CDivinityStats_Equipment_Attributes_Weapon : public CDivinityStats_Equipment_Attributes
	{
		DamageType DamageType;
		int32_t MinDamage;
		int32_t MaxDamage;
		int32_t DamageBoost;
		int32_t DamageFromBase;
		int32_t CriticalDamage;
		float WeaponRange;
		int32_t CleaveAngle;
		float CleavePercentage;
		int32_t AttackAPCost;
	};

	struct CDivinityStats_Equipment_Attributes_Shield : public CDivinityStats_Equipment_Attributes
	{
		int32_t ArmorValue;
		int32_t ArmorBoost;
		int32_t MagicArmorValue;
		int32_t MagicArmorBoost;
		int32_t Blocking;
	};

	struct CDivinityStats_Equipment_Attributes_Armor : public CDivinityStats_Equipment_Attributes
	{
		int32_t ArmorValue;
		int32_t ArmorBoost;
		int32_t MagicArmorValue;
		int32_t MagicArmorBoost;
	};

	struct CDivinityStats_Item : public CRPGStats_ObjectInstance
	{
		uint32_t _Pad0;
		uint32_t U1;
		EquipmentStatsType ItemType;
		uint32_t ItemSlot;
		WeaponType WeaponType;
		int32_t AnimType; // -1 = Not overridden
		uint32_t WeaponRange;
		uint32_t IsIdentified; // Saved
		uint32_t U2;
		uint32_t U3;
		uint32_t U4;
		bool IsTwoHanded;
		bool ShouldSyncStats;
		bool U5;
		bool HasModifiedSkills; // Saved
		uint32_t U6;
		FixedString Skills; // Saved
		DamageType DamageTypeOverwrite; // Saved // -1 = Not overridden
		uint32_t Durability; // Saved
		uint32_t DurabilityCounter; // Saved
		uint32_t U7;
		FixedString ItemTypeReal; // Saved
		FixedString U8;
		CDivinityStats_Equipment_Attributes ** DynamicAttributes_Start;
		CDivinityStats_Equipment_Attributes ** DynamicAttributes_End;
		uint64_t U9[2];
		StatAttributeFlags AttributeFlags;
		int32_t MaxCharges; // -1 = Not overridden
		int32_t Charges; // -1 = Not overridden
		uint8_t LevelGroupIndex;
		uint8_t RootGroupIndex;
		uint8_t NameGroupIndex;
		uint8_t NameIndex;
		uint32_t NameCool;
		ObjectSet<void *> Tags_Maybe;
		ObjectSet<FixedString> BoostNameSet;
		uint64_t U10[12];

		int32_t GetPhysicalResistance();
		int32_t GetPiercingResistance();
		int32_t GetMagicResistance();
		int32_t GetCorrosiveResistance();

		bool HasTalent(TalentType talent);
		int32_t GetAbility(AbilityType ability);
	};

	struct EoCGameRandom
	{
		uint64_t Unkn0;
		uint32_t Unkn1[7];
		uint8_t Unkn2;
	};

	struct CharacterEquippedItem
	{
		int32_t ItemStatsHandle;
		ItemSlot ItemSlot;
		bool IsEquipped;
	};

	struct CharacterDynamicStat : public ProtectedGameObject<CharacterDynamicStat>
	{
		int32_t CharacterInstanceId;
		uint8_t _Pad0[4];
		CDivinityStats * DivStats;

		int32_t SummonLifelinkModifier;
		int32_t Strength;
		int32_t Memory;
		int32_t Intelligence;
		int32_t Movement;
		int32_t MovementSpeedBoost;
		int32_t Finesse;
		int32_t Wits;
		int32_t Constitution;

		int32_t FireResistance;
		int32_t EarthResistance;
		int32_t WaterResistance;
		int32_t AirResistance;
		int32_t PoisonResistance;
		int32_t ShadowResistance;

		int32_t Willpower;
		int32_t Bodybuilding;
		int32_t PiercingResistance;
		int32_t PhysicalResistance;
		int32_t CorrosiveResistance;
		int32_t MagicResistance;
		int32_t CustomResistance;

		int32_t Sight;
		int32_t Hearing;
		int32_t FOV;
		int32_t APMaximum;
		int32_t APStart;
		int32_t APRecovery;
		int32_t CriticalChance;
		int32_t Initiative;
		int32_t Vitality;
		int32_t VitalityBoost;
		int32_t MagicPoints;
		int32_t Level;
		int32_t Gain;

		int32_t Armor;
		int32_t MagicArmor;
		int32_t ArmorBoost;
		int32_t MagicArmorBoost;
		int32_t ArmorBoostGrowthPerLevel;
		int32_t MagicArmorBoostGrowthPerLevel;
		int32_t DamageBoost;
		int32_t DamageBoostGrowthPerLevel;

		int32_t Accuracy;
		int32_t Dodge;
		int32_t MaxResistance;
		int32_t LifeSteal;
		int32_t Weight;
		int32_t ChanceToHitBoost;
		int32_t RangeBoost;
		int32_t APCostBoost;
		int32_t SPCostBoost;
		int32_t MaxSummons;
		int32_t Abilities[40];
		int32_t BonusWeaponDamageMultiplier;
		TalentArray Talents;
		TalentArray RemovedTalents;
		int16_t Traits[18];
		uint32_t BoostConditionsMask;
		FixedString TranslationKey;
		FixedString BonusWeapon;
		uint64_t Reflection;
		uint32_t AttributeHandle;
		uint32_t StepsType;
		uint64_t AttributeFlagsObjectId;
		uint32_t Unkn4;
	};

	namespace esv { struct Character; }

	struct DamagePairList;
	struct HitDamageInfo;
	struct SkillPrototype;
	struct IGameObject;

	struct CDivinityStats_Character : public CRPGStats_ObjectInstance
	{
		typedef void (* HitInternalProc)(CDivinityStats_Character * self, CDivinityStats_Character *attackerStats,
			CDivinityStats_Item *item, DamagePairList *damageList, HitType hitType, bool noHitRoll,
			bool forceReduceDurability, HitDamageInfo *damageInfo, CRPGStats_Object_Property_List *skillProperties,
			HighGroundBonus highGroundFlag, CriticalRoll criticalRoll);

		uint32_t _Pad0;
		int32_t CurrentVitality;
		int32_t CurrentArmor;
		int32_t CurrentMagicArmor;
		int32_t ArmorAfterHitCooldownMultiplier;
		int32_t MagicArmorAfterHitCooldownMultiplier;
		int32_t MPStart;
		int32_t CurrentAP;
		int32_t BonusActionPoints; // Saved
		int32_t Experience; // Saved
		int32_t Reputation;
		uint8_t Flanked; // Saved
		uint8_t _Pad1[3];
		int32_t Karma; // Saved
		StatCharacterFlags Flags;
		uint8_t _Pad2[3];
		uint32_t Unkn0;
		ObjectSet<int> TraitOrder; // Saved
		int32_t MaxResistance;
		int32_t HasTwoHandedWeapon;
		IGameObject * Character;
		int32_t Unkn2;
		int32_t IsIncapacitatedRefCount;
		CharacterDynamicStat ** DynamicStats;
		CharacterDynamicStat ** DynamicStatsEnd;
		CharacterDynamicStat ** DynamicStatsCapacityEnd;
		CharacterDynamicStat * StatsFromStatsEntry;
		CharacterEquippedItem ** ItemStats;
		CharacterEquippedItem ** ItemStatsEnd;
		CharacterEquippedItem ** ItemStatsCapacityEnd;
		ObjectSet<void *> SurfacePathInfluences;
		int32_t ActiveBoostConditions[16]; // Saved
		EoCGameRandom DamageRng;
		uint8_t _Pad4[3];
		EoCGameRandom CriticalHitRng;
		uint8_t _Pad5[3];
		int32_t MaxVitality;
		int32_t BaseMaxVitality;
		int32_t MaxArmor;
		int32_t BaseMaxArmor;
		int32_t MaxMagicArmor;
		int32_t BaseMaxMagicArmor;
		float Sight;
		float BaseSight;
		StatAttributeFlags AttributeFlags;
		StatAttributeFlags BaseAttributeFlags;
		StatAttributeFlags ItemBoostedAttributeFlags;
		bool AttributeFlagsUpdated;
		uint8_t _Pad6[3];
		int32_t MaxSummons;
		int32_t BaseMaxSummons;
		int32_t MaxMpOverride; // Saved
		TalentArray DisabledTalents; // Saved

		std::optional<int32_t> GetStat(FixedString const& name, bool excludeBoosts);
		std::optional<int32_t> GetHitChance(CDivinityStats_Character * target);
		bool HasTalent(TalentType talent, bool excludeBoosts);
		int32_t GetAbility(AbilityType ability, bool excludeBoosts);
		CDivinityStats_Item * GetItemBySlot(ItemSlot slot, bool mustBeEquipped);
		CDivinityStats_Item * GetMainWeapon();
		CDivinityStats_Item * GetOffHandWeapon();
		CDivinityStats_Item * GetShield();
		int32_t GetPhysicalResistance(bool excludeBoosts);
		int32_t GetPiercingResistance(bool excludeBoosts);
		int32_t GetMagicResistance(bool excludeBoosts);
		int32_t GetCorrosiveResistance(bool excludeBoosts);
		int32_t GetDamageBoost();
		bool IsBoostActive(uint32_t conditionsMask);
	};

	typedef int32_t (CDivinityStats_Character__GetStat)(CDivinityStats_Character * self, bool baseStats);
	typedef int32_t (CDivinityStats_Character__GetStatNoBoosts)(CDivinityStats_Character * self);
	typedef int32_t (CDivinityStats_Character__GetStatWithBoosts)(CDivinityStats_Character * self, bool baseStats, bool excludeBoosts);
	typedef int32_t (CDivinityStats_Character__GetStatWithInit)(CDivinityStats_Character * self, bool baseStats, int32_t initialValue);
	typedef float (CDivinityStats_Character__GetStatHearing)(CDivinityStats_Character * self, CDivinityStats_Character * other, bool baseStats);
	typedef int32_t (CDivinityStats_Character__GetHitChance)(CDivinityStats_Character * attacker, CDivinityStats_Character * target);
	typedef bool (CDivinityStats_Character__GetTalent)(CDivinityStats_Character * self, TalentType talentId, char excludeBoosts);
	typedef int32_t (CDivinityStats_Character__GetAbility)(CDivinityStats_Character * self, AbilityType abilityId, char excludeBoosts, char noLoneWolfBonus);
	
	typedef void (* eoc__SkillPrototypeManager__Init)(void * self);

	template <class T>
	struct CHandleArray : public ProtectedGameObject<CHandleArray<T>>
	{
		__int64 VMT;
		T ** Start;
		T ** End;
		T ** BufAllocationTail;
		__int64 field_20;

		inline std::size_t Size() const
		{
			return End - Start;
		}
	};

	template <class T>
	struct CDivinityStatsList : public ProtectedGameObject<CDivinityStatsList<T>>
	{
		void * VMT;
		CHandleArray<T> Handles;
		struct CDivinityStats * Stats;

		T * Get(int32_t index) const
		{
			if (index >= 0 && index < Handles.Size()) {
				return Handles.Start[index];
			} else {
				return nullptr;
			}
		}
	};

	struct CDivinityStats : public ProtectedGameObject<CDivinityStats>
	{
		void * VMT;
		bool SomeFlag;
		uint8_t _Pad[7];
		CDivinityStatsList<CDivinityStats_Item> * ItemList;
		CDivinityStatsList<CDivinityStats_Character> * CharacterList;
		char SomeBuf0x50b[80];
		CDivinityStats_Item * DefaultWeaponStats;
		ObjectSet<FixedString> FSSetUnkn;
		uint64_t Unkn;
	};

	struct RPGEnumeration : public ProtectedGameObject<RPGEnumeration>
	{
		FixedString Name;
		Map<FixedString, int32_t> Values;
	};

	struct CRPGStats_Modifier : public ProtectedGameObject<CRPGStats_Modifier>
	{
		int32_t RPGEnumerationIndex;
		int32_t LevelMapIndex;
		int32_t UnknownZero;
		uint32_t _Padding;
		FixedString Name;
	};

	struct ModifierList
	{
		CNamedElementManager<CRPGStats_Modifier> Attributes;
		FixedString Name;

		CRPGStats_Modifier * GetAttributeInfo(FixedString const& name, int * attributeIndex) const;
	};

	struct CRPGStats_ExtraData : public ProtectedGameObject<CRPGStats_ExtraData>
	{
		Map<FixedString, float> Properties;
	};

	struct CDivinityStats_Condition
	{
		void * ScriptCheckBlock;
		FixedString Name;
	};

	struct CRPGStats_Conditions_Manager
	{
		Map<FixedString, CDivinityStats_Condition *> Conditions;
		uint8_t _Pad1[4];
		ObjectSet<STDString> Variables;
	};

	struct CRPGStats_ItemType_Manager : public CNamedElementManager<uint64_t>
	{
		uint64_t Unknown;
	};

	struct TextBuffer
	{
		wchar_t * Buf;
		uint64_t Capacity;
		uint64_t Length;
		uint64_t Unknown;

		void Replace(WStringView replacement);
	};

	namespace eoc
	{
		struct Text
		{
			struct Param
			{
				int32_t PlaceholderOffset;
				int32_t Unknown[2];
				int32_t PlaceholderSize;
			};

			void * VMT;
			TextBuffer * Buf;
			uint32_t Unknown[2];
			Param Params[8];

			void ReplaceParam(int index, WStringView replacement);
		};
	}

	struct SkillPrototype
	{
		typedef void (* FormatDescriptionParam)(SkillPrototype *skillPrototype, CDivinityStats_Character *tgtCharStats, 
			eoc::Text *eocText, int paramIndex, bool isFromItem, float xmm9_4_0, FixedString * paramText,
			ObjectSet<STDString> * stdStringSet);

		typedef void (* GetSkillDamage)(SkillPrototype * self, struct DamagePairList * damageList, 
			CRPGStats_ObjectInstance *attackerStats, bool isFromItem, bool stealthed, float * attackerPosition,
			float * targetPosition, DeathType * pDeathType, int level, bool noRandomization);

		// void * VMT;
		int RPGStatsObjectIndex;
		SkillType SkillTypeId;
		FixedString SkillId;
		uint16_t Ability;
		uint8_t _Pad[2];
		uint32_t Tier;
		uint32_t Requirement;
		uint32_t Level;
		uint32_t MagicCost;
		uint32_t MemoryCost;
		uint32_t ActionPoints;
		float Cooldown;
		float CooldownReduction;
		float ChargeDuration;
		STDWString DisplayName;
		FixedString Icon;
		uint8_t AiFlags;
		uint8_t _Pad2[7];
		SkillPrototype * RootSkillPrototype;
		ObjectSet<SkillPrototype *> SomePrototypes;

		CRPGStats_Object * GetStats() const;
	};

	struct StatusPrototype
	{
		typedef void (* FormatDescriptionParam)(StatusPrototype *prototype, CRPGStats_ObjectInstance* owner,
			CRPGStats_ObjectInstance* statusSource, float multiplier, eoc::Text * text, int paramIndex,
			FixedString * param, ObjectSet<STDString> * paramSet);

		void * VMT;
		int RPGStatsObjectIndex;
		int StatusId;
		FixedString StatusName;
		TranslatedString DisplayName;
		FixedString Icon;
		bool HasStats;
		uint8_t _Pad[7];
		PrimitiveSet<uint32_t> AbsorbSurfaceTypes; // eoc::ESurfaceType

		CRPGStats_Object * GetStats() const;
	};

	struct CSkillSet : public ProtectedGameObject<CSkillSet>
	{
		FixedString Name;
		ObjectSet<FixedString> Skills;
	};

	struct CSkillSetManager : public CNamedElementManager<CSkillSet>
	{
		CSkillSet * ParsedSkillSet;
	};

	struct CEquipmentGroup : public ProtectedGameObject<CEquipmentGroup>
	{
		FixedString Name;
		ObjectSet<FixedString> Equipment;
	};

	struct CEquipmentSet : public ProtectedGameObject<CEquipmentSet>
	{
		FixedString Name;
		CEquipmentGroup ** FirstGroup;
		CEquipmentGroup ** LastGroup;
		CEquipmentGroup ** GroupBufEnd;
	};

	struct CEquipmentSetManager : public CNamedElementManager<CEquipmentSet>
	{
		CEquipmentSet * ParsedEquipmentSet;
	};

	struct CRPGStats_DeltaModifier : public Noncopyable<CRPGStats_DeltaModifier>
	{
		ObjectSet<int> BoostIndices;
		ObjectSet<int> BoostCounts;
		FixedString ModifierType;
		int SlotType{ (int)ItemSlot::Sentinel };
		WeaponType WeaponType{ WeaponType::Sentinel };
		ArmorType ArmorType{ ArmorType::Sentinel };
		HandednessType Handedness{ HandednessType::Any };
		FixedString Name;
		FixedString BoostType;
		int MinLevel{ -1 };
		int MaxLevel{ -1 };
		int MinLevelBoosted{ -1 };
		int MaxLevelBoosted{ -1 };
		int Frequency{ 0 };
		int Frequency2{ 0 };
	};

	struct CRPGStats_DeltaModifier_List : public CNamedElementManager<CRPGStats_DeltaModifier>
	{};

	struct CRPGStatsManager : public ProtectedGameObject<CRPGStatsManager>
	{
		CNamedElementManager<RPGEnumeration> modifierValueList;
		CNamedElementManager<ModifierList> modifierList;
		CNamedElementManager<CRPGStats_Object> objects;
		CNamedElementManager<CRPGStats_LevelMap> LevelMaps;
		CNamedElementManager<CRPGStats_DeltaModifier_List> DeltaMods;
		CNamedElementManager<uint64_t> treasureSubtables;
		CNamedElementManager<uint64_t> treasureTables;
		CRPGStats_ItemType_Manager itemTypes;
		Map<FixedString, CRPGStats_Object_Property_List*> PropertyLists;
		uint8_t _Pad1[4];
		CRPGStats_Conditions_Manager ConditionsManager;
		STDWString WStr1;
		uint64_t Unkn1[5];
		CRPGStats_ExtraData * ExtraData;
		RefMap<FixedString, void *> RefMap1;
		RefMap<FixedString, void *> RefMap2;
		Map<FixedString, FixedString> FSMap1;
		uint8_t _Pad2[4];
		Map<FixedString, int> FSMapInt1;
		uint8_t _Pad3[4];
		Map<FixedString, int> FSMapInt2;
		uint8_t _Pad4[4];
		Map<FixedString, int> FSMapInt3;
		uint8_t _Pad5[4];
		RefMap<FixedString, void *> FSMap2;
		uint64_t Unkn2[7];
		ObjectSet<FixedString, GameMemoryAllocator, true> ModifierFSSet;
		ObjectSet<StatAttributeFlags> AttributeFlags;
		uint64_t unknown2[140];
		CEquipmentSetManager * EquipmentSetManager;
		CSkillSetManager * SkillSetManager;
		uint64_t Unkn3;
		void * ItemCombinationManager;
		uint64_t Unkn4;
		FixedString FS1;
		Map<FixedString, uint64_t> FSMapUInt64;
		uint8_t _Pad6[4];
		uint64_t Unkn5;
		PrimitiveSet<void *> DataBufferSet;
		uint64_t Unkn6;
		void * DivinityStats;
		void * CritSection;
		uint64_t Unkn7[5];

		CRPGStats_Modifier * GetModifierInfo(FixedString const& modifierListName, FixedString const& modifierName);
		ModifierList * GetTypeInfo(CRPGStats_Object * object);
		RPGEnumeration * GetAttributeInfo(CRPGStats_Object * object, FixedString const& attributeName, int & attributeIndex);
		std::optional<char const *> GetAttributeString(CRPGStats_Object * object, FixedString const& attributeName);
		std::optional<int> GetAttributeInt(CRPGStats_Object * object, FixedString const& attributeName);
		std::optional<int> GetAttributeIntScaled(CRPGStats_Object * object, FixedString const& attributeName, int level);
		bool SetAttributeString(CRPGStats_Object * object, FixedString const& attributeName, const char * value);
		bool SetAttributeInt(CRPGStats_Object * object, FixedString const& attributeName, int32_t value);
		bool ObjectExists(FixedString const& statsId, FixedString const& type);

		std::optional<int> EnumLabelToIndex(FixedString const& enumName, char const* enumLabel);
		FixedString EnumIndexToLabel(FixedString const& enumName, int index);
		int GetOrCreateFixedString(const char * value);
		std::optional<StatAttributeFlags> StringToAttributeFlags(const char * value);
		void* BuildScriptCheckBlock(STDString const& source);
		void* BuildScriptCheckBlockFromProperties(STDString const& source);
	};

	typedef void* (*ScriptCheckBlock__Build)(STDString const& str, ObjectSet<STDString> const& variables, int offset, int length);

	CRPGStats_Object * StatFindObject(char const * name);
	CRPGStats_Object * StatFindObject(int index);
#pragma pack(pop)

	template <class TTag>
	std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStat * getter,
		WrappableFunction<TTag, CDivinityStats_Character__GetStat> & wrapper,
		CDivinityStats_Character * character, bool original, bool baseStats)
	{
		if (getter == nullptr) {
			return {};
		}

		if (original || !wrapper.IsWrapped()) {
			return getter(character, baseStats);
		} else {
			return wrapper.CallWithHooks(character, baseStats);
		}
	}

	template <class TTag>
	std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStatNoBoosts * getter,
		WrappableFunction<TTag, CDivinityStats_Character__GetStatNoBoosts> & wrapper,
		CDivinityStats_Character * character, bool original, bool baseStats)
	{
		if (getter == nullptr) {
			return {};
		}

		if (original || !wrapper.IsWrapped()) {
			return getter(character);
		} else {
			return wrapper.CallWithHooks(character);
		}
	}

	template <class TTag>
	std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStatWithBoosts * getter,
		WrappableFunction<TTag, CDivinityStats_Character__GetStatWithBoosts> & wrapper,
		CDivinityStats_Character * character, bool original, bool baseStats)
	{
		if (getter == nullptr) {
			return {};
		}

		if (original || !wrapper.IsWrapped()) {
			return getter(character, baseStats, false);
		} else {
			return wrapper.CallWithHooks(character, baseStats, false);
		}
	}

	template <class TTag>
	std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStatWithInit * getter,
		WrappableFunction<TTag, CDivinityStats_Character__GetStatWithInit> & wrapper,
		CDivinityStats_Character * character, bool original, bool baseStats)
	{
		if (getter == nullptr) {
			return {};
		}

		if (original || !wrapper.IsWrapped()) {
			return getter(character, baseStats, 0);
		} else {
			return wrapper.CallWithHooks(character, baseStats, 0);
		}
	}

	template <class TTag>
	std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStatHearing * getter,
		WrappableFunction<TTag, CDivinityStats_Character__GetStatHearing> & wrapper,
		CDivinityStats_Character * character, bool original, bool baseStats)
	{
		if (getter == nullptr) {
			return {};
		}

		if (original || !wrapper.IsWrapped()) {
			return (int)(getter(character, nullptr, baseStats) * 100.0f);
		} else {
			return (int)(wrapper.CallWithHooks(character, nullptr, baseStats) * 100.0f);
		}
	}

	struct CharacterStatsGetters
	{

		union {
			void * Ptrs[26];
			struct {
#define DEFN_GETTER(type, name) CDivinityStats_Character__Get##type * Get##name;
#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

				CDivinityStats_Character__GetHitChance * GetHitChance;
				CDivinityStats_Character__GetTalent * GetTalent;
				CDivinityStats_Character__GetAbility * GetAbility;
			};
		};

#define DEFN_GETTER(type, name) \
	enum class name##Tag{}; \
	WrappableFunction<name##Tag, CDivinityStats_Character__Get##type> Wrapper##name;

#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

		enum class HitChanceTag {}; \
		WrappableFunction<HitChanceTag, CDivinityStats_Character__GetHitChance> WrapperHitChance;

		bool Wrapped{ false };

		void WrapAll();
		void UnwrapAll();

		std::optional<int32_t> GetStat(CDivinityStats_Character * character, FixedString const& name, 
			bool original, bool baseValues);
	};
}
