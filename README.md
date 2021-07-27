# Depends on

```
linux-kernel >= 4.16
bash >= 4
util-linux
dnsmasq
p7zip
curl
file
nfs-kernel-server
ncurses-bin
```

# Dirs

```text
/bootcode/[id]/id
/bootcode/[id]/name
/bootcode/[id]/bootcode.bin

/images/current_id
/images/[id]/id
/images/[id]/name
/images/[id]/bootfs
/images/[id]/rootfs

/nodes/current_id
/nodes/[id]/id
/nodes/[id]/name
/nodes/[id]/serial
/nodes/[id]/options
/nodes/[id]/image -> /images/[id]
/nodes/[id]/bootfs -> rootfs/boot
/nodes/[id]/rootfs_ro -> /images/[id]/rootfs
/nodes/[id]/rootfs_rw
/nodes/[id]/rootfs_work
/nodes/[id]/rootfs (overlay lower=image/rootfs,upper=rootfs_rw,work=rootfs_work rootfs)

/tftp/bootcode.bin -> /bootcode/[id]/bootcode.bin
/tftp/[serial] -> /nodes/[id]/bootfs
```

# CLI

## bootcode (only required for rpi < 4)

- [ ] bootcode get <id> <img_id>
- [ ] bootcode rm <id>
- [ ] bootcode ls
- [ ] bootcode set <id>
- [ ] bootcode rename <id> <name>

## images

- [x] image get <name> <url>
- [x] image rm <id>
- [x] image ls
- [x] image rename <id> <name>

## nodes

- [x] node create <name> [img_id]
- [x] node rm <id>
- [x] node ls
- [x] node rename <id> <name>
- [x] node setimg <id> <img_id>
- [x] node attach <id> <serial>
- [x] node detach <id>
- [ ] node option add <id> <key> <value>
- [ ] node option del <id> <key>
- [ ] node option show <id>

## nfs

- [ ] nfs start
- [ ] nfs stop
- [ ] nfs restart
- [ ] nfs status
- [ ] nfs clients
- [ ] nfs endpoints

## dnsmasq

- [ ] dnsmasq start
- [ ] dnsmasq stop
- [ ] dnsmasq restart
- [ ] dnsmasq status
- [ ] dnsmasq range [ip]

# /etc/default/dnsmasq

```
ENABLED=1
DNSMASQ_OPTS="-p0"
```

# /etc/dnsmasq.conf

```
dhcp-range=10.211.55.255,proxy
log-dhcp
enable-tftp
tftp-root=/var/lib/kpi/tftp
pxe-service=0,"Raspberry Pi Boot"
```
