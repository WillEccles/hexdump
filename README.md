# hexdump
A simple command-line utility used to dump files to hex.

# Building
Use `make`.

# Usage
`hexdump <sourcefile> [destfile] [maxbuffer]`

- `sourcefile` is the file to be dump. This is required.
- `destfile` is the file to store the hex result in. This is optional (stdout will be used if not specified).
- `maxbuffer` is the number of bytes that can be buffered. 0 will not buffer, writing each byte to the output file as they are read. If a buffer is specified, a buffer will be used to store data between reading and writing.
