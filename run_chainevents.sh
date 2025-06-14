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

file=${1?Error: file is not given}

echo Running my macro chainevents.C from file ${file}

 root.exe -l -b << EOF
	 .x chainevents.C("${file}")
EOF

echo "I finished running! ${file}" >> checkcondor_chain.txt

echo all done
