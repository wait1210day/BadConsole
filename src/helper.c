#include <stdio.h>
#include "helper.h"

/**
static struct option getopt_longopts[] = {
    { "help", no_argument, NULL, 'h' },
    { "melt-file", required_argument, NULL, 'm' },
    { "time-speed", required_argument, NULL, 't' },
    { "frame-skipping-rate", required_argument, NULL, 'f' },
    { "audio-wav", required_argument, NULL, 'w' },
    { "pulseaudio-dynamic-lib", required_argument, NULL, 'l' },
    { NULL, no_argument, NULL, 0 }
};
*/

void helper_displayHelp (char const *cmda) {
    printf ("Melter: Melted file player %s\n", VBC_version);
    printf ("Usage: %s [arguments | file]\n", cmda);
    puts ("\nArguments:");
    puts (" --help                           Display this help message");
    puts (" --version                        Display version and copyright");
    puts (" --details                        Display building configs");
    puts (" --melt-file              <str>   Specify a path of melted file");
    puts (" --time-speed             <float> Specify time for each frame");
    puts (" --frame-skipping-rate    <float> Setup frame-skiper trigger");
    puts (" --audio-wav              <str>   Specify a path of wav file");
    puts (" --pulseaudio-dynamic-lib <str>   Specify a path of libpause-simple.so");
    puts ("\nBug report: <mashhiro.io@qq.com / wait1210day@github.com>");
    puts ("\nHave a good day!");
}

void helper_displayVersion (void) {
    printf ("Melter: %s\n", VBC_version);
    printf ("%s\n", VBC_license);
    printf ("Copyright (C) %s wait1210day@github.com\n", VBC_copyrightTime);
    printf ("Copyright (C) %s Free Software Foundation\n", VBC_copyrightTime);
}

void helper_displayDetails (void) {
    printf ("VBC_author:\t%s\n", VBC_author);
    printf ("VBC_version:\t%s\n", VBC_version);
    printf ("VBC_license:\t%s\n", VBC_license);
    printf ("VBC_platform:\t%s", VBC_platform);
    printf ("VBC_builder:\t%s\n", VBC_builder);
    printf ("VBC_language:\t%s\n", VBC_language);
#ifdef CCB_enable_pulseaudio
    printf ("CCB_enable_pulseaudio:\t%d\n", CCB_enable_pulseaudio);
#else
	printf ("CCB_enable_pulseaudio:\tNotSupported\n");
#endif
    printf ("VBC_audio_drv:\t%s\n", VBC_PA);
}
