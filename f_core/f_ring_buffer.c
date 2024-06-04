internal u64
ring_buffer_write(u8 *ring_base, u64 ring_size, u64 pos, void *src, u64 write_size) {
  write_size = Min(write_size, ring_size);
  u64 first_part_write_off = pos%ring_size;
  u64 second_part_write_off = 0;
  String first_part = string((u8 *)src, write_size);
  String second_part = StringLiteral("");
  if(first_part_write_off + write_size > ring_size) {
    first_part.size = ring_size - first_part_write_off;
    second_part = string((u8 *)src + first_part.size, write_size-first_part.size);
  }
  if(first_part.size != 0) {
    MemoryCopy(ring_base + first_part_write_off, first_part.str, first_part.size);
  }
  if(second_part.size != 0) {
    MemoryCopy(ring_base + second_part_write_off, second_part.str, second_part.size);
  }
  return write_size;
}

internal u64
ring_buffer_read(u8 *ring_base, u64 ring_size, u64 pos, void *dst, u64 read_size) {
  read_size = Min(read_size, ring_size);
  u64 first_part_read_off = pos%ring_size;
  u64 second_part_read_off = 0;
  u64 first_part_read_size = read_size;
  u64 second_part_read_size = 0;
  if(first_part_read_off + read_size > ring_size) {
    first_part_read_size = ring_size - first_part_read_off;
    second_part_read_size = read_size - first_part_read_size;
  }
  if(first_part_read_size != 0) {
    MemoryCopy(dst, ring_base + first_part_read_off, first_part_read_size);
  }
  if(second_part_read_size != 0) {
    MemoryCopy((u8 *)dst + first_part_read_size, ring_base + second_part_read_off, second_part_read_size);
  }
  return read_size;
}
