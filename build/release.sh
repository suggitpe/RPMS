#!/bin/sh

#modify the following for install specifics
CODE_BASE=/home/suggitpe/codebase/RPMS/trunk/
INST_DIR=/home/suggitpe/RPMS/
WORK_AREA=/home/suggitpe/test/RPMS/
TESTAPP_BASE=/home/suggitpe/RpmsTestApps/
DEST_SVRS="pgdsX00 pgdsX01 pgdsX02"
USER=suggitpe

## do not modify the following
VERSION=v1.2.0
SRC_DIR=${CODE_BASE}/build/install/
TAR_ARCH=${CODE_BASE}/_archiveTars
TAR_NAME=RPMS_install_${VERSION}.tar

echo "************************************ Setting up perms"
cd ${SRC_DIR}
chmod 755 *
chmod -R 444 lib/*
chmod -R 555 bin/*

echo "************************************ Creating TAR file"
cd ${SRC_DIR}; rm -f ${SRC_DIR}/${TAR_NAME}; tar -cf ${SRC_DIR}/${TAR_NAME} bin/* lib/* doc/*
cd ${SRC_DIR}/../..; tar -rf ${SRC_DIR}/${TAR_NAME} xml/dtd/*
cd ${SRC_DIR}/../..; tar -rf ${SRC_DIR}/${TAR_NAME} xml/xsl/*
gzip -f ${SRC_DIR}/${TAR_NAME}

for a in ${DEST_SVRS}
do
    echo "************************************ Releasing RPMS ${VERSION} to $a"
    ssh ${a} chmod -R u+w ${INST_DIR}/${VERSION}
    ssh ${a} chmod -R u+w ${INST_DIR}/${VERSION}/*
    ssh ${a} rm -fr ${INST_DIR}/${VERSION}
    ssh ${a} mkdir -p ${INST_DIR}/${VERSION}
    scp ${SRC_DIR}/${TAR_NAME}.gz ${USER}@${a}:${INST_DIR}/${VERSION}
    ssh ${a} gunzip ${INST_DIR}/${VERSION}/${TAR_NAME}.gz
    ssh ${a} tar --touch -xf ${INST_DIR}/${VERSION}/${TAR_NAME} --directory ${INST_DIR}/${VERSION}
    ssh ${a} rm ${INST_DIR}/${VERSION}/${TAR_NAME}
    ssh ${a} mkdir -p ${WORK_AREA}/${VERSION}/config
    ssh ${a} mkdir -p ${WORK_AREA}/${VERSION}/rpmstest
    scp ${CODE_BASE}/config/* ${USER}@${a}:${WORK_AREA}/${VERSION}/config/
    scp ${CODE_BASE}/test/rpmstestscripts/*.pl ${USER}@${a}:${WORK_AREA}/${VERSION}/rpmstest/
    ssh ${a} chmod -R a-w ${INST_DIR}/${VERSION}/*
    ssh ${a} mkdir -p ${TESTAPP_BASE}/bin
    scp ${CODE_BASE}/test/testapps/* ${USER}@${a}:${TESTAPP_BASE}/bin
    ssh ${a} chmod -R a+w ${TESTAPP_BASE}/bin
    ssh ${a} chmod -R a+w ${TESTAPP_BASE}/bin
done

mv ${SRC_DIR}/${TAR_NAME}.gz ${TAR_ARCH}
${CODE_BASE}/createBackup.pl 
