              .data
              .align 0x1000
              .global kernel_pagedir
kernel_pagedir:
              //    0x00000000 -> 0x00000000 (rw)
              .long 0x00000083

              .skip 767 * 4, 0

              //    0xC0000000 -> 0x00000000 (rw)
              .long 0x00000083
              //    0xC0400000 -> 0x00400000 (rw)
              .long 0x00400083

              .skip 254 * 4, 0

              .size kernel_pagedir, . - kernel_pagedir
