              .text
prepare_isr_common:
              //    Push general purpose registers.
              pusha
              mov   %esp, %edx

              //    Emulate the frame pointer so that call stack traces work
              //    across interrupts.
              //
              //    The interrupted eip:
              mov   0x28(%esp), %ecx
              //    Add 5 because this is the instruction that was interrupted,
              //    not the return address of a call.  print_call_stack
              //    subtracts 5 from the saved eip so that the printed address
              //    is the call instruction.  Adding 5 undoes that.
              add   $5, %ecx
              push  %ecx
              //    The interrupted ebp:
              push  0x0c(%esp)
              //    Set ebp to the stack.
              mov   %esp, %ebp

              //    Push segment selectors.
              push  %ds
              push  %es
              push  %fs
              push  %gs

              //    Set the segment selectors to good values.
              mov   $0x10, %ax
              mov   %ax, %ds
              mov   %ax, %es
              mov   %ax, %fs
              mov   %ax, %gs

              //    Call isr_common with a pointer to the isr_frame.
              push  %edx
              call  isr_common
              add   $0x4, %esp

              //    Pop the segment selectors.
              pop %gs
              pop %fs
              pop %es
              pop %ds

              //    Remove the emulated frame pointer.
              add   $0x8, %esp

              //    Pop the general purpose registers.
              popa

              //    Pop vector and error code.
              add   $0x8, %esp

              //    Return to the caller.
              iret

              .macro define_isr n
isr_\()\n\():
              push  $0x0            // Push error code.
              push  $0x\n           // Push vector.
              jmp   prepare_isr_common
              .endm

              .macro define_isr_with_error_code n
isr_\()\n\():
              // Error code is pushed by CPU.
              push  $0x\n           // Push vector.
              jmp   prepare_isr_common
              .endm

              define_isr 00
              define_isr 01
              define_isr 02
              define_isr 03
              define_isr 04
              define_isr 05
              define_isr 06
              define_isr 07
              define_isr_with_error_code 08
              define_isr 09
              define_isr_with_error_code 0a
              define_isr_with_error_code 0b
              define_isr_with_error_code 0c
              define_isr_with_error_code 0d
              define_isr_with_error_code 0e
              define_isr 0f
              define_isr 10
              define_isr_with_error_code 11
              define_isr 12
              define_isr 13
              define_isr 14
              define_isr 15
              define_isr 16
              define_isr 17
              define_isr 18
              define_isr 19
              define_isr 1a
              define_isr 1b
              define_isr 1c
              define_isr 1d
              define_isr 1e
              define_isr 1f
              define_isr 20
              define_isr 21
              define_isr 22
              define_isr 23
              define_isr 24
              define_isr 25
              define_isr 26
              define_isr 27
              define_isr 28
              define_isr 29
              define_isr 2a
              define_isr 2b
              define_isr 2c
              define_isr 2d
              define_isr 2e
              define_isr 2f
              define_isr 30
              define_isr 31
              define_isr 32
              define_isr 33
              define_isr 34
              define_isr 35
              define_isr 36
              define_isr 37
              define_isr 38
              define_isr 39
              define_isr 3a
              define_isr 3b
              define_isr 3c
              define_isr 3d
              define_isr 3e
              define_isr 3f
              define_isr 40
              define_isr 41
              define_isr 42
              define_isr 43
              define_isr 44
              define_isr 45
              define_isr 46
              define_isr 47
              define_isr 48
              define_isr 49
              define_isr 4a
              define_isr 4b
              define_isr 4c
              define_isr 4d
              define_isr 4e
              define_isr 4f
              define_isr 50
              define_isr 51
              define_isr 52
              define_isr 53
              define_isr 54
              define_isr 55
              define_isr 56
              define_isr 57
              define_isr 58
              define_isr 59
              define_isr 5a
              define_isr 5b
              define_isr 5c
              define_isr 5d
              define_isr 5e
              define_isr 5f
              define_isr 60
              define_isr 61
              define_isr 62
              define_isr 63
              define_isr 64
              define_isr 65
              define_isr 66
              define_isr 67
              define_isr 68
              define_isr 69
              define_isr 6a
              define_isr 6b
              define_isr 6c
              define_isr 6d
              define_isr 6e
              define_isr 6f
              define_isr 70
              define_isr 71
              define_isr 72
              define_isr 73
              define_isr 74
              define_isr 75
              define_isr 76
              define_isr 77
              define_isr 78
              define_isr 79
              define_isr 7a
              define_isr 7b
              define_isr 7c
              define_isr 7d
              define_isr 7e
              define_isr 7f
              define_isr 80
              define_isr 81
              define_isr 82
              define_isr 83
              define_isr 84
              define_isr 85
              define_isr 86
              define_isr 87
              define_isr 88
              define_isr 89
              define_isr 8a
              define_isr 8b
              define_isr 8c
              define_isr 8d
              define_isr 8e
              define_isr 8f
              define_isr 90
              define_isr 91
              define_isr 92
              define_isr 93
              define_isr 94
              define_isr 95
              define_isr 96
              define_isr 97
              define_isr 98
              define_isr 99
              define_isr 9a
              define_isr 9b
              define_isr 9c
              define_isr 9d
              define_isr 9e
              define_isr 9f
              define_isr a0
              define_isr a1
              define_isr a2
              define_isr a3
              define_isr a4
              define_isr a5
              define_isr a6
              define_isr a7
              define_isr a8
              define_isr a9
              define_isr aa
              define_isr ab
              define_isr ac
              define_isr ad
              define_isr ae
              define_isr af
              define_isr b0
              define_isr b1
              define_isr b2
              define_isr b3
              define_isr b4
              define_isr b5
              define_isr b6
              define_isr b7
              define_isr b8
              define_isr b9
              define_isr ba
              define_isr bb
              define_isr bc
              define_isr bd
              define_isr be
              define_isr bf
              define_isr c0
              define_isr c1
              define_isr c2
              define_isr c3
              define_isr c4
              define_isr c5
              define_isr c6
              define_isr c7
              define_isr c8
              define_isr c9
              define_isr ca
              define_isr cb
              define_isr cc
              define_isr cd
              define_isr ce
              define_isr cf
              define_isr d0
              define_isr d1
              define_isr d2
              define_isr d3
              define_isr d4
              define_isr d5
              define_isr d6
              define_isr d7
              define_isr d8
              define_isr d9
              define_isr da
              define_isr db
              define_isr dc
              define_isr dd
              define_isr de
              define_isr df
              define_isr e0
              define_isr e1
              define_isr e2
              define_isr e3
              define_isr e4
              define_isr e5
              define_isr e6
              define_isr e7
              define_isr e8
              define_isr e9
              define_isr ea
              define_isr eb
              define_isr ec
              define_isr ed
              define_isr ee
              define_isr ef
              define_isr f0
              define_isr f1
              define_isr f2
              define_isr f3
              define_isr f4
              define_isr f5
              define_isr f6
              define_isr f7
              define_isr f8
              define_isr f9
              define_isr fa
              define_isr fb
              define_isr fc
              define_isr fd
              define_isr fe
              define_isr ff

              .section .rodata
              .global isr_array
isr_array:
              .long isr_00
              .long isr_01
              .long isr_02
              .long isr_03
              .long isr_04
              .long isr_05
              .long isr_06
              .long isr_07
              .long isr_08
              .long isr_09
              .long isr_0a
              .long isr_0b
              .long isr_0c
              .long isr_0d
              .long isr_0e
              .long isr_0f
              .long isr_10
              .long isr_11
              .long isr_12
              .long isr_13
              .long isr_14
              .long isr_15
              .long isr_16
              .long isr_17
              .long isr_18
              .long isr_19
              .long isr_1a
              .long isr_1b
              .long isr_1c
              .long isr_1d
              .long isr_1e
              .long isr_1f
              .long isr_20
              .long isr_21
              .long isr_22
              .long isr_23
              .long isr_24
              .long isr_25
              .long isr_26
              .long isr_27
              .long isr_28
              .long isr_29
              .long isr_2a
              .long isr_2b
              .long isr_2c
              .long isr_2d
              .long isr_2e
              .long isr_2f
              .long isr_30
              .long isr_31
              .long isr_32
              .long isr_33
              .long isr_34
              .long isr_35
              .long isr_36
              .long isr_37
              .long isr_38
              .long isr_39
              .long isr_3a
              .long isr_3b
              .long isr_3c
              .long isr_3d
              .long isr_3e
              .long isr_3f
              .long isr_40
              .long isr_41
              .long isr_42
              .long isr_43
              .long isr_44
              .long isr_45
              .long isr_46
              .long isr_47
              .long isr_48
              .long isr_49
              .long isr_4a
              .long isr_4b
              .long isr_4c
              .long isr_4d
              .long isr_4e
              .long isr_4f
              .long isr_50
              .long isr_51
              .long isr_52
              .long isr_53
              .long isr_54
              .long isr_55
              .long isr_56
              .long isr_57
              .long isr_58
              .long isr_59
              .long isr_5a
              .long isr_5b
              .long isr_5c
              .long isr_5d
              .long isr_5e
              .long isr_5f
              .long isr_60
              .long isr_61
              .long isr_62
              .long isr_63
              .long isr_64
              .long isr_65
              .long isr_66
              .long isr_67
              .long isr_68
              .long isr_69
              .long isr_6a
              .long isr_6b
              .long isr_6c
              .long isr_6d
              .long isr_6e
              .long isr_6f
              .long isr_70
              .long isr_71
              .long isr_72
              .long isr_73
              .long isr_74
              .long isr_75
              .long isr_76
              .long isr_77
              .long isr_78
              .long isr_79
              .long isr_7a
              .long isr_7b
              .long isr_7c
              .long isr_7d
              .long isr_7e
              .long isr_7f
              .long isr_80
              .long isr_81
              .long isr_82
              .long isr_83
              .long isr_84
              .long isr_85
              .long isr_86
              .long isr_87
              .long isr_88
              .long isr_89
              .long isr_8a
              .long isr_8b
              .long isr_8c
              .long isr_8d
              .long isr_8e
              .long isr_8f
              .long isr_90
              .long isr_91
              .long isr_92
              .long isr_93
              .long isr_94
              .long isr_95
              .long isr_96
              .long isr_97
              .long isr_98
              .long isr_99
              .long isr_9a
              .long isr_9b
              .long isr_9c
              .long isr_9d
              .long isr_9e
              .long isr_9f
              .long isr_a0
              .long isr_a1
              .long isr_a2
              .long isr_a3
              .long isr_a4
              .long isr_a5
              .long isr_a6
              .long isr_a7
              .long isr_a8
              .long isr_a9
              .long isr_aa
              .long isr_ab
              .long isr_ac
              .long isr_ad
              .long isr_ae
              .long isr_af
              .long isr_b0
              .long isr_b1
              .long isr_b2
              .long isr_b3
              .long isr_b4
              .long isr_b5
              .long isr_b6
              .long isr_b7
              .long isr_b8
              .long isr_b9
              .long isr_ba
              .long isr_bb
              .long isr_bc
              .long isr_bd
              .long isr_be
              .long isr_bf
              .long isr_c0
              .long isr_c1
              .long isr_c2
              .long isr_c3
              .long isr_c4
              .long isr_c5
              .long isr_c6
              .long isr_c7
              .long isr_c8
              .long isr_c9
              .long isr_ca
              .long isr_cb
              .long isr_cc
              .long isr_cd
              .long isr_ce
              .long isr_cf
              .long isr_d0
              .long isr_d1
              .long isr_d2
              .long isr_d3
              .long isr_d4
              .long isr_d5
              .long isr_d6
              .long isr_d7
              .long isr_d8
              .long isr_d9
              .long isr_da
              .long isr_db
              .long isr_dc
              .long isr_dd
              .long isr_de
              .long isr_df
              .long isr_e0
              .long isr_e1
              .long isr_e2
              .long isr_e3
              .long isr_e4
              .long isr_e5
              .long isr_e6
              .long isr_e7
              .long isr_e8
              .long isr_e9
              .long isr_ea
              .long isr_eb
              .long isr_ec
              .long isr_ed
              .long isr_ee
              .long isr_ef
              .long isr_f0
              .long isr_f1
              .long isr_f2
              .long isr_f3
              .long isr_f4
              .long isr_f5
              .long isr_f6
              .long isr_f7
              .long isr_f8
              .long isr_f9
              .long isr_fa
              .long isr_fb
              .long isr_fc
              .long isr_fd
              .long isr_fe
              .long isr_ff
