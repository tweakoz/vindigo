/*************************************************************************

    Diet Go Go

*************************************************************************/

class dietgo_state
{
public:
	static void *alloc(running_machine &machine) { return auto_alloc_clear(&machine, dietgo_state(machine)); }

	dietgo_state(running_machine &machine) { }

	/* memory pointers */
	UINT16 *  pf1_rowscroll;
	UINT16 *  pf2_rowscroll;
	UINT16 *  spriteram;
//  UINT16 *  paletteram;    // currently this uses generic palette handling (in decodev.c)
	size_t    spriteram_size;

	/* devices */
	running_device *maincpu;
	running_device *audiocpu;
	running_device *deco16ic;
};



/*----------- defined in video/dietgo.c -----------*/

VIDEO_UPDATE( dietgo );
