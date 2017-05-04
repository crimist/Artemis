#include "../include/main.h"

/* Just to be 100% sure that we don't accidently put it in a proper build */
#ifdef DEBUG

void _printd(const char *file, const char *func, const int line, const char *fmt, ...)
{
	printf("[%s->%s:%d] ", func, file, line);

	va_list args;
	va_start(args, fmt);
	int ret = vfprintf(stdout, fmt, args);
	va_end(args);

	printf("\n");

	if (ret < 0)
	{
		fprintf(stderr, "Print Debug Failed!\nExiting...\n");
		exit(EXIT_FAILURE);
	}
	return;
}

void _pktd(const char *file, const char *func, const int line, struct iphdr *iph, struct tcphdr *tcph)
{
	printf("[%s->%s:%d] ", func, file, line);
	printf(
		"Dupming packet contense:\n"
		"IP Header:\n"
		"\tihl = %d\n"
		"\tversion = %d\n"
		"\ttos = %d\n"
		"\ttot_len = %d\n"
		"\tid = %d\n"
		"\tfrag_off = %d\n"
		"\tttl = %d\n"
		"\tprotocol = %d\n"
		"TCP Header:\n"
		"\tsource = %s\n"
		"\tseq = %d\n"
		"\tack_seq = %d\n"
		"\tdoff = %d\n"
		"\twindow = %d\n"
		"\turg_ptr = %d\n"
		"\tdest = %s\n",
		iph->ihl,
		iph->version,
		iph->tos,
		iph->tot_len,
		iph->id,
		iph->frag_off,
		iph->ttl,
		iph->protocol,
		ipv4_unpack(tcph->source),
		tcph->seq,
		tcph->ack_seq,
		tcph->doff,
		tcph->window,
		tcph->urg_ptr,
		ipv4_unpack(tcph->dest));

	printf("Flags = ");
	if (tcph->fin)
		printf("FIN ");
	if (tcph->syn)
		printf("SYN ");
	if (tcph->rst)
		printf("RST ");
	if (tcph->psh)
		printf("PSH ");
	if (tcph->ack)
		printf("ACK ");
	if (tcph->urg)
		printf("URG ");

	printf("\n#END#\n");
}

#endif /* DEBUG */
