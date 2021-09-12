#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

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
OLD_PATH=/home/rishab/assignment-1-Rishab-Shah/finder-app


if [ $# -lt 1 ]
then
	echo "-----Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "-----Using passed directory ${OUTDIR} for output"
fi

#Won't recreate if the directory already exits
mkdir -p ${OUTDIR}

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
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- defconfig
    make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- modules
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- dtbs
fi 

#TODO: Add Image to outdir
echo "-----Adding the Image in outdir"
sudo cp ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}
echo "-----Creating the staging directory for the root filesystem"

cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "-----Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs

fi


# TODO: Create necessary base directories
mkdir -p rootfs
cd rootfs 
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p home/conf
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log
tree -d


echo "-----Created necessary base directories"
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
echo "====================================HEY HERE++++++++++++++++++++++++++++++++++++"

make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CONFIG_PREFIX=$OUTDIR/rootfs install

echo "NOW HERE ____________________________________________________________________________________________________________________________________________________"

echo "-----Library dependencies"
cd "${OUTDIR}/rootfs"
ls -alh
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"


echo "REACHED HERE"
export SYSROOT=$(${CROSS_COMPILE}gcc -print-sysroot)
# CD_PATH=$($(SYSROOT) -print-sysroot)
# TODO: Add library dependencies to rootfs
cd "${SYSROOT}"
ls -l lib/

echo "-----AGAIN DOING THIS STEP"

ls -l lib/ld-linux-aarch64.so.1

cd "${OUTDIR}/rootfs"


sudo cp -L ${SYSROOT}/lib/ld-linux-aarch64.* lib
sudo cp -L ${SYSROOT}/lib64/libm.so.* lib64
sudo cp -L ${SYSROOT}/lib64/libresolv.so.* lib64
sudo cp -L ${SYSROOT}/lib64/libc.so.* lib64




# sudo cp -a ${SYSROOT}/lib/ld-linux-aarch64.so.1 lib
# sudo cp -a ${SYSROOT}/lib64/libm.so.6 lib64
# sudo cp -a ${SYSROOT}/lib64/libresolv.so.2 lib64
# sudo cp -a ${SYSROOT}/lib64/libc.so.6 lib64


echo "-----completed copying libraries mentioned above"

# TODO: Make device nodes
cd "${OUTDIR}/rootfs"
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

ls -l dev

echo "-----completed device nodes step"
# TODO: Clean and build the writer utility
cd "${OLD_PATH}"
make clean
make CROSS_COMPILE=aarch64-none-linux-gnu-
file writer
echo "-----wrter utility is built"

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs


cd "${OUTDIR}/rootfs/home/"
sudo cp ${OLD_PATH}/writer ${OUTDIR}/rootfs/home/
sudo cp ${OLD_PATH}/finder.sh  ${OUTDIR}/rootfs/home/
sudo cp ${OLD_PATH}/conf/username.txt ${OUTDIR}/rootfs/home/conf/
sudo cp ${OLD_PATH}/finder-test.sh ${OUTDIR}/rootfs/home/
sudo cp ${OLD_PATH}/autorun-qemu.sh ${OUTDIR}/rootfs/home/

# TODO: Chown the root directory
cd "${OUTDIR}"
sudo chown -R root:root *

# TODO: Create initramfs.cpio.gz
cd "${OUTDIR}/rootfs"
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
echo "-----completed initramfs step find"

cd ..
gzip -f initramfs.cpio

echo "-----gzip completed"

