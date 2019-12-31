#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <errno.h>
#include <stdio.h>

int main()
{
    struct ifaddrs *interfaceArray = NULL, *tempIfAddr = NULL;
    void *tempAddrPtr = NULL;
    int rc = 0;
    char addressOutputBuffer[INET6_ADDRSTRLEN];

    rc = getifaddrs(&interfaceArray);  /* retrieve the current interfaces */
    if (rc == 0)
    {    
        for(tempIfAddr = interfaceArray; tempIfAddr != NULL; tempIfAddr = tempIfAddr->ifa_next)
        {
            if(tempIfAddr->ifa_addr->sa_family == AF_INET)
                tempAddrPtr = &((struct sockaddr_in *)tempIfAddr->ifa_addr)->sin_addr;
            else
                tempAddrPtr = &((struct sockaddr_in6 *)tempIfAddr->ifa_addr)->sin6_addr;

            printf("Internet Address:  %s \n",
                    inet_ntop(tempIfAddr->ifa_addr->sa_family,
                        tempAddrPtr,
                        addressOutputBuffer,
                        sizeof(addressOutputBuffer)));

            printf("LineDescription :  %s \n", tempIfAddr->ifa_name);
            if(tempIfAddr->ifa_netmask != NULL)
            {
                if(tempIfAddr->ifa_netmask->sa_family == AF_INET)
                    tempAddrPtr = &((struct sockaddr_in *)tempIfAddr->ifa_netmask)->sin_addr;
                else
                    tempAddrPtr = &((struct sockaddr_in6 *)tempIfAddr->ifa_netmask)->sin6_addr;

                printf("Netmask         :  %s \n",
                        inet_ntop(tempIfAddr->ifa_netmask->sa_family,
                            tempAddrPtr,
                            addressOutputBuffer,
                            sizeof(addressOutputBuffer)));
            }
            if(tempIfAddr->ifa_ifu.ifu_broadaddr != NULL)
            {
                /* If the ifa_flags field indicates that this is a P2P interface */
                if(tempIfAddr->ifa_flags & IFF_POINTOPOINT)
                {
                    printf("Destination Addr:  ");
                    if(tempIfAddr->ifa_ifu.ifu_dstaddr->sa_family == AF_INET)
                        tempAddrPtr = &((struct sockaddr_in *)tempIfAddr->ifa_ifu.ifu_dstaddr)->sin_addr;
                    else
                        tempAddrPtr = &((struct sockaddr_in6 *)tempIfAddr->ifa_ifu.ifu_dstaddr)->sin6_addr;
                }
                else
                {
                    printf("Broadcast Addr  :  ");
                    if(tempIfAddr->ifa_ifu.ifu_broadaddr->sa_family == AF_INET)
                        tempAddrPtr = &((struct sockaddr_in *)tempIfAddr->ifa_ifu.ifu_broadaddr)->sin_addr;
                    else
                        tempAddrPtr = &((struct sockaddr_in6 *)tempIfAddr->ifa_ifu.ifu_broadaddr)->sin6_addr;
                }

                printf("%s \n",
                        inet_ntop(tempIfAddr->ifa_ifu.ifu_broadaddr->sa_family,
                            tempAddrPtr,
                            addressOutputBuffer,
                            sizeof(addressOutputBuffer)));
            }
            printf("\n");
        }

        freeifaddrs(interfaceArray);             /* free the dynamic memory */
        interfaceArray = NULL;                   /* prevent use after free  */
    }
    else
    {
        printf("getifaddrs() failed with errno =  %d %s \n",
                errno, strerror(errno));
        return rc;
    }
}
