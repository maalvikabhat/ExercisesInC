#include "trout.h"

/* variables we might want to configure */
Rec *rec = (Rec *) sendbuf;

void sig_alrm (int signo)
{
  Write (pipefd[1], "", 1);  /* write 1 null byte to pipe */
  return;
}

int process_ip (struct ip *ip, int len)
{
  int hlen1, hlen2, icmplen;
  struct icmp *icmp;
  struct ip *hip;
  struct udphdr *udp;
  seq = 0;
  u_short dport = 32768 + 668;        /* destination port -- hopefully unused */

  hlen1 = ip->ip_hl << 2;                        /* length of IP header */
  icmp = (struct icmp *) (recvbuf + hlen1);
  icmplen = len - hlen1;

  if (icmplen < 8 + 20 + 8) return 0;

  if (icmp->icmp_type != ICMP_TIME_EXCEEDED &&
      icmp->icmp_type != ICMP_DEST_UNREACH)
    return 0;

  /* hip is the header of the enclosed IP packets, supposedly
     the header of the packet that caused the error */

  hip = (struct ip *) (recvbuf + hlen1 + 8);
  if (hip->ip_p != IPPROTO_UDP) return 0;

  hlen2 = hip->ip_hl << 2;
  udp = (struct udphdr *) (recvbuf + hlen1 + 8 + hlen2);

  if (udp->source != htons (sport)) return 0;
  if (udp->dest != htons (dport + seq)) return 0;

  if (icmp->icmp_type == ICMP_TIME_EXCEEDED) {
    if (icmp->icmp_code == ICMP_EXC_TTL) {
      return -2;
    } else {
      return 0;
    }
  }

  if (icmp->icmp_type == ICMP_DEST_UNREACH) {
    if (icmp->icmp_code == ICMP_PORT_UNREACH) {
      return -1;
    } else {
      return 0;
    }
  }
  return 0;
}



int recv_dgram ()
{
  int err;
  socklen_t len;
  ssize_t n;
  struct ip *ip;
  int maxfdp1 = max (recvfd, pipefd[0]) + 1;
  fd_set rset[1];  
  FD_ZERO (rset);

  alarm(3);       /* set the timeout alarm to handle dropped packets */

  while (1) {
    FD_SET (recvfd, rset);
    FD_SET (pipefd[0], rset);

    n = select (maxfdp1, rset, NULL, NULL, NULL);
    if (n < 0 && errno != EINTR) {
      err_sys ("select error");
    }

    if (FD_ISSET (recvfd, rset)) {
      len = salen;
      n = recvfrom (recvfd, recvbuf, sizeof(recvbuf), 0, sarecv, &len);
      err = errno;
      Gettimeofday (recvtv, NULL);   /* get time of packet arrival */
      if (n < 0 && err != EAGAIN) {
	err_sys ("recvfrom error");
      }
    }

    if (FD_ISSET (pipefd[0], rset)) {
      Read (pipefd[0], &n, 1);
      return -3;                 /* timeout */
    }

    ip = (struct ip *) recvbuf;
    return process_ip (ip, n);
  }
}

/* sub_tv: subtract minus from plus and put the result in res */

void sub_tv (Timeval *plus, Timeval *minus, Timeval *res)
{
  res->tv_sec = plus->tv_sec - minus->tv_sec;
  res->tv_usec = plus->tv_usec - minus->tv_usec;

  if (res->tv_usec < 0) {
    res->tv_sec--;
    res->tv_usec += 1000000;
  }
}

/* time_to_double: convert a Timeval to a double.  This only
   works with Timevals that are small (like the difference between
   two real Timevals) */

double time_to_double (Timeval *time)
{
  return time->tv_sec * 1000.0 + time->tv_usec / 1000.0;
}

/* print_report: prints all the information about a successful round trip */

void print_report ()
{
  int stat;
  char str[NI_MAXHOST];
      
  stat = sock_cmp_addr (sarecv, salast, salen);

  /* if this reply comes from source different from the previous
     one, print the full host information */

  if (stat != 0) {
    stat = getnameinfo (sarecv, salen, str, sizeof(str), NULL, 0, 0);
    if (stat == 0) {
      printf (" %s (%s)", str, Sock_ntop_host (sarecv, salen));
    } else {
      printf (" %s", Sock_ntop_host (sarecv, salen));
    }
    memcpy (salast, sarecv, salen);
  }

  /* calculate and print the round trip time using user-level timestamps */

  sub_tv (recvtv, sendtv, difftv);

  printf ("  %.3f", time_to_double (difftv));
}

/* send_dgram: generate an outgoing UDP packet */

    /* the second effort send is a kludge to handle a funny
       thing, which is that the last host seems to refuse the
       second or third connection consistently, which might
       might mean that something breaks when we get the
       ICMP_DEST_UNREACH error.  The second attempt seems
       to succeed consistently. */

void send_dgram (int ttl)
{
  int n;
  datalen = sizeof (Rec);         /* length of the data in a datagram */
  seq = 0;
  u_short dport = 32768 + 668;        /* destination port -- hopefully unused */

  rec->seq = seq++;
  sock_set_port (sasend, salen, htons(dport+seq));

  Gettimeofday (sendtv, NULL);
  n = sendto(sendfd, sendbuf, datalen, 0, sasend, salen);

  if (n==-1 && errno == ECONNREFUSED) {
    Gettimeofday (sendtv, NULL);
    n = sendto(sendfd, sendbuf, datalen, 0, sasend, salen);
  }

  if (n != datalen) {
    err_sys("sendto error");
  }
}

/* send_probes: sends a set of probes with the given ttl and
   then waits for the replies.  The weird TOS options are there
   as a signal to the kernel to identify clink packets so it can
   fill in the timestamps.  I am assuming that they don't have
   any actual effect. */

int send_probes (int ttl) 
{
  int probe, code, done;

  Setsockopt (sendfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
  bzero (salast, salen);
  int nprobes = 2;

  printf ("%2d  ", ttl);
  fflush (stdout);

  done = 0; /* count the number of probes that generate an ICMP_DEST_UNREACH */

  for (probe = 0; probe < nprobes; probe++) {
    send_dgram (ttl);
    code = recv_dgram ();

    if (code == -3) {
      printf (" *");
    } else {
      print_report ();
    }

    if (code == -1) done++;
    fflush (stdout);
  }
  printf ("ms\n");
  return done;
}

/* loop_ttl: starting with ttl=1, gradually increase ttl until
   we start getting ICMP_DEST_UNREACH instead of ICMP_TIME_EXCEEDED */

void loop_ttl ()
{
  int ttl, done;

  Pipe (pipefd);     /* the pipe for the alarm handler */
  max_ttl = 30;


  recvfd = socket (sasend->sa_family, SOCK_RAW, IPPROTO_ICMP);
  if (recvfd == -1) {
    if (errno == EPERM) {
      printf ("\nclink was unable to open a raw socket.  The most\n");
      printf ("likely cause is that you are not running it as root.\n");
      exit (1);
    } else {
      err_sys ("opening raw socket in clink");
    }
  }

  fcntl (recvfd, F_SETFL, O_NONBLOCK);
  setuid (getuid ());

  sendfd = socket (sasend->sa_family, SOCK_DGRAM, 0);

  sabind->sa_family = sasend->sa_family;
  sport = (getpid() & 0xffff) | 0x8000;       /* source UDP port # */
  sock_set_port (sabind, salen, htons(sport));
  Bind (sendfd, sabind, salen);

  Signal (SIGALRM, sig_alrm);

  for (ttl = 1; ttl <= max_ttl; ttl++) {
    done = send_probes (ttl);
    if (done > 0) break;
  }
}
