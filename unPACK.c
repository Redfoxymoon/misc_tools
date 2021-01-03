#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifdef _WIN32
#define le32toh(x) x
#else
#include <endian.h>
#endif

/* header magic: 0x50 0x41 0x43 0x4b 0x?? 0x?? 0x?? 0x?? */
/* file table... */
/* starting offset: 32-bit integer */
/* length: 32-bit integer */
/* filename length: 1 byte */
/* filename: 14 bytes + null byte, OR 6 bytes + null byte ??? */

struct file_entry
{
	uint32_t offset;
	uint32_t length;
	uint32_t namelength;
	char filename[15];
};

int main(int argc, char *argv[])
{
	char buf[256];
	const char *magic = "PACK";
	FILE *in_file, *out_file;
	struct file_entry entry;
	uint32_t i, offset;

	if(argc < 2)
		exit(1);

	in_file = fopen(argv[1], "rb");

	fread(buf, 1, 8, in_file);

	if(memcmp(magic, buf, 4)) {
		puts("not a PACK archive");
		exit(1);
	}

	while(1) {
		fread(buf, 1, 24, in_file);

		entry.offset = le32toh(*(uint32_t*)&buf[0]);
		entry.length = le32toh(*(uint32_t*)&buf[4]);
		entry.namelength = buf[8];

		/* read filename */
		memcpy(entry.filename, buf + 9, entry.namelength);
		entry.filename[entry.namelength] = '\0';

		/* special exception. ??? */
		if(entry.namelength == 6)
			fseek(in_file, -8, SEEK_CUR);

		/* null entry? end of table */
		if(entry.offset == 0 && entry.length == 0)
			break;

		if(buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00 && buf[4] == 0x00 && \
				buf[5] == 0x00 && buf[6] == 0x00 && buf[7] == 0x00)
			break;

#if 1
		printf("filename: %s\n", entry.filename);
		printf("length: %u\n", entry.length);
#endif

		/* save current position */
		offset = ftell(in_file);

		out_file = fopen(entry.filename, "wb");

		fseek(in_file, entry.offset, SEEK_SET);
		/* write file to disk */
		for(i = 0; i < entry.length; i++)
			(void)fputc(fgetc(in_file), out_file);

		/* restore stream position */
		fseek(in_file, offset, SEEK_SET);
	}

	return 0;
}
