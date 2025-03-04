#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_MSGSIZE 1024
#include "vtuner-utils.h"

__thread char msg[MAX_MSGSIZE];

static struct sockaddr_in udplog_saddr;
static int udplog_fd = -1;
static int udplog_enabled = 0;

void write_message(const unsigned int mtype, const int level, const char* fmt, ... ) {
  if( !(mtype & dbg_mask ) )
    return;

  if( level <= dbg_level ) {

    char tn[MAX_MSGSIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(tn, sizeof(tn), fmt, ap);
    va_end(ap);
    strncat(msg, tn, MAX_MSGSIZE - strlen(msg) - 1);

    if(use_syslog) {
      int priority;
      switch(level) {
        case 1: priority=LOG_ERR; break;
        case 2: priority=LOG_WARNING; break;
        case 3: priority=LOG_INFO; break;
        default: priority=LOG_DEBUG; break;
      }
      syslog(priority, "%s", msg);
    } else {
      fprintf(stderr, "%s", msg);
    }

    if(udplog_fd > -1 && udplog_enabled)
      sendto(udplog_fd, msg, strlen(msg), 0, (const struct sockaddr *)&udplog_saddr, sizeof(udplog_saddr));
  }

  strncpy(msg, "", sizeof(msg));
}

void init_message(const char* fmt, ... ) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);
}

void append_message(const char* fmt, ... ) {
  char tn[MAX_MSGSIZE];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(tn, sizeof(tn), fmt, ap);
  va_end(ap);

  strncat(msg, tn, sizeof(msg) - strlen(msg) - 1);
}

int open_udplog(char *ipaddr, int portnum) {

  if(udplog_fd != -1)
    return 0;

  udplog_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  memset(&udplog_saddr, 0, sizeof(udplog_saddr));
  udplog_saddr.sin_family = AF_INET;
  inet_aton(ipaddr, &udplog_saddr.sin_addr);
  udplog_saddr.sin_port = htons(portnum);

  return 0;
}

void udplog_enable(int ena) {
  udplog_enabled = ena;
}
