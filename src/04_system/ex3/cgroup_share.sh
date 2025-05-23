mkdir -p /sys/fs/cgroup/cpu
mount -t cgroup -o cpu cpu /sys/fs/cgroup/cpu

mkdir /sys/fs/cgroup/cpu/low
mkdir /sys/fs/cgroup/cpu/high
echo 3 > /sys/fs/cgroup/cpuset/high/cpuset.cpus
echo 3 > /sys/fs/cgroup/cpuset/low/cpuset.cpus
# Share
echo 25 > /sys/fs/cgroup/cpu/low/cpu.shares
echo 75 > /sys/fs/cgroup/cpu/high/cpu.shares
