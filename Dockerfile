FROM espressif/idf:release-v4.4
RUN mkdir -pv /root/workspace/cmakebuild
WORKDIR /root/workspace
COPY components ./components
COPY main ./main
COPY CMakeLists.txt ./
COPY partitions.csv ./
COPY README.md ./
COPY sdkconfig.defaults ./
RUN /bin/bash -c "source /opt/esp/idf/export.sh"
RUN cmake /home/workspace/
RUN make
