// Tensor VM - vmdisk/vmfloppy.c

#include "../log.h"
#include "vmfloppy.h"

#include <stdlib.h>

vmfloppy insert_flp(char *img_path, uint8_t drive_id)
{
	vmfloppy image;

	if ((image = (vmfloppy)malloc(sizeof(floppy))) == NULL)
	{
		Log(ERROR, "Failed to create VM floppy device.");
		return NULL;
	}

	image->img_record		= NULL;
	image->img_path			= img_path;
	image->img_size			= 1474560;
	image->write_protected	= true;
	image->drive_id			= drive_id;

	Log(INFO, "Inserted VM floppy \"%s\" as %c:.", image->img_path, image->drive_id);
	return image;
}

int load_floppy(vmfloppy image)
{
	
	if ((image->img_record = fopen(image->img_path, "rb")) == NULL)
	{
		Log(ERROR, "Failed to load VM floppy: \"%s\".", image->img_path);
		free(image);
		return EXIT_FAILURE;
	}

	Log(INFO, "Loaded VM floppy \"%s\" as %c:.", image->img_path, image->drive_id);
	return 0;
}

void destroy_img(vmfloppy image, bool bad_path)
{
	Log(INFO, "Running VM floppy destroy.");
	if (!bad_path)	fclose(image->img_record);
	Log(INFO, "Destroyed VM image: %c:.", image->drive_id);
	free(image);
}
