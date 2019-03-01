FROM ubuntu:16.04 as build

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y libsqlite3-dev libsmbclient-dev libssh2-1-dev libssh-dev libaio-dev $EXTRALIBS \
        mysql-server \
        php php-cli php-gd php-imap php-mysql php-curl php-readline \
	    libmysqlclient-dev build-essential libmatheval-dev libmagic-dev \
        libgd-dev rsync valgrind-dbg libxml2-dev \
	    cmake ssh phpmyadmin \
        libssh-dev curl build-essential python \
        ca-certificates git openssh-server openssh-sftp-server libwebsockets-dev

ADD . /friendup

RUN mkdir -p /friendup/build/cfg/crt

WORKDIR /friendup

RUN export FRIEND_FOLDER="/friendup"
RUN export FRIEND_BUILD="${FRIEND_FOLDER}/build"
RUN export CFG_PATH="${FRIEND_BUILD}/cfg/cfg.ini"


#            $SUDO openssl req -newkey rsa:2048 -nodes -sha512 -x509 -days 3650 -nodes -out "$FRIEND_BUILD/cfg/crt/certificate.pem" -keyout "$FRIEND_BUILD/cfg/crt/key.pem"
#            $SUDO ln -s "$keyPath" "$FRIEND_BUILD/cfg/crt/key.pem"
#            $SUDO ln -s "$certificatePath" "$FRIEND_BUILD/cfg/crt/certificate.pem"

RUN make clean setup release install

# Required for about window
COPY repository build/resources/repository

# Required to bootstrap / update database
COPY db build/db

FROM ubuntu:16.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y libsqlite3-dev libsmbclient-dev libssh2-1-dev libssh-dev libaio-dev $EXTRALIBS \
        mysql-client \
        php php-cli php-gd php-imap php-mysql php-curl php-readline \
            libmysqlclient-dev build-essential libmatheval-dev libmagic-dev \
        libgd-dev rsync valgrind-dbg libxml2-dev \
            cmake ssh phpmyadmin \
        libssh-dev curl python \
        ca-certificates git openssh-server openssh-sftp-server libwebsockets-dev

ENV FRIEND_FOLDER="/friendup"
ENV CFG_PATH="${FRIEND_FOLDER}/cfg/cfg.ini"

RUN mkdir -p ${FRIEND_FOLDER}/cfg/crt

COPY --from=build /friendup/build ${FRIEND_FOLDER}

ADD db db
ADD cfg.ini ${CFG_PATH}

WORKDIR ${FRIEND_FOLDER}

ADD entrypoint.sh entrypoint.sh

RUN chmod u+x entrypoint.sh

EXPOSE 6500
EXPOSE 6502

CMD ./entrypoint.sh
