#ifndef F_RING_BUFFER_H
#define F_RING_BUFFER_H

internal u64 ring_buffer_write(u8 *ring_base, u64 ring_size, u64 pos, void *src, u64 write_size);
internal u64 ring_buffer_read(u8 *ring_base, u64 ring_size, u64 pos, void *dst, u64 read_size);
#define RingWriteStruct(base, size, pos, ptr) RingWrite((base), (size), (pos), (ptr), sizeof(*(ptr)))
#define RingReadStruct(base, size, pos, ptr) RingRead((base), (size), (pos), (ptr), sizeof(*(ptr)))

#endif // F_RING_BUFFER_H