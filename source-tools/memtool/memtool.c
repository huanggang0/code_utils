#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define PAGE_SIZE  4096UL
#define PAGE_MASK  (PAGE_SIZE - 1)
#define MODE_READ	1
#define MODE_WRITE	2
#define MIN_BYTES	64

static void memdev_usage(void){
    printf("\n");
    printf("--------usage----------\n");
    printf("memtool r addr\n");
    printf("memtool r addr length\n");
    printf("memtool w addr value\n");
    printf("memtool w addr value length\n");
    printf("\n");
}

static int memdev_parse_arg(char *arg){
    int ret;
    ret = strtoul(arg, 0, 0);
    if(errno < 0){
        memdev_usage();
        exit(0);
    }
    return ret;
}

static void memdev_read(int phy_addr,char *virt_addr,int length){
    int i = 0;
    for (i=0;i < length; i+=4)
    {
        if((i % 16) == 0){
            printf("0x%08x  :  ",(unsigned int)(phy_addr) + i);
        }
        printf("0x%08x ",*(unsigned int *)(virt_addr+i));

        if(((i+4) % 16) == 0)
            printf("\n");
    }
    printf("\n");
}

static void memdev_write(char *addr,int value,int length){
    int i = 0;
    for (i=0;i < length; i+=4){
        *(volatile unsigned int *)(addr+i) = value;
    }
    //printf("write virt address: 0x%08x value:0x%08x len:0x%08x OK!\n",addr,value,length);
}

int main(int argc,char *argv[])
{
    unsigned char *virt_align,*virt_addr;
    unsigned int phy_align,phy_addr;
    unsigned int len_read,len_align = PAGE_SIZE,len_write;
    unsigned int mode = MODE_READ,i,value;
    int fd,ret;
    if(argc < 3 || argc > 5){
        memdev_usage();
        return -1;
    }

    if (strcmp(argv[1], "w") == 0){
        mode = MODE_WRITE;
    }else if(strcmp(argv[1], "r") == 0){
        mode = MODE_READ;
    }else{
        memdev_usage();
        return -1;
    }
    fd = open("/dev/mem", O_RDWR|O_SYNC);
    if (fd == -1){
        printf("memtool : open /dev/mem failed!\n");
        return (-1);
    }
    phy_addr = memdev_parse_arg(argv[2]);
    phy_align = phy_addr & (~PAGE_MASK);
    /* READ */
    if(mode == MODE_READ){
        if(argc == 3){
            len_read = MIN_BYTES;
            len_align = PAGE_SIZE;
        }else if(argc == 4){
            len_read  = memdev_parse_arg(argv[3]);
            len_align = (len_read + PAGE_SIZE - 1) & (~PAGE_MASK);
        }else{
            memdev_usage();
            return 0;
        }
        /* WRITE */
    }else if(mode == MODE_WRITE){
        if(argc == 4){
			value = memdev_parse_arg(argv[3]);
            len_write = 4;
            len_align = PAGE_SIZE;
        }else if(argc == 5){
			value = memdev_parse_arg(argv[3]);
            len_write = memdev_parse_arg(argv[4]);
            len_align = (len_write + PAGE_SIZE - 1) & (~PAGE_MASK);
        }else{
            memdev_usage();
            return 0;
        }
    }
    /* page-aligned virt address */	
    virt_align = mmap(NULL, len_align, PROT_READ|PROT_WRITE, MAP_SHARED, fd, phy_align);

    if(virt_align == (void *) -1){
        printf("memtool : mmap NULL pointer!\n");
        return -1;
    }
    else{
        /* user virt address */
        virt_addr = virt_align + (phy_addr & PAGE_MASK);
    }

    if(mode == MODE_READ){
        memdev_read(phy_addr,virt_addr,len_read);

    }if(mode == MODE_WRITE){
        memdev_write(virt_addr,value,len_write);
        memdev_read(phy_addr,virt_addr,MIN_BYTES);
    }

    close(fd);
    munmap(virt_align,len_align);
    return (1);
}