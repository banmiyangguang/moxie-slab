#########################################################################
# File Name: RunSlabHashTable.sh
# Author: fas
# Created Time: 2018年10月21日 星期日 12时03分18秒
#########################################################################
#!/bin/bash


base_times=100000
base_datalen=128
base_count=15

for ((i=1;i<=$base_count;++i))
do
    this_datalen=`expr $base_datalen \* $i`
    this_times=$base_times
    echo datalen:$this_datalen times:$this_times
    ./Slab_Hashtable $this_datalen $this_times >> Slab_Hashtable_result.txt
    ./Assoc_hashtable $this_datalen $this_times >> Assoc_Hashtable_result.txt
done



