FROM centos:7
MAINTAINER Denis Kolodin <deniskolodin@gmail.com>

EXPOSE 8000

ENV SOURCES=/sources

RUN yum update -y
RUN yum install -y file gcc gcc-c++ openssl-devel make sqlite sqlite-devel mysql mysql-devel postgresql postgresql-devel 
RUN curl -sSf https://static.rust-lang.org/rustup.sh | sh -s -- --disable-sudo --channel=nightly

RUN mkdir -p $SOURCES
ADD ./ $SOURCES
WORKDIR $SOURCES/backend
RUN cargo install diesel_cli
RUN ~/.cargo/bin/diesel setup
RUN cargo build --release

CMD cargo run --release
