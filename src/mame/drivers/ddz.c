/*
    Dou Di Zhu (by IGS?)

    uses Crystal System-like hardware (same CPU + Video Chip, but single PCB)
    meant to be some kind of poker game?

    Rom appears to be encrypted / scrambled

*/

#include "driver.h"
#include "cpu/se3208/se3208.h"
#include "video/vrender0.h"
#include "machine/ds1302.h"
#include "sound/vrender0.h"



static ADDRESS_MAP_START( ddz_mem, ADDRESS_SPACE_PROGRAM, 32 )
	AM_RANGE(0x00000000, 0x003fffff) AM_ROM AM_WRITENOP
ADDRESS_MAP_END

static VIDEO_START(ddz)
{
}


static VIDEO_UPDATE(ddz)
{
	return 0;
}

static VIDEO_EOF(ddz)
{

}

static INTERRUPT_GEN(ddz_interrupt)
{
//  IntReq(24);     //VRender0 VBlank
}

static INPUT_PORTS_START(ddz)

INPUT_PORTS_END


static const struct VR0Interface vr0_interface =
{
	0x04800000
};


static MACHINE_DRIVER_START( ddz )
	MDRV_CPU_ADD(SE3208, 43000000)
	MDRV_CPU_PROGRAM_MAP(ddz_mem,0)
 	MDRV_CPU_VBLANK_INT(ddz_interrupt,1)

	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(DEFAULT_60HZ_VBLANK_DURATION)

	//MDRV_MACHINE_RESET(ddz)

	//MDRV_NVRAM_HANDLER(generic_0fill)

	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER )
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_RGB15)
	MDRV_SCREEN_SIZE(320, 240)
	MDRV_SCREEN_VISIBLE_AREA(0, 319, 0, 239)
	MDRV_PALETTE_LENGTH(8192)

	MDRV_VIDEO_START(ddz)
	MDRV_VIDEO_UPDATE(ddz)
	MDRV_VIDEO_EOF(ddz)

	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(VRENDER0, 0)
	MDRV_SOUND_CONFIG(vr0_interface)
	MDRV_SOUND_ROUTE(0, "left", 1.0)
	MDRV_SOUND_ROUTE(1, "right", 1.0)
MACHINE_DRIVER_END



ROM_START( ddz )
	ROM_REGION( 0x400000, REGION_CPU1, 0 )
	ROM_LOAD("ddz.001.rom",  0x000000, 0x400000, CRC(b379f823) SHA1(531885b35d668d22c75a9759994f4aca6eacb046) )
	ROM_LOAD("ddz.002.rom",  0x000000, 0x400000, CRC(285c744d) SHA1(2f8bc70825e55e3114015cb263e786df35cde275) )
	ROM_LOAD("ddz.003.rom",  0x000000, 0x400000, CRC(61c9b5c9) SHA1(0438417398403456a1c49408881797a94aa86f49) )
ROM_END


GAME( 200?, ddz,	0,  ddz, ddz, 0, ROT0, "IGS?", "Dou Di Zhu", GAME_NOT_WORKING )
