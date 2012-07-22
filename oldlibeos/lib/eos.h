/* libeos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _EOS_H_
#define _EOS_H_

#define __STDC_FORMAT_MACROS

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdarg>
#include <cctype>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ERR(x) (errcode = x.errcode)

#define NOTFOUND 0xFFFFFFFF
#define ERROR 0xFFFFFFFE

extern int errno;

// To guarantee fixed-width datatypes on all machines

#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif // HAVE_SYS_TYPES_H
#ifdef HAVE_STDINT_H
	#include <stdint.h>
#endif // HAVE_STDINT_H
#ifdef HAVE_INTTYPES_H
	#include <inttypes.h>
#endif // HAVE_INTTYPES_H

#if defined (HAVE_SYS_TYPES_H)
	typedef int8_t int8;
	typedef u_int8_t uint8;
	typedef int16_t int16;
	typedef u_int16_t uint16;
	typedef int32_t int32;
	typedef u_int32_t uint32;
	typedef int64_t int64;
	typedef u_int64_t uint64;
#elif defined (HAVE_STDINT_H)
	typedef int8_t int8;
	typedef uint8_t uint8;
	typedef int16_t int16;
	typedef uint16_t uint16;
	typedef int32_t int32;
	typedef uint32_t uint32;
	typedef int64_t int64;
	typedef uint64_t uint64;
#elif defined (HAVE_INTTYPES_H)
	typedef int8_t int8;
	typedef uint8_t uint8;
	typedef int_16_t int16;
	typedef uint16_t uint16;
	typedef int32_t int32;
	typedef uint32_t uint32;
	typedef int64_t int64;
	typedef uint64_t uint64;
#else
// Hope for the best (will most likely fail on 64bit machines resulting in
// an unusable/broken binary)
	typedef signed char int8;
	typedef unsigned char uint8;
	typedef signed short int16;
	typedef unsigned short uint16;
	typedef signed int int32;
	typedef unsigned int uint32;
	typedef signed long long int64;
	typedef unsigned long long uint64;
#endif

// I hope that always holds true... :/
typedef float float32;
typedef double float64;

#define S_FLOAT32 "%f"
#define S_FLOAT64 "%lf"
#ifdef PRId64
	#define S_INT64 "%" PRId64
#else
	// Hope for the best
	#define S_INT64 "%Ld"
#endif // PRId64
#ifdef PRIu64
	#define S_UINT64 "%" PRIu64
#else
	// Hope for the best
	#define S_UINT64 "%Lu"
#endif

// As found in the ResType fields in various Aurora files
// (Is this list complete?)
typedef enum
{
	NWN_FILE_UNDEFINED = -1,
	NWN_FILE_RES = 0,
	NWN_FILE_BMP = 1,
	NWN_FILE_MVE = 2,
	NWN_FILE_TGA = 3,
	NWN_FILE_WAV = 4,
	NWN_FILE_PLT = 6,
	NWN_FILE_INI = 7,
	NWN_FILE_BMU = 8,
	NWN_FILE_MPG = 9,
	NWN_FILE_TXT = 10,
	NWN_FILE_PLH = 2000,
	NWN_FILE_TEX = 2001,
	NWN_FILE_MDL = 2002,
	NWN_FILE_THG = 2003,
	NWN_FILE_FNT = 2005,
	NWN_FILE_LUA = 2007,
	NWN_FILE_SLT = 2008,
	NWN_FILE_NSS = 2009,
	NWN_FILE_NCS = 2010,
	NWN_FILE_MOD = 2011,
	NWN_FILE_ARE = 2012,
	NWN_FILE_SET = 2013,
	NWN_FILE_IFO = 2014,
	NWN_FILE_BIC = 2015,
	NWN_FILE_WOK = 2016,
	NWN_FILE_2DA = 2017,
	NWN_FILE_TLK = 2018,
	NWN_FILE_TXI = 2022,
	NWN_FILE_GIT = 2023,
	NWN_FILE_BTI = 2024,
	NWN_FILE_UTI = 2025,
	NWN_FILE_BTC = 2026,
	NWN_FILE_UTC = 2027,
	NWN_FILE_DLG = 2029,
	NWN_FILE_ITP = 2030,
	NWN_FILE_BTT = 2031,
	NWN_FILE_UTT = 2032,
	NWN_FILE_DDS = 2033,
	NWN_FILE_UTS = 2035,
	NWN_FILE_LTR = 2036,
	NWN_FILE_GFF = 2037,
	NWN_FILE_FAC = 2038,
	NWN_FILE_BTE = 2039,
	NWN_FILE_UTE = 2040,
	NWN_FILE_BTD = 2041,
	NWN_FILE_UTD = 2042,
	NWN_FILE_BTP = 2043,
	NWN_FILE_UTP = 2044,
	NWN_FILE_DFT = 2045,
	NWN_FILE_DTF = 2045,
	NWN_FILE_GIC = 2046,
	NWN_FILE_GUI = 2047,
	NWN_FILE_CSS = 2048,
	NWN_FILE_CCS = 2049,
	NWN_FILE_BTM = 2050,
	NWN_FILE_UTM = 2051,
	NWN_FILE_DWK = 2052,
	NWN_FILE_PWK = 2053,
	NWN_FILE_BTG = 2054,
	NWN_FILE_UTG = 2055,
	NWN_FILE_JRL = 2056,
	NWN_FILE_SAV = 2057,
	NWN_FILE_UTW = 2058,
	NWN_FILE_4PC = 2059,
	NWN_FILE_SSF = 2060,
	NWN_FILE_HAK = 2061,
	NWN_FILE_NWM = 2062,
	NWN_FILE_BIK = 2063,
	NWN_FILE_NDB = 2064,
	NWN_FILE_PTM = 2065,
	NWN_FILE_PTT = 2066,
	NWN_FILE_1DA = 9996,
	NWN_FILE_ERF = 9997,
	NWN_FILE_BIF = 9998,
	NWN_FILE_KEY = 9999,
	NWN_FILE_GVT = 20000,
	NWN_FILE_PT  = 20001,
	NWN_FILE_NFO = 20002,
} NWNFileType;

// Base types, i.e. MOD, HAK & SAV are ERFs with different 4 magic bytes
typedef enum
{
	NWN_BASE_UNDEFINED = -1,
	NWN_BASE_ERF = 0,
	NWN_BASE_2DA = 1,
	NWN_BASE_KEY = 2,
	NWN_BASE_BIF = 3,
	NWN_BASE_GFF = 4,
	NWN_BASE_SSF = 5,
	NWN_BASE_TLK = 6,
	NWN_BASE_OTHER = 7
} NWNBaseType;

// Languages; *M = male/neutral, *F = female
//
// Note: ENGLISHF isn't handled "correctly" by the game, it just overrules
//       existing ENGLISHM fields, the actual gender of the PC is ignored
typedef enum
{
	NWN_LANG_ENGLISHM = 0,
	NWN_LANG_ENGLISHF = 1,
	NWN_LANG_FRENCHM = 2,
	NWN_LANG_FRENCHF = 3,
	NWN_LANG_GERMANM = 4,
	NWN_LANG_GERMANF = 5,
	NWN_LANG_ITALIANM = 6,
	NWN_LANG_ITALIANF = 7,
	NWN_LANG_SPANISHM = 8,
	NWN_LANG_SPANISHF = 9,
	NWN_LANG_POLISHM = 10,
	NWN_LANG_POLISHF = 11,
	NWN_LANG_MAX = 12
// TODO: Support for asian languages (utf8?) [low]
//	NWN_LANG_KOREANM = 256,
//	NWN_LANG_KOREANF = 257,
//	NWN_LANG_CHINESETRADM = 258,
//	NWN_LANG_CHINESETRADF = 259,
//	NWN_LANG_CHINESESIMPM = 260,
//	NWN_LANG_CHINESESIMPF = 261,
//	NWN_LANG_JAPANESEM = 262,
//	NWN_LANG_JAPANESEF = 263
} NWNLang;

// Variable types as used in GFFs; types marked with "Complex" are written
// differently, the data value in the field is just a pointer then. Except
// for UINT64, INT64 and FLOAT64, special functions for reading these types
// out of a GFF exist.
typedef enum
{
	NWN_VAR_INT8 = 0,
	NWN_VAR_UINT8 = 1,
	NWN_VAR_UINT16 = 2,
	NWN_VAR_INT16 = 3,
	NWN_VAR_UINT32 = 4,
	NWN_VAR_INT32 = 5,
	NWN_VAR_UINT64 = 6,     // Complex
	NWN_VAR_INT64 = 7,      // Complex
	NWN_VAR_FLOAT32 = 8,
	NWN_VAR_FLOAT64 = 9,    // Complex
	NWN_VAR_EXOSTR = 10,    // Complex
	NWN_VAR_RESREF = 11,    // Complex
	NWN_VAR_EXOLOCSTR = 12, // Complex
	NWN_VAR_VOID = 13,      // Complex
	NWN_VAR_STRUCT = 14,    // Complex
	NWN_VAR_LIST = 15,      // Complex
	NWN_VAR_MAX = 16
} NWNVarType;

// As found in soundsets (SSF)
typedef enum
{
	NWN_SOUND_ATTACK = 0,
	NWN_SOUND_BATTLECRY1 = 1,
	NWN_SOUND_BATTLECRY2 = 2,
	NWN_SOUND_BATTLECRY3 = 3,
	NWN_SOUND_HEALME = 4,
	NWN_SOUND_HELP = 5,
	NWN_SOUND_ENEMIESSIGHTED = 6,
	NWN_SOUND_FLEE = 7,
	NWN_SOUND_TAUNT = 8,
	NWN_SOUND_GUARDME = 9,
	NWN_SOUND_HOLD = 10,
	NWN_SOUND_ATTACKGRUNT1 = 11,
	NWN_SOUND_ATTACKGRUNT2 = 12,
	NWN_SOUND_ATTACKGRUNT3 = 13,
	NWN_SOUND_PAINGRUNT1 = 14,
	NWN_SOUND_PAINGRUNT2 = 15,
	NWN_SOUND_PAINGRUNT3 = 16,
	NWN_SOUND_NEARDEATH = 17,
	NWN_SOUND_DEATH = 18,
	NWN_SOUND_POISONED = 19,
	NWN_SOUND_SPELLFAILED = 20,
	NWN_SOUND_WEAPONINEFFECTIVE = 21,
	NWN_SOUND_FOLLOWME = 22,
	NWN_SOUND_LOOKHERE = 23,
	NWN_SOUND_GROUPPARTY = 24,
	NWN_SOUND_MOVEOVER = 25,
	NWN_SOUND_PICKLOCK = 26,
	NWN_SOUND_SEARCH = 27,
	NWN_SOUND_GOSTEALTHY = 28,
	NWN_SOUND_CANDO = 29,
	NWN_SOUND_CANNOTDO = 30,
	NWN_SOUND_TASKCOMPLETE = 31,
	NWN_SOUND_ENCUMBERED = 32,
	NWN_SOUND_SELECTED = 33,
	NWN_SOUND_HELLO = 34,
	NWN_SOUND_YES = 35,
	NWN_SOUND_NO = 36,
	NWN_SOUND_STOP = 37,
	NWN_SOUND_REST = 38,
	NWN_SOUND_BORED = 39,
	NWN_SOUND_GOODBYE = 40,
	NWN_SOUND_THANKYOU = 41,
	NWN_SOUND_LAUGH = 42,
	NWN_SOUND_CUSS = 43,
	NWN_SOUND_CHEER = 44,
	NWN_SOUND_SOMETHINGTOSAY = 45,
	NWN_SOUND_GOODIDEA = 46,
	NWN_SOUND_BADIDEA = 47,
	NWN_SOUND_THREATEN = 48
} NWNSoundType;

typedef enum
{
	NWN_OBJECT_BLUEPRINT = 0,
	NWN_OBJECT_INSTANCE = 1,
	NWN_OBJECT_SAVE = 2
} NWNObjectState;

typedef enum
{
	NWN_ITEM_NORMAL = 0,
	NWN_ITEM_INVENTORY = 1,
	NWN_ITEM_BICINVENTORY = 2,
	NWN_ITEM_EINVENTORY = 3,
	NWN_ITEM_BICEINVENTORY = 4,
} NWNItemState;

typedef enum
{
	NWN_CREATURE_NORMAL = 0,
	NWN_CREATURE_BIC = 1
} NWNCreatureState;

typedef enum
{
	NWN_CREATURE_SUBDIV = 0,
	NWN_CREATURE_MONOLITH = 1
} NWNCreatureAppearance;

typedef enum
{
	NWN_MODEL_UNDEFINED = -1,
	NWN_MODEL_SIMPLE = 0,
	NWN_MODEL_LAYERED = 1,
	NWN_MODEL_COMPOSITE = 2,
	NWN_MODEL_ARMOR = 3
} NWNModelType;

typedef enum
{
	NWN_EQUIP_NONE = 0,
	NWN_EQUIP_HEAD = 1,
	NWN_EQUIP_CHEST = 2,
	NWN_EQUIP_BOOTS = 4,
	NWN_EQUIP_ARMS = 8,
	NWN_EQUIP_RIGHTHAND = 16,
	NWN_EQUIP_LEFTHAND = 32,
	NWN_EQUIP_CLOAK = 64,
	NWN_EQUIP_LEFTRING = 128,
	NWN_EQUIP_RIGHTRING = 256,
	NWN_EQUIP_NECK = 512,
	NWN_EQUIP_BELT = 1024,
	NWN_EQUIP_ARROWS = 2048,
	NWN_EQUIP_BULLETS = 4096,
	NWN_EQUIP_BOLTS = 8192
} NWNEquipped;

typedef enum
{
	NWN_STYPE_UNDEFINED = 0,
	NWN_STYPE_INT = 3,
	NWN_STYPE_FLOAT = 4,
	NWN_STYPE_STRING = 5,
	NWN_STYPE_OBJECT = 6,
	NWN_STYPE_EFFECT = 10,
	NWN_STYPE_EVENT = 11,
	NWN_STYPE_LOCATION = 12,
	NWN_STYPE_TALENT = 13,
	NWN_STYPE_ITEMPROPERTY = 14,
	NWN_STYPE_RESERVED1 = 15,
	NWN_STYPE_RESERVED2 = 16,
	NWN_STYPE_RESERVED3 = 17,
	NWN_STYPE_RESERVED4 = 18,
	NWN_STYPE_RESERVED5 = 19
} NWNStackElementType;

typedef enum
{
	NWN_ATYPE_UNDEFINED = 0,
	NWN_ATYPE_INT = 1,
	NWN_ATYPE_FLOAT = 2,
	NWN_ATYPE_OBJECT = 3,
	NWN_ATYPE_STRING = 4,
	NWN_ATYPE_SCRIPTSITUATION = 5
} NWNActionParameterType;

typedef enum
{
	NWN_EVENT_UNDEFINED = 0,
	NWN_EVENT_TIMED_EVENT = 1,
	NWN_EVENT_ENTERED_TRIGGER = 2,
	NWN_EVENT_LEFT_TRIGGER = 3,
	NWN_EVENT_REMOVE_FROM_AREA = 4,
	NWN_EVENT_APPLY_EFFECT = 5,
	NWN_EVENT_CLOSE_OBJECT = 6,
	NWN_EVENT_OPEN_OBJECT = 7,
	NWN_EVENT_SPELL_IMPACT = 8,
	NWN_EVENT_PLAY_ANIMATION = 9,
	NWN_EVENT_SIGNAL_EVENT = 10,
	NWN_EVENT_DESTROY_OBJECT = 11,
	NWN_EVENT_UNLOCK_OBJECT = 12,
	NWN_EVENT_LOCK_OBJECT = 13,
	NWN_EVENT_REMOVE_EFFECT = 14,
	NWN_EVENT_ON_MELEE_ATTACKED = 15,
	NWN_EVENT_DECREMENT_STACKSIZE = 16,
	NWN_EVENT_SPAWN_BODY_BAG = 17,
	NWN_EVENT_FORCED_ACTION = 18,
	NWN_EVENT_ITEM_ON_HIT_SPELL_IMPACT = 19,
	NWN_EVENT_BROADCAST_AOO = 20,
	NWN_EVENT_BROADCAST_SAFE_PROJECTILE = 21,
	NWN_EVENT_FEEDBACK_MESSAGE = 22,
	NWN_EVENT_ABILITY_EFFECT_APPLIED = 23,
	NWN_EVENT_SUMMON_CREATURE = 24,
	NWN_EVENT_ACQUIRE_ITEM = 25
} NWNEventType;

typedef enum
{
	NWN_QB_EMPTY = 0,
	NWN_QB_ITEM = 1,
	NWN_QB_SPELL = 2,
	NWN_QB_SKILL = 3,
	NWN_QB_FEAT = 4,
	NWN_QB_SCRIPT = 5,
	NWN_QB_DIALOG = 6,
	NWN_QB_ATTACK = 7,
	NWN_QB_EMOTE = 8,
	NWN_QB_CASTSPELL_ITEMPROPERTY = 9,
	NWN_QB_MODE_TOGGLE = 10,
	NWN_QB_MACRO = 18,
	NWN_QB_POSSESS_FAMILIAR = 38,
	NWN_QB_ASSOCIATE_COMMAND = 39,
	NWN_QB_EXAMINE = 40,
	NWN_QB_BARTER = 41,
	NWN_QB_QUICKCHAT = 42,
	NWN_QB_CANCEL_POLYMORPH = 43,
	NWN_QB_SPELLLIKE_ABILITY = 44
} NWNQBObjectType;

typedef enum
{
	NWN_PALETTE_STANDARD = 0,
	NWN_PALETTE_SKELETON = 1
} NWNPaletteState;

#define INVALID_OBJECT_ID 0x7F000000

// Flags for the TLK file, indices into the flags bitfields
// (i.e.: if(tlk.flags[0][TLK_TEXT_PRESENT]) foobar();)
#define TLK_BF_TEXT_PRESENT 0
#define TLK_BF_SND_PRESENT 1
#define TLK_BF_SNDLENGTH_PRESENT 2
// Same flags as OR-able values
#define TLK_TEXT_PRESENT 1
#define TLK_SND_PRESENT 2
#define TLK_SNDLENGTH_PRESENT 4

// Flags for areas, indices into the flags bitfield
#define AREA_BF_INTERIOR 0
#define AREA_BF_UNDERGROUND 1
#define AREA_BF_NATURAL 2
// Same flags as OR-able values
#define AREA_INTERIOR 1
#define AREA_UNDERGROUND 2
#define AREA_NATURAL 4

// Area windpower
#define AREA_WIND_NONE 0
#define AREA_WIND_WEAK 1
#define AREA_WIND_STRONG 2
// Area weather
#define AREA_WEATHER_CLEAR 0
#define AREA_WEATHER_RAIN 1
#define AREA_WEATHER_SNOW 2

// Animationstates of door
#define DOOR_ANIM_CLOSED 0
#define DOOR_ANIM_OPENED1 1
#define DOOR_ANIM_OPENED2 2

// Animationstates of placeables
#define PLACEABLE_ANIM_DEFAULT 0
#define PLACEABLE_ANIM_OPEN 1
#define PLACEABLE_ANIM_CLOSED 2
#define PLACEABLE_ANIM_DESTROYED 3
#define PLACEABLE_ANIM_ACTIVATED 4
#define PLACEABLE_ANIM_DEACTIVATED 5
// Animationstates of saved placeables
#define PLACEABLE_SANIM_DEFAULT 0
#define PLACEABLE_SANIM_DESTROYED 72
#define PLACEABLE_SANIM_ACTIVATED 73
#define PLACEABLE_SANIM_DEACTIVATED 74
#define PLACEABLE_SANIM_OPENED 75
#define PLACEABLE_SANIM_CLOSED 76

// Variable types
#define VAR_TYPE_UNDEFINED -1
#define VAR_TYPE_INT 0
#define VAR_TYPE_FLOAT 1
#define VAR_TYPE_STRING 2
#define VAR_TYPE_OBJECT 3
#define VAR_TYPE_LOCATION 4

// Triggers
#define TRIGGER_LINK_NOTHING 0
#define TRIGGER_LINK_DOOR 1
#define TRIGGER_LINK_WAYPOINT 2
#define TRIGGER_TYPE_GENERIC 0
#define TRIGGER_TYPE_TRANSITION 1
#define TRIGGER_TYPE_TRAP 2

// Flags for spells, indices into the flags bitfield
#define SPELL_BF_READY 0
#define SPELL_BF_SPONTANEOUSLY 1
#define SPELL_BF_UNLIMITED 2
// Same flags as OR-able values
#define SPELL_READY 1
#define SPELL_SPONTANEOUSLY 2
#define SPELL_UNLIMITED 4

// Spells metamagic (NOT OR-able!)
#define SPELL_META_NONE 0
#define SPELL_META_EMPOWER 1
#define SPELL_META_EXTEND 2
#define SPELL_META_MAXIMIZE 4
#define SPELL_META_QUICKEN 8
#define SPELL_META_SILENT 16
#define SPELL_META_STILL 32

// QBModeToggle
#define QBMODETOGGLE_DETECT 0
#define QBMODETOGGLE_STEALTH 1

// Phenotype
#define CREATURE_PHENOTYPE_NORMAL 0
#define CREATURE_PHENOTYPE_FAT 1

// Encounter spawn options
#define ENCOUNTER_SPAWN_CONTINUOUS 0
#define ENCOUNTER_SPAWN_SINGLE 1

// Sounds
#define SOUND_TIMES_SPECIFIC 0
#define SOUND_TIMES_DAY 1
#define SOUND_TIMES_NIGHT 2
#define SOUND_TIMES_ALWAYS 3

// Stores
#define STORE_ITEMS_ARMOR 0
#define STORE_ITEMS_MISCELLANEOUS 1
#define STORE_ITEMS_POTIONS 2
#define STORE_ITEMS_RINGS 3
#define STORE_ITEMS_WEAPONS 4
#define STORE_ITEMS_MAX 5

// Journal
#define JOURNAL_PRIO_HIGHEST 0
#define JOURNAL_PRIO_HIGH 1
#define JOURNAL_PRIO_MEDIUM 2
#define JOURNAL_PRIO_LOW 3
#define JOURNAL_PRIO_LOWEST 4

// Palette node types
#define PALETTE_NODE_IF_NOT_EMPTY 0
#define PALETTE_NODE_NEVER 1
#define PALETTE_NODE_CUSTOM 2

// Localized strings
typedef struct
{
	uint32 stringref;
	std::string str[NWN_LANG_MAX];
} ExoLocString;

typedef struct
{
	uint32 area;
	float32 x, y, z, dirx, diry, dirz;
} Location;

typedef class MemSeg
{
	public:
		uint8 *mem;
		uint32 size;

		MemSeg();
		MemSeg(const MemSeg &src);
		~MemSeg();
		MemSeg &operator=(const MemSeg &src);
		int alloc(int size);
		void deAlloc(void);
} MemSeg;

typedef class Variable
{
	public:
		std::string name;
		uint32 type;

		Variable();
		Variable(const Variable &other);
		~Variable();
		Variable &operator=(const Variable &src);
		void set(std::string name, int32 value);
		void set(std::string name, float32 value);
		void set(std::string name, std::string value);
		void set(std::string name, uint32 value);
		void set(std::string name, Location value);
		void get(int32 &value);
		void get(float32 &value);
		void get(std::string &value);
		void get(uint32 &value);
		void get(Location &value);

	protected:
		union
		{
			int32 tint;
			float32 tfloat;
			char *tstr;
			uint32 tobj;
			Location tloc;
		} value;

	friend class VarTable;
} variable;

typedef class BitField
{
	public:
		BitField();
		BitField(const uint16 &x);
		void set(uint8 x);
		void unSet(uint8 x);
		void toggle(uint8 x);
		void clear(void);
		bool isMax(uint8 x);
		bool operator[](uint8 x);
		uint16 operator=(uint16 x);
		operator uint16();

	private:
		uint16 field;
} bitfield;

class ErrorHandler
{
	public:
		int errcode;
		std::string aderrstr;

		ErrorHandler();
		std::string getStrError(int error=-1);

	protected:
		void lowerStr(std::string &str);
};

// Base class for portable binary reading and writing
// (endianness is handled automatically)
class BinReadWrite: public ErrorHandler
{
	public:
		std::string filename;

		BinReadWrite();
		virtual ~BinReadWrite();
		BinReadWrite &operator=(const BinReadWrite &src);
		BinReadWrite(const BinReadWrite &src);
		bool reading(void);
		bool writing(void);
		uint32 getMemSize(void);
		uint8 *getMem(void);
		uint32 getFileSize(void);
		uint32 swap32(uint32 x);
		uint64 swap64(uint64 x);

	protected:
		int openRead(uint8 *memseg, uint32 size); // Read from memory
		int openRead(std::string fname); // Read from file
		int openWrite(void); // Write to memory
		int openWrite(std::string fname); // Write to file
		void binClose(void);
		int seek(long offset, std::_Ios_Seekdir whence = std::ios::beg);
		uint32 tell(void);

		// Reading functions
		uint8 *readFileIntoMem(std::string fname, uint32 &size);
		int rawRead(char *buf, uint32 n);
		int rawRead(std::string &str, uint32 n);
		int rawRead(std::ofstream *ofile, uint32 n);
		int rawReadLine(std::string &str);
		template<class dt> int binRead(dt &var)
		{
			if((opmode != 1) && (opmode != 2)) return errcode = 6;

			if(opmode == 1)
			{
				binfile->read((char*) &var, sizeof(dt));
				if(fail()) return errcode = 3;
			}
			else
			{
				memcpy((char*) &var, mem, sizeof(dt));
				mem += sizeof(dt);
			}

#ifdef WORDS_BIGENDIAN
			if(sizeof(dt) == 4) var = swap32(var);
			else if(sizeof(dt) == 8) var = swap64(var);
#endif
			return errcode = 0;
		}
		template<class dt> int binRead(dt *var, int n)
		{
			if((opmode != 1) && (opmode != 2)) return errcode = 6;

			for(int i=0;i<n;i++)
				if(errcode = binRead(var[i])) return errcode;
			return errcode = 0;
		}
		template<class dt> int binRead(int n, dt *var, ...)
		{
			if((opmode != 1) && (opmode != 2)) return errcode = 6;

			va_list va;
			dt *nvar;

			if((errcode = binRead(*var))) return errcode;
			va_start(va, var);
			for(int i=1;i<n;i++)
			{
				nvar = va_arg(va, dt *);
				if((errcode = binRead(*nvar))) return errcode;
			}
			va_end(va);
			return errcode = 0;
		}

		// Writing functions
		int rawWrite(const char *buf, uint32 n);
		int rawWrite(std::string &str);
		template<class dt> int binWrite(dt var)
		{
			if((opmode != 3) && (opmode != 4)) return errcode = 7;

#ifdef WORDS_BIGENDIAN
			if(sizeof(dt) == 4) var = swap32(var);
			else if(sizeof(dt) == 8) var = swap64(var);
#endif

			if(opmode == 3)
			{
				binfile->write((char*) &var, sizeof(dt));
				if(fail()) return errcode = 3;
			}
			else
			{
				int nfree = memsize-(mem-mems);
				if(nfree < (int)sizeof(dt)) reserve(sizeof(dt)-nfree);
				memcpy(mem, (char*) &var, sizeof(dt));
				mem += sizeof(dt);
			}
			return errcode = 0;
		}
		template<class dt> int binWrite(dt *var, int n)
		{
			if((opmode != 3) && (opmode != 4)) return errcode = 7;

			for(int i=0;i<n;i++)
				if((errcode = binWrite(var[i]))) return errcode;
			return errcode = 0;
		}
		template<class dt> int binWrite(int n, dt var, ...)
		{
			if((opmode != 3) && (opmode != 4)) return errcode = 7;

			va_list va;
			dt nvar;

			if((errcode = binWrite(var))) return errcode;
			va_start(va, var);
			for(int i=1;i<n;i++)
			{
				nvar = va_arg(va, dt);
				if((errcode = binWrite(nvar))) return errcode;
			}
			va_end(va);
			return errcode = 0;
		}

	private:
		std::fstream *binfile;
		uint8 *mems, *mem;
		uint32 memsize;
		bool tofree;
		char opmode; // 0 = nothing opened
		             // 1 = read from file
		             // 2 = read from memory
		             // 3 = write to file
		             // 4 = write to memory

		bool fail(std::ios *stream=NULL);
		int reserve(uint32 size); // Reserve size more bytes (only for mode 4)
};

class NWNFile: public BinReadWrite
{
	public:
		NWNFileType type;
		float64 version;

		NWNFile();
		virtual bool typeIsCorrect(void); // Is the type corrent for this class?
		bool versionIsCorrect(void); // Are the version requirements met?
		int open(std::string fname);
		int open(uint8 *memseg, uint32 size);
		int open(MemSeg &memseg);
		void close(void);
		virtual int init(int n=-1); // General init, after which the class is
		                            // guaranteed to be completely initialized
		virtual int readHeader(void);
		virtual void deInit(void); // Resets the class completely (only neccesary
		                           // should one instance be used again)

		const char *getFileMagicByResType(int restype);
		const char *getFileMagicByExt(std::string ext);
		const char *getFileExtByResType(int restype);
		const char *getFileExtByMagic(std::string magic);
		NWNFileType getFileResTypeByMagic(std::string magic);
		NWNFileType getFileResTypeByExt(std::string ext);
		NWNBaseType getBaseType(NWNFileType type);

	protected:
		float64 versionwanted;
		uint32 initstages;
		bitfield inited;

		int checkFormat(void);
		NWNFileType getFileType(void);
		float64 getFileVersion(void);
		int readExoString(std::string &str);
		int readResRef(std::string &str);

	private:
		static const char *file_magic[];
		static const char *file_ext[];
		static const int file_restype[];
};

class ERFFile: public NWNFile
{
	public:

		// Number of languages in the description list, length of description
		uint32 langcount, locstrsize;
		// Number of entries, offset to description list, offset to key list
		uint32 entrycount, offlocstr, offkeylist;
		// Offset to ressources list, build year and day
		uint32 offreslist, buildyear, buildday;
		// The description
		ExoLocString desc;
		// Key entries
		std::vector<std::string> resrefs;
		std::vector<NWNFileType> restypes;
		std::vector<uint32> resids, offsets, sizes;

		ERFFile();
		bool typeIsCorrect(void);
		int init(int n=-1); // Does readHeader(), readDescription() & readKeyList()
		int readHeader(void);
		int readDescription(void);
		int readKeyList(void);
		void deInit(void);

		uint32 getEntryNum(std::string resref, NWNFileType restype);
		uint8 *dumpEntryIntoMem(uint32 entry);
		int dumpEntryIntoMem(uint32 entry, MemSeg &memseg);
		int dumpEntryIntoFile(uint32 entry, std::string fname);

		int beginWrite(std::string fname, uint32 n=0, ExoLocString *descr=NULL);
		int beginWrite(uint32 n=0, ExoLocString *descr=NULL);
		int writeData(std::string resref, NWNFileType restype, uint8 *memseg,
				uint32 size, bool copy);
		int writeData(std::string resref, NWNFileType restype, std::string fname);
		int endwrite(uint8 **mem=NULL, uint32 *size=NULL);

	protected:
		std::vector<uint8 *> datas;
		std::vector<bool> datac;
		bool pronto;
		uint32 prontowritten;

		int writePronto(uint32 n, ExoLocString &descr);
		int finalWriteCalc(void);
		int finalWriteHeader(void);
		int finalWriteDescription(void);
		int finalWriteKeyList(void);
		int finalWriteResList(void);
		int finalWriteEntries(void);
};

class TwoDAFile: public NWNFile
{
	public:
		std::string defstr; // Default std::string
		std::vector<std::string> names;

		TwoDAFile();
		bool typeIsCorrect(void);
		int init(int n=-1); // Does readHeader(), readArray() & closes the file
		int readHeader(void);
		int readArray(void); // Reads the whole file into the array
		void deInit(void);

		uint32 getNumRows(void);
		uint32 getNumCols(void);
		uint32 getColByName(std::string name);
		template<class dt> int getElement(uint32 row, uint32 col, dt &elem)
		{
			if((col >= names.size()) || (row >= array.size()) || (col == NOTFOUND))
			{
				strToVal(defstr, elem);
				return 1;
			}
			if(array[row][col] == "****")
			{
				std::string empty="";
				strToVal(empty, elem);
				return 2;
			}
			strToVal(array[row][col], elem);
			return 0;
		}
		template<class dt> int setElement(uint32 row, uint32 col, dt &elem)
		{
			if((col >= names.size()) || (row >= array.size())) return errcode = 13;
			if(col == NOTFOUND) return errcode = 26;
			array[row][col].clear();
			valToStr(elem, array[row][col]);
			return errcode = 0;
		}
		int addRow(void);
		int addColumn(std::string name);
		// Doesn't really delete the row (that would break every script and resource
		// referring this 2da), the whole row will be set to "****"s instead.
		int delRow(uint32 row);
		// See above, the whole column will be set to "****"s.
		int delColumn(uint32 col);

		int write(std::string fname);
		int write(uint8 **mem, uint32 *size);

	protected:
		long tablestart;
		std::vector< std::vector<std::string> > array;

		int getElementStr(std::string &str, std::string &elem, uint32 start,
				uint32 *found=NULL);
		int getElements(std::string &str, std::vector<std::string> &list);
		void strToVal(std::string &str, int32 &val);
		void strToVal(std::string &str, float32 &val);
		void strToVal(std::string &str, std::string &val);
		void valToStr(int32 val, std::string &str);
		void valToStr(float32 val, std::string &str);
		void valToStr(std::string &val, std::string &str);

		bool hasSpace(std::string &val);
		inline uint32 max(uint32 a, uint32 b);
		int colLengths(std::vector<uint32> &lengths);
		int finalWrite(void);
};

class KEYFile: public NWNFile
{
	public:
		// Number of BIFs, number of resources
		uint32 bifcount, keycount;
		// Offset to file and key list, build year and day
		uint32 offbiflist, offkeylist, buildyear, buildday;
		std::vector<uint32> fsizes; // Files sizes
		std::vector<uint16> drives; // Drives (bit 0 = installdir, bit 1 = cd?)
		std::vector<std::string> fnames; // File names

		KEYFile();
		bool typeIsCorrect(void);
		// Does readHeader(), readBifList(), readKeyList() & closes the file
		int init(int n=-1);
		int readHeader(void);
		int readBifList(void);
		int readKeyList(void);
		void deInit(void);

		// Get just one entry of the file list, instead of reading the whole thing
		int getListEntry(uint32 n, std::string &fname, uint16 &drive, uint32 &size);
		// Get one resource without having read the whole keylist
		// (fails with an error if the whole keylist was already read)
		int getKeyEntry(uint32 n, std::string &resref, NWNFileType &restype,
				uint32 &resid);
		// Get one resource associated with a specific bif
		// (reads the whole keylist if necessary, but doesn't close the file)
		int getKeyEntry(uint32 bif, uint32 n, std::string &resref, NWNFileType &restype,
				uint32 &index);
		int getKeyEntry(std::string resref, NWNFileType restype, uint32 &bif, uint32 &n);
		uint32 getNumResources(uint32 bif);
		// Variable ResID = (idxfile << 20) + idxbif)
		// Fixed ResID = (idxfile << 20) + (idxbif << 14)
		// idxfile: index into this KEY's file table (a BIF)
		// idxbif: index into that BIF's ressource table
		uint32 getIdxFile(uint32 resid);
		uint32 getIdxBif(uint32 resid); // Variable Ressources

		// Add BIFs and resources
		int addBif(std::string fname, uint16 drive, uint32 fsize);
		int addResource(uint32 bif, std::string &resref, NWNFileType restype,
				uint32 index);
		// Delete BIFs and resources
		int delBif(uint32 n, uint32 count=1);
		int delResource(uint32 bif, uint32 n, uint32 count=1);

		int write(std::string fname);
		int write(uint8 **mem, uint32 *size);

		// TODO: Monitor whether fixed resources will be implemented someday [low]

	protected:
		typedef struct
		{
			std::string resref;
			NWNFileType restype;
			uint32 index; // index into the BIF
		} Resource;

		std::vector< std::vector<Resource> > resources; // resources[BIF][idx]
		uint32 offfnamelist;

		int finalWrite(void);
		int finalWriteCalc(void);
		int finalWriteHeader(void);
		int finalWriteBifList(void);
		int finalWriteFNameList(void);
		int finalWriteKeyList(void);
};

class BIFFile: public NWNFile
{
	public:
		// Number of variable and fixed ressources
		uint32 vrescount, frescount;
		// Offset to variable ressourse list
		uint32 offvreslist;
		// ResIDs, offsets, file sizes
		std::vector<uint32> resids, offsets, fsizes;
		// ResTypes
		std::vector<NWNFileType> restypes;

		BIFFile();
		bool typeIsCorrect(void); // Is the type corrent for this class?
		int init(int n=-1); // Does readHeader() & readResList()
		int readHeader(void);
		int readResList(void);
		void deInit(void);
		// It probably is faster to read the reslist continuously, but takes up
		// more ram, therefore getResListEntry() exists
		// (if only one or so entry is needed)
		int getResListEntry(int n, uint32 &resid, uint32 &offset, uint32 &fsize, NWNFileType &restype);
		// ResID = (idx << 20) + x)
		// If the BIF came with the official CDs, x = idx
		int getIdx(uint32 resid);
		bool withCd(uint32 resid);
		uint8 *dumpEntryIntoMem(uint32 offset, uint32 size);
		uint8 *dumpEntryIntoMem(uint32 entry);
		int dumpEntryIntoMem(uint32 entry, MemSeg &memseg);
		int dumpEntryIntoFile(uint32 offset, uint32 size, std::string fname);
		int dumpEntryIntoFile(uint32 entry, std::string fname);

		int beginWrite(std::string fname, uint32 n=0);
		int beginWrite(uint32 n=0);
		int writeData(NWNFileType restype, bool withCd, bool var,
				uint8 *memseg, uint32 size, bool copy);
		int writeData(NWNFileType restype, bool withCd, bool var, std::string fname);
		int endwrite(uint8 **mem=NULL, uint32 *size=NULL);

	protected:
		std::vector<uint8 *> datas;
		std::vector<bool> datac, variable;
		bool pronto;
		uint32 prontowritten;

		int writePronto(uint32 n);
		int finalWriteCalc(void);
		int finalWriteHeader(void);
		int finalWriteVResList(void);
		int finalWriteVRes(void);
};

class GFFFile: public NWNFile
{
	public:

		typedef struct
		{
			uint32 id, fieldcount;
			std::vector<std::string *> labels;
			std::vector<NWNVarType> types;
			std::vector<uint64> values;
		} Struct;

		// Number of elements in struct, field and label arrays
		uint32 structcount, fieldcount, labelcount;
		// Number of bytes in field data block, field indices and list indices arrays
		uint32 fielddatasize, fieldidxsize, listidxsize;
		// Offsets to struct, field and label array
		uint32 offstruct, offfield, offlabel;
		// Offsets to field data block, field indices and list indices array
		uint32 offfielddata, offfieldidx, offlistidx;
		// Labels
		std::vector<std::string> labels;
		// The top level struct
		Struct topstruct;

		GFFFile();
		bool typeIsCorrect(void); // Is the type corrent for this class?
		int init(int n=-1); // Does readHeader(), readLabels() & getStruct()
		int readHeader(void);
		int readLabels(void);
		void deInit(void);
		int getStruct(uint32 n, Struct &sct);
		int getField(uint32 n, Struct &sct);
		int getFloat(uint32 n, Struct &sct, float32 &flt);
		int getFloat(std::string label, Struct &sct, float32 &flt);
		int getFloat(uint32 n, Struct &sct, float64 &flt);
		int getFloat(std::string label, Struct &sct, float64 &flt);
		int getResRef(uint32 n, Struct &sct, std::string &str);
		int getResRef(std::string label, Struct &sct, std::string &str);
		int getExoString(uint32 n, Struct &sct, std::string &str);
		int getExoString(std::string label, Struct &sct, std::string &str);
		int getExoLocString(uint32 n, Struct &sct, ExoLocString &lstr);
		int getExoLocString(std::string label, Struct &sct, ExoLocString &lstr);
		uint8 *getRaw(uint32 n, Struct &sct, uint32 &size);
		uint8 *getRaw(std::string label, Struct &sct, uint32 &size);
		int getRaw(uint32 n, Struct &sct, MemSeg &memseg);
		int getRaw(std::string label, Struct &sct, MemSeg &memseg);
		int getList(uint32 n, Struct &sct, std::vector<uint32> &list);
		int getList(std::string label, Struct &sct, std::vector<uint32> &list);
		int getList(uint32 n, Struct &sct, std::vector<Struct> &structs);
		int getList(std::string label, Struct &sct, std::vector<Struct> &structs);
		uint32 getIdxByLabel(std::string label, Struct &sct);
		int getIdxByLabel(std::string *labels, uint32 count, GFFFile::Struct &top,
			std::vector<uint32> &n, std::string errstr);

		uint32 addLabel(std::string lbl);
		int beginWrite(std::string fname);
		int beginWrite(void);
		int writeStruct(uint32 &n, std::string lbl, uint32 parent);
		int writeLabel(uint32 sct, std::string lbl);
		int writeSetLabel(uint32 sct, std::string lbl);
		int writeSetId(uint32 sct, uint32 id);
		int writeVar(uint32 sct, NWNVarType type, std::string lbl, uint64 value);
		int writeFloat(uint32 sct, std::string lbl, float32 value);
		int writeFloat(uint32 sct, std::string lbl, float64 value);
		int writeResRef(uint32 sct, std::string lbl, std::string value);
		int writeExoString(uint32 sct, std::string lbl, std::string value);
		int writeExoLocString(uint32 sct, std::string lbl, ExoLocString &value);
		int writeList(uint32 &n, uint32 sct, std::string lbl);
		int writeListStruct(uint32 &n, uint32 list);
		int writeData(uint32 sct, std::string lbl, uint8 *memseg, uint32 size,
				bool copy);
		int writeData(uint32 sct, std::string lbl, MemSeg memseg, bool copy);
		int writeData(uint32 sct, std::string lbl, std::string fname);
		int endwrite(uint8 **mem=NULL, uint32 *size=NULL);

	protected:
		std::vector<Struct> structs;
		std::vector<std::string> resrefs, exostrs;
		std::vector<ExoLocString> exolocstrs;
		std::vector< std::vector<uint32> > lists;
		std::vector<uint32> listoffs, dataoffs, datasizes;
		std::vector<uint8 *> datas;
		std::vector<bool> datac;
		uint32 datasize;

		int prepareGet(uint32 n, Struct &sct, NWNVarType reqtype, int32 off,
				bool dseek=true);
		int finalWriteHeader(void);
		int finalWriteStructs(void);
		int finalWriteField(uint32 sct, uint32 f, uint32 &ofield,
				uint32 &ofieldd);
		int finalWriteResRef(std::string &resref, uint32 &ofieldd);
		int finalWriteExoStr(std::string &exostr, uint32 &ofieldd);
		int finalWriteExoLocStr(ExoLocString &exolocstr, uint32 &ofieldd);
		int finalWriteLists(void);
		int finalWriteLabels(void);
		int finalWriteData(void);
};

// Soundset
class SSFFile: public NWNFile
{
	public:
		// Number of entries, offset to entry table
		uint32 entrycount, offtable;
		// The StringRefs and ResRefs
		// Each index into these vectors has a meaning, see NWNSoundType
		std::vector<std::string> resrefs;
		std::vector<uint32> stringrefs;

		SSFFile();
		bool typeIsCorrect(void);
		// Does readHeader(), readEntries(), readData() & closes the file
		int init(int n=-1);
		int readHeader(void);
		int readEntries(void);
		int readData(void);
		void deInit(void);

		int add(std::string resref, uint32 stringref);
		// Deletes the last entr(y/ies), deleting from the middle would mess up the
		// soundset. Set it to "", 0xFFFFFFFF instead.
		int del(uint32 count=1);
		int set(uint32 n, std::string resref, uint32 stringref);

		int write(std::string fname);
		int write(uint8 **mem, uint32 *size);

	protected:
		std::vector<uint32> offsets; // offsets to the entries' data

		int finalWrite(void);
		int finalWriteCalc(void);
		int finalWriteHeader(void);
		int finalWriteEntries(void);
		int finalWriteData(void);
};

class TLKFile: public NWNFile
{
	public:
		uint32 strcount, offstrings;
		NWNLang language;
		std::vector<bitfield> flags;
		std::vector<std::string> sndresrefs, strings;
		std::vector<float32> sndlengths;

		TLKFile();
		bool typeIsCorrect(void);
		// Does readHeader(), readTable(), readStrings() and closes the file
		int init(int n=-1);
		int readHeader(void);
		int readTable(void);
		int readStrings(void);
		// Gets a std::string, reads it from the file if necessary
		int getString(uint32 n, std::string &str);

		int add(uint16 flag, std::string sndresref, float32 sndlen, std::string str);
		int set(uint32 n, uint16 flag, std::string sndresref, float32 sndlen,
				std::string str);
		// Deletes the last std::string(s). Since strings in talktables are referred to
		// by indices, deleting from the middle would mess the tlk, set it to
		// 0, "", 0, "" instead...
		int del(uint32 count=1);
		void deInit(void);

		int write(std::string fname);
		int write(uint8 **mem, uint32 *size);

	protected:
		std::vector<uint32> offsets, strlengths;
		int finalWrite(void);
		int finalWriteCalc(void);
		int finalWriteHeader(void);
		int finalWriteTable(void);
		int finalWriteStrings(void);
};

typedef class VarTable: public ErrorHandler
{
	public:
		void add(std::string name, int32 value);
		void add(std::string name, float32 value);
		void add(std::string name, std::string value);
		void add(std::string name, uint32 value);
		void add(std::string name, Location value);
		bool set(std::string name, int32 value);
		bool set(std::string name, float32 value);
		bool set(std::string name, std::string value);
		bool set(std::string name, uint32 value);
		bool set(std::string name, Location value);
		bool get(std::string name, int32 &value);
		bool get(std::string name, float32 &value);
		bool get(std::string name, std::string &value);
		bool get(std::string name, uint32 &value);
		bool get(std::string name, Location &value);
		void del(uint32 n, uint32 count=1);
		void pop(void);
		void clear(void);
		bool empty(void);
		uint32 size(void);
		std::string getName(uint32 n);

		int load(GFFFile &gff, uint32 vln, GFFFile::Struct &top);
		int save(GFFFile &gff, uint32 vln);

	protected:
		std::vector<variable> variables;

		int getLocation(Location &loc, GFFFile &gff, GFFFile::Struct &top);
		int writeLocation(Location &loc, GFFFile &gff, uint32 sct);
} vartable;

class ScriptEvent: public ErrorHandler
{
	public:
		uint16 type;
		std::vector<int32> ints;
		std::vector<float32> floats;
		std::vector<std::string> strings;
		std::vector<uint32> objects;

		ScriptEvent();
		void registerLabels(std::string stype, std::string sint, std::string sfloat, std::string sstr,
			std::string sobj, std::string sintv, std::string sfloatv, std::string sstrv, std::string sobjv);
		void registerSid(uint32 id);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(GFFFile &gff, uint32 top);

	protected:
		std::string stype, sint, sfloat, sstr, sobj;
		std::string sintv, sfloatv, sstrv, sobjv;
		uint32 sid;
		int getInts(GFFFile &gff, uint32 iln, GFFFile::Struct &top);
		int getFloats(GFFFile &gff, uint32 fln, GFFFile::Struct &top);
		int getStrings(GFFFile &gff, uint32 sln, GFFFile::Struct &top);
		int getObjects(GFFFile &gff, uint32 oln, GFFFile::Struct &top);
};

class Effect: public ErrorHandler
{
	public:

		bool exposed, iconshown, skiponload;
		int16 type, subtype;
		int32 numintegers;
		uint32 creatorid, spellid, expireday, expiretime;
		uint64 id;
		float32 duration;
		std::vector<int32> ints;
		std::vector<float32> floats;
		std::vector<std::string> strings;
		std::vector<uint32> objects;

		Effect();
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(GFFFile &gff, uint32 top);

	protected:
		int getInts(GFFFile &gff, uint32 iln, GFFFile::Struct &top);
		int getFloats(GFFFile &gff, uint32 fln, GFFFile::Struct &top);
		int getStrings(GFFFile &gff, uint32 sln, GFFFile::Struct &top);
		int getObjects(GFFFile &gff, uint32 oln, GFFFile::Struct &top);
};

class ScriptSituation: public ErrorHandler
{
	public:
		typedef struct
		{
			uint8 multiclass, casterlevel, metatype;
			int32 id, type, itempropertyindex;
			uint32 item;
		} ScriptTalent;
		typedef struct
		{
			NWNStackElementType type;
			int32 valint;
			float32 valfloat;
			std::string valstring;
			uint32 valobject;
			class Effect valeffect;
			class ScriptEvent valevent;
			Location vallocation;
			ScriptTalent valtalent;
		} StackElement;
		typedef struct
		{
			int32 basepointer, stackpointer, totalsize;
			std::vector<StackElement> elements;
		} Stack;

		MemSeg code;
		int32 ipointer, secpointer;
		uint32 codesize;
		std::string name;
		Stack stack;

		ScriptSituation();
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(GFFFile &gff, uint32 top);

	protected:
		void groundStackElement(StackElement &element);
		int getStack(GFFFile &gff, uint32 ssn);
		int getStackElements(GFFFile &gff, uint32 seln, GFFFile::Struct &top);
		int getEffect(GFFFile &gff, uint32 esn, StackElement &element);
		int getEvent(GFFFile &gff, uint32 esn, StackElement &element);
		int getLocation(GFFFile &gff, uint32 lsn, StackElement &element);
		int getTalent(GFFFile &gff, uint32 tsn, StackElement &element);
		int writeStackElements(GFFFile &gff, uint32 list);
		int writeLocation(GFFFile &gff, uint32 sct, Location &loc);
		int writeTalent(GFFFile &gff, uint32 sct, ScriptTalent &talent);
};

class Action: public ErrorHandler
{
	public:
		typedef struct
		{
			NWNActionParameterType type;
			int32 valint;
			float32 valfloat;
			uint32 valobject;
			std::string valstring;
			ScriptSituation valscriptsituation;
		} Parameter;

		uint16 groupactionid;
		uint32 actionid;
		std::vector<Parameter> parameters;
		// uint16 numparams;

		Action();
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(GFFFile &gff, uint32 top);

	protected:
		void groundParameter(Parameter &parameter);
		int getParameters(GFFFile &gff, uint32 pln, GFFFile::Struct &top);
		int getScriptSituation(GFFFile &gff, uint32 psn, Parameter &element);
		int writeParameters(GFFFile &gff, uint32 list);
};

class Event: public ErrorHandler
{
	public:
		NWNEventType type;
		uint32 callerid, objectid, day, time;
		uint8 remove;
		int32 playanimation;
		uint32 broadcast;
		uint64 id;
		class Effect effect;
		class ScriptSituation scriptsituation;
		class ScriptEvent scriptevent;

		Event();
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(GFFFile &gff, uint32 top);

	protected:
		int getRemove(GFFFile &gff, uint32 rsn);
		int getPlay(GFFFile &gff, uint32 psn);
		int getBroadCast(GFFFile &gff, uint32 bsn);
		int getEffect(GFFFile &gff, uint32 esn);
		int getScriptSituation(GFFFile &gff, uint32 sssn);
		int getScriptEvent(GFFFile &gff, uint32 sesn);
};

class ModuleIFO: public ErrorHandler
{
	public:
		typedef struct
		{
			uint32 number;
			std::string value;
		} token;

		// 0. bit: Expansion 1; 1. bit: Expansions 2; ...
		bool savegame, nwm;
		uint8 dawnhour, duskhour, starthour, startday, startmonth, xpscale;
		uint8 minperhour;
		uint16 startminute, startsecond, startmilisecond;
		MemSeg id;
		uint32 startyear, version, transition;
		uint32 nextcharid0, nextcharid1, nextobjid0, nextobjid1;
		int32 creatorid, maxhenchmen;
		uint64 nexteffectid;
		float32 entryx, entryy, entryz, entrydirx, entrydiry;
		std::string talktable, entryarea, tag, mingamever, startmovie;
		std::string onacquireitem, onactivateitem, oncliententer, onclientleave;
		std::string oncutsceneabort, onheartbeat, onmoduleload, onmodulestart;
		std::string onplayerdeath, onplayerdying, onplayerequipitem, onplayerlevelup;
		std::string onplayerrest, onplayerunequipitem, onplayerrespawn, onunacquireitem;
		std::string onuserdefined;
		std::string nwmresname;
		std::vector<uint32> areaids;
		std::vector<std::string> haks, areas, cachenss;
		std::vector<token> tokens;
		std::vector<class Event> events;
		std::vector<class Creature> players;
		std::vector<class TURD> turds;
		vartable variables;
		ExoLocString desc, name;
		bitfield expansions;

		ModuleIFO();
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(MemSeg &memseg);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);
		void checkRanges(void);

	protected:
		int getHaks(GFFFile &gff, uint32 hln, uint32 hn, GFFFile::Struct &top);
		int getAreas(GFFFile &gff, uint32 aln, GFFFile::Struct &top);
		int getCacheNss(GFFFile &gff, uint32 cln, GFFFile::Struct &top);
		int getTokens(GFFFile &gff, uint32 tln, GFFFile::Struct &top);
		int getEvents(GFFFile &gff, uint32 eln, GFFFile::Struct &top);
		int getPlayers(GFFFile &gff, uint32 pln, GFFFile::Struct &top);
		int getTurds(GFFFile &gff, uint32 tln, GFFFile::Struct &top);
		int writeEvents(GFFFile &gff, uint32 top);
		int writePlayers(GFFFile &gff, uint32 top);
		int writeTurds(GFFFile &gff, uint32 top);
};

class Dialog: public ErrorHandler
{
	public:
		typedef class Entry
		{
			public:
				typedef struct Reply
				{
					bool link;
					std::string linkcomment, active;
					class Dialog::Entry *child;
				} Reply;

				std::vector<Dialog::Entry::Reply> replies;
				std::vector<class Dialog::Entry *> parents;
				std::string comment, speaker, script, sound, quest;
				ExoLocString text;
				uint32 anim, delay, questentry;
				bool pc;

				Entry();
				void del(void);
				void tidy(void);
				void add(Entry *e, std::string active="", bool link=false, std::string lcomment="");
				uint32 countWords(void);

				uint32 id;
				std::vector<uint32> repliesids;

			protected:
				bool isWSp(char c);
				uint32 countWords(std::string &str);

			friend class Dialog;
		} Entry;

		std::vector<std::string> active;
		std::vector<Entry *> entries;
		bool preventzoom;
		uint32 delayentry, delayreply, wordcount;
		std::string endconv, endconvabort;

		Dialog();
		~Dialog();
		Dialog &operator=(const Dialog &src);
		Dialog(const Dialog &src);
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(MemSeg &memseg);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);
		void add(Entry *e, std::string active="");
		void del(Entry *e);
		uint32 countWords(void);

	protected:
		typedef struct StartEntry
		{
			uint32 index;
			std::string active;
		} StartEntry;

		int getEntries(GFFFile &gff, uint32 eln, std::vector<Entry *> &entries,
				GFFFile::Struct &top);
		int getReplies(GFFFile &gff, GFFFile::Struct s, uint32 rln,
				Entry &entry);
		int getStartList(GFFFile &gff, uint32 sln, std::vector<StartEntry> &startlist,
				GFFFile::Struct &top);
		void resolveEntries(std::vector<StartEntry> &sl, std::vector<Entry *> &pe,
				std::vector<Entry *> &pr);
		void resolveEntries(std::vector<Entry *> &pe, std::vector<Entry *> &pr,
				Entry **entry, Entry *parent, uint32 idx, bool link, bool pc);
		void indexEntries(Entry *e, uint32 &idxe, uint32 &idxr);
		void copyFlat(Entry *e, std::vector<Entry *> &pe, std::vector<Entry *> &pr);
		void selectFlat(Entry *e, std::vector<Entry *> &pe, std::vector<Entry *> &pr);
		int writeEntry(GFFFile &gff, uint32 list, Entry &e);
};

class Area: public ErrorHandler
{
	public:
		typedef struct AreaTile
		{
			bool animloop1, animloop2, animloop3;
			uint8 mainlight1, mainlight2, srclight1, srclight2;
			uint8 orientation;
			uint32 height, id;
		} AreaTile;

		NWNObjectState objectstate;
		// ARE - Static stuff
		bitfield flags;
		bool daynight, isnight, sunshadow, moonshadow, norest;
		uint8 clightning, crain, csnow, lightscheme, shadowopacity, sunfog, moonfog;
		uint8 skybox, windpower, pvp;
		uint16 loadscreen;
		uint32 sunambient, sundiffuse, sunfogcolor;
		uint32 moonambient, moondiffuse, moonfogcolor;
		int32 width, height, modlisten, modspot;
		uint32 version;
		float32 fogclipdist;
		std::string resref, tag, comments, tileset;
		std::string onenter, onexit, onheartbeat, onuserdefined;
		ExoLocString name;
		std::vector<AreaTile> tiles;
		// GIT - Dynamic stuff
		bool weatherstarted;
		uint8 ambientdayvol, ambientnightvol, currentweather;
		int32 ambientday, ambientnight, envaudio;
		int32 musicbattle, musicday, musicnight, musicdelay;
		vartable variables;
		std::vector<class Creature> creatures;
		std::vector<class Door> doors;
		std::vector<class Encounter> encounters;
		std::vector<class Item> items;
		std::vector<class Placeable> placeables;
		std::vector<class Sound> sounds;
		std::vector<class Store> stores;
		std::vector<class Trigger> triggers;
		std::vector<class Waypoint> waypoints;
		std::vector<class Effect> effects;
		// GIC - Comments on all objects
		std::vector<std::string> comcreatures, comdoors, comencounters;
		std::vector<std::string> comitems, complaceables, comsounds;
		std::vector<std::string> comstores, comtriggers, comwaypoints;

		Area();
		int load(std::string arefile, std::string gitfile, std::string gicfile);
		int load(uint8 *are, uint32 aresize, uint8 *git, uint32 gitsize,
				uint8 *gic, uint32 gicsize);
		int load(MemSeg &are, MemSeg &gic, MemSeg &git);
		int load(GFFFile *are, GFFFile::Struct *aretop,
		         GFFFile *git, GFFFile::Struct *gittop,
		         GFFFile *gic, GFFFile::Struct *gictop);
		int save(std::string arefile, std::string gitfile, std::string gicfile);
		int save(uint8 **are, uint32 *aresize, uint8 **git, uint32 *gitsize,
				uint8 **gic, uint32 *gicsize);
		int save(GFFFile &are, uint32 aretop, GFFFile &git, uint32 gittop,
			GFFFile &gic, uint32 gictop);
		void checkRanges(void);

	protected:

		int loadAre(GFFFile &gff, GFFFile::Struct &top);
		int loadGit(GFFFile &gff, GFFFile::Struct &top);
		int loadGic(GFFFile &gff, GFFFile::Struct &top);
		int saveAre(GFFFile &gff, uint32 top);
		int saveGit(GFFFile &gff, uint32 top);
		int saveGic(GFFFile &gff, uint32 top);
		int getComments(GFFFile &gff, uint32 cln, std::vector<std::string> &comments,
				GFFFile::Struct &top);
		int getTiles(GFFFile &gff, uint32 tln, GFFFile::Struct &top);
		int getProperties(GFFFile &gff, uint32 psn);
		int getEffects(GFFFile &gff, uint32 eln, GFFFile::Struct &top);
		int getCreatures(GFFFile &gff, uint32 cln, GFFFile::Struct &top);
		int getDoors(GFFFile &gff, uint32 dln, GFFFile::Struct &top);
		int getEncounters(GFFFile &gff, uint32 eln, GFFFile::Struct &top);
		int getItems(GFFFile &gff, uint32 iln, GFFFile::Struct &top);
		int getPlaceables(GFFFile &gff, uint32 pln, GFFFile::Struct &top);
		int getSounds(GFFFile &gff, uint32 sln, GFFFile::Struct &top);
		int getStores(GFFFile &gff, uint32 sln, GFFFile::Struct &top);
		int getTriggers(GFFFile &gff, uint32 tln, GFFFile::Struct &top);
		int getWaypoints(GFFFile &gff, uint32 wln, GFFFile::Struct &top);
		int writeComments(GFFFile &gff, uint32 cln, uint32 id,
				std::vector<std::string> &comments);
		int writeEffects(GFFFile &gff, uint32 top);
};

class Situated: public ErrorHandler
{
	public:

		NWNObjectState objectstate;
		bool removekey, interruptable, lockable, locked, plot, trapped;
		bool trapdetectable, trapdisarmable, traponeshot, keyrequired;
		uint8 animationstate, hardness, openlockdc, closelockdc, traptype;
		uint8 trapdetectdc, disarmdc, fortitude, will, reflex, paletteid;
		int16 hp, currenthp;
		uint16 portraitid;
		uint32 appearance, faction, animationday, animationtime, objectid;
		float32 x, y, z, bearing;
		std::string templateresref, conversation, tag, comment;
		std::string onclosed, ondamaged, ondeath, ondisarm, onheartbeat, onlock;
		std::string onmeleeattacked, onopen, onspellcastat, ontraptriggered;
		std::string onunlock, onuserdefined, keyname;
		ExoLocString name, description;
		vartable variables;
		std::vector<class Effect> effects;
		std::vector<class Action> actions;

		Situated();
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(GFFFile &gff, uint32 top);
		void checkRanges(void);

	protected:
		int getEffects(GFFFile &gff, uint32 eln, GFFFile::Struct &top);
		int getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top);
		int writeActions(GFFFile &gff, uint32 top);
		int writeEffects(GFFFile &gff, uint32 top);
};

class Door: public Situated
{
	public:
		uint8 generictype, linkedtoflags;
		uint16 loadscreen;
		std::string linkedto;
		std::string onclick, onfailtoopen;

		Door();
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);
		void checkRanges();
};

class Placeable: public Situated
{
	public:

		bool isstatic, useable, hasinventory, diewhenempty, lightstate;
		uint8 bodybag;
		uint32 trapcreator, trapfaction;
		std::string portal;
		std::string oninventorydisturbed, onused;
		std::vector<class Item> items;

		Placeable();
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);
		void checkRanges();

	protected:
		int getItems(GFFFile &gff, uint32 iln, GFFFile::Struct &top);
};

class Item: public ErrorHandler
{
	public:
		typedef struct
		{
			bool useable;
			int8 costtable, param, paramvalue, usesperday;
			uint16 costvalue, propertyname, subtype;
		} Property;

		NWNObjectState objectstate;
		NWNItemState itemstate;
		NWNModelType modeltype;
		NWNEquipped equipped;
		bool cursed, plot, stolen, dropable, identified, pickpocketable;
		uint8 paletteid, charges;
		uint8 cloth1color, cloth2color;
		uint8 leather1color, leather2color;
		uint8 metal1color, metal2color;
		uint8 modelpart1, modelpart2, modelpart3;
		uint8 armorneck, armortorso, armorbelt, armorpelvis;
		uint8 armorlshoul, armorlbicep, armorlfarm, armorlhand;
		uint8 armorlthigh, armorlshin, armorlfoot;
		uint8 armorrshoul, armorrbicep, armorrfarm, armorrhand;
		uint8 armorrthigh, armorrshin, armorrfoot;
		uint8 armorrobe;
		uint16 stacksize;
		int32 baseitem, reposx, reposy;
		uint32 cost, addcost, objectid;
		float32 x, y, z, dirx, diry;
		std::string tag, comment;
		std::string templateresref, inventoryres, equipres;
		ExoLocString name, description, descidentified;
		vartable variables;
		std::vector<Property> properties;
		std::vector<class Item> items;

		Item();
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);
		void checkRanges(void);

	protected:
		int getProperties(GFFFile &gff, uint32 pln, GFFFile::Struct &top);
		int getItems(GFFFile &gff, uint32 iln, GFFFile::Struct &top);
		int writeItems(GFFFile &gff, uint32 top);
};

class Waypoint: public ErrorHandler
{
	public:
		NWNObjectState objectstate;
		bool hasmapnote, mapnoteenabled;
		uint8 appearance, paletteid;
		uint32 objectid;
		float32 x, y, z, dirx, diry;
		std::string tag, comment;
		std::string templateresref;
		ExoLocString name, description, mapnote;
		vartable variables;
		std::vector<class Action> actions;

		Waypoint();
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);
		void checkRanges(void);

	protected:
		int getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top);
		int writeActions(GFFFile &gff, uint32 top);
};

class Trigger: public ErrorHandler
{
	public:
		typedef struct
		{
			float32 x, y, z;
		} Point;

		NWNObjectState objectstate;
		bool trapflag, traponeshot, trapdetectable, trapdisarmable;
		uint8 type, cursor, paletteid;
		uint8 linkedtoflags, traptype, trapdetectdc, trapdisarmdc;
		uint16 loadscreenid, portraitid;
		uint32 creatorid, faction, objectid;
		float32 highlightheight, x, y, z, dirx, diry;
		std::string tag, comment, linkedto;
		std::string onclick, ondisarm, ontraptriggered;
		std::string scriptheartbeat, scriptonenter, scriptonexit, scriptuserdefine;
		std::string templateresref;
		ExoLocString name;
		vartable variables;
		std::vector<Point> points;
		std::vector<Action> actions;

		Trigger();
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);
		void checkRanges(void);

	protected:
		int getGeometry(GFFFile &gff, uint32 gln, GFFFile::Struct &top);
		int getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top);
		int writeActions(GFFFile &gff, uint32 top);
};

class Creature: public ErrorHandler
{
	public:
		typedef struct
		{
			int8 uses;
			uint16 feat;
		} Feat;
		typedef struct
		{
			bool ready;
			uint8 metamagic;
			uint16 spell;
			bitfield flags;
		} Spell;
		typedef struct
		{
			bool kspells, mspells;
			uint8 domain1, domain2, school;
			uint8 spellsleft[10];
			uint16 level;
			uint32 cclass;
			std::vector<Spell> knownspells[10];
			std::vector<Spell> memorizedspells[10];
		} Class;
		typedef struct
		{
			bool epic;
			uint8 ability, hitdie, cclass;
			uint16 skillpoints;
			std::vector<uint8> skills;
			std::vector<uint16> knownspells[10], feats;
		} LvlStat;
		typedef struct
		{
			uint8 spelllevel;
			uint16 spell;
			bitfield flags;
		} SpecialAbility;
		typedef struct
		{
			int32 id;
			std::string expression;
		} Expression;
		typedef struct
		{
			int8 data;
			uint32 objectid;
		} Perception;
		typedef struct
		{
			bool decays;
			int32 amount, duration;
			uint32 objectid, day, time;
		} PersonalReputation;
		typedef struct
		{
			uint8 modifier;
			int8 modifiertype, weaponwield;
			int8 versuslawchaos, versusgoodevil, versusrace;
		} AttackDamage;
		typedef struct
		{
			NWNQBObjectType type;
			uint8 castpropidx, castsubpropidx;
			uint8 contreposx, contreposy;
			uint8 itemreposx, itemreposy;
			uint8 secitemreposx, secitemreposy;
			uint8 domainlevel, metatype, multiclass;
			int32 intparam1;
			uint32 iteminvslot;
			uint32 seciteminvslot;
			std::string commandlabel, commandline;
		} QBObject;
		// TODO: What's in all the other quickbar types? [high]

		NWNObjectState objectstate;
		NWNCreatureState creaturestate;
		NWNCreatureAppearance creatureappearance;
		bool commandable, dm, pc, destroyable, plot, immortal, interruptable;
		bool disarmable, lootable, nopermdeath, deadselectable, raiseable;
		bool stealthmode, detectmode, mod_isprimaryplr, pm_ispolymorphed;
		int8 movementrate;
		uint8 fortitudesavethrow, reflexsavethrow, willsavethrow;
		uint8 ambientanimstate, baseattackbonus, naturalac;
		uint8 strength, dexterity, constitution, intelligence, wisdom, charisma;
		uint8 gender, race, goodevil, lawfulchaotic, overridebab, listening;
		uint8 bodybag, tail, wings, startingpackage;
		uint8 paletteid, perceptionrange, mclasslevupin;
		uint8 bodyhead, bodyneck, bodytorso, bodybelt, bodypelvis;
		uint8 bodylshoul, bodylbicep, bodylfarm, bodylhand;
		uint8 bodyrshoul, bodyrbicep, bodyrfarm, bodyrhand;
		uint8 bodylthigh, bodylshin, bodylfoot;
		uint8 bodyrthigh, bodyrshin, bodyrfoot;
		uint8 colorskin, colorhair, colortattoo1, colortattoo2;
		int16 maxhitpoints, hitpoints, currenthitpoints, ac;
		int16 fortitudebonus, reflexbonus, willbonus, creaturesize, pregamecurrent;
		uint16 appearance, skillpoints, soundset, factionid, portraitid;
		int32 age, cradjust, walkrate, familiartype, mod_mapnumareas, phenotype;
		uint32 experience, gold, animationday, animationtime, decaytime;
		uint32 bodybagid, objectid, areaid, masterid, sitobject;
		uint32 encounterobject;
		float32 challengerating, x, y, z, dirx, diry;
		std::string subrace, deity, familiarname, tag, comment, mod_commntyname;
		std::string conversation, templateresref, equipres;
		std::string scriptattacked, scriptdamaged, scriptdeath, scriptdialogue;
		std::string scriptdisturbed, scriptendround, scriptheartbeat;
		std::string scriptonblocked, scriptonnotice, scriptrested, scriptspawn;
		std::string scriptspellat, scriptuserdefine, portrait;
		ExoLocString description, firstname, lastname, mod_firstname, mod_lastname;
		std::vector<Class> classes;
		std::vector<uint8> skills;
		std::vector<Feat> feats;
		std::vector<int32> reputations;
		std::vector<class Item> items, equippeditems;
		std::vector<class Effect> effects;
		std::vector<class Action> actions;
		std::vector<SpecialAbility> specialabilities;
		std::vector<Expression> expressions;
		std::vector<Perception> perceptions;
		std::vector<PersonalReputation> preputations;
		std::vector<LvlStat> lvlstats;
		MemSeg mapareasdata;
		std::vector<MemSeg> mapdatas;

		// CombatInfo (equipped weapons?)
		int8 numattacks, spellresistance, arcanespellfail, armorcheckpen;
		int8 unarmeddamdice, unarmeddamdie, onhandcritrng, onhandcritmult;
		int8 offhandweaponeq, offhandcritrng, offhandcritmult, creaturedice;
		int8 creaturedie, reaturedice, reaturedie, eaturedice, eaturedie;
		int8 damagedice, damagedie;
		uint8 onhandattackmod, onhanddamagemod, offhandattackmod, offhanddamagemod;
		uint32 leftequip, rightequip;
		std::string leftstring, rightstring;
		std::vector<AttackDamage> attack, damage;
		QBObject quickbar[36];
		vartable variables;

		Creature();
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);
		void checkRanges(void);

	protected:
		int getCombatInfo(GFFFile &gff, uint32 csn);
		int getClasses(GFFFile &gff, uint32 cln, GFFFile::Struct &top);
		int getItems(GFFFile &gff, uint32 iln, GFFFile::Struct &top, bool equip);
		int getSpellsLeft(GFFFile &gff, uint32 sln, GFFFile::Struct &top,
				Class &cls);
		int getSpellLevel(GFFFile &gff, uint32 sln, GFFFile::Struct &top,
				std::vector<Spell> &spells);
		int getSkills(GFFFile &gff, uint32 sln, GFFFile::Struct &top);
		int getFeats(GFFFile &gff, uint32 fln, GFFFile::Struct &top);
		int getSpecAbilities(GFFFile &gff, uint32 saln, GFFFile::Struct &top);
		int getExpressions(GFFFile &gff, uint32 eln, GFFFile::Struct &top);
		int getPerceptions(GFFFile &gff, uint32 pln, GFFFile::Struct &top);
		int getPreputations(GFFFile &gff, uint32 prln, GFFFile::Struct &top);
		int getReputations(GFFFile &gff, uint32 rln, GFFFile::Struct &top);
		int getEffects(GFFFile &gff, uint32 eln, GFFFile::Struct &top);
		int getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top);
		int getMapDatas(GFFFile &gff, uint32 mdln, GFFFile::Struct &top);
		int getAttackDamage(GFFFile &gff, uint32 adln, GFFFile::Struct &top,
				std::vector<AttackDamage> &list);
		int getQuickBar(GFFFile &gff, uint32 qln, GFFFile::Struct &top);
		int getLvlStats(GFFFile &gff, uint32 lsln, GFFFile::Struct &top);
		int writeSpellsLeft(GFFFile &gff, uint32 top, uint8 sl[10]);
		int writeKSpells(GFFFile &gff, uint32 top, std::vector<Spell> spells[10]);
		int writeMSpells(GFFFile &gff, uint32 top, std::vector<Spell> spells[10]);
		int writeActions(GFFFile &gff, uint32 top);
		int writeEffects(GFFFile &gff, uint32 top);
		int writeQuickBar(GFFFile &gff, uint32 top);
		int writeAttackDamage(GFFFile &gff, uint32 list,
				std::vector<AttackDamage> &ad, uint32 id);
		int writeLvlStats(GFFFile &gff, uint32 top);
};

class Encounter: public ErrorHandler
{
	public:
		typedef struct
		{
			float32 x, y, z;
		} Point;
		typedef struct
		{
			float32 x, y, z, orientation;
		} SpawnPoint;
		typedef struct
		{
			bool singlespawn;
			int32 appearance;
			float32 cr;
			std::string resref;
		} EncounterCreature;

		NWNObjectState objectstate;
		bool active, playeronly, reset, started;
		uint8 maxcreatures, reccreatures, exhausted, paletteid;
		int32 resettime, respawns, arealistmaxsize;
		int32 currentspawns, customscriptid, numberspawned;
		uint32 difficultyindex, faction, spawnoption;
		uint32 heartbeatday, heartbeattime;
		uint32 lastentered, lastleft, lastspawnday, lastspawntime, objectid;
		float32 x, y, z, areapoints, spawnpoolactive;
		std::string tag, comment;
		std::string onentered, onexhausted, onexit, onheartbeat, onuserdefined;
		std::string templateresref;
		ExoLocString name;
		std::vector<Point> points;
		std::vector<SpawnPoint> spawnpoints;
		std::vector<EncounterCreature> creatures;
		std::vector<class Action> actions;
		vartable variables;

		Encounter();
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);
		void checkRanges(void);

	protected:
		uint32 difficulty;

		int getGeometry(GFFFile &gff, uint32 gln, GFFFile::Struct &top);
		int getSpawnPoints(GFFFile &gff, uint32 spln, GFFFile::Struct &top);
		int getCreatures(GFFFile &gff, uint32 cln, GFFFile::Struct &top);
		int getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top);
		int writeActions(GFFFile &gff, uint32 top);
};

class Sound: public ErrorHandler
{
	public:
		NWNObjectState objectstate;
		bool active, continuous, random, randomposition;
		bool looping, positional, generatedtype;
		uint8 priority, times, volume, volumevrtn, paletteid;
		uint32 hours, interval, intervalvrtn, objectid;
		float32 elevation, maxdistance, mindistance, x, y, z;
		float32 pitchvariation, randomrangex, randomrangey;
		std::string tag, comment;
		std::string templateresref;
		ExoLocString name;
		std::vector<std::string> sounds;
		std::vector<class Action> actions;
		vartable variables;

		Sound();
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);
		void checkRanges(void);

	protected:
		int getSounds(GFFFile &gff, uint32 sln, GFFFile::Struct &top);
		int getActions(GFFFile &gff, uint32 aln, GFFFile::Struct &top);
		int writeActions(GFFFile &gff, uint32 top);
};

class Store: public ErrorHandler
{
	public:
		typedef struct
		{
			bool infinite;
			uint16 reposx, reposy;
			std::string resref;
		} StoreItem;

		NWNObjectState objectstate;
		bool blackmarket;
		uint8 id;
		int32 bm_markdown, markdown, markup, maxbuyprice, storegold, identifyprice;
		uint32 objectid;
		float32 x, y, z, dirx, diry;
		std::string tag, comment;
		std::string onopenstore, onstoreclosed;
		std::string templateresref;
		ExoLocString name;
		std::vector<StoreItem> items[STORE_ITEMS_MAX];
		std::vector<int32> willnotbuy, willonlybuy;
		vartable variables;

		Store();
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);
		void checkRanges(void);

	protected:
		int getStoreLists(GFFFile &gff, uint32 sln, GFFFile::Struct top);
		int getStoreList(GFFFile &gff, uint32 sln, GFFFile::Struct top,
				std::vector<StoreItem> &items);
		int getBuyLimits(GFFFile &gff, uint32 brln, GFFFile::Struct top,
				std::vector<int32> &limits);
		int writeStoreList(GFFFile &gff, uint32 top, std::vector<StoreItem> &list);
		int writeBuyLimits(GFFFile &gff, uint32 list, std::vector<int32> &items);
};

// "Temporary User Resource Data" *groan*
class TURD: public ErrorHandler
{
	public:
		typedef struct
		{
			bool decays;
			int32 amount, duration;
			uint32 day, time, objectid;
		} PersonalReputation;

		int32 mapnumareas;
		uint32 calendarday, timeofday, playerid, areaid;
		float32 x, y, z, dirx, diry, dirz;
		std::string commntyname;
		ExoLocString firstname;
		ExoLocString lastname;
		MemSeg mapareasdata;
		std::vector<class Effect> effects;
		std::vector<PersonalReputation> preputations;
		std::vector<int32> reputations;
		std::vector<MemSeg> mapdatas;
		vartable variables;

		TURD();
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(GFFFile &gff, uint32 top);

	protected:
		int getEffects(GFFFile &gff, uint32 eln, GFFFile::Struct &top);
		int getPreputations(GFFFile &gff, uint32 prln, GFFFile::Struct &top);
		int getReputations(GFFFile &gff, uint32 rln, GFFFile::Struct &top);
		int getMapDatas(GFFFile &gff, uint32 mdln, GFFFile::Struct &top);
		int writeEffects(GFFFile &gff, uint32 top);
};

class Journal: public ErrorHandler
{
	public:
		typedef struct
		{
			bool end;
			uint32 id;
			ExoLocString text;
		} Entry;
		typedef struct
		{
			uint8 priority;
			uint32 xp;
			std::string tag, comment;
			ExoLocString name;
			std::vector<Entry> entries;
		} Category;

		std::vector<Category> categories;
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(MemSeg &memseg);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);

	protected:
		int getCategories(GFFFile &gff, uint32 cln, GFFFile::Struct &top);
		int getEntries(GFFFile &gff, uint32 eln, GFFFile::Struct &top,
				std::vector<Entry> &entries);
		int writeEntries(GFFFile &gff, uint32 top, std::vector<Entry> &entries);
};

class Faction: public ErrorHandler
{
	public:
		typedef struct
		{
			bool global;
			uint32 parentid;
			std::string name;
		} Entry;

		std::vector<Entry> factions;
		// [src][dest]
		std::vector< std::vector<uint32> > reputations;
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(MemSeg &memseg);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);

	protected:
		int getFactions(GFFFile &gff, uint32 fln, GFFFile::Struct &top);
		int getReputations(GFFFile &gff, uint32 rln, GFFFile::Struct &top);
};

class Palette: public ErrorHandler
{
	public:
		typedef struct TreeNode
		{
			uint8 type, id;
			uint32 strref;
			float32 cr;
			std::string name, deleteme, resref, faction;
			std::vector<struct TreeNode *> nodes;
		} TreeNode;

		NWNPaletteState palettestate;
		std::string tilesetresref;
		NWNFileType restype;
		std::vector<TreeNode *> nodes;

		Palette();
		int load(std::string fname);
		int load(uint8 *mem, uint32 size);
		int load(GFFFile &gff, GFFFile::Struct &top);
		int save(std::string fname);
		int save(uint8 **mem, uint32 *size);
		int save(GFFFile &gff, uint32 top);

	protected:
		int getNodes(GFFFile &gff, uint32 nln, GFFFile::Struct &top,
				std::vector<TreeNode *> &nodel);
		int writeNodes(GFFFile &gff, uint32 top, std::vector<TreeNode *> &nodel,
				std::string lname, uint32 sid);
};

class GameDefaults: public ErrorHandler
{
	public:
		std::string basedir;
		TwoDAFile gender, racialtypes, classes, appearance, baseitems, armor;
		TwoDAFile bodybag, creaturesize, creaturespeed, domains, feat, itempropdef;
		TwoDAFile itemprops, itemvalue, masterfeats, metamagic, phenotype;
		TwoDAFile portraits, skills, spellschools, spells;

		int load(std::string basedir);

	protected:
		typedef struct
		{
			uint32 n, strn;
		} ResFound;
		typedef struct Finder
		{
			std::vector<KEYFile> keys;
			std::vector<std::string> bifs, resnames;
			std::vector<uint32> residx;
			std::vector< std::vector<ResFound> > founds;
			NWNFileType restype;
			int (GameDefaults::*load)(MemSeg &mem, uint32 idx);
		} Finder;

		void clearFinder(Finder &finder);
		int findRes(Finder &finder);
		int loadRes(Finder &finder);
		int load2da(Finder &finder);
		int load2daMem(MemSeg &mem, uint32 idx);
};

class Module: public ErrorHandler
{
	public:
		ModuleIFO ifo;
		Faction factions;
		Journal journal;
		std::vector<Dialog> dialogs;
		std::vector<Area> areas;
		std::vector<ERFFile> haks;

		int load(std::string filen);
		int save(std::string filen);

	protected:
		typedef struct
		{
			std::string resref;
			uint32 are, git, gic;
		} AreaFile;
};

#endif // _EOS_H_

// vim:tabstop=2
