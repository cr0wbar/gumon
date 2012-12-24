gu(glielmo's) mon(itor)
=========================
crowbar MMXII

A simple Linux system  monitor written in C for bar, dzen2, etc..

Configuration
-------------
All the configuration is done in the config.h file.
These are the sections available so far:

+ CPUs (T)
+ MEMORY info
+ SWAP info
+ Available space on hard disks
+ Disks IO (T)
+ Network Interfaces IO (T)
+ Time and Uptime 
+ Temperatures reading (T) 
+ Battery status and remaining percentage (T)
+ Music Player Daemon info (can be removed by undefining MPD in config.h)
+ ALSA mixer status and volume (can be removed by undefining ALSA in config.h)
+ Outside Temperature obtained using an ICAO code (can be removed by undefining WEATHER in config.h)

Every section marked with (T) means that is configured in this way, let's take the CPUs section as an example:
```
static const char *cpus[] = {"cpu","cpu0","cpu1"}; 

static const char *cpusf[][3] = { {"| CPU : LOW%.0f | ","| CPU : NORM%.0f | ","| CPU : HIGH%.0f | "}
                                  {"CPU0: LOW%.0f | ","CPU0: NORM%.0f | ","CPU0: HIGH%.0f | "},
                                  {"CPU1: LOW%.0f | ","CPU1: NORM%.0f | ","CPU1: HIGH%.0f | "}};

static const float cpusthres[][2] = { {25.,75.},
                                      {50.,75.},
                                      {50.,80.} };
```
1. The first item is an array that contains the name of the cpus that will be monitored;
2. the second  item is a matrix containing the formats with which the cpu's average load will be printed;
3. the third item is a matrix containg the thresholds for the different formats.

For example if the load for cpu is < 25 , for cpu0 is > 50 and <75 and for cpu1 is > 80, the output will be

```
| CPU : LOW%.0f | CPU0: NORM%.0f | CPU1: HIGH%.0f |
```

Every other section has a format char array, used to format the output of the data, and optionally a list of interfaces to monitor.

For example
```
static const char *mountpoints[] = {"/","/home"}; /*List of mountpoints to monitor*/
static const char *mountpointsf[] = {"/ %.2f GB | ","/home %.2f GB"}; /* format of the remaining size in GB */
```

The Print Order
-------------
At the end of the file the order with which all the data will be printed can be set,
by adding elements to the `porder[]` array. 
All the possible choices are in the `enum PrintOrder` definition.
For example, let's assume that one wants to print the Network,Cpu and Memory info in this order.

Then porder can be set as follows:
```
static const enum PrintOrder porder[] = {Pnetwork,Pcpus,Pmem};
```

The Plsep,Pcsep,Prsep items, will respectively print the strings in `separators[0]`,`separators[1]`,`separators[2]`.

Defaults
-------------
The default config.h prints data formatted for the awesome `bar`, and uses the stlarch_font. Change it as you wish!

The weather script
-------------
Along with the gumon binary, a weather.sh script is installed.
Change the `CODE` and `NAME` parameters in the script to the ICAO code and location name of your choice. 
When called the scripts outputs the temperature for the given location.
If weather is defined in the config.h file, the script can be called 
every `WEATHER_TIME` seconds by gumon and the output will be printed.

Usage
-------------
`gumon` output can be print by passing its output to another program such as `bar` or `dzen2` via pipe.
For example: 
```
gumon | bar
```
or 
```
gumon | dzen2
```
