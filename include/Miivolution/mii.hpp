#pragma once

#ifndef MIIVOLUTION_MII_HPP
#define MIIVOLUTION_MII_HPP

#if DOLPHIN_INCLUDES
#include <dolphin/types.h>
#else
#include <revolution/types.h>
#endif

namespace miivolution::mii {

#define MII_NAME_LENGTH			    10
#define MII_CREATOR_NAME_LENGTH		10

// TODO: Update with canonical struct replication from RFLi_Types.hpp
// TODO: Add converter from MII_DATA_STRUCT -> RFLiCharData (and vice versa)
// TODO: getGuestMii(index) -> MII_DATA_STRUCT

struct MII_DATA_STRUCT {
	u16 invalid:1; // doesn't seem to have any effect?
	u16 isGirl:1;
	u16 month:4;
	u16 day:5;
	u16 favColor:4;
	u16 isFavorite:1;

	u16 name[MII_NAME_LENGTH];

	u8 height; // 0 - 127
	u8 weight; // 0 - 127

 	u8 miiID1; // Unique 32-bit Mii identifier, generated based on the number of
 	u8 miiID2; // 4-second intervals since Jan 1, 2006 UTC.
 	u8 miiID3;  // The top 3 bits are a prefix that determines if a Mii is "special" "foreign" or regular
 	u8 miiID4;

	u8 systemID0; // Checksum8 of first 3 bytes of mac addr
	u8 systemID1; // mac addr 3rd-to-last byte
	u8 systemID2; // mac addr 2nd-to-last byte
	u8 systemID3; // mac addr last byte

	u16 faceShape:3; // 0 - 7
	u16 skinColor:3; // 0 - 5
	u16 facialFeature:4; // 0 - 11
	u16 unknown1:3; // Mii appears unaffected by changes to this data
	u16 mingleOff:1; // 0 = Mingle, 1 = Don't Mingle
	u16 unknown2:1; // Mii appears unaffected by changes to this data
	u16 downloaded:1; // If the Mii has been downloaded from the Check Mii Out Channel

	u16 hairType:7; // 0 - 71, Value is non-sequential with regard to page, row and column
	u16 hairColor:3; // 0 - 7
	u16 hairPart:1; // 0 = Normal, 1 = Reversed
	u16 unknown3:5;

	u32 eyebrowType:5; // 0 - 23, Value is non-sequential with regard to page, row and column
	u32 unknown4:1;
	u32 eyebrowRotation:4; // 0 - 11, Default value varies based on eyebrow type
	u32 unknown5:6;
	u32 eyebrowColor:3; // 0 - 7
	u32 eyebrowSize:4;	 // 0 - 8, Default = 4
	u32 eyebrowVertPos:5; // 3 - 18, Default = 10
	u32 eyebrowHorizSpacing:4; // 0 - 12, Default = 2

	u32 eyeType:6; // 0 - 47, Value is non-sequential with regard to page, row and column
	u32 unknown6:2;
	u32 eyeRotation:3; // 0 - 7, Default value varies based on eye type
	u32 eyeVertPos:5; // 0 - 18, Default = 12, Smaller Numbers = Higher Up
	u32 eyeColor:3; // 0 - 5
	u32 unknown7:1;
	u32 eyeSize:3; // 0 - 7, Default = 4
	u32 eyeHorizSpacing:4; // 0 - 12, Default = 2
	u32 unknown8:5;

	u16 noseType:4; // 0 - 11, Value is non-sequential with regard to row and column
	u16 noseSize:4; // 0 - 8, Default = 4
	u16 noseVertPos:5; // 0 - 18, Default = 9
	u16 unknown9:3;

	u16 lipType:5; // 0 - 23, Value is non-sequential with regard to page, row and column
	u16 lipColor:2; // 0 - 2
	u16 lipSize:4; // 0 - 8, Default = 4
	u16 lipVertPos:5; // 0 - 18, Default = 13

	u16 glassesType:4; // 0 - 8
	u16 glassesColor:3; // 0 - 5
	u16 unknown10:1; // when turned on mii does not appear (use not known)
	u16 glassesSize:3; // 0 - 7, Default = 4
	u16 glassesVertPos:5; // 0 - 20, Default = 10

	u16 mustacheType:2; // 0 - 3
	u16 beardType:2; // 0 - 3
	u16 facialHairColor:3; // 0 - 7
	u16 mustacheSize:4; // 0 - 8, Default = 4
	u16 mustacheVertPos:5; // 0 - 16, Default = 10

	u16 moleOn:1; // 0 = No Mole, 1 = Has Mole
	u16 moleSize:4; // 0 - 8, Default = 4
	u16 moleVertPos:5; // 0 - 30, Default = 20
	u16 moleHorizPos:5; // 0 - 16, Default = 2
	u16 unknown11:1;

	u16 creatorName[MII_CREATOR_NAME_LENGTH];
};

bool serializeMii(const MII_DATA_STRUCT& m, u8* out, u32 outSize);
bool deserializeMii(const u8* in, u32 inSize, MII_DATA_STRUCT& m);

}

#endif // MIIVOLUTION_MII_HPP
