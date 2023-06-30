FROM espressif/idf:release-v4.4
RUN mkdir -pv /root/workspace/cmakebuild
WORKDIR /root/workspace
COPY sdkconfig.defaults README.md CMakeLists.txt partitions.csv .
COPY components ./components
COPY main ./main
RUN /bin/bash -c "source /opt/esp/idf/export.sh && cmake -B cmakebuild -S . && cd cmakebuild && make"
WORKDIR /root/workspace/cmakebuild

CMD ["/bin/bash"]
