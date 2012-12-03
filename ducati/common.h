#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef __packed
#define __packed __attribute__((packed))
#endif

#define __u64 unsigned long long
#define __u32 unsigned int
#define __u16 unsigned short

#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8  char


#define write32(ADDR, VAL)    ( *(unsigned int *)ADDR = (unsigned int)(VAL) )
#define read32(ADDR)          ( *(unsigned int *)ADDR )


#endif /* _COMMON_H_ */
