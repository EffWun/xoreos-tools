/* xoreos-tools - Tools to help with xoreos development
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
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
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/util.cpp
 *  Utility functions to handle files used in BioWare's Aurora engine.
 */

#include "common/util.h"
#include "common/ustring.h"
#include "common/filepath.h"

#include "aurora/util.h"

namespace Aurora {

/** File type <-> extension mapping. */
struct FileExtension {
	FileType type;
	const char *extension;
};

static const FileExtension fileExtensions[] = {
	{kFileTypeNone,           ""    },
	{kFileTypeRES,            ".res"},
	{kFileTypeBMP,            ".bmp"},
	{kFileTypeMVE,            ".mve"},
	{kFileTypeTGA,            ".tga"},
	{kFileTypeWAV,            ".wav"},
	{kFileTypePLT,            ".plt"},
	{kFileTypeINI,            ".ini"},
	{kFileTypeBMU,            ".bmu"},
	{kFileTypeMPG,            ".mpg"},
	{kFileTypeTXT,            ".txt"},
	{kFileTypeWMA,            ".wma"},
	{kFileTypeWMV,            ".wmv"},
	{kFileTypeXMV,            ".xmv"},
	{kFileTypePLH,            ".plh"},
	{kFileTypeTEX,            ".tex"},
	{kFileTypeMDL,            ".mdl"},
	{kFileTypeTHG,            ".thg"},
	{kFileTypeFNT,            ".fnt"},
	{kFileTypeLUA,            ".lua"},
	{kFileTypeSLT,            ".slt"},
	{kFileTypeNSS,            ".nss"},
	{kFileTypeNCS,            ".ncs"},
	{kFileTypeMOD,            ".mod"},
	{kFileTypeARE,            ".are"},
	{kFileTypeSET,            ".set"},
	{kFileTypeIFO,            ".ifo"},
	{kFileTypeBIC,            ".bic"},
	{kFileTypeWOK,            ".wok"},
	{kFileType2DA,            ".2da"},
	{kFileTypeTLK,            ".tlk"},
	{kFileTypeTXI,            ".txi"},
	{kFileTypeGIT,            ".git"},
	{kFileTypeBTI,            ".bti"},
	{kFileTypeUTI,            ".uti"},
	{kFileTypeBTC,            ".btc"},
	{kFileTypeUTC,            ".utc"},
	{kFileTypeDLG,            ".dlg"},
	{kFileTypeITP,            ".itp"},
	{kFileTypeBTT,            ".btt"},
	{kFileTypeUTT,            ".utt"},
	{kFileTypeDDS,            ".dds"},
	{kFileTypeBTS,            ".bts"},
	{kFileTypeUTS,            ".uts"},
	{kFileTypeLTR,            ".ltr"},
	{kFileTypeGFF,            ".gff"},
	{kFileTypeFAC,            ".fac"},
	{kFileTypeBTE,            ".bte"},
	{kFileTypeUTE,            ".ute"},
	{kFileTypeBTD,            ".btd"},
	{kFileTypeUTD,            ".utd"},
	{kFileTypeBTP,            ".btp"},
	{kFileTypeUTP,            ".utp"},
	{kFileTypeDFT,            ".dft"},
	{kFileTypeDTF,            ".dtf"},
	{kFileTypeGIC,            ".gic"},
	{kFileTypeGUI,            ".gui"},
	{kFileTypeCSS,            ".css"},
	{kFileTypeCCS,            ".ccs"},
	{kFileTypeBTM,            ".btm"},
	{kFileTypeUTM,            ".utm"},
	{kFileTypeDWK,            ".dwk"},
	{kFileTypePWK,            ".pwk"},
	{kFileTypeBTG,            ".btg"},
	{kFileTypeUTG,            ".utg"},
	{kFileTypeJRL,            ".jrl"},
	{kFileTypeSAV,            ".sav"},
	{kFileTypeUTW,            ".utw"},
	{kFileType4PC,            ".4pc"},
	{kFileTypeSSF,            ".ssf"},
	{kFileTypeHAK,            ".hak"},
	{kFileTypeNWM,            ".nwm"},
	{kFileTypeBIK,            ".bik"},
	{kFileTypeNDB,            ".ndb"},
	{kFileTypePTM,            ".ptm"},
	{kFileTypePTT,            ".ptt"},
	{kFileTypeNCM,            ".ncm"},
	{kFileTypeMFX,            ".mfx"},
	{kFileTypeMAT,            ".mat"},
	{kFileTypeMDB,            ".mdb"},
	{kFileTypeSAY,            ".say"},
	{kFileTypeTTF,            ".ttf"},
	{kFileTypeTTC,            ".ttc"},
	{kFileTypeCUT,            ".cut"},
	{kFileTypeKA,             ".ka" },
	{kFileTypeJPG,            ".jpg"},
	{kFileTypeICO,            ".ico"},
	{kFileTypeOGG,            ".ogg"},
	{kFileTypeSPT,            ".spt"},
	{kFileTypeSPW,            ".spw"},
	{kFileTypeWFX,            ".wfx"},
	{kFileTypeUGM,            ".ugm"},
	{kFileTypeQDB,            ".qdb"},
	{kFileTypeQST,            ".qst"},
	{kFileTypeNPC,            ".npc"},
	{kFileTypeSPN,            ".spn"},
	{kFileTypeUTX,            ".utx"},
	{kFileTypeMMD,            ".mmd"},
	{kFileTypeSMM,            ".smm"},
	{kFileTypeUTA,            ".uta"},
	{kFileTypeMDE,            ".mde"},
	{kFileTypeMDV,            ".mdv"},
	{kFileTypeMDA,            ".mda"},
	{kFileTypeMBA,            ".mba"},
	{kFileTypeOCT,            ".oct"},
	{kFileTypeBFX,            ".bfx"},
	{kFileTypePDB,            ".pdb"},
	{kFileTypeTheWitcherSave, ".TheWitcherSave"},
	{kFileTypePVS,            ".pvs"},
	{kFileTypeCFX,            ".cfx"},
	{kFileTypeLUC,            ".luc"},
	{kFileTypePRB,            ".prb"},
	{kFileTypeCAM,            ".cam"},
	{kFileTypeVDS,            ".vds"},
	{kFileTypeBIN,            ".bin"},
	{kFileTypeWOB,            ".wob"},
	{kFileTypeAPI,            ".api"},
	{kFileTypeProperties,     ".properties"},
	{kFileTypePNG,            ".png"},
	{kFileTypeLYT,            ".lyt"},
	{kFileTypeVIS,            ".vis"},
	{kFileTypeRIM,            ".rim"},
	{kFileTypePTH,            ".pth"},
	{kFileTypeLIP,            ".lip"},
	{kFileTypeBWM,            ".bwm"},
	{kFileTypeTXB,            ".txb"},
	{kFileTypeTPC,            ".tpc"},
	{kFileTypeMDX,            ".mdx"},
	{kFileTypeRSV,            ".rsv"},
	{kFileTypeSIG,            ".sig"},
	{kFileTypeMAB,            ".mab"},
	{kFileTypeQST2,           ".qst"},
	{kFileTypeSTO,            ".sto"},
	{kFileTypeMDX2,           ".mdx"},
	{kFileTypeTXB2,           ".txb"},
	{kFileTypeART,            ".art"},
	{kFileTypeBIP,            ".bip"},
	{kFileType1DA,            ".1da"},
	{kFileTypeERF,            ".erf"},
	{kFileTypeBIF,            ".bif"},
	{kFileTypeKEY,            ".key"},

	{kFileTypeDBF,            ".dbf"},
	{kFileTypeCDX,            ".cdx"},
	{kFileTypeFPT,            ".fpt"},

	{kFileTypeFXA,            ".fxa"},
	{kFileTypeFXE,            ".fxe"},
	{kFileTypeFXM,            ".fxm"},
	{kFileTypeFXS,            ".fxs"},
	{kFileTypeXML,            ".xml"},
	{kFileTypeWLK,            ".wlk"},
	{kFileTypeUTR,            ".utr"},
	{kFileTypeSEF,            ".sef"},
	{kFileTypePFX,            ".pfx"},
	{kFileTypeTFX,            ".tfx"},
	{kFileTypeIFX,            ".ifx"},
	{kFileTypeLFX,            ".lfx"},
	{kFileTypeBBX,            ".bbx"},
	{kFileTypePFB,            ".pfb"},
	{kFileTypeGR2,            ".gr2"},
	{kFileTypeUPE,            ".upe"},
	{kFileTypeUSC,            ".usc"},
	{kFileTypeULT,            ".ult"},
	{kFileTypeFX ,            ".fx" },
	{kFileTypeMAX,            ".max"},
	{kFileTypeDOC,            ".doc"},
	{kFileTypeSCC,            ".scc"},

	{kFileTypePAL,            ".pal"},
	{kFileTypeCBGT,           ".cbgt"},
	{kFileTypeCDPTH,          ".cdpth"},
	{kFileTypeVX ,            ".vx" },
	{kFileTypeHERF,           ".herf"},
	{kFileTypeSADL,           ".sadl"},
	{kFileTypeSDAT,           ".sdat"},
	{kFileTypeSMALL,          ".small"},
	{kFileTypeNSBCA,          ".nsbca"},
	{kFileTypeEMIT,           ".emit"},
	{kFileTypeSPL,            ".spl"},
	{kFileTypeNBFP,           ".nbfp"},
	{kFileTypeNBFS,           ".nbfs"},
	{kFileTypeSMP,            ".smp"},
	{kFileTypeITM,            ".itm"},
	{kFileTypeNFTR,           ".nftr"},
	{kFileTypeRAW,            ".raw"},
	{kFileTypeNCLR,           ".nclr"},
	{kFileTypeNCGR,           ".ncgr"},
	{kFileTypeDICT,           ".dict"},

	{kFileTypeANB,            ".anb"},
	{kFileTypeANI,            ".ani"},
	{kFileTypeCNS,            ".cns"},
	{kFileTypeCUR,            ".cur"},
	{kFileTypeEVT,            ".evt"},
	{kFileTypeFDL,            ".fdl"},
	{kFileTypeFXO,            ".fxo"},
	{kFileTypeGAD,            ".gad"},
	{kFileTypeGDA,            ".gda"},
	{kFileTypeGFX,            ".gfx"},
	{kFileTypeLDF,            ".ldf"},
	{kFileTypeLST,            ".lst"},
	{kFileTypeMAL,            ".mal"},
	{kFileTypeMAO,            ".mao"},
	{kFileTypeMMH,            ".mmh"},
	{kFileTypeMOP,            ".mop"},
	{kFileTypeMOR,            ".mor"},
	{kFileTypeMSH,            ".msh"},
	{kFileTypeMTX,            ".mtx"},
	{kFileTypeNCC,            ".ncc"},
	{kFileTypePHY,            ".phy"},
	{kFileTypePLO,            ".plo"},
	{kFileTypeSTG,            ".stg"},
	{kFileTypeTBI,            ".tbi"},
	{kFileTypeTNT,            ".tnt"},
	{kFileTypeARL,            ".arl"},
	{kFileTypeFEV,            ".fev"},
	{kFileTypeFSB,            ".fsb"},
	{kFileTypeOPF,            ".opf"},

	{kFileTypeMOV,            ".mov"},
	{kFileTypeCURS,           ".curs"},
	{kFileTypePICT,           ".pict"},
	{kFileTypeRSRC,           ".rsrc"},
	{kFileTypePLIST,          ".plist"},

	{kFileTypeCRE,            ".cre"},
	{kFileTypePSO,            ".pso"},
	{kFileTypeVSO,            ".vso"},
	{kFileTypeABC,            ".abc"},
	{kFileTypeSBM,            ".sbm"},
	{kFileTypePVD,            ".pvd"},
	{kFileTypeAMP,            ".amp"},
	{kFileTypePLA,            ".pla"},
	{kFileTypePK,             ".pk" }
};

FileType getFileType(const Common::UString &path) {
	const Common::UString ext = Common::FilePath::getExtension(path);

	for (int i = 0; i < ARRAYSIZE(fileExtensions); i++)
		if (ext.equalsIgnoreCase(fileExtensions[i].extension))
			return fileExtensions[i].type;

	return kFileTypeNone;
}

Common::UString setFileType(const Common::UString &path, FileType type) {
	Common::UString ext;

	for (int i = 0; i < ARRAYSIZE(fileExtensions); i++)
		if (fileExtensions[i].type == type) {
			ext = fileExtensions[i].extension;
			break;
		}

	return Common::FilePath::changeExtension(path, ext);
}

bool isMale(Language language) {
	return !isFemale(language);
}

bool isFemale(Language language) {
	return (language == kLanguageEnglishFemale)            ||
	       (language == kLanguageFrenchFemale)             ||
	       (language == kLanguageGermanFemale)             ||
	       (language == kLanguageItalianFemale)            ||
	       (language == kLanguageSpanishFemale)            ||
	       (language == kLanguagePolishFemale)             ||
	       (language == kLanguageKoreanFemale)             ||
	       (language == kLanguageChineseTraditionalFemale) ||
	       (language == kLanguageChineseSimplifiedFemale)  ||
	       (language == kLanguageJapaneseFemale);
}

bool isLanguageStandard(Language language) {
	return (language == kLanguageEnglishMale)              ||
	       (language == kLanguageEnglishMale)              ||
	       (language == kLanguageEnglishFemale)            ||
	       (language == kLanguageFrenchMale)               ||
	       (language == kLanguageFrenchFemale)             ||
	       (language == kLanguageGermanMale)               ||
	       (language == kLanguageGermanFemale)             ||
	       (language == kLanguageItalianMale)              ||
	       (language == kLanguageItalianFemale)            ||
	       (language == kLanguageSpanishMale)              ||
	       (language == kLanguageSpanishFemale)            ||
	       (language == kLanguagePolishMale)               ||
	       (language == kLanguagePolishFemale)             ||
	       (language == kLanguageKoreanMale)               ||
	       (language == kLanguageKoreanFemale)             ||
	       (language == kLanguageChineseTraditionalMale)   ||
	       (language == kLanguageChineseTraditionalFemale) ||
	       (language == kLanguageChineseSimplifiedMale)    ||
	       (language == kLanguageChineseSimplifiedFemale)  ||
	       (language == kLanguageJapaneseMale)             ||
	       (language == kLanguageJapaneseFemale);
}

bool isLanguageTheWitcher(Language language) {
	return (language == kLanguageWitcherDefault)            ||
	       (language == kLanguageWitcherEnglish)            ||
	       (language == kLanguageWitcherFinalEnglish)       ||
	       (language == kLanguageWitcherFinalEnglishShort)  ||
	       (language == kLanguageWitcherPolish)             ||
	       (language == kLanguageWitcherGerman)             ||
	       (language == kLanguageWitcherFrench)             ||
	       (language == kLanguageWitcherSpanish)            ||
	       (language == kLanguageWitcherItalian)            ||
	       (language == kLanguageWitcherRussian)            ||
	       (language == kLanguageWitcherCzech)              ||
	       (language == kLanguageWitcherHungarian)          ||
	       (language == kLanguageWitcherKorean)             ||
	       (language == kLanguageWitcherChineseTraditional) ||
	       (language == kLanguageWitcherChineseSimplified);
}

Common::UString getPlatformDescription(Platform platform) {
	static const char *names[] = {
		"Windows", "Nintendo DS", "Mac OS X", "Xbox", "PlayStation 3", "Xbox 360", "Unknown"
	};

	return names[platform];
}

} // End of namespace Aurora
