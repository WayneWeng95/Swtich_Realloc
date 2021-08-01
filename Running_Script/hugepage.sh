#!bin/bash

echo "Check the system hugepage state: "

cat /proc/filesystems | fgrep huge

sudo cat /proc/sys/vm/nr_hugepages

echo "<number> > /proc/sys/vm/nr_hugepages"

cat /sys/devices/system/node/node*/meminfo | fgrep Huge

echo "mounting the hugepagefs by using: "

# mount -t hugetlbfs \
#       -o uid=1000,gid=1000,mode=0755  none /mnt/huge

echo "mount -t hugetlbfs -o uid=<value>,gid=<value>,mode=<0755> none /mnt/huge"