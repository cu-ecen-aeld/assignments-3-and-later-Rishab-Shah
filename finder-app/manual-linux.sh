#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.
# Edited by: Rishab Shah

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-
SYSROOT=aarch64-none-linux-gnu-gcc
# OLD_PATH=/home/rishab/assignment-1-Rishab-Shah/finder-app

# Check which directory is used
if [ $# -lt 1 ]
then
	echo "-----Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "-----Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

# Won't recreate if the directory already exits
cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    	#Clone only if the repository does not exist.
	echo "-----CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "-----Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    make ARCH=arm64 CROSS_COMPILE=${CROSS_COMPILE} mrproper
    echo "-----Completed make arm-gcc mrproper step"
    make ARCH=arm64 CROSS_COMPILE=${CROSS_COMPILE} defconfig
    echo "-----Completed make arm-gcc defconfig step"
    make -j6 ARCH=arm64 CROSS_COMPILE=${CROSS_COMPILE} all
    echo "-----Completed make arm-gcc all step"
    make ARCH=arm64 CROSS_COMPILE=${CROSS_COMPILE} modules
    echo "-----Completed make arm-gcc modules step"
    make ARCH=arm64 CROSS_COMPILE=${CROSS_COMPILE} dtbs
    echo "-----Completed make arm-gcc dtbs step"
fi 


# TODO: Add Image to outdir
cp ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}
echo "-----Added the Image in outdir"


# Staging
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "-----Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
	sudo rm  -rf ${OUTDIR}/rootfs
fi
echo "-----Created the staging directory for the root filesystem"


# TODO: Create necessary base directories
mkdir -p rootfs
cd rootfs 
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p home/conf
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log
echo "-----Created necessary base directories"


# Busy box steps
cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean
    make defconfig
else
    cd busybox
fi
echo "-----Entered busy box"


# TODO: Make and install busybox
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CONFIG_PREFIX=$OUTDIR/rootfs install
echo "-----Installed busybox"


# TODO: Reading binaries
cd "${OUTDIR}/rootfs"
ls -alh
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"
echo "-----Library dependencies as follows"


# TODO: Add library dependencies to rootfs
export SYSROOT=$(${CROSS_COMPILE}gcc -print-sysroot)
cd "${SYSROOT}"
ls -l lib/
ls -l lib/ld-linux-aarch64.so.1
echo "-----Completed exporting the sysroot path and change of directory"


# TODO: Copying files to qemu
cd "${OUTDIR}/rootfs"
sudo cp -L ${SYSROOT}/lib/ld-linux-aarch64.* lib
sudo cp -L ${SYSROOT}/lib64/libm.so.* lib64
sudo cp -L ${SYSROOT}/lib64/libresolv.so.* lib64
sudo cp -L ${SYSROOT}/lib64/libc.so.* lib64
echo "-----Completed copying libraries to qemu environment"


# TODO: Make device nodes
cd "${OUTDIR}/rootfs"
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1
ls -l dev
echo "-----Completed device nodes step"


# TODO: Clean and build the writer utility
cd "${FINDER_APP_DIR}"
echo "${FINDER_APP_DIR}"
make clean
make CROSS_COMPILE=aarch64-none-linux-gnu-
file writer
echo "-----Completed wrter utility verification"


# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cd "${FINDER_APP_DIR}"
echo "${FINDER_APP_DIR}"
cp ${FINDER_APP_DIR}/writer ${OUTDIR}/rootfs/home/
cp ${FINDER_APP_DIR}/finder.sh  ${OUTDIR}/rootfs/home/
cp ${FINDER_APP_DIR}/conf/username.txt ${OUTDIR}/rootfs/home/conf/
cp ${FINDER_APP_DIR}/finder-test.sh ${OUTDIR}/rootfs/home/
cp ${FINDER_APP_DIR}/autorun-qemu.sh ${OUTDIR}/rootfs/home/
echo "-----Completed copying files to QEMU environment"


# TODO: Chown the root directory
cd "${OUTDIR}"
sudo chown -R root:root rootfs
echo "-----Completed chown step"


# TODO: Create initramfs.cpio.gz
cd "${OUTDIR}/rootfs"
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
echo "-----Completed initramfs step"


# TODO: Gzip step as follows
cd ..
gzip -f initramfs.cpio
echo "-----Completed gzip"

