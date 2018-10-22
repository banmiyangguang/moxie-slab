#########################################################################
# File Name: RunSlabHashTable.sh
# Author: fas
# Created Time: 2018年10月21日 星期日 12时03分18秒
#########################################################################
#!/bin/bash


base_times=(10000)
base_datalen=(2048)
count_times=50

for ((i=1;i<=$count_times;++i))
do
    this_datalen=`expr $base_datalen \* $i`
    this_times=`echo "$base_times*$base_datalen/$this_datalen" | bc`
    echo datalen:$this_datalen times:$this_times
    ./Slab_Hashtable $this_datalen $this_times >> Slab_Hashtable_result.txt
    ./Unoredered_map $this_datalen $this_times >> Unoredered_map_result.txt
done



