#########################################################################
# File Name: RunSlabHashTable.sh
# Author: fas
# Created Time: 2018年10月21日 星期日 12时03分18秒
#########################################################################
#!/bin/bash

base_times=(1000)
base_datalen=(2048)
count_times=50

for ((i=1;i<=$count_times;++i))
do
    this_datalen=`expr $base_datalen \* $i`
    #echo datalen:$this_datalen times:$base_times
    ./Slab_Hashtable $this_datalen $base_times >> Slab_Hashtable_result.txt
    ./Unoredered_map $this_datalen $base_times >> Unoredered_map_result.txt
done



