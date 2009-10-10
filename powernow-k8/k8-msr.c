/*	Tool for display fid/vid information for AMD CPU
 *
 *  (C) 2004 Bruno Ducrot <ducrot@poupinou.org>
 *  (C) 2007 Anton Repko <trcheton@gmail.com>
 *
 *  Licensed under the terms of the GNU GPL License version 2.
 *
 * Based on code found in
 * linux/arch/i386/kernel/cpu/cpufreq/powernow-k8.c
 * and originally developed by Paul Devriendt
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#define MCPU 32

#define MSR_FIDVID_CTL		0xc0010041
#define MSR_FIDVID_STATUS	0xc0010042

#define MSR_S_HI_CURRENT_VID	0x000000ff
#define MSR_S_LO_CURRENT_FID	0x000000ff

static int get_fidvid(uint32_t cpu, uint64_t *msr)
{
	int err = 1;
	*msr = 0;
	long fd;
	char file[20];

	if (cpu > MCPU)
		goto out;

	sprintf(file, "/dev/cpu/%d/msr", cpu);

	fd = open(file, O_RDONLY);
	if (fd < 0)
		goto out;
	lseek(fd, MSR_FIDVID_STATUS, SEEK_CUR);
	if (read(fd, msr, 8) != 8)
		goto err1;

	err = 0;
err1:
	close(fd);
out:
	return err;
}

static int set_fidvid(uint32_t cpu, uint32_t fid, uint32_t vid)
{
	int err = 1;
	uint64_t msr = 0;
	long fd;
	char file[20];

	if (cpu > MCPU)
		goto out;

	sprintf(file, "/dev/cpu/%d/msr", cpu);

	fd = open(file, O_WRONLY);
	if (fd < 0)
		goto out;
	lseek(fd, MSR_FIDVID_CTL, SEEK_CUR);
	msr = 0x3e800010000ull + ((uint64_t )vid * 256 + fid);
	if (write(fd, &msr, 8) != 8)
		goto err1;
	err = 0;
err1:
	close(fd);
out:
	return err;
}

/* Return a frequency in MHz, given an input fid */
static uint32_t find_freq_from_fid(uint32_t fid)
{
	return 800 + (fid * 100);
}

/* Return a voltage in miliVolts, given an input vid */
static uint32_t find_millivolts_from_vid(uint32_t vid)
{
	return 1550 - (vid * 25);
}

/* Return fid, given a frequency in MHz */
static uint32_t find_fid_from_freq(uint32_t freq)
{
	return freq / 100 - 8;
}

/* Return vid, given a voltage in miliVolts */
static uint32_t find_vid_from_millivolts(uint32_t mvolt)
{
	return (1550 - mvolt) / 25;
}

int main (int argc, char *argv[])
{
	int err;
	int cpu = 0;
	uint32_t fid, vid, freq, mvolt;
	uint64_t msr;
	
	if ((argc == 2) && (!strcmp (argv[1], "-i"))) {

		err = get_fidvid(cpu, &msr);
		if (err) {
			printf("can't get fid, vid from MSR\n");
			printf("Possible trouble: you don't run a powernow-k8 capable cpu\n");
			printf("or you are not root, or the msr driver is not present\n");
			exit(1);
		}
		fid = ((uint32_t )(msr & 0xffffffffull)) & MSR_S_LO_CURRENT_FID;
		vid = ((uint32_t )(msr>>32 & 0xffffffffull)) & MSR_S_HI_CURRENT_VID;
		printf("cpu %d currently at %d MHz and %d mV\n",
				cpu,
				find_freq_from_fid(fid),
				find_millivolts_from_vid(vid));
	} else if ((argc == 2) && (!strcmp (argv[1], "-I"))) {
		err = get_fidvid(cpu, &msr);
		if (err) {
			printf("can't get fid, vid from MSR\n");
			printf("Possible trouble: you don't run a powernow-k8 capable cpu\n");
			printf("or you are not root, or the msr driver is not present\n");
			exit(1);
		}
		printf("cpu %d:\n", cpu);
		printf("msr\t%llx\n", msr);
		fid = ((uint32_t )(msr & 0xffffffffull)) & MSR_S_LO_CURRENT_FID;
		printf("CurrFreq:	%u MHz\n", find_freq_from_fid(fid));
		fid = ((uint32_t )(msr>>8 & 0xffffffffull)) & MSR_S_LO_CURRENT_FID;
		printf("StartFreq:	%u MHz\n", find_freq_from_fid(fid));
		fid = ((uint32_t )(msr>>16 & 0xffffffffull)) & MSR_S_LO_CURRENT_FID;
		printf("MaxFreq:	%u MHz\n", find_freq_from_fid(fid));
		vid = ((uint32_t )(msr>>24 & 0xffffffffull)) & MSR_S_HI_CURRENT_VID;
		printf("MaxRampVolt:	%u mV\n", find_millivolts_from_vid(vid));
		vid = ((uint32_t )(msr>>32 & 0xffffffffull)) & MSR_S_HI_CURRENT_VID;
		printf("CurrVolt:	%u mV\n", find_millivolts_from_vid(vid));
		vid = ((uint32_t )(msr>>40 & 0xffffffffull)) & MSR_S_HI_CURRENT_VID;
		printf("StartVolt:	%u mV\n", find_millivolts_from_vid(vid));
		vid = ((uint32_t )(msr>>48 & 0xffffffffull)) & MSR_S_HI_CURRENT_VID;
		printf("MaxVolt:	%u mV\n", find_millivolts_from_vid(vid));
	} else if ((argc == 4) && (!strcmp (argv[1], "-s"))) {
		freq = atoi (argv[2]);
		mvolt = atoi (argv[3]);
		if ((freq >= 800) && (freq <= 2400) && (mvolt >= 800) && (mvolt <= 1500)) {
			fid = find_fid_from_freq(freq);
			vid = find_vid_from_millivolts(mvolt);
		} else 
			goto help;
		err = set_fidvid(cpu, fid, vid);
		if (err) {
			printf("can't set fid, vid in MSR\n");
			printf("Possible trouble: you don't run a powernow-k8 capable cpu\n");
			printf("or you are not root, or the msr driver is not present\n");
			exit(1);
		}
	} else 
		goto help;


	
	return 0;

help:	printf("Usage:	k8-msr -i\n");
	printf("	k8-msr -I\n");
	printf("	k8-msr -s FREQENCY VOLTAGE\n");
	printf("Options:\n");
	printf("   -i	display current frequency and voltage\n");
	printf("   -I	more info\n");
	printf("   -s	set frequency (in MHz, value from 800 to 2400) and\n");
	printf("	voltage (in mV, value from 800 to 1550)\n\n");

	return 0;
}
