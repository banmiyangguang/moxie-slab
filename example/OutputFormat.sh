#########################################################################
# File Name: OutputFormat.sh
# Author: fas
# Created Time: 2018年10月21日 星期日 12时03分18秒
#########################################################################
#!/bin/bash

Slab_insert_per_sec=`awk -F'[ :]' 'BEGIN{ORS=",";}{print $8;}' ./Slab_Hashtable_result.txt`
Slab_find_per_sec=`awk -F'[ :]' 'BEGIN{ORS=",";}{print $12;}' Slab_Hashtable_result.txt`
Assoc_insert_per_sec=`awk -F'[ :]' 'BEGIN{ORS=",";}{print $8;}' Assoc_Hashtable_result.txt`
Alab_find_per_sec=`awk -F'[ :]' 'BEGIN{ORS=",";}{print $12;}' Assoc_Hashtable_result.txt`
echo "Slab_insert_per_sec:${Slab_insert_per_sec}" >> output_format_result.txt
echo "Slab_find_per_sec:${Slab_find_per_sec}" >> output_format_result.txt
echo "Assoc_insert_per_sec:${Assoc_insert_per_sec}" >> output_format_result.txt
echo "Alab_find_per_sec:${Alab_find_per_sec}" >> output_format_result.txt

