FROM alpine:3.12

RUN apk --no-cache add bash wget ncurses file p7zip curl
COPY . /opt/

ENTRYPOINT [ "/opt/kpi.sh" ]
