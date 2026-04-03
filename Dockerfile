FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ wget libssl-dev ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN wget -q https://raw.githubusercontent.com/yhirose/cpp-httplib/v0.15.3/httplib.h -O httplib.h \
 && wget -q https://raw.githubusercontent.com/nlohmann/json/v3.11.3/single_include/nlohmann/json.hpp -O json.hpp

COPY main.cpp .

RUN mkdir -p kundali_files

RUN g++ -std=c++17 -O2 -Wno-return-type -o kundali main.cpp -lpthread -lssl -lcrypto

EXPOSE 8080

CMD ["./kundali"]
