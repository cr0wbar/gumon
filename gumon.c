/*gumon.c
  Simple system monitor for Linux
  Author: crowbar
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/statvfs.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include "config.h"

#define LEN(X)  (sizeof X / sizeof X[0])
#define OFFSET(X) ( (6-X) > 0 ? (6-len) : 0 )

#ifdef ALSA
#include <alsa/asoundlib.h>
#endif

#ifdef MPD
#include <libmpd-1.0/libmpd/libmpd.h>
#define MPD_BUF_LEN 128
#define MPD_STATUS_BUF_LEN 4
#endif

#define BUF_LEN 256
#define SHORT_BUF_LEN 128 

struct MemInfo {
  unsigned long int total;
  unsigned long int free;
  unsigned long int buffers;
  unsigned long int cached;
  unsigned long int swaptotal;
  unsigned long int swapfree;
  unsigned long int swapcached;
};

struct CpuLoad {
  unsigned long int A[4],B[4];
  float loadavg;
}; 

struct TimeInfo {
  time_t t;
  struct tm tm;
  long double uptime;
  unsigned int uptime_days;
  unsigned int uptime_hours;
  unsigned int uptime_minutes;
  unsigned int uptime_seconds;
  char uptime_nice[5];
};

struct NetInfo {
  unsigned long int recv[2];
  unsigned long int sent[2];
  float downspeed; /* KB/s */
  float upspeed; /* KB/s */
};

struct DiskInfo {
  float avail; /* GB */
  float total; /* GB */
  struct statvfs data; /* RAW DATA */
};

struct DiskIOInfo {
  unsigned long int read[2],write[2];
  float read_speed,write_speed;
};

#ifdef ALSA
struct VolumeInfo {
  snd_mixer_t *handle;
  snd_mixer_elem_t *elem;
  snd_mixer_selem_id_t *sid;
  long min,max,vol;
  int mute;
};
#endif

#ifdef MPD
struct MpdInfo {
  char display[MPD_BUF_LEN];
  char status[MPD_STATUS_BUF_LEN];
  MpdState mpd_status;
};
#endif

#ifdef BATTERY
struct BatteryInfo{
  char status[16];
  unsigned long int charge_full;
  unsigned long int charge_now;
  float charge_percentage;
};
#endif

int SetMemInfo(struct MemInfo *mem){
  char buf[BUF_LEN];
  FILE  *fp;

  if( (fp = fopen("/proc/meminfo","r")) == NULL){
    fprintf(stderr,"ERR: can't open /proc/meminfo\n");
    exit(EXIT_FAILURE);
  }

  while(fgets(buf,BUF_LEN,fp) != NULL){
    if( strncmp("MemTotal",buf,8) == 0 ){
      sscanf(buf+9,"%lu",&(mem->total));
    }
    else if( strncmp("MemFree",buf,7) == 0 ){
      sscanf(buf+8,"%lu",&(mem->free));
    }
    else if( strncmp("Buffers",buf,7) == 0 ){
      sscanf(buf+8,"%lu",&(mem->buffers));
    }
    else if( strncmp("Cached",buf,6) == 0 ){
      sscanf(buf+7,"%lu",&(mem->cached));
    }
    else if( strncmp("Buffers",buf,7) == 0 ){
      sscanf(buf+8,"%lu",&(mem->buffers));
    }
    else if( strncmp("SwapCached",buf,10) == 0 ){
      sscanf(buf+11,"%lu",&(mem->swapcached));
    }
    else if( strncmp("SwapTotal",buf,9) == 0 ){
      sscanf(buf+10,"%lu",&(mem->swaptotal));
    }
    else if( strncmp("SwapFree",buf,8) == 0 ){
      sscanf(buf+9,"%lu",&(mem->swapfree));
    }
  }
  fclose(fp);
  return 1;
}

int SetNetInfo(const char *IFname, struct NetInfo *net){
  FILE *fp;
  char buf[BUF_LEN];
  int len = strlen(IFname);
  unsigned long int prova;
  if( (fp = fopen("/proc/net/dev","r")) == NULL ){
    fprintf(stderr,"ERR: can't open /proc/net/route\n");
    exit(EXIT_FAILURE);
  }

  net->recv[0] = net->recv[1];
  net->sent[0] = net->sent[1];

  while(fgets(buf,BUF_LEN,fp) != NULL){
    if( strncmp(IFname,buf + OFFSET(len),len) == 0 ){
      sscanf(buf+ OFFSET(len) + len + 1,"%lu %*d %*d %*d %*d %*d %*d %*d %lu",&(net->recv[1]),&(net->sent[1]));
      net->downspeed = (float)(net->recv[1] - net->recv[0])/(float)(TIMER*1024) ; /*KB*/
      net->upspeed = (float)(net->sent[1] - net->sent[0])/(float)(TIMER*1024); /*KB*/
      fclose(fp);
      return 1;
    }
  }
  fclose(fp);
  return 0;
}

int IsOnDaInternetz(const char *IFname){
  FILE *fp;
  char buf[BUF_LEN];

  if( (fp = fopen("/proc/net/route","r")) == NULL ){
    fprintf(stderr,"ERR: can't open /proc/net/route\n");
    exit(EXIT_FAILURE);
  }
  while( fgets(buf,BUF_LEN,fp) != NULL){
    if(strncmp(IFname,buf,strlen(IFname)) == 0){
      fclose(fp);
      return 1;
    }
  }
  fclose(fp);
  return 0;
}

int WirelessSS(const char* IFname,int *SS){
  FILE *fp;
  char buf[BUF_LEN];
  int len = strlen(IFname);

  if( (fp = fopen("/proc/net/wireless","r")) == NULL){
    fprintf(stderr,"ERR: can't open /proc/net/wireless\n");
    exit(EXIT_FAILURE);
  }

  while( fgets(buf,BUF_LEN,fp)  != NULL ){
    if( strncmp(IFname,buf + OFFSET(len),len) == 0 ){
      sscanf(buf + OFFSET(len) + len +1,"%*d %d",SS);
      fclose(fp);
      return 1;
    }
  }
  fclose(fp);
  return 0;

}

int SetCpuLoad(const char *cpuname,struct CpuLoad *cpu){
  FILE *fp;
  char buf[BUF_LEN];

  if( (fp = fopen("/proc/stat","r")) == NULL ){
    fprintf(stderr,"ERR: can't open /proc/stat\n");
    exit(EXIT_FAILURE);
  }
  
  cpu->A[0] = cpu->B[0];
  cpu->A[1] = cpu->B[1];
  cpu->A[2] = cpu->B[2];
  cpu->A[3] = cpu->B[3];

  while( fgets(buf,BUF_LEN,fp) != NULL){
    if(strncmp(cpuname,buf,strlen(cpuname)) == 0){
      sscanf(buf+strlen(cpuname),"%lu %lu %lu %lu",&(cpu->B[0]),&(cpu->B[1]),&(cpu->B[2]),&(cpu->B[3]));
      cpu->loadavg = (float)(cpu->B[0]+cpu->B[1]+cpu->B[2]-cpu->A[0]-cpu->A[1]-cpu->A[2])/(float)(cpu->B[0]+cpu->B[1]+cpu->B[2]+cpu->B[3]-cpu->A[0]-cpu->A[1]-cpu->A[2]-cpu->A[3]);
      fclose(fp);
      return 1;
    }
  }
  fclose(fp);

  return 0;

}

int SetCpuFreq(const char *cpuindex,float *freq){

  FILE *fp;
  char buf[BUF_LEN];
  int flag = 0;

  if( (fp = fopen("/proc/cpuinfo","r")) == NULL ){
    fprintf(stderr,"ERR: can't open /proc/cpuinfo\n");
    exit(EXIT_FAILURE);
  }

  while(fgets(buf,BUF_LEN,fp) != NULL){
    if( strncmp(buf,"processor",9) == 0 && strncmp(cpuindex,buf+12,1) == 0 )
      flag=1;
    else if( strncmp("cpu MHz",buf,7) == 0 && flag ){
      sscanf(buf+10,"%f",freq);
      fclose(fp);
      return 1;
    }
  }

  fclose(fp);
  return 0;

}

int SetDiskInfo(const char *path, struct DiskInfo *disk){
  if( statvfs(path,&(disk->data)) < 0 ){
    return 0; 
  }
  disk->avail = ((float)disk->data.f_bavail*(float)disk->data.f_bsize/1073741824.); /*GB*/
  disk->total = ((float)disk->data.f_blocks*(float)disk->data.f_bsize/1073741824.); /*GB*/
  return 1;
}

int SetDiskIOInfo(const char *dev,const char *root,struct DiskIOInfo *data){
  FILE *fp;
  int ssize;
  char buf[BUF_LEN];
 
  sprintf(buf,"/sys/block/%s/queue/physical_block_size",root);
  if( (fp = fopen(buf,"r")) == NULL ){
    return 0;
  }
  fscanf(fp,"%d",&ssize);
  fclose(fp);
  
  if( (fp = fopen("/proc/diskstats","r")) == NULL ){
    fprintf(stderr,"ERR: can't open /proc/diskstats\n");
    exit(EXIT_FAILURE);
  }
  
  data->read[0] = data->read[1];
  data->write[0] = data->write[1];
  
  while( fgets(buf,BUF_LEN,fp) !=NULL ){
    if(strncmp(dev,buf+13,strlen(dev))==0){
      sscanf(buf+13+strlen(dev)," %*d %*d %lu %*d %*d %*d %lu",&(data->read[1]),&(data->write[1]));
      fclose(fp);
      data->read_speed = (float)(ssize*(data->read[1] - data->read[0]))/(1048576.*TIMER); /*MB*/
      data->write_speed = (float)(ssize*(data->write[1] - data->write[0]))/(1048576.*TIMER); /*MB*/
      return 1;
    }
  }
  return 0;

}

int SetTimeInfo(struct TimeInfo *crono){
  FILE *fp;

  if( (fp = fopen("/proc/uptime","r")) == NULL ){
    fprintf(stderr,"ERR: can't open /proc/uptime\n");
    exit(EXIT_FAILURE);
  }
  fscanf(fp,"%Lf",&(crono->uptime));
  fclose(fp);

  crono->uptime_days = (unsigned int)(crono->uptime/86400.);
  crono->uptime_hours = (unsigned int)((crono->uptime - (long double)(crono->uptime_days*86400))/3600.); 
  crono->uptime_minutes = (unsigned int)((crono->uptime - (long double)(crono->uptime_days*86400 + crono->uptime_hours*3600))/60.);
  crono->uptime_seconds = (unsigned int)((crono->uptime - (long double)(crono->uptime_days*86400 + crono->uptime_hours*3600 + crono->uptime_minutes*60)));

  crono->t = time(NULL);
  crono->tm = *localtime(&(crono->t));

  if(crono->uptime_days){
    sprintf(crono->uptime_nice,"%dd %dh",crono->uptime_days,crono->uptime_hours);
  }
  else if(crono->uptime_hours){
    sprintf(crono->uptime_nice,"%dh %dm",crono->uptime_hours,crono->uptime_minutes);
  }
  else{
    sprintf(crono->uptime_nice,"%dm %ds",crono->uptime_minutes,crono->uptime_seconds);
  }

  return 1;
}

int SetTemperatures(const char *filename,float *temp){
  FILE *fp;
  char buf[BUF_LEN];
  int i;
  for(i=0;i<10;i++) {
    char candidateDir[BUF_LEN];
    sprintf(candidateDir,"/sys/devices/platform/coretemp.0/hwmon/hwmon%d/",i); 
    DIR *dir = opendir(candidateDir);
    if (dir) {
      closedir(dir);
      strcpy(buf,candidateDir);
      strcat(buf,filename);
      if( (fp = fopen(buf,"r")) == NULL ){
	fprintf(stderr,"ERR:couldn't open %s\n",buf);
	return 0;
      }
      if( fgets(buf,BUF_LEN,fp) == NULL){
	fclose(fp);
	fprintf(stderr,"ERR:empty temperature file\n");
	return 0;
      }
      fclose(fp);
      sscanf(buf,"%f",temp);
      *temp/=1000.;
      return 1;
    } else if (ENOENT == errno) {
      continue;
    } else {
      fprintf(stderr, "ERR:opening directory %s\n",buf);
      return 0;
    }
  }
  return 0;
}

#ifdef ALSA
void  VolumeEvent(snd_mixer_elem_t *elem,unsigned int mask){

  if( mask&0x1){
    struct VolumeInfo *data = (struct VolumeInfo *)snd_mixer_elem_get_callback_private(elem);
    
    if( snd_mixer_selem_get_playback_volume(elem,0,&(data->vol)) != 0 ){
      fprintf(stderr,"ERR:snd_mixer_selem_get_playback_volume failed\n");
      exit(EXIT_FAILURE);
    }

    if( snd_mixer_selem_get_playback_switch(elem,0,&(data->mute)) != 0 ){
      fprintf(stderr,"ERR:snd_mixer_selem_get_playback_switch failed\n");
      exit(EXIT_FAILURE);
    }

  }
}

int InitVolumeInfo(struct VolumeInfo *vol){

  if( snd_mixer_open(&(vol->handle),0) != 0){
    fprintf(stderr,"ERR:snd_mixer_open failed\n");
    return 0;
  }

  if( snd_mixer_attach(vol->handle,soundcard) != 0 ){
    fprintf(stderr,"ERR:snd_mixer_attach failed\n");
    return 0;
  }

  if( snd_mixer_selem_register(vol->handle, NULL, NULL) != 0){
    fprintf(stderr,"ERR:snd_mixer_selem_register failed\n");
    return 0;
  }

  if( snd_mixer_load(vol->handle) != 0) {
    fprintf(stderr,"ERR:snd_mixer_load failed\n");
    return 0;
  }

  snd_mixer_selem_id_alloca(&(vol->sid));
  snd_mixer_selem_id_set_index(vol->sid, 0);
  snd_mixer_selem_id_set_name(vol->sid, mixername);
  vol->elem = snd_mixer_find_selem(vol->handle, vol->sid);

  if( vol->elem == NULL){
    fprintf(stderr,"ERR:snd_mixer_find_selem failed\n");
    return 0;
  }

  snd_mixer_elem_set_callback(vol->elem,(snd_mixer_elem_callback_t)VolumeEvent);
  snd_mixer_elem_set_callback_private(vol->elem,vol);

  if( snd_mixer_selem_get_playback_volume_range(vol->elem,&(vol->min),&(vol->max)) !=0 ) {
    fprintf(stderr,"ERR:couldn't get volume range\n");
    return 0;
  }

  VolumeEvent(vol->elem,1);

  return 1;
}
#endif

#ifdef MPD
void SetMpdInfo(MpdObj *mi, ChangedStatusType what, struct MpdInfo *music){

  if(what&MPD_CST_STATE || what&MPD_CST_SONGID){

    mpd_Song *song = mpd_playlist_get_current_song(mi);
    music->mpd_status = mpd_player_get_state(mi);

    if(song){
      if(song->title && song->artist){
	mpd_song_markup(music->display,MPD_BUF_LEN,"%title% - %artist%",song);
      }
      else if(song->title){
	mpd_song_markup(music->display,MPD_BUF_LEN,"%title%",song);
      }
      else if(song->name){
       mpd_song_markup(music->display,MPD_BUF_LEN,"%name%",song);
      }    
      else if(song->file){
       mpd_song_markup(music->display,MPD_BUF_LEN,"%file%",song);
      }    
    }
   
    if( music->mpd_status == MPD_PLAYER_PAUSE ){
      strcpy(music->status,GUMON_PAUSE);
    }
    else if( music->mpd_status == MPD_PLAYER_PLAY ){
      strcpy(music->status,GUMON_PLAY);
    }
    else if( music->mpd_status == MPD_PLAYER_STOP ){
      strcpy(music->status,GUMON_STOP);
    }
  }

}

void MpdConnChanged(MpdObj *mi, int connect,int *MpdIsConnected){
  if(!connect){
    mpd_disconnect(mi);
  }
  *MpdIsConnected = connect;
}

#endif

#ifdef BATTERY
int SetBatteryInfo(const char *BATNAME,struct BatteryInfo *bat){
  FILE *fp;
  char filename[BUF_LEN];
  
  sprintf(filename,"/sys/class/power_supply/%s/energy_now",BATNAME);
  if( (fp = fopen(filename,"r")) == NULL ){
    fprintf(stderr,"ERR: Can't open %s\n",filename);
    return 0;
  }
  fscanf(fp,"%ld",&(bat->charge_now));
  fclose(fp);

  sprintf(filename,"/sys/class/power_supply/%s/energy_full",BATNAME);
  if( (fp = fopen(filename,"r")) == NULL ){
    fprintf(stderr,"ERR: Can't open %s\n",filename);
    return 0;
  }
  fscanf(fp,"%ld",&(bat->charge_full));
  fclose(fp);
  bat->charge_percentage = 100.*(float)bat->charge_now/(float)bat->charge_full;
  if(bat->charge_percentage > 100.){
    bat->charge_percentage = 100; //Overflow protection :))
  }
  sprintf(filename,"/sys/class/power_supply/%s/status",BATNAME);
  if( (fp = fopen(filename,"r")) == NULL ){
    fprintf(stderr,"ERR: Can't open %s\n",filename);
    return 0;
  }
  fscanf(fp,"%s",bat->status);
  fclose(fp);
  return 1;
}
#endif

int ThresSelect(const float value,const float thres[2]){

  if( value < thres[0]){
    return 0;
  }
  else if( value < thres[1]){
    return 1;
  }
  return 2;
}

int main(void){

  static int i,j,SS;
  static struct CpuLoad data_cpus_load[LEN(cpus_load)];
  static float data_cpus_freq[LEN(cpus_freq)];
  static struct NetInfo data_netIFS[LEN(netIFS)];
  static struct MemInfo data_mem;
  static struct DiskInfo data_mountpoints[LEN(mountpoints)];
  static struct DiskIOInfo data_diskiodevs[LEN(diskIOdevs)];
  static float data_temps[LEN(temperatures)];
  static struct TimeInfo data_crono;
  static char temp_buf[SHORT_BUF_LEN];

#ifdef BATTERY
  static struct BatteryInfo data_battery;
  static int bat_counter = 0,bat_result;
#endif

#ifdef ALSA
static struct VolumeInfo data_volume;
  if( !InitVolumeInfo(&data_volume) ){
    fprintf(stderr,"ERR: can't initialize connection with ALSA mixer.\n");
    exit(EXIT_FAILURE);
  }
#endif

#ifdef MPD
  MpdObj *mpd = mpd_new(MPD_HOST,MPD_PORT,MPD_PASSWD);
  static struct MpdInfo music;
  static int MpdIsConnected = 0;
  /*Mpd Init*/
  mpd_signal_connect_status_changed(mpd,(StatusChangedCallback)SetMpdInfo, &music);
  mpd_signal_connect_connection_changed(mpd,(ConnectionChangedCallback)MpdConnChanged,&MpdIsConnected);
  mpd_set_connection_timeout(mpd, MPD_CONN_TIMEOUT);
#endif

  /*MAIN LOOP*/
  while(1){

    /*Mem&Swap data*/
    SetMemInfo(&data_mem);
    /*Time,Date & Uptime data*/
    SetTimeInfo(&data_crono);

    /*Print Order Loop*/
    for(j=0;j<LEN(porder);j++){
      switch(porder[j]){

	/*CPUs data&print*/
      case Pcpuload:
	for(i=0;i<LEN(cpus_load);i++){
	  SetCpuLoad(cpus_load[i],&data_cpus_load[i]);
	  fprintf(stdout,cpusf_load[i][ThresSelect(data_cpus_load[i].loadavg*100.,cpusthres_load[i])],100.*data_cpus_load[i].loadavg);
	}
	break;

      case Pcpufreq:
	for(i=0;i<LEN(cpus_freq);i++){
	  SetCpuFreq(cpus_freq[i],&data_cpus_freq[i]);
	  fprintf(stdout,cpusf_freq[i][ThresSelect(data_cpus_freq[i],cpusthres_freq[i])],data_cpus_freq[i]);
	}
	break;

	/*Mem Print*/
      case Pmem:
	fprintf(stdout,
		memf,
	        100.*(float)(data_mem.total-data_mem.free-data_mem.buffers-data_mem.cached)/(float)(data_mem.total),
		(float)(data_mem.total-data_mem.free-data_mem.buffers-data_mem.cached)/(float)1024.);
	break;

	/*Swap print*/
      case Pswap:
	fprintf(stdout,
		swapf,
	        100.*(float)(data_mem.swaptotal-data_mem.swapfree-data_mem.swapcached)/(float)(data_mem.swaptotal));
	break;

	/*Mountpoints(Disks) data&print*/
      case Pmountpoints:
	for(i=0;i<LEN(mountpoints);i++){
	  if( SetDiskInfo(mountpoints[i],&data_mountpoints[i]) ){
	    fprintf(stdout,mountpointsf[i],data_mountpoints[i].avail);
	  }
	}
	break;

	/*DiskIO data&print*/
      case PdiskIO:
	for(i=0;i<LEN(diskIOdevs);i++){
	  if( SetDiskIOInfo(diskIOdevs[i],diskIOblocks[i],&data_diskiodevs[i]) ){
	    fprintf(stdout,
		    diskIOfwrite[i][ThresSelect(data_diskiodevs[i].write_speed,diskIOwthres[i])],
		    data_diskiodevs[i].write_speed);
	    fprintf(stdout,
		    diskIOfread[i][ThresSelect(data_diskiodevs[i].read_speed,diskIOrthres[i])],
		    data_diskiodevs[i].read_speed);
	  }
	}
	break;

	/*Network Interfaces data&print */
      case Pnetwork:
	for(i=0;i<LEN(netIFS);i++){
	  if( IsOnDaInternetz(netIFS[i]) ){
	    SetNetInfo(netIFS[i],&data_netIFS[i]);
	    fprintf(stdout,
		    netIFSfdown[i][ThresSelect(data_netIFS[i].downspeed,netIFSdthres[i])],
		    data_netIFS[i].downspeed);
	    fprintf(stdout,
		    netIFSfup[i][ThresSelect(data_netIFS[i].upspeed,netIFSuthres[i])],
		    data_netIFS[i].upspeed);
	  }
	  /*if(WirelessSS(netIFS[i],&SS)){
	    sprintf(temp_buf,WSSf,(float)SS/70.);
	    fputs(temp_buf,stdout);
	    }*/
	}
	break;

#ifdef ALSA
	/*Volume data&print*/
      case Pvolume:
	snd_mixer_handle_events(data_volume.handle);
	fprintf(stdout,
		volumef,
		volumestatus[data_volume.mute],
		100.*(float)(data_volume.vol-data_volume.min)/(float)(data_volume.max-data_volume.min));
	break;	
#endif

#ifdef BATTERY	
	/*Battery data&print*/
      case Pbattery:
	if(!(bat_counter%BATTERY_TIMER) ){
	  bat_result = SetBatteryInfo(battery,&data_battery);
	  bat_counter = 0;
	}
	if(bat_result){
	  fprintf(stdout,
		  batteryf[ThresSelect(data_battery.charge_percentage,batterythres)],
		  data_battery.status,
		  data_battery.charge_percentage);
	}
	bat_counter++;
	break;
#endif	
	
#ifdef MPD
	/*MPD data&print*/
      case Pmpd:
	if(MpdIsConnected){
	  mpd_status_update(mpd);
	  if(music.mpd_status != MPD_PLAYER_STOP){
	    fprintf(stdout,mpdnormf,music.status,music.display);
	  }
	  else{
	    fprintf(stdout,mpdstopf,music.status);
	  }
	}
	else{
	  if(MPD_PASSWD){
	    mpd_send_password(mpd);
	  }
	  mpd_connect(mpd);
	}
	break;
#endif

	/*Time print*/
      case Ptime:
	strftime(temp_buf,SHORT_BUF_LEN,timef,&(data_crono.tm));
	fputs(temp_buf,stdout);
	break;
	
	/*Uptime print*/
      case Puptime:
	fprintf(stdout,uptimef,data_crono.uptime_nice);
	break;
      
      case Ptemperatures:
	for (i=0;i<LEN(temperatures);i++){
	  if( SetTemperatures(temperatures[i],&data_temps[i]) ){
	    fprintf(stdout,temperaturesf[i][ThresSelect(data_temps[i],temperaturesthres[i])],data_temps[i]);
	  }
	}
	break;
	
	/*Separators*/
      case Plsep:
	fputs(separators[0],stdout);
	break;
	
      case Pcsep:
	fputs(separators[1],stdout);
	break;
	
      case Prsep:
	fputs(separators[2],stdout);
	break;
	
      }
    }
    /*The End*/
    fputs("\n",stdout);
    fflush(stdout);
    
    sleep(TIMER);
  }
}

