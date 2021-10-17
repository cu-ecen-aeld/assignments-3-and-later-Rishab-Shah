# Output log from when echo “hello_world” > /dev/faulty is run <br /> 
Unable to handle kernel NULL pointer dereference at virtual address 0000000000000000 <br /> 
Mem abort info: <br /> 
  ESR = 0x96000046 <br /> 
  EC = 0x25: DABT (current EL), IL = 32 bits <br /> 
  SET = 0, FnV = 0 <br /> 
  EA = 0, S1PTW = 0 <br /> 
Data abort info: <br /> 
  ISV = 0, ISS = 0x00000046 <br /> 
  CM = 0, WnR = 1 <br /> 
user pgtable: 4k pages, 39-bit VAs, pgdp=0000000041fe5000 <br /> 
[0000000000000000] pgd=0000000042084003, p4d=0000000042084003, pud=0000000042084003, pmd=0000000000000000 <br /> 
Internal error: Oops: 96000046 [#3] SMP <br />  
Modules linked in: hello(O) faulty(O) scull(O) <br /> 
CPU: 0 PID: 159 Comm: sh Tainted: G      D    O      5.10.7 #1 <br /> 
Hardware name: linux,dummy-virt (DT) <br /> 
pstate: 80000005 (Nzcv daif -PAN -UAO -TCO BTYPE=--) <br /> 
pc : faulty_write+0x10/0x20 [faulty] <br /> 
lr : vfs_write+0xc0/0x290 <br /> 
sp : ffffffc010c53db0 <br /> 
x29: ffffffc010c53db0 x28: ffffff8001ff0000 <br /> 
x27: 0000000000000000 x26: 0000000000000000 <br />
x25: 0000000000000000 x24: 0000000000000000 <br />
x23: 0000000000000000 x22: ffffffc010c53e30 <br />
x21: 00000000004c9900 x20: ffffff8001f9fb00 <br />
x19: 0000000000000012 x18: 0000000000000000 <br />
x17: 0000000000000000 x16: 0000000000000000 <br />
x15: 0000000000000000 x14: 0000000000000000 <br />
x13: 0000000000000000 x12: 0000000000000000 <br />
x11: 0000000000000000 x10: 0000000000000000 <br />
x9 : 0000000000000000 x8 : 0000000000000000 <br />
x7 : 0000000000000000 x6 : 0000000000000000 <br />
x5 : ffffff8002087ce8 x4 : ffffffc008677000 <br />
x3 : ffffffc010c53e30 x2 : 0000000000000012 <br />
x1 : 0000000000000000 x0 : 0000000000000000 <br />
Call trace: <br />
 faulty_write+0x10/0x20 [faulty] <br />
 ksys_write+0x6c/0x100 <br />
 __arm64_sys_write+0x1c/0x30 <br />
 el0_svc_common.constprop.0+0x9c/0x1c0 <br />
 do_el0_svc+0x70/0x90 <br />
 el0_svc+0x14/0x20 <br />
 el0_sync_handler+0xb0/0xc0 <br />
 el0_sync+0x174/0x180 <br />
Code: d2800001 d2800000 d503233f d50323bf (b900003f)  <br />
---[ end trace 5f56549fa8e1c4b8 ]--- <br />
 
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Objdump output:
/home/rishab/assignment-4-Rishab-Shah/buildroot/output/build/ldd-7dae648a9f304151b0e268a62d0d2a1007793b4f/misc-modules/faulty.ko:     file format elf64-littleaarch64
<br />

Disassembly of section .text: <br />

0000000000000000 <faulty_write>: <br />
   0:   d2800001        mov     x1, #0x0                        // #0    <br />
   4:   d2800000        mov     x0, #0x0                        // #0    <br />
   8:   d503233f        paciasp                                          <br />
   c:   d50323bf        autiasp                                          <br />
  10:   b900003f        str     wzr, [x1]                                <br />
  14:   d65f03c0        ret                                              <br />
  18:   d503201f        nop                                              <br />
  1c:   d503201f        nop                                              <br />


-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Explanation:
From the call trace it is clear that, the problem occured when faulty_write was called. <br />
On performing the disassembly of the faulty.ko file, it is seen that the 0 is updated on a memory address 0. <br />
i.e. a invalid memory location is accessed on memory location 4. <br />

This would cause to update any random memory and create issue. <br />
Hence, when this happens the qemu reboots to fix the error. <br />

----------------------------------------------------------------------------------------------------------------------------------------------------------------------

