FROM espressif/idf:release-v4.4 as initial
RUN mkdir -pv /root/workspace/cmakebuild
WORKDIR /root/workspace
COPY sdkconfig.defaults README.md CMakeLists.txt partitions.csv .
COPY components ./components

ARG KEYBOARD_FILES_FOLDER=default
FROM initial as keyboard
COPY main main/
ADD keyboards/${KEYBOARD_FILES_FOLDER} ./main
RUN /bin/bash -c "source /opt/esp/idf/export.sh && cmake -B cmakebuild -S . && cd cmakebuild && make"
WORKDIR /root/workspace/cmakebuild

CMD ["/bin/bash"]
