FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ wget libssl-dev ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Download header-only libraries
RUN wget -q https://raw.githubusercontent.com/yhirose/cpp-httplib/v0.15.3/httplib.h -O httplib.h \
 && wget -q https://raw.githubusercontent.com/nlohmann/json/v3.11.3/single_include/nlohmann/json.hpp -O json.hpp

# Copy source file
COPY main.cpp .

# Create folder for saved kundali txt files
RUN mkdir -p kundali_files

# Compile — added -lstdc++fs for std::filesystem support
RUN g++ -std=c++17 -O2 -o kundali main.cpp -lpthread -lssl -lcrypto -lstdc++fs

EXPOSE 8080

CMD ["./kundali"]
