#!/bin/sh

# stolen from ffmpeg configure like a pig
# this is brutal and stupid... then easy and explicit

# Prevent locale nonsense from breaking basic text processing.
LC_ALL=C
export LC_ALL

major=0
minor=0
micro=0
asm_name=cmingcnasm
asm_slib_name=lib$asm_name.so.$major.$minor.$micro
asm_slib_soname=lib$asm_name.so.$major
asm_slib_export_map=lib$asm_name.export_map
asm_dbin_name=$asm_name
dis_name=cmingcndis
dis_slib_name=lib$dis_name.so.$major.$minor.$micro
dis_slib_soname=lib$dis_name.so.$major
dis_slib_export_map=lib$dis_name.export_map
dis_dbin_name=$dis_name

#-------------------------------------------------------------------------------
ulinux_target_arch_asm_srcs='
    ulinux/arch/args_env.s
'
ulinux_c_srcs='
    ulinux/utils/mem.c
    ulinux/utils/ascii/string/vsprintf.c
    ulinux/utils/ascii/block/conv/decimal/decimal.c
    ulinux/utils/ascii/block/conv/hexadecimal/hexadecimal.c
    ulinux/utils/ascii/block/conv/binary/binary.c
'
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
shared_srcs='
    msgs.c
'
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
asm_lib_shared_objs='
    msgs.o
'
asm_lib_ulinux_objs='
    decimal.o
    binary.o
    hexadecimal.o
    vsprintf.o
    mem.o
'
asm_lib_srcs='
    src_parse.c
    m_emit.c
    libcmingcnasm.c	
'
asm_bin_ulinux_objs='
    args_env.o
    mem.o
    vsprintf.o
'
asm_bin_srcs='
    cmingcnasm.c
'
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
dis_lib_shared_objs='
    msgs.o
'
dis_lib_ulinux_objs='
    vsprintf.o
    mem.o
'
dis_lib_srcs='
    libcmingcndis.c
'
dis_bin_ulinux_objs='
    args_env.o
    mem.o
    vsprintf.o
'
dis_bin_srcs='
    cmingcndis.c
'
#-------------------------------------------------------------------------------


clean_do(){
    files="
        $asm_lib_shared_objs
        $asm_lib_ulinux_objs
        $asm_bin_ulinux_objs
        $asm_slib_name
        $asm_dbin_name
        $dis_lib_shared_objs
        $dis_lib_ulinux_objs
        $dis_bin_ulinux_objs
        $dis_slib_name
        $dis_dbin_name
    "

    for src_file in $dis_bin_srcs $dis_lib_srcs $asm_bin_srcs $asm_lib_srcs
    do
        obj=${src_file%.c}
        obj=${obj}.o
        files="$obj $files"
    done

    rm -f $files
    rm -f ulinux/arch
    rmdir ulinux
    exit 0
}

################################################################################

# find source path
if test -f make; then
    src_path=.
else
    src_path=$(cd $(dirname "$0"); pwd)
    echo "$src_path" | grep -q '[[:blank:]]' &&
        die "Out of tree builds are impossible with whitespace in source path."
    test -e "$src_path/config.h" &&
        die "Out of tree builds are impossible with config.h in source dir."
fi

is_in(){
    value=$1
    shift
    for var in $*; do
        [ $var = $value ] && return 0
    done
    return 1
}

append(){
    var=$1
    shift
    eval "$var=\"\$$var $*\""
}

die(){
    echo "$0"
    exit 1
}

die_unknown(){
    echo "Unknown option \"$1\"."
    echo "See $0 --help for available options."
    exit 1
}

set_default(){
    for opt; do
        eval : \${$opt:=\$${opt}_default}
    done
}

CMDLINE_SET='
    arch
    asm
    pic_cc
    slib_ccld_tmpl
    dbin_ccld
'

#command line, set defaults
arch_default=$(uname -m | sed -e s/i.86/x86/ -e s/parisc64/parisc/ -e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/sh.*/sh/)
asm_default=as 
pic_cc_default='gcc -Wall -Wextra -Wno-missing-field-initializers -c -fpic -O0'
slib_ccld_tmpl_default='gcc -shared -nostdlib -Wl,-soname=$soname,--version-script=$export_map'
dbin_ccld_default="gcc -nostdlib"
ln_s_default='ln -sf'

set_default arch asm pic_cc slib_ccld_tmpl dbin_ccld ln_s

show_help(){
    cat <<EOF
Usage: make [options] [operations]

Operations: [default is to build the shared libraries and binaries]:
  clean                    clean build products


Options: [defaults in brackets after descriptions]

Help options:
  --help                   print this message

Advanced options (experts only):
  --arch=ARCH                  use ulinux target ARCH [$arch_default]
  --asm=ASM                    use target arch assembler command line ASM [$asm_default]
  --pic-cc=CC                  use target arch C compiler command line CC for PIC (Position Independant Code) object [$pic_cc_default]
  --slib-ccld-tmpl=CCLD        use linker command line template CCLD for target arch shared lib [$slib_ccld_tmpl_default]
  --dbin-ccld=CCLD             use linker command line CCLD for target arch dynamic binaries [$dbin_ccld_default]
EOF
  exit 0
}

for opt do
    optval="${opt#*=}"
    case "$opt" in
        clean) clean_do
        ;;
        --help|-h) show_help
        ;;
        *)
            optname="${opt%%=*}"
            optname="${optname#--}"
            optname=$(echo "$optname" | sed 's/-/_/g')
            if is_in $optname $CMDLINE_SET; then
                eval $optname='$optval'
            elif is_in $optname $CMDLINE_APPEND; then
                append $optname "$optval"
            else
                die_unknown $opt
            fi
        ;;
    esac
done


#-------------------------------------------------------------------------------
#configure our ultra-thin linux user API abstraction layer
rm -f ulinux/arch
mkdir -p ulinux
$ln_s $src_path/ulinux/archs/$arch ulinux/arch
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#assemble src files
target_arch_asm_srcs="
	$ulinux_target_arch_asm_srcs
"

for src_file in $target_arch_asm_srcs
do
    obj=${src_file%.s}
    obj=${obj}.o
    echo ASM $src_file
    $asm -o $(basename $obj) $src_file
done
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#C compile pic src files
pic_srcs="
	$ulinux_c_srcs
	$shared_srcs
	$asm_lib_srcs
	$asm_bin_srcs
	$dis_lib_srcs
	$dis_bin_srcs
"
for src_file in $pic_srcs
do
    obj=${src_file%.c}
    obj=${obj}.o
    echo PIC_CC $src_file
    $pic_cc -o $(basename $obj) -I./ -I$src_path $src_path/$src_file
    pic_objs="$obj $pic_objs"
done
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#link the asm slib
for src_file in $asm_lib_srcs
do
    obj=${src_file%.c}
    obj=${obj}.o
    asm_slib_objs="$obj $asm_slib_objs"
done
asm_slib_objs="$asm_slib_objs $asm_lib_shared_objs $asm_lib_ulinux_objs"

soname=$asm_slib_soname
export_map=$src_path/$asm_slib_export_map
slib_ccld=$(eval echo $slib_ccld_tmpl)
echo SLIB_CCLD $asm_slib_name
$slib_ccld -o $asm_slib_name $asm_slib_objs
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#link the asm binary
for src_file in $asm_bin_srcs
do
    obj=${src_file%.c}
    obj=${obj}.o
    asm_dbin_objs="$obj $asm_dbin_objs"
done
asm_dbin_objs="$asm_dbin_objs $asm_bin_ulinux_objs"
echo DBIN_CCLD $asm_dbin_name
$dbin_ccld -o $asm_dbin_name $asm_slib_name $asm_dbin_objs
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#link the dis slib
for src_file in $dis_lib_srcs
do
    obj=${src_file%.c}
    obj=${obj}.o
    dis_slib_objs="$obj $dis_slib_objs"
done
dis_slib_objs="$dis_slib_objs $dis_lib_shared_objs $dis_lib_ulinux_objs"

soname=$dis_slib_soname
export_map=$src_path/$dis_slib_export_map
slib_ccld=$(eval echo $slib_ccld_tmpl)
echo SLIB_CCLD $dis_slib_name
$slib_ccld -o $dis_slib_name $dis_slib_objs
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#link the dis binary
for src_file in $dis_bin_srcs
do
    obj=${src_file%.c}
    obj=${obj}.o
    dis_dbin_objs="$obj $dis_dbin_objs"
done
dis_dbin_objs="$dis_dbin_objs $dis_bin_ulinux_objs"
echo DBIN_CCLD $dis_dbin_name
$dbin_ccld -o $dis_dbin_name $dis_slib_name $dis_dbin_objs
#-------------------------------------------------------------------------------
