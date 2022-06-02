This is **q**uick adaptive **b**inary arithmetic **c**oder

Created in intention to replace standard `Huff_Compress()`/`Huff_Decompress()` functions used for infostring compression in `connect` commands, 
**NOT** compatible with existing huffman binary stream

It is using 8-bit probablities along with 32-bit carryless arithmetics and needs less that 1kb memory (for 0-order model)

Range and probability updates implemented in a branchless way so it might be the fastest in that class

Some comparisons with `Huff_` functions:

- compression rate is about 10% better
- encode/decode speed is about 20% faster
- memory usage is more than 50 times lower (<1k vs ~50k)
