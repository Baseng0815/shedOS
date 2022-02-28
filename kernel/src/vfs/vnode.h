#ifndef _VNODE_H
#define _VNODE_H

struct vnode_ops {
        void (*read)(uint8_t *dst, size_t offset, size_t len);
        void (*write)(const uint8_t *src, size_t offset, size_t len);
};

struct vnode {
        struct vnode_ops ops;
};

#endif
