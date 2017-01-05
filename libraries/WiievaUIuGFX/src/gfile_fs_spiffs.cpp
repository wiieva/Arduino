
#include "gfx.h"

#if GFX_USE_GFILE && GFILE_NEED_USERFS

#include "src/gfile/gfile_fs.h"
#include "FS.h"

#include <string.h>


class SpiffsBufReader {
	const int READ_BUF_SIZE = 1024;
public:
	bool open (GFILE *f) {
		if (!init) {
			SPIFFS.begin ();
			init = true;
		}
		if (!f || !f->obj)
			return false;
		if (gf == f)
			return true;
		delete sf;
		sf = new File (SPIFFS.open ((const char *)(f->obj),"r"));
		gf = f;
		if (!buf_data)
			buf_data = new uint8_t [READ_BUF_SIZE];
		buf_size = 0;
		return true;
	}
	void close (GFILE *f) {
		if (gf == f) {
			delete sf;
			delete buf_data;
			sf = nullptr;
			gf = nullptr;
			buf_data = nullptr;
		}
		free (f->obj);
		f->obj = nullptr;
	} 
	int read (GFILE *f, void *buf, int size) {
		if (!open (f))
			return 0;
		uint8_t *pbuf = (uint8_t*)buf;
		int size_left = size;
		long int pos = f->pos;

		while (size_left) {

			int size_avail = std::min (size_left,(int)(buf_size + buf_pos - pos));
			if (pos >= buf_pos && pos < (buf_pos + buf_size) && size_avail > 0) {
				memcpy (pbuf,buf_data+pos-buf_pos,size_avail);
				pos += size_avail;
				pbuf += size_avail;
				size_left -= size_avail;
			} else {
				sf->seek (pos);
				buf_pos = pos;
				buf_size = sf->read (buf_data,READ_BUF_SIZE);
				if (buf_size <= 0) {
					buf_size = 0;
					break;
				}
			}
		}
		return size-size_left;
	}
	GFILE *gf = nullptr;
	File *sf = nullptr;
	uint8_t *buf_data = nullptr;
	long int buf_pos;
	int buf_size;
	bool init = false;
};

static SpiffsBufReader reader;

static bool_t USERExists(const char *fname) {
	return SPIFFS.exists (fname);
}

static long int	USERFilesize(const char *fname) {
	return SPIFFS.open (fname,"r").size ();
}

static bool_t USEROpen(GFILE *f, const char *fname) {
	int l = strlen (fname) + 2;
	f->obj = malloc (l);
	((char *)f->obj)[0] = '/';
	strcpy (((char *)f->obj) + (fname[0]=='/'?0:1),fname);
	f->pos = 0;
	return reader.open(f);
}

static void USERClose(GFILE *f) {
	reader.close (f);
}

static int USERRead(GFILE *f, void *buf, int size) {
	return reader.read (f,buf,size);
}

static bool_t USERSetpos(GFILE *f, long int pos) {
	f->pos = pos;
}

static long int USERGetsize(GFILE *f) {
	if (!reader.open (f))
		return 0;
	return reader.sf->size ();
}

static bool_t USEREof(GFILE *f) {
	if (!reader.open (f))
		return true;
	return f->pos >= reader.sf->size ();
}

extern "C" GFILEVMT FsUSERVMT = {
	GFSFLG_CASESENSITIVE|GFSFLG_SEEKABLE|GFSFLG_FAST,	// flags
	'S',												// prefix
	0, USERExists, USERFilesize, 0,
	USEROpen, USERClose, USERRead, 0,
	USERSetpos, USERGetsize, USEREof,
	0, 0, 0,
	#if GFILE_NEED_FILELISTS
		0, 0, 0
	#endif
};

#endif //GFX_USE_GFILE && GFILE_NEED_USERFS
