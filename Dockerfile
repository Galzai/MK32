FROM espressif/idf:release-v4.4
RUN mkdir -pv /root/workspace/cmakebuild
COPY components /home/workspace/components
COPY main /home/workspace/main
COPY CMakeLists.txt /home/workspace/
COPY partitions.csv /home/workspace/
COPY README.md /home/workspace/
COPY sdkconfig.defaults /home/workspace/
WORKDIR /home/workspace/cmakebuild
RUN /bin/bash -c "source /opt/esp/idf/export.sh"
RUN cmake /home/workspace/
RUN make
