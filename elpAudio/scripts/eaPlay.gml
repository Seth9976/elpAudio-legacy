//image_index=0
ResetFloatText()

if ds_list_size(global.list)<1 exit
if global.pstate!=EA_NONE FMODInstanceStop(global.trackhandle)
MusicPlay(dslist(global.list,argument0))
