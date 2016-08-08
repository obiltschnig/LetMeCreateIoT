#include <stdio.h>

#include <contiki.h>
#include <contiki-net.h>


#define SERVER_IP_ADDR "fe80:0000:0000:0000:28e9:3285:421c:bc82"

#define SERVER_PORT 3000
#define CLIENT_PORT 3001

#define BUFFER_SIZE 64

PROCESS(main_process, "Main process");
AUTOSTART_PROCESSES(&main_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(main_process, ev, data)
{
    PROCESS_BEGIN();

    {
        // Due to the way Contiki protothreads work this needs to be static,
        // otherwise the data will be lost when switching to a different thread
        static struct uip_udp_conn * conn;
        static char buffer[BUFFER_SIZE];
        static int i = 0;
        printf("===START===\n");

        // Below steps can be avoided by using the udp_new_connection function from core/network.h
        // First convert the IP address into a structure
        uip_ipaddr_t addr;
        uiplib_ipaddrconv(SERVER_IP_ADDR, &addr);

        // Create a new udp connection
        conn = udp_new(&addr, UIP_HTONS(SERVER_PORT), NULL);

        // Bind the connection locally to a port. For the connection to work both server & client
        // need to call their respective ports in udp_new & udp_bind
        udp_bind(conn, UIP_HTONS(CLIENT_PORT));

        while(1)
        {
            i++;
            sprintf(buffer, "Hello number %i from client", i);

            printf("Sending data: %s\n", buffer);
            uip_udp_packet_send(conn, buffer, strlen(buffer));

            // We need to poll tcpip and let it fire an event once data has been sent.
            tcpip_poll_udp(conn);
            PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
        }

    }

    PROCESS_END();
}

/*---------------------------------------------------------------------------*/