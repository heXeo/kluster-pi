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
/nodes/[id]/image -> /images/[id]
/nodes/[id]/rootfs_rw
/nodes/[id]/rootfs_work
/nodes/[id]/rootfs (overlay lower=image/rootfs,upper=rootfs_rw,work=rootfs_work rootfs)
/nodes/[id]/bootfs_rw
/nodes/[id]/bootfs_work
/nodes/[id]/bootfs (overlay lower=image/bootfs,upper=bootfs_rw,work=bootfs_work bootfs)

/tftp/bootcode.bin -> /bootcode/[id]/bootcode.bin
/tftp/[serial] -> /nodes/[id]/bootfs

/nfs/[node_id]/rootfs -> /nodes/[id]/rootfs
/nfs/[node_id]/bootfs -> /nodes/[id]/bootfs
```

# CLI

## bootcode

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

- [ ] node create <name> <img_id>
- [x] node rm <id>
- [x] node ls
- [x] node rename <id> <name>
- [x] node setimg <id>
- [x] node attach <id> <serial>
- [x] node detach <id>
