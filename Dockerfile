FROM espressif/idf:release-v4.4
RUN mkdir /home/workspace
RUN mkdir /home/workspace/cmakebuild
COPY components /home/workspace/components
COPY main /home/workspace/main
COPY CMakeLists.txt /home/workspace/
COPY partitions.csv /home/workspace/
COPY README.md /home/workspace/
COPY sdkconfig.defaults /home/workspace/
WORKDIR /home/workspace/cmakebuild
RUN /bin/bash -c "source /opt/esp/idf/export.sh"
ENV PATH="/opt/esp/tools/cmake/3.20.3/bin:${PATH}"
ENV PATH="/opt/esp/idf/components/esptool_py/esptool:${PATH}"
ENV PATH="/opt/esp/idf/components/espcoredump:${PATH}"
ENV PATH="/opt/esp/idf/components/partition_table:${PATH}"
ENV PATH="/opt/esp/idf/components/app_update:${PATH}"
ENV PATH="/opt/esp/tools/xtensa-esp32-elf/esp-2021r2-8.4.0/xtensa-esp32-elf/bin:${PATH}"
ENV PATH="/opt/esp/tools/xtensa-esp32s2-elf/esp-2021r2-8.4.0/xtensa-esp32s2-elf/bin:${PATH}"
ENV PATH="/opt/esp/tools/xtensa-esp32s3-elf/esp-2021r2-8.4.0/xtensa-esp32s3-elf/bin:${PATH}"
ENV PATH="/opt/esp/tools/riscv32-esp-elf/esp-2021r2-8.4.0/riscv32-esp-elf/bin:${PATH}"
ENV PATH="/opt/esp/tools/esp32ulp-elf/2.28.51-esp-20191205/esp32ulp-elf-binutils/bin:${PATH}"
ENV PATH="/opt/esp/tools/esp32s2ulp-elf/2.28.51-esp-20191205/esp32s2ulp-elf-binutils/bin:${PATH}"
ENV PATH="/opt/esp/tools/cmake/3.20.3/bin:${PATH}"
ENV PATH="/opt/esp/tools/openocd-esp32/v0.10.0-esp32-20211111/openocd-esp32/bin:${PATH}"
ENV PATH="/opt/esp/python_env/idf4.4_py3.8_env/bin:${PATH}"
ENV PATH="/opt/esp/idf/tools:${PATH}"
RUN cmake /home/workspace/
RUN make
