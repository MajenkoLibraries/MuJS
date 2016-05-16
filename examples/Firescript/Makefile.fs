#BLOCKS=140
BLOCKS=160
BLOCKSIZE=512

.PHONY: objects/fs.dat

objects/fs.dat:
	dd if=/dev/zero of=$@ bs=${BLOCKSIZE} count=${BLOCKS}
	mkfs.vfat $@
	mkdir tmp
	sudo mount -o loop $@ tmp
	-sudo cp -r files/* tmp/
	sudo umount tmp
	rmdir tmp
