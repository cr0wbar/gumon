/*Options*/
#define MPD /*if defined it shows the current mpd status and track*/

#define ALSA /*If defined it monitors a mixer status and volume*/

#define NOBATTERY /*If defined it monitors the battery*/

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

/*CPUs*/
static const char *cpus_load[] = {"cpu"}; /*cpus in /proc/cpustats to monitor*/
static const char *cpusf_load[][3] = { {"| %{F#FF00FFFF}\%{F-} %{F#FF0FFF00}%.0f\%{F-} | ",           
				   "| %{F#FF00FFFF}\%{F-} %{U#FFFFB917}%{F#FFFFB917}%{+u}%.0f%{U-}%{F-}%{-u} | ",
				   "| %{F#FF00FFFF}\%{F-} %{U#FFFF1778}%{F#FFFF1778}%{+u}%.0f%{U-}%{F-}%{-u} | "}}; /*CPUs format [Low,Norm,High]*/
static const float cpusthres_load[][2] = { {25.,75.} };

static const char *cpus_freq[] = {"0","1"};
static const char * cpusf_freq[][3] =  { {"%{F#FF0FFF00}%.0f%{F-} | ",           
					  " %{U#FFFFB917}%{F#FFFFB917}%{+u}%.0f%{U-}%{F-}%{-u} | ",
					  " %{U#FFFF1778}%{F#FFFF1778}%{+u}%.0f%{U-}%{F-}%{-u} | "},
					 {"%{F#FF0FFF00}%.0f%{F-} | ",           
					  " %{U#FFFFB917}%{F#FFFFB917}%{+u}%.0f%{U-}%{F-}%{-u} | ",
					  " %{U#FFFF1778}%{F#FFFF1778}%{+u}%.0f%{U-}%{F-}%{-u} | "} };

static const float cpusthres_freq[][2] = { {1000.,1600.},
					   {1000.,1600.}};

/*MEM&SWAP*/
static const char memf[] = "%{F#FF00FFFF}%{F-} %{F#FF0FFF00}%.1f %.0lfMB\%{F-} | "; /*Memory format*/
static const char swapf[] = "%{F#FF00FFFF}%{F-} %{F#FF0FFF00}%.2fMB\%{F-} | "; /*Swap format*/

/*Disks Info*/
static const char *mountpoints[] = {"/"}; /*Mountpoints to monitor via statvfs for free space*/
static const char *mountpointsf[] = {"%{F#FF00FFFF}/%{F-} %{F#FF0FFF00}%.2fGB\%{F-} | "}; /*Mountpoints format*/

/*Disks IO*/
static const char *diskIOdevs[] = {"sda"}; /* block devices to monitor, can be either sdaX for a single partition, or sda for a whole disk */ 
static const char *diskIOblocks[] = {"sda"}; /* correspondant root(?) block device. This is needed in order to get the disk block size */ 
static const char *diskIOfwrite[][3] = { {"%{F#FF00FFFF}/%{F-} %{F#FF0FFF00}%.2fMB\%{F-} ",
					  "%{F#FF00FFFF}/%{F-} %{U#FFFFB917}%{F#FFFFB917}%{+u}%.2fMB%{U-}%{F-}%{-u} ",
					  "%{F#FF00FFFF}/%{F-} %{U#FFFF1778}%{F#FFFF1778}%{+u}%.2fMB%{U-}%{F-}%{-u} "} };/*Write speed format*/
static const float diskIOwthres[][2] = { {0.01,10.} }; /*Write thresholds in MB/s*/
static const char *diskIOfread[][3] = { {"%{F#FF0FFF00}%.2fMB\%{F-} | ",
					 "%{U#FFFFB917}%{F#FFFFB917}%{+u}%.2fMB%{U-}%{F-}%{-u} | ",
					 "%{U#FFFF1778}%{F#FFFF1778}%{+u}%.2fMB%{U-}%{F-}%{-u} | "} };/*Read Speed format*/
static const float diskIOrthres[][2] = { {0.01,10.} }; /*Read thresholds in MB/s */

/*Network*/
static const char *netIFS[] = {"wlan0","eth0"}; /*Network IFs to monitor, will be shown only if connected*/

static const char *netIFSfdown[][3] = { {"%{F#FF00FFFF}%{F-} %{F#FF0FFF00}%.1fKB%{F-} ",
					 "%{F#FF00FFFF}%{F-} %{U#FFFFB917}%{F#FFFFB917}%{+u}%.1fKB%{U-}%{F-}%{-u} ",
					 "%{F#FF00FFFF}%{F-} %{U#FFFF1778}%{F#FFFF1778}%{+u}%.1fKB%{U-}%{F-}%{-u} "}, 
			  	        {"%{F#FF00FFFF}%{F-} %{F#FF0FFF00}%.1fKB\%{F-} ",
					 "%{F#FF00FFFF}%{F-} %{U#FFFFB917}%{F#FFFFB917}%{+u}%.1fKB%{U-}%{F-}%{-u} ",
					 "%{F#FF00FFFF}%{F-} %{U#FFFF1778}%{F#FFFF1778}%{+u}%.1fKB%{U-}%{F-}%{-u} "} }; /*Download speed KB/s formats*/
static const float netIFSdthres[][2] = { {300.,600.}, /*Network IFs download thresholds in KB/s*/
					 {300.,600.} };

static const char *netIFSfup[][3] = { {"%{F#FF0FFF00}%.1fKB%{F-} | ",
				       "%{U#FFFFB917}%{F#FFFFB917}%{+u}%.1fKB%{U-}%{F-}%{-u} | ",
				       "%{U#FFFF1778}%{F#FFFF1778}%{+u}%.1fKB%{U-}%{F-}%{-u} | "}, /*Upload speed in KB/s formats*/
				      {"%{F#FF0FFF00}%.1fKB%{F-} | ",
				       "%{U#FFFFB917}%{F#FFFFB917}%{+u}%.1fKB%{U-}%{F-}%{-u} | ",
				       "%{U#FFFF1778}%{F#FFFF1778}%{+u}%.1fKB%{U-}%{F-}%{-u} | "} };

static const float netIFSuthres[][2] = { {300.,600.}, /*Network IFs upload thresholds in KB/s*/
					 {300.,600.} };

static const char WSSf[] = {"%{F#FF00FFFF} %{F#FF0FFF00}%.2f%{F-} | "};/*If the Net IF is wireless, the signal strenght 
							   is printed along, this is the format of the 
							   signal strenght >=0.00 <=1.00*/ 

/*Time*/
static const char timef[] = "| %{F#FFFF1778}%{F-} %a %d/%m/%y | %{F#FFFF1778}%{F-} %H:%M:%S | ";/*Time format, see man strftime*/
static const char uptimef[] = "%{F#FFFF1778}%{F-} %s |"; /*Uptime format, string is the same as in conky's uptime_short*/

/*Temperature reading*/
static const char *temperatures[] = {"temp2_input","temp3_input"}; /*files in /sys/devices/platform/coretemp.0/ to monitor*/
static const char *temperaturesf[][3] = { {"| %{F#FF00FFFF} %{F#FF0FFF00}%.0f°%{F-} ",
					   "| %{F#FF00FFFF} %{U#FFFFB917}%{F#FFFFB917}%{+u}%.0f°%{U-}%{F-}%{-u} ",
					   "| %{F#FF00FFFF} %{U#FFFF1778}%{F#FFFF1778}%{+u}%.0f°%{U-}%{F-}%{-u} "},
					  {"%{F#FF0FFF00}%.0f°%{F-} | ",
					   "%{U#FFFFB917}%{F#FFFFB917}%{+u}%.0f°%{U-}%{F-}%{-u} | ",
					   "%{U#FFFF1778}%{F#FFFF1778}%{+u}%.0f°%{U-}%{F-}%{-u} | "} }; /*Temps format*/

static const float temperaturesthres[][2] = { {60.,90.}, /*Temps thresholds in degrees celsius*/
					      {60.,90.} };

#ifdef ALSA
/*Audio*/
const static char soundcard[] =  "default"; /*Soundcard to monitor*/
const static char mixername[] =  "Master"; /*Mixer to monitor*/
const static char *volumestatus[] = {"| %{F#FFFF1778}%{F-} ","| %{F#FF00FFFF}%{F-} "}; /*Volume status 1.Mute 2.Normal*/
const static char volumef[] = "%s %{F#FF0FFF00}%.0f%{F-} | ";/*Volume format (status and volume level in % */
#endif

/*MPD*/
#ifdef MPD
static const char mpdnormf[] = "%{F#FF00FFFF}%{F-} %{F#FF00FFFF}%s%{F-} %{F#FFFFB917}%s%{F-} | "; /*Mpd status and song format*/
static const char mpdstopf[] = "%{F#FF00FFFF}%{F-} %{F#FFFF1778}%s%{F-}  | "; /*Mpd status format when stopped*/
#endif

#ifdef BATTERY
/*BATTERY*/
static const char battery[] = "BAT0"; /*folder in /sys/class/power_supply */
static const char *batteryf[3] = {"%{F#FF00FFFF}%{F-} %s %{U#FFFF1778}%{F#FFFF1778}%{+u}%.2f%{U-}%{F-}%{-u} | ",
				  "%{F#FF00FFFF}%{F-} %s %{U#FFFFB917}%{F#FFFFB917}%{+u}%.2f%{U-}%{F-}%{-u} | ",
				  "%{F#FF00FFFF}%{F-} %s %{F#FF0FFF00}%.2f%{F-} | "}; /*Battery Formats*/
static const float batterythres[2] = {25.,50.}; /*Battery Thresholds (percentage)*/
#endif

/*Separators*/
static const char *separators[] ={"%{l}","%{c}","%{r}"}; /* Left,Central and Right separators*/
 
/*Don't touch this!*/
enum PrintOrder {
  Plsep,
  Pcsep,
  Prsep,
  Pcpuload,
  Pcpufreq,
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
  Ptime,
  Puptime,
  Ptemperatures
};

/*Here the print order is set*/
static const enum PrintOrder porder[] ={Pcpuload,Pmem,Pmountpoints,PdiskIO,Pnetwork,Pcsep,Pvolume,Pmpd,Prsep,Ptime,Puptime};
