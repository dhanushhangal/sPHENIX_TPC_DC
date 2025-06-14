#! /bin/bash

SHOW(){
cat << EOF
Running in directory:
$(pwd)

Directory contents:
$(ls -1a)

EOF
}

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh
export MYINSTALL=~/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
#printenv

#nIter=${1?Error: no number of iteration is given}
nEvents=3200
nrun=53513 
# 1044 segments for run 54966
# statrting segment from 50, ending at 500
filelist=${1?Error: filelist  is not given}


echo Running my macro Fun4All_CombinedDataReconstruction_hits.C from file number $nrun filelist ${filelist} with ${nEvents} events
         #std::cout.setstate(std::ios_base::failbit);
         #.L Trke_RecoInit.C

 root.exe -l -b << EOF
         .x Fun4All_CombinedDataReconstruction_hits.C(${nEvents},"${filelist}")
EOF

echo "I finished running! ${nEvents} ${filelist}" >> checkcondor.txt

echo all done
