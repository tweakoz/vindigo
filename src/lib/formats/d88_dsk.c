/*
 *   D77 and D88 disk images
 *
 *
 *   Header (total size = 0x2b0 bytes):
 *     0x00 - Disk label
 *     0x1a - Write protect (bit 3)
 *     0x1b - 2D/2DD format (bit 3)
 *     0x1c-1f - image size (should match file size)
 *     0x20 - offsets for each track (max 164)
 *
 *   Sectors (0x110 bytes each, typically)
 *     0x00 - Sector info
 *          byte 0 - track number
 *          byte 1 - side (0 or 1)
 *          byte 2 - sector number
 *     0x10 - sector data
 *
 *   Images can be concatenated together.
 *   Sectors can be in any order.
 *   Tracks are in the order:
 *          Track 0 side 0
 *          Track 0 side 1
 *          Track 1 side 0
 *          ...
 *
 *
 */

#include "flopimg.h"
#include "imageutl.h"

#define D88_HEADER_LEN 0x2b0

struct d88_tag
{
	UINT32 image_size;
	UINT32 trackoffset[164];
	UINT8 write_protect;
	UINT8 disk_type;
	UINT8 heads;
};

static struct d88_tag *get_d88_tag(floppy_image_legacy *floppy)
{
	return (d88_tag *)floppy_tag(floppy);
}

static int d88_get_sector_id(floppy_image_legacy *floppy, int head, int track, int sector_index)
{
	struct d88_tag* tag = get_d88_tag(floppy);
	UINT32 offset;
	UINT8 sector_hdr[16];
	int x;

	offset = tag->trackoffset[(track*tag->heads)+head];

	if(offset == 0)
		return 0;

	floppy_image_read(floppy,sector_hdr,offset,16);

	// get to sector indexed
	x=0;
	while(x<sector_index)
	{
		offset += ((sector_hdr[15] << 8) | sector_hdr[14]);
		offset += 16;
		floppy_image_read(floppy,sector_hdr,offset,16);
		x++;
	}

	return sector_hdr[2];
}

static int d88_get_tracks_per_disk(floppy_image_legacy *floppy)
{
	return 82;  // 82 tracks per side
}

static int d88_get_heads_per_disk(floppy_image_legacy *floppy)
{
	struct d88_tag* tag = get_d88_tag(floppy);
	return tag->heads;
}

static int d88_get_sectors_per_track(floppy_image_legacy *floppy, int head, int track)
{
	struct d88_tag* tag = get_d88_tag(floppy);
	UINT32 offset;
	UINT8 sector_hdr[16];

	offset = tag->trackoffset[(track*tag->heads)+head];

	floppy_image_read(floppy,sector_hdr,offset,16);

	return sector_hdr[4];
}

static floperr_t d88_get_sector_length(floppy_image_legacy *floppy, int head, int track, int sector, UINT32 *sector_length)
{
	struct d88_tag* tag = get_d88_tag(floppy);
	UINT32 offset;
	UINT8 sector_hdr[16];
	UINT32 len;
	int count,secs;

	offset = tag->trackoffset[(track*tag->heads)+head];

	floppy_image_read(floppy,sector_hdr,offset,16);
	secs = sector_hdr[4];

	for(count=0;count<secs;count++)
	{
		floppy_image_read(floppy,sector_hdr,offset,16);
		if(sector == sector_hdr[2])
		{
			if(sector_length)
				*sector_length = (sector_hdr[15] << 8) | sector_hdr[14];
			return FLOPPY_ERROR_SUCCESS;
		}
		len = (sector_hdr[15] << 8) | sector_hdr[14];
		len += 16;
		offset += len;
	}

	return FLOPPY_ERROR_SEEKERROR;
}

static floperr_t d88_read_track(floppy_image_legacy *floppy, int head, int track, UINT64 offset, void *buffer, size_t buflen)
{
//  floperr_t err;

	return FLOPPY_ERROR_UNSUPPORTED;
}

static UINT32 d88_get_sector_offset(floppy_image_legacy* floppy, int head, int track, int sector)
{
	struct d88_tag* tag = get_d88_tag(floppy);
	UINT32 offset = 0;
	UINT8 sector_hdr[16];
	UINT32 len;
	UINT32 secs;
	int count;

	// get offset of the beginning of the track
	offset = tag->trackoffset[(track*tag->heads)+head];

	floppy_image_read(floppy,sector_hdr,offset,16);
	secs = sector_hdr[4];

	for(count=0;count<secs;count++)
	{
		floppy_image_read(floppy,sector_hdr,offset,16);
		if(sector == sector_hdr[2])
		{
			LOG_FORMATS("d88_get_sector_offset - track %i, side %i, sector %02x, returns %08x\n",track,head,sector,offset+16);
			return offset + 16;
		}
		len = (sector_hdr[15] << 8) | sector_hdr[14];
		len += 16;
		offset += len;
	}
	LOG_FORMATS("d88_get_sector_offset - track %i, side %i, sector %02x, not found\n",track,head,sector);
	return 0;
}

static floperr_t d88_get_indexed_sector_info(floppy_image_legacy *floppy, int head, int track, int sector_index, int *cylinder, int *side, int *sector, UINT32 *sector_length, unsigned long *flags)
{
	struct d88_tag* tag = get_d88_tag(floppy);
	UINT32 offset;
	UINT8 sector_hdr[16];
	int x;

	offset = tag->trackoffset[(track*tag->heads)+head];

	if(offset == 0)
		return FLOPPY_ERROR_SEEKERROR;


	floppy_image_read(floppy,sector_hdr,offset,16);

	if(sector_index >= sector_hdr[4])
		return FLOPPY_ERROR_SEEKERROR;

	// get to sector indexed
	x=0;
	while(x<sector_index)
	{
		offset += ((sector_hdr[15] << 8) | sector_hdr[14]);
		offset += 16;
		floppy_image_read(floppy,sector_hdr,offset,16);
		x++;
	}

	if(offset > tag->image_size || offset == 0)
		return FLOPPY_ERROR_SEEKERROR;

	if(sector_length)
		*sector_length = (sector_hdr[15] << 8) | sector_hdr[14];
	if(cylinder)
		*cylinder = sector_hdr[0];
	if(side)
		*side = sector_hdr[1];
	if(sector)
		*sector = sector_hdr[2];
	if(flags)
		*flags = 0;

	return FLOPPY_ERROR_SUCCESS;
}

static floperr_t d88_read_sector(floppy_image_legacy *floppy, int head, int track, int sector, void *buffer, size_t buflen)
{
	UINT64 offset;
	UINT32 sector_length;

	offset = d88_get_sector_offset(floppy,head,track,sector);

	if(d88_get_sector_length(floppy,head,track,sector,&sector_length) != FLOPPY_ERROR_SUCCESS)
		return FLOPPY_ERROR_SEEKERROR;

	if(offset == 0)
		return FLOPPY_ERROR_SEEKERROR;

	if(buflen > sector_length)
		return FLOPPY_ERROR_INTERNAL;

	floppy_image_read(floppy,buffer,offset,sector_length);

	return FLOPPY_ERROR_SUCCESS;
}

static floperr_t d88_read_indexed_sector(floppy_image_legacy *floppy, int head, int track, int sector, void *buffer, size_t buffer_len)
{
	int sec;

	sec = d88_get_sector_id(floppy,head,track,sector);
	return d88_read_sector(floppy,head,track,sec,buffer,buffer_len);
}

static floperr_t d88_write_sector(floppy_image_legacy *floppy, int head, int track, int sector, const void *buffer, size_t buflen, int ddam)
{
	UINT64 offset;
	UINT32 sector_length;

	offset = d88_get_sector_offset(floppy,head,track,sector);

	if(d88_get_sector_length(floppy,head,track,sector,&sector_length) != FLOPPY_ERROR_SUCCESS)
		return FLOPPY_ERROR_SEEKERROR;

	if(offset == 0)
		return FLOPPY_ERROR_SEEKERROR;

	if(buflen > sector_length)
		return FLOPPY_ERROR_INTERNAL;

	floppy_image_write(floppy,buffer,offset,sector_length);

	return FLOPPY_ERROR_SUCCESS;
}

static floperr_t d88_write_indexed_sector(floppy_image_legacy *floppy, int head, int track, int sector, const void *buffer, size_t buflen, int ddam)
{
	int sec;

	sec = d88_get_sector_id(floppy,head,track,sector);
	return d88_write_sector(floppy,head,track,sec,buffer,buflen,ddam);
}

static void d88_get_header(floppy_image_legacy* floppy,UINT32* size, UINT8* prot, UINT8* type, UINT32* offsets)
{
	UINT8 header[D88_HEADER_LEN];
	int x,s;

	floppy_image_read(floppy,header,0,D88_HEADER_LEN);

#ifdef SPOT_DUPLICATES
		// there exist many .d88 files with same data and different headers and
		// this allows to spot duplicates, making easier to debug softlists.
		UINT32 temp_size = floppy_image_size(floppy);
		UINT8 tmp_copy[temp_size - D88_HEADER_LEN];
		floppy_image_read(floppy,tmp_copy,D88_HEADER_LEN,temp_size - D88_HEADER_LEN);
		printf("CRC16: %d\n", ccitt_crc16(0xffff, tmp_copy, temp_size - D88_HEADER_LEN));
#endif

	if(prot)
		*prot = header[0x1a];
	if(type)
		*type = header[0x1b];
	if(size)
	{
		s = 0;
		s |= header[0x1f] << 24;
		s |= header[0x1e] << 16;
		s |= header[0x1d] << 8;
		s |= header[0x1c];
		*size = s;
	}
	if(offsets)
	{
		for(x=0;x<164;x++)
		{
			s = 0;
			s |= header[0x23 + (x*4)] << 24;
			s |= header[0x22 + (x*4)] << 16;
			s |= header[0x21 + (x*4)] << 8;
			s |= header[0x20 + (x*4)];
			*(offsets+x) = s;
		}
	}
}

FLOPPY_IDENTIFY(d88_dsk_identify)
{
	UINT32 size;

	d88_get_header(floppy,&size,NULL,NULL,NULL);

	if(floppy_image_size(floppy) == size)
	{
		*vote = 100;
	}
	else
	{
		*vote = 0;
	}
	return FLOPPY_ERROR_SUCCESS;
}

FLOPPY_CONSTRUCT(d88_dsk_construct)
{
	struct FloppyCallbacks *callbacks;
	struct d88_tag *tag;
	UINT32 size;
	UINT8 prot,type = 0;
	UINT32 offs[164];
	int x;

	if(params)
	{
		// create
		return FLOPPY_ERROR_UNSUPPORTED;
	}
	else
	{
		// load
		d88_get_header(floppy,&size,&prot,&type,offs);
	}

	tag = (d88_tag *)floppy_create_tag(floppy,sizeof(struct d88_tag));
	if (!tag)
		return FLOPPY_ERROR_OUTOFMEMORY;

	tag->write_protect = prot;
	tag->disk_type = type;
	tag->heads = 2;
	if (tag->disk_type==0x30 || tag->disk_type==0x40) tag->heads = 1;

	tag->image_size = size;
	for(x=0;x<164;x++)
		tag->trackoffset[x] = offs[x];

	callbacks = floppy_callbacks(floppy);
	callbacks->read_track = d88_read_track;
	callbacks->get_heads_per_disk = d88_get_heads_per_disk;
	callbacks->get_tracks_per_disk = d88_get_tracks_per_disk;
	callbacks->get_sector_length = d88_get_sector_length;
	callbacks->read_sector = d88_read_sector;
	callbacks->read_indexed_sector = d88_read_indexed_sector;
	callbacks->write_sector = d88_write_sector;
	callbacks->write_indexed_sector = d88_write_indexed_sector;
	callbacks->get_indexed_sector_info = d88_get_indexed_sector_info;
	callbacks->get_sectors_per_track = d88_get_sectors_per_track;


	return FLOPPY_ERROR_SUCCESS;
}



/***************************************************************************

    Copyright Olivier Galibert
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be
          used to endorse or promote products derived from this software
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/

/*********************************************************************

    formats/d88_dsk.h

    D88 disk images

*********************************************************************/

#include "emu.h"
#include "d88_dsk.h"

d88_format::d88_format()
{
}

const char *d88_format::name() const
{
	return "d88";
}

const char *d88_format::description() const
{
	return "D88 disk image";
}

const char *d88_format::extensions() const
{
	return "d77,d88,1dd";
}

int d88_format::identify(io_generic *io, UINT32 form_factor)
{
	int size = io_generic_size(io);
	UINT8 h[32];

	io_generic_read(io, h, 0, 32);
	if((LITTLE_ENDIANIZE_INT32(*(UINT32 *)(h+0x1c)) == size) &&
	   (h[0x1b] == 0x00 || h[0x1b] == 0x10 || h[0x1b] == 0x20 || h[0x1b] == 0x30 || h[0x1b] == 0x40))
		return 100;

	return 0;
}

bool d88_format::load(io_generic *io, UINT32 form_factor, floppy_image *image)
{
	UINT8 h[32];

	io_generic_read(io, h, 0, 32);

	int cell_count = 0;
	int track_count = 0;
	int head_count = 0;
	switch(h[0x1b]) {
	case 0x00:
		cell_count = 100000;
		track_count = 42;
		head_count = 2;
		image->set_variant(floppy_image::DSDD);
		break;

	case 0x10:
		cell_count = 100000;
		track_count = 82;
		head_count = 2;
		image->set_variant(floppy_image::DSQD);
		break;

	case 0x20:
		cell_count = form_factor == floppy_image::FF_35 ? 200000 : 166666;
		track_count = 82;
		head_count = 2;
		image->set_variant(floppy_image::DSHD);
		break;

	case 0x30:
		cell_count = 100000;
		track_count = 42;
		head_count = 1;
		image->set_variant(floppy_image::SSDD);
		break;

	case 0x40:
		cell_count = 100000;
		track_count = 82;
		head_count = 1;
		image->set_variant(floppy_image::SSQD);
		break;
	}

	if(!head_count)
		return false;

	UINT32 track_pos[164];
	io_generic_read(io, track_pos, 32, 164*4);

	for(int track=0; track < track_count; track++)
		for(int head=0; head < head_count; head++) {
			int pos = LITTLE_ENDIANIZE_INT32(track_pos[track * head_count + head]);
			if(!pos)
				continue;

			UINT32 track_data[210000];
			UINT8 sect_data[65536];
			int tpos = 0;

			// gap 4a , IAM and gap 1
			for(int i=0; i<80; i++) mfm_w(track_data, tpos, 8, 0x4e);
			for(int i=0; i<12; i++) mfm_w(track_data, tpos, 8, 0x00);
			for(int i=0; i< 3; i++) raw_w(track_data, tpos, 16, 0x5224);
			mfm_w(track_data, tpos, 8, 0xfc);
			for(int i=0; i<50; i++) mfm_w(track_data, tpos, 8, 0x4e);

			// Updated after reading the first header
			int sector_count = 1;
			int gap3 = 84;
			for(int i=0; i<sector_count; i++) {
				UINT8 hs[16];
				io_generic_read(io, hs, pos, 16);
				UINT16 size = LITTLE_ENDIANIZE_INT16(*(UINT16 *)(hs+14));
				io_generic_read(io, sect_data, pos+16, size);
				pos += 16+size;

				if(i == 0) {
					sector_count = LITTLE_ENDIANIZE_INT16(*(UINT16 *)(hs+4));
					if(size < 512)
						gap3 = form_factor == floppy_image::FF_35 ? 54 : 50;
					else
						gap3 = form_factor == floppy_image::FF_35 ? 84 : 80;
				}

				int cpos;
				UINT16 crc;
				// sync and IDAM and gap 2
				for(int j=0; j<12; j++) mfm_w(track_data, tpos, 8, 0x00);
				cpos = tpos;
				for(int j=0; j< 3; j++) raw_w(track_data, tpos, 16, 0x4489);
				mfm_w(track_data, tpos, 8, 0xfe);
				mfm_w(track_data, tpos, 8, hs[0]);
				mfm_w(track_data, tpos, 8, hs[1]);
				mfm_w(track_data, tpos, 8, hs[2]);
				mfm_w(track_data, tpos, 8, hs[3]);
				crc = calc_crc_ccitt(track_data, cpos, tpos);
				mfm_w(track_data, tpos, 16, crc);
				for(int j=0; j<22; j++) mfm_w(track_data, tpos, 8, 0x4e);

				// sync, DAM, data and gap 3
				for(int j=0; j<12; j++) mfm_w(track_data, tpos, 8, 0x00);
				cpos = tpos;
				for(int j=0; j< 3; j++) raw_w(track_data, tpos, 16, 0x4489);
				mfm_w(track_data, tpos, 8, 0xfb);
				for(int j=0; j<size; j++) mfm_w(track_data, tpos, 8, sect_data[j]);
				crc = calc_crc_ccitt(track_data, cpos, tpos);
				mfm_w(track_data, tpos, 16, crc);
				for(int j=0; j<gap3; j++) mfm_w(track_data, tpos, 8, 0x4e);
			}

			// Gap 4b

			if(tpos > cell_count)
				throw emu_fatalerror("d88_format: Incorrect layout on track %d head %d, expected_size=%d, current_size=%d", track, head, cell_count, tpos);
			while(tpos < cell_count-15) mfm_w(track_data, tpos, 8, 0x4e);
			raw_w(track_data, tpos, cell_count-tpos, 0x9254 >> (16+tpos-cell_count));

			generate_track_from_levels(track, head, track_data, cell_count, 0, image);
		}

	return true;
}


bool d88_format::save(io_generic *io, floppy_image *image)
{
	return true;
}

bool d88_format::supports_save() const
{
	return true;
}

const floppy_format_type FLOPPY_D88_FORMAT = &floppy_image_format_creator<d88_format>;
