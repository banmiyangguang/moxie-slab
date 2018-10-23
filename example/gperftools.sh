#########################################################################
# File Name: gperftools.sh
# Author: fas
# Created Time: 2018年10月23日 星期二 12时19分04秒
#########################################################################
#!/bin/bash

#/opt/gperftools-2.7/bin/pprof --pdf ./Assoc_hashtable ./gperftool.profile  >  graph.pdf
/opt/gperftools-2.7/bin/pprof --pdf ./Slab_Hashtable ./gperftool.profile  >  graph.pdf

