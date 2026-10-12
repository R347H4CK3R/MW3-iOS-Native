#!/usr/bin/env python3
"""Minimal deliberately bounded x86 PE32 -> portable C lift.
This validates mechanics on a generated toy PE, NOT general IW5 translation.
Supports only: mov eax,imm32; add eax,imm32; ret. Rejects all others.
"""
import argparse
from pathlib import Path
import struct

def make_fixture(path):
    code = b"\xB8" + struct.pack("<I", 40) + b"\x05" + struct.pack("<I", 2) + b"\xC3"
    hdr = bytearray(0x200)
    hdr[:2] = b"MZ"
    struct.pack_into("<I", hdr, 0x3c, 0x80)
    hdr[0x80:0x84] = b"PE\0\0"
    struct.pack_into("<HHIIIHH", hdr, 0x84, 0x14c, 1, 0, 0, 0, 0xe0, 0x102)
    opt=0x98
    struct.pack_into("<H",hdr,opt,0x10b)
    struct.pack_into("<I",hdr,opt+16,0x1000)
    struct.pack_into("<I",hdr,opt+28,0x400000)
    struct.pack_into("<I",hdr,opt+32,0x1000)
    struct.pack_into("<I",hdr,opt+36,0x200)
    struct.pack_into("<I",hdr,opt+56,0x2000)
    struct.pack_into("<I",hdr,opt+60,0x200)
    section=opt+0xe0
    hdr[section:section+8]=b".text\0\0\0"
    struct.pack_into("<IIIIIIHHI",hdr,section+8,len(code),0x1000,0x200,0x200,0,0,0,0,0x60000020)
    path.write_bytes(hdr + code.ljust(0x200,b"\x00"))

def lift(path):
    data=path.read_bytes()
    if len(data)<0x400 or data[:2]!=b"MZ": raise ValueError("Not a PE fixture")
    pe=struct.unpack_from("<I",data,0x3c)[0]
    if data[pe:pe+4]!=b"PE\0\0": raise ValueError("Invalid PE signature")
    machine,sections=struct.unpack_from("<HH",data,pe+4)
    opt=pe+24
    if machine!=0x14c or sections!=1 or struct.unpack_from("<H",data,opt)[0]!=0x10b:
        raise ValueError("Only tiny PE32 x86 fixture supported")
    entry=struct.unpack_from("<I",data,opt+16)[0]
    section=opt+0xe0
    virtual,rawsize,rawptr=struct.unpack_from("<III",data,section+12)
    if not virtual <= entry < virtual + rawsize: raise ValueError("Invalid entry")
    pos=rawptr+(entry-virtual)
    ops=[]
    while pos<len(data) and len(ops)<16:
        op=data[pos]
        if op in (0xb8,0x05):
            if pos+5>len(data): raise ValueError("Truncated instruction")
            imm=struct.unpack_from("<I",data,pos+1)[0]
            ops.append(("mov" if op==0xb8 else "add",imm))
            pos+=5
        elif op==0xc3:
            ops.append(("ret",0))
            break
        else: raise ValueError(f"Unsupported x86 instruction 0x{op:02x}")
    if not ops or ops[-1][0]!="ret": raise ValueError("No terminating ret")
    lines=["#include <stdint.h>","#include <stdio.h>","static uint32_t lifted(void) {","  uint32_t eax=0;"]
    for operation,imm in ops:
        if operation=="mov": lines.append(f"  eax=UINT32_C({imm});")
        elif operation=="add": lines.append(f"  eax+=UINT32_C({imm});")
    lines+=["  return eax;","}","int main(void) { printf(\"%u\\n\", lifted()); return 0; }"]
    return "\n".join(lines)+"\n"

if __name__=="__main__":
    a=argparse.ArgumentParser()
    a.add_argument("--fixture",type=Path)
    a.add_argument("--input",type=Path)
    a.add_argument("--output",type=Path)
    x=a.parse_args()
    if x.fixture: make_fixture(x.fixture)
    if x.input and x.output: x.output.write_text(lift(x.input))
