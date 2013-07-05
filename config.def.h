/*Options*/
#define MPD /*if defined it shows the current mpd status and track*/

#define WEATHER /*If defined it launches the weather.sh script*/

#define ALSA /*If defined it monitors a mixer status and volume*/

#define BATTERY /*If defined it monitors the battery*/

#define TIMER 1 /*Update the data every TIMER seconds*/

#ifdef BATTERY
#define BATTERY_TIMER 100 /*Update the Battery data every BATTERY_TIMER seconds*/
#endif

#ifdef MPD
#define MPD_PORT 6600
#define MPD_HOST "localhost"
#define MPD_PASSWD NULL
#define MPD_CONN_TIMEOUT 2 /*MPD CONNECTION timeout in seconds*/

#define GUMON_PAUSE "" /*Status to show when mpd is paused*/
#define GUMON_PLAY "" /*Status to show when mpd is playing*/
#define GUMON_STOP "" /*Status to show when mpd is stopped*/
#endif

#ifdef WEATHER
#define WEATHER_TIMER 600 /*Every  WEATHER_TIME seconds the program checks for weather by calling weather.sh*/
#endif

/*CPUs*/
static const char *cpus[] = {"cpu"}; /*cpus in /proc/cpustats to monitor*/
static const char *cpusf[][3] = { {"| \\f3\\fr \\f4%.0f\\fr | ",           
				   "| \\f3\\fr \\u5\\f5%.0f\\ur\\fr | ",
				   "| \\f3\\fr \\u6\\f6%.0f\\ur\\fr | "}}; /*CPUs format [Low,Norm,High]*/
static const float cpusthres[][2] = { {25.,75.} };

/*MEM&SWAP*/
static const char memf[] = "\\f3\\fr \\f4%.1f %.0lfMB\\fr | "; /*Memory format*/
static const char swapf[] = "\\f3\\fr \\f4%.2fMB\\fr | "; /*Swap format*/

/*Disks Info*/
static const char *mountpoints[] = {"/"}; /*Mountpoints to monitor via statvfs for free space*/
static const char *mountpointsf[] = {"\\f3/\\fr \\f4%.2fGB\\fr | "}; /*Mountpoints format*/

/*Disks IO*/
static const char *diskIOdevs[] = {"sda"}; /* block devices to monitor, can be either sdaX for a single partition, or sda for a whole disk */ 
static const char *diskIOblocks[] = {"sda"}; /* correspondant root(?) block device. This is needed in order to get the disk block size */ 
static const char *diskIOfwrite[][3] = { {"\\f3/\\fr \\f4%.2fMB\\fr ","\\f3/\\fr \\u5\\f5%.2fMB\\fr\\ur ","\\f3/\\fr \\u6\\f6%.2fMB\\ur\\fr "} };/*Write speed format*/
static const float diskIOwthres[][2] = { {0.01,10.} }; /*Write thresholds in MB/s*/
static const char *diskIOfread[][3] = { {"\\f4%.2fMB\\fr | ","\\u5\\f5%.2fMB\\ur\\fr | ","\\u6\\f6%.2fMB\\ur\\fr | "} };/*Read Speed format*/
static const float diskIOrthres[][2] = { {0.01,10.} }; /*Read thresholds in MB/s */

/*Network*/
static const char *netIFS[] = {"wlan0","eth0"}; /*Network IFs to monitor, will be shown only if connected*/

static const char *netIFSfdown[][3] = { {"\\f3\\fr \\f4%.1fKB\\fr ",
					 "\\f3\\fr \\u5\\f5%.1fKB\\ur\\fr ",
					 "\\f3\\fr \\u6\\f6%.1fKB\\ur\\fr "}, 
			  	        {"\\f3\\fr \\f4%.1fKB\\fr ",
					 "\\f3\\fr \\u5\\f5%.1fKB\\ur\\fr ",
					 "\\f3\\fr \\u6\\f6%.1fKB\\ur\\fr "} }; /*Download speed KB/s formats*/
static const float netIFSdthres[][2] = { {300.,600.}, /*Network IFs download thresholds in KB/s*/
					 {300.,600.} };

static const char *netIFSfup[][3] = { {"\\f4%.1fKB\\fr | ",
				       "\\u5\\f5%.1fKB\\ur\\fr | ",
				       "\\u6\\f6%.1fKB\\ur\\fr | "}, /*Upload speed in KB/s formats*/
				      {"\\f4%.1fKB\\fr | ",
				       "\\u5\\f5%.1fKB\\ur\\fr | ",
				       "\\u6\\f6%.1fKB\\ur\\fr | "} };
static const float netIFSuthres[][2] = { {300.,600.}, /*Network IFs upload thresholds in KB/s*/
					 {300.,600.} };

static const char WSSf[] = {"\\f3\\fr \\f4%.2f\\fr | "};/*If the Net IF is wireless, the signal strenght 
							   is printed along, this is the format of the 
							   signal strenght >=0.00 <=1.00*/ 

/*Time*/
static const char timef[] = "\\f6\\fr %a %d/%m/%y | \\f6\\fr %H:%M:%S | ";/*Time format, see man strftime*/
static const char uptimef[] = "\\f6\\fr %s |"; /*Uptime format, string is the same as in conky's uptime_short*/

/*Temperature reading*/
static const char *temperatures[] = {"temp2_input","temp3_input"}; /*files in /sys/devices/platform/coretemp.0/ to monitor*/
static const char *temperaturesf[][3] = { {"| \\f3\\fr \\f4%.0f°\\fr ",
					   "| \\f3\\fr \\u5\\f5%.0f°\\ur\\fr ",
					   "| \\f3\\fr \\u6\\f6%.0f°\\ur\\fr "},
					  {"\\f4%.0f°\\fr | ",
					   "\\u5\\f5%.0f°\\ur\\fr | ",
					   "\\u6\\f6%.0f°\\ur\\fr | "} }; /*Temps format*/
static const float temperaturesthres[][2] = { {60.,90.}, /*Temps thresholds in degrees celsius*/
					      {60.,90.} };

#ifdef ALSA
/*Audio*/
const static char soundcard[] =  "default"; /*Soundcard to monitor*/
const static char mixername[] =  "Master"; /*Mixer to monitor*/
const static char *volumestatus[] = {"| \\f8\\fr ","| \\f3\\fr "}; /*Volume status 1.Mute 2.Normal*/
const static char volumef[] = "%s \\f4%.0f\\fr | ";/*Volume format (status and volume level in % */
#endif

/*MPD*/
#ifdef MPD
static const char mpdnormf[] = "\\f3\\fr \\f3%s\\fr \\f5%s\\fr | "; /*Mpd status and song format*/
static const char mpdstopf[] = "\\f3\\fr \\f6%s\\fr  | "; /*Mpd status format when stopped*/
#endif

#ifdef BATTERY
/*BATTERY*/
static const char battery[] = "BAT0"; /*folder in /sys/class/power_supply */
static const char *batteryf[3] = {"\\f3\\fr %s \\u6\\f6%.2f\\ur\\fr | ",
				  "\\f3\\fr %s \\u5\\f5%.2f\\ur\\fr | ",
				  "\\f3\\fr %s \\f4%.2f\\fr | "}; /*Battery Formats*/
static const float batterythres[2] = {25.,50.}; /*Battery Thresholds (percentage)*/
#endif

/*Separators*/
static const char *separators[] ={"\\l","\\c","\\r"}; /* Left,Central and Right separators*/
 
/*Don't touch this!*/
enum PrintOrder {
  Plsep,
  Pcsep,
  Prsep,
  Pcpus,
  Pmem,
  Pswap,
  Pmountpoints,
  PdiskIO,
  Pnetwork,
#ifdef ALSA
  Pvolume,
#endif
#ifdef MPD
  Pmpd,
#endif
#ifdef BATTERY
  Pbattery,
#endif
#ifdef WEATHER
  Pweather,
#endif
  Ptime,
  Puptime,
  Ptemperatures
};

/*Here the print order is set*/
static const enum PrintOrder porder[] = {Pcpus,Pmem,Pmountpoints,PdiskIO,Pnetwork,Pcsep,Pvolume,Pmpd,Prsep,Ptemperatures,Pweather,Ptime,Puptime};
