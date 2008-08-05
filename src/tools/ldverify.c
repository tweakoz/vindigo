/***************************************************************************

    ldverify.c

    Laserdisc AVI/CHD verifier.

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "aviio.h"
#include "bitmap.h"
#include "chd.h"
#include "vbiparse.h"



/***************************************************************************
    CONSTANTS
***************************************************************************/



/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

typedef struct _movie_info movie_info;
struct _movie_info
{
	double	framerate;
	int		numframes;
	int		width;
	int		height;
	int		samplerate;
	int		channels;
};



/***************************************************************************
    GLOBAL VARIABLES
***************************************************************************/

static UINT8 *chdbuffer;
static UINT8 chdinterlaced;

static int video_first_whitefield = -1;
static int video_saw_leadin = FALSE;
static int video_saw_leadout = FALSE;
static int video_last_frame = -1;
static int video_last_chapter = -1;
static int video_cadence = -1;
static UINT32 video_cadence_history = 0;
static int video_prev_whitefield = -1;

static int audio_min_lsample = 32767;
static int audio_min_rsample = 32767;
static int audio_max_lsample = -32768;
static int audio_max_rsample = -32768;
static int audio_min_lsample_count = 0;
static int audio_min_rsample_count = 0;
static int audio_max_lsample_count = 0;
static int audio_max_rsample_count = 0;
static int audio_sample_count = 0;



/***************************************************************************
    AVI HANDLING
***************************************************************************/

/*-------------------------------------------------
    open_avi - open an AVI file and return
    information about it
-------------------------------------------------*/

static void *open_avi(const char *filename, movie_info *info)
{
	const avi_movie_info *aviinfo;
	avi_error avierr;
	avi_file *avi;

	/* open the file */
	avierr = avi_open(filename, &avi);
	if (avierr != AVIERR_NONE)
	{
		fprintf(stderr, "Error opening AVI file: %s\n", avi_error_string(avierr));
		return NULL;
	}

	/* extract movie info */
	aviinfo = avi_get_movie_info(avi);
	info->framerate = (double)aviinfo->video_timescale / (double)aviinfo->video_sampletime;
	info->numframes = aviinfo->video_numsamples;
	info->width = aviinfo->video_width;
	info->height = aviinfo->video_height;
	info->samplerate = aviinfo->audio_samplerate;
	info->channels = aviinfo->audio_channels;
	
	return avi;
}


/*-------------------------------------------------
    read_avi - read a frame from an AVI file
-------------------------------------------------*/

static int read_avi(void *file, int frame, bitmap_t *bitmap, INT16 *lsound, INT16 *rsound, int *samples)
{
	const avi_movie_info *aviinfo = avi_get_movie_info(file);
	UINT32 firstsample = ((UINT64)aviinfo->audio_samplerate * (UINT64)frame * (UINT64)aviinfo->video_sampletime + aviinfo->video_timescale - 1) / (UINT64)aviinfo->video_timescale;
	UINT32 lastsample = ((UINT64)aviinfo->audio_samplerate * (UINT64)(frame + 1) * (UINT64)aviinfo->video_sampletime + aviinfo->video_timescale - 1) / (UINT64)aviinfo->video_timescale;
	avi_error avierr;
	
	/* read the frame */
	avierr = avi_read_video_frame_yuy16(file, frame, bitmap);
	if (avierr != AVIERR_NONE)
		return FALSE;

	/* read the samples */
	avierr = avi_read_sound_samples(file, 0, firstsample, lastsample - firstsample, lsound);
	avierr = avi_read_sound_samples(file, 1, firstsample, lastsample - firstsample, rsound);
	if (avierr != AVIERR_NONE)
		return FALSE;
	*samples = lastsample - firstsample;
	return TRUE;
}


/*-------------------------------------------------
    close_avi - close an AVI file
-------------------------------------------------*/

static void close_avi(void *file)
{
	avi_close(file);
}



/***************************************************************************
    CHD HANDLING
***************************************************************************/

/*-------------------------------------------------
    open_chd - open a CHD file and return
    information about it
-------------------------------------------------*/

static void *open_chd(const char *filename, movie_info *info)
{
	int fps, fpsfrac, width, height, interlaced, channels, rate;
	char metadata[256];
	chd_error chderr;
	chd_file *chd;

	/* open the file */
	chderr = chd_open(filename, CHD_OPEN_READ, NULL, &chd);
	if (chderr != CHDERR_NONE)
	{
		fprintf(stderr, "Error opening CHD file: %s\n", chd_error_string(chderr));
		return NULL;
	}

	/* get the metadata */
	chderr = chd_get_metadata(chd, AV_METADATA_TAG, 0, metadata, sizeof(metadata), NULL, NULL);
	if (chderr != CHDERR_NONE)
	{
		fprintf(stderr, "Error getting A/V metadata: %s\n", chd_error_string(chderr));
		chd_close(chd);
		return NULL;
	}

	/* extract the info */
	if (sscanf(metadata, AV_METADATA_FORMAT, &fps, &fpsfrac, &width, &height, &interlaced, &channels, &rate) != 7)
	{
		fprintf(stderr, "Improperly formatted metadata\n");
		chd_close(chd);
		return NULL;
	}

	/* extract movie info */
	info->framerate = (fps * 1000000 + fpsfrac) / 1000000.0;
	info->numframes = chd_get_header(chd)->totalhunks;
	info->width = width;
	info->height = height;
	info->samplerate = rate;
	info->channels = channels;

	/* convert to an interlaced frame */
	chdinterlaced = interlaced;
	if (interlaced)
	{
		info->framerate /= 2;
		info->numframes = (info->numframes + 1) / 2;
		info->height *= 2;
	}
	
	/* allocate a buffer */
	chdbuffer = malloc(chd_get_header(chd)->hunkbytes);
	if (chdbuffer == NULL)
	{
		fprintf(stderr, "Out of memory allocating chd buffer\n");
		chd_close(chd);
		return NULL;
	}
	
	return chd;
}


/*-------------------------------------------------
    read_chd - read a frame from a CHD file
-------------------------------------------------*/

static int read_chd(void *file, int frame, bitmap_t *bitmap, INT16 *lsound, INT16 *rsound, int *samples)
{
	int interlace_factor = chdinterlaced ? 2 : 1;
	int fieldnum, chnum, sampnum, x, y;
	int channels, width, height;
	chd_error chderr;
	UINT8 *source;

	/* loop over fields */
	*samples = 0;
	for (fieldnum = 0; fieldnum < interlace_factor; fieldnum++)
	{
		int startsamples = *samples;

		/* read the frame */
		chderr = chd_read(file, frame * interlace_factor + fieldnum, chdbuffer);
		if (chderr != CHDERR_NONE)
			return FALSE;

		/* parse out the info */
		channels = chdbuffer[5];
		*samples += (chdbuffer[6] << 8) | chdbuffer[7];
		width = (chdbuffer[8] << 8) | chdbuffer[9];
		height = ((chdbuffer[10] << 8) | chdbuffer[11]) & 0x7fff;
		source = chdbuffer + 12 + chdbuffer[4];
		
		/* make sure the data makes sense */
		if (width != bitmap->width || height != bitmap->height / interlace_factor)
		{
			fprintf(stderr, "Inconsistent frame width/height!\n");
			return FALSE;
		}

		/* copy in sample data */
		for (chnum = 0; chnum < channels; chnum++)
		{
			INT16 *dest = (chnum == 0) ? lsound : rsound;
			for (sampnum = startsamples; sampnum < *samples; sampnum++)
			{
				INT16 sample = *source++ << 8;
				dest[sampnum] = sample | *source++;
			}
		}
		
		/* copy in the bitmap data */
		for (y = fieldnum; y < height * interlace_factor; y += interlace_factor)
		{
			UINT16 *dest = BITMAP_ADDR16(bitmap, y, 0);
			for (x = 0; x < width; x++)
			{
				UINT16 pixel = *source++;
				*dest++ = pixel | (*source++ << 8);
			}
		}
	}
	return TRUE;
}


/*-------------------------------------------------
    close_chd - close a CHD file
-------------------------------------------------*/

static void close_chd(void *file)
{
	if (chdbuffer != NULL)
		free(chdbuffer);
	chd_close(file);
}



/***************************************************************************
    CORE IMPLEMENTATION
***************************************************************************/

/*-------------------------------------------------
    verify_video - verify video frame
-------------------------------------------------*/

static void verify_video(int frame, bitmap_t *bitmap)
{
	const int fields_per_frame = 2;
	int fieldnum;
	
	/* loop over fields */
	for (fieldnum = 0; fieldnum < fields_per_frame; fieldnum++)
	{
		int field = frame * fields_per_frame + fieldnum;
		vbi_metadata metadata;
		
		/* output status */
		if (frame % 10 == 0 && fieldnum == 0)
			printf("%6d.%d...\r", frame, fieldnum);
		
		/* parse the VBI data */
		vbi_parse_all(BITMAP_ADDR16(bitmap, fieldnum, 0), bitmap->rowpixels * 2, bitmap->width, 8, &metadata);
		
		/* if we have data in both 17 and 18, it should match */
		if (metadata.line17 != 0 && metadata.line18 != 0 && metadata.line17 != metadata.line18)
		{
			printf("%6d.%d: line 17 and 18 data does not match (17=%06X 18=%06X) (WARNING)\n", frame, fieldnum, metadata.line17, metadata.line18);
			printf("%6d.%d: selected %06X based on bit confidence\n", frame, fieldnum, metadata.line1718);
		}
		
		/* is this a lead-in code? */
		if (metadata.line1718 == 0x88ffff)
		{
			/* if we haven't seen lead-in yet, detect it */
			if (!video_saw_leadin)
			{
				video_saw_leadin = TRUE;
				printf("%6d.%d: lead-in code detected\n", frame, fieldnum);
			}
			
			/* if we've previously seen chapters/frames, that's weird */
			if (video_last_frame != -1 || video_last_chapter != -1)
				printf("%6d.%d: lead-in code detected after frame/chapter data (WARNING)\n", frame, fieldnum);
		}
		
		/* is this a lead-out code? */
		if (metadata.line1718 == 0x80eeee)
		{
			/* if we haven't seen lead-in yet, detect it */
			if (!video_saw_leadout)
			{
				video_saw_leadout = TRUE;
				printf("%6d.%d: lead-out code detected\n", frame, fieldnum);
				if (video_last_frame != -1)
					printf("%6d.%d: final frame number was %d\n", frame, fieldnum, video_last_frame);
				else
					printf("%6d.%d: never detected any frame numbers (ERROR)\n", frame, fieldnum);
			}
			
			/* if we've previously seen chapters/frames, that's weird */
			if (video_last_frame == -1)
				printf("%6d.%d: lead-out code detected with no frames detected beforehand (WARNING)\n", frame, fieldnum);
		}
		
		/* is this a frame code? */
		if ((metadata.line1718 & 0xf80000) == 0xf80000)
		{
			int framenum = ((metadata.line1718 >> 0) & 15) * 1 + ((metadata.line1718 >> 4) & 15) * 10 + ((metadata.line1718 >> 8) & 15) * 100 + ((metadata.line1718 >> 12) & 15) * 1000 + ((metadata.line1718 >> 16) & 7) * 10000;
			
			/* did we see any leadin? */
			if (!video_saw_leadin)
			{
   				printf("%6d.%d: detected frame number but never saw any lead-in (WARNING)\n", frame, fieldnum);
   				video_saw_leadin = TRUE;
   			}
			
			/* if this is the first frame, make sure it's 1 */
			if (video_last_frame == -1)
			{
			    if (framenum == 1)
    				printf("%6d.%d: detected frame 1\n", frame, fieldnum);
			    else
    				printf("%6d.%d: first frame number is not 1 (%d) (ERROR)\n", frame, fieldnum, framenum);
    	    }

			/* print an update every 1000 frames */
			if (framenum % 1000 == 0)
   				printf("%6d.%d: detected frame %d\n", frame, fieldnum, framenum);
			
			/* if this frame is not consecutive, it's an error */
			if (video_last_frame != -1 && framenum != video_last_frame + 1)
				printf("%6d.%d: gap in frame number sequence (%d->%d) (ERROR)\n", frame, fieldnum, video_last_frame, framenum);
				
			/* remember the frame number */
			video_last_frame = framenum;
		}
		
		/* is the whiteflag set? */
		if (metadata.white)
		{
    		/* if this is the first white flag we see, count it */
    		if (video_first_whitefield == -1)
    		{
    			video_first_whitefield = field;
    			printf("%6d.%d: first white flag seen\n", frame, fieldnum);
    		}
    		
    		/* if we've seen frames, but we're not yet to the lead-out, check the cadence */
    		if (video_last_frame != -1 && !video_saw_leadout)
    		{
    		    /* make sure we have a proper history */
    		    if (video_prev_whitefield != -1)
    		        video_cadence_history = (video_cadence_history << 4) | ((field - video_prev_whitefield) & 0x0f);
    		    video_prev_whitefield = field;
    		    
    		    /* if we don't know our cadence yet, determine it */
    		    if (video_cadence == -1 && (video_cadence_history & 0xf00) != 0)
    		    {
    		        if ((video_cadence_history & 0xfff) == 0x222)
    		        {
    		            printf("%6d.%d: detected 2:2 cadence\n", frame, fieldnum);
    		            video_cadence = 4;
    		        }
    		        else if ((video_cadence_history & 0xfff) == 0x323)
    		        {
    		            printf("%6d.%d: detected 3:2 cadence\n", frame, fieldnum);
    		            video_cadence = 5;
    		        }
    		        else if ((video_cadence_history & 0xfff) == 0x232)
    		        {
    		            printf("%6d.%d: detected 2:3 cadence\n", frame, fieldnum);
    		            video_cadence = 5;
    		        }
    		        else
    		        {
    		            printf("%6d.%d: unknown cadence (history %d:%d:%d) (WARNING)\n", frame, fieldnum, 
    		                    (video_cadence_history >> 8) & 15, (video_cadence_history >> 4) & 15, video_cadence_history & 15);
    		        }
    		    }
    		    
    		    /* if we know our cadence, make sure we stick to it */
    		    if (video_cadence != -1)
    		    {
    		        if (video_cadence == 4 && (video_cadence_history & 0xfff) != 0x222)
    		        {
    		            printf("%6d.%d: missed cadence (history %d:%d:%d) (WARNING)\n", frame, fieldnum, 
    		                    (video_cadence_history >> 8) & 15, (video_cadence_history >> 4) & 15, video_cadence_history & 15);
    		            video_cadence = -1;
    		            video_cadence_history = 0;
    		        }
    		        else if (video_cadence == 5 && (video_cadence_history & 0xfff) != 0x323 && (video_cadence_history & 0xfff) != 0x232)
    		        {
    		            printf("%6d.%d: missed cadence (history %d:%d:%d) (WARNING)\n", frame, fieldnum, 
    		                    (video_cadence_history >> 8) & 15, (video_cadence_history >> 4) & 15, video_cadence_history & 15);
    		            video_cadence = -1;
    		            video_cadence_history = 0;
    		        }
    		    }
    		}
        }
	}
}


/*-------------------------------------------------
    verify_video_final - final verification
-------------------------------------------------*/

static void verify_video_final(int frame, bitmap_t *bitmap)
{
	int fields_per_frame = (bitmap->height >= 288) ? 2 : 1;
	int field = frame * fields_per_frame;

    /* did we ever see any lead-out? */
	if (video_saw_leadin && !video_saw_leadout)
		printf("Track %6d.%d: detected lead-in but never saw any lead-out (WARNING)\n", field / fields_per_frame, 0);
}
	

/*-------------------------------------------------
    verify_audio - verify audio data
-------------------------------------------------*/

static void verify_audio(const INT16 *lsound, const INT16 *rsound, int samples)
{
	int sampnum;
	
	/* count the overall samples */
	audio_sample_count += samples;
	
	/* iterate over samples, tracking min/max */
	for (sampnum = 0; sampnum < samples; sampnum++)
	{
	    /* did we hit a minimum on the left? */
	    if (lsound[sampnum] < audio_min_lsample)
	    {
	        audio_min_lsample = lsound[sampnum];
	        audio_min_lsample_count = 1;
	    }
	    else if (lsound[sampnum] == audio_min_lsample)
	        audio_min_lsample_count++;
	        
	    /* did we hit a maximum on the left? */
	    if (lsound[sampnum] > audio_max_lsample)
	    {
	        audio_max_lsample = lsound[sampnum];
	        audio_max_lsample_count = 1;
	    }
	    else if (lsound[sampnum] == audio_max_lsample)
	        audio_max_lsample_count++;

	    /* did we hit a minimum on the right? */
	    if (rsound[sampnum] < audio_min_rsample)
	    {
	        audio_min_rsample = rsound[sampnum];
	        audio_min_rsample_count = 1;
	    }
	    else if (rsound[sampnum] == audio_min_rsample)
	        audio_min_rsample_count++;
	        
	    /* did we hit a maximum on the right? */
	    if (rsound[sampnum] > audio_max_rsample)
	    {
	        audio_max_rsample = rsound[sampnum];
	        audio_max_rsample_count = 1;
	    }
	    else if (rsound[sampnum] == audio_max_rsample)
	        audio_max_rsample_count++;
	}
}


/*-------------------------------------------------
    verify_audio_final - final verification
-------------------------------------------------*/

static void verify_audio_final(void)
{
    printf("Audio summary:\n");
    printf("  Channel 0 minimum: %6d (with %9d/%9d samples at minimum)\n", audio_min_lsample, audio_min_lsample_count, audio_sample_count);
    printf("  Channel 0 maximum: %6d (with %9d/%9d samples at maximum)\n", audio_max_lsample, audio_max_lsample_count, audio_sample_count);
    printf("  Channel 1 minimum: %6d (with %9d/%9d samples at minimum)\n", audio_min_rsample, audio_min_rsample_count, audio_sample_count);
    printf("  Channel 1 maximum: %6d (with %9d/%9d samples at maximum)\n", audio_max_rsample, audio_max_rsample_count, audio_sample_count);
}
	

/*-------------------------------------------------
    usage - display program usage
-------------------------------------------------*/

static int usage(void)
{
	fprintf(stderr, "Usage: \n");
	fprintf(stderr, "  ldverify [avifile.avi|chdfile.chd]\n");
	return 1;
}


/*-------------------------------------------------
    main - main entry point
-------------------------------------------------*/

int main(int argc, char *argv[])
{
	movie_info info = { 0 };
	INT16 *lsound, *rsound;
	const char *srcfile;
	bitmap_t *bitmap;
	int srcfilelen;
	int samples = 0;
	void *file;
	int isavi;
	int frame;
	
	/* verify arguments */
	if (argc < 2)
		return usage();
	srcfile = argv[1];
	
	/* check extension of file */
	srcfilelen = strlen(srcfile);
	if (srcfilelen < 4)
		return usage();
	if (tolower(srcfile[srcfilelen-3]) == 'a' && tolower(srcfile[srcfilelen-2]) == 'v' && tolower(srcfile[srcfilelen-1]) == 'i')
		isavi = TRUE;
	else if (tolower(srcfile[srcfilelen-3]) == 'c' && tolower(srcfile[srcfilelen-2]) == 'h' && tolower(srcfile[srcfilelen-1]) == 'd')
		isavi = FALSE;
	else
		return usage();
	
	/* open the file */
	printf("Processing file: %s\n", srcfile);
	file = isavi ? open_avi(srcfile, &info) : open_chd(srcfile, &info);
	if (file == NULL)
	{
		fprintf(stderr, "Unable to open file '%s'\n", srcfile);
		return 1;
	}
	
	/* comment on the video dimensions */
	printf("Video dimensions: %dx%d\n", info.width, info.height);
	if (info.width != 720)
		printf("WARNING: Unexpected video width (should be 720)\n");
	if (info.height != 524)
		printf("WARNING: Unexpected video height (should be 262 or 524)\n");

	/* comment on the video frame rate */
	printf("Video frame rate: %.2fHz\n", info.framerate);
	if ((int)(info.framerate * 100.0 + 0.5) != 2997)
		printf("WARNING: Unexpected frame rate (should be 29.97Hz)\n");

	/* comment on the sample rate */
	printf("Sample rate: %dHz\n", info.samplerate);
	if (info.samplerate != 48000)
		printf("WARNING: Unexpected sampele rate (should be 48000Hz)\n");
	
	/* allocate a bitmap */
	bitmap = bitmap_alloc(info.width, info.height, BITMAP_FORMAT_YUY16);
	if (bitmap == NULL)
	{
		fprintf(stderr, "Out of memory creating %dx%d bitmap\n", info.width, info.height);
		return 1;
	}
	
	/* allocate sound buffers */
	lsound = malloc(info.samplerate * sizeof(*lsound));
	rsound = malloc(info.samplerate * sizeof(*rsound));
	if (lsound == NULL || rsound == NULL)
	{
		fprintf(stderr, "Out of memory allocating sound buffers of %d bytes\n", info.samplerate * sizeof(*rsound));
		return 1;
	}
	
	/* loop over frames */
	frame = 0;
	while (isavi ? read_avi(file, frame, bitmap, lsound, rsound, &samples) : read_chd(file, frame, bitmap, lsound, rsound, &samples))
	{
		verify_video(frame, bitmap);
		verify_audio(lsound, rsound, samples);
		frame++;
	}
	
	/* close the files */
	isavi ? close_avi(file) : close_chd(file);
	
	/* final output */
	verify_video_final(frame, bitmap);
	verify_audio_final();
	
	/* free memory */
	bitmap_free(bitmap);
	free(lsound);
	free(rsound);

	return 0;
}
