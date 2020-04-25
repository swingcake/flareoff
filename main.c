#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/list.h>
#include <asm/uaccess.h>
#include <linux/udp.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>             
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/netdevice.h>      
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h> 
#include <linux/skbuff.h>         
#include <linux/tcp.h>                    
#include <linux/icmp.h>

#define host1 192.168.10.1
#define host2 192.168.10.2
#define webserver 192.168.10.3

static struct nf_hook_ops netfilter;

unsigned int main(unsigned int hooknum, 
		struct sk_buff *skb, 
		const struct net_device *in, 
		const struct net_device *out, 
		int (*okfn) (struct sk_buff*))
{
	
	struct iphdr *ip_header = (struct iphdr *)skb_network_header(skb);
	
	//Get source and destination IP address in the packet
	unsigned int src_ip = (unsigned int)ip_header->saddr;
	unsigned int dest_ip = (unsigned int)ip_header->daddr;

	//If the packet coming at loopback interface, accept it
	if(strcmp(in->name,"lo") == 0)
		return NF_ACCEPT;

	//Check if the packet is a TCP packet, 6 is the protocol number for TCP
	if (ip_header->protocol == 6) { 
		
		//Using Socket Kernel Buffer to extract transport layer header
		struct tcphdr *tcp_header = (struct tcphdr *)(skb_transport_header(skb));
		
		//Extracting the tansport layer source and destination port
		unsigned int src_port = (unsigned int)ntohs(tcp_header->source);
		unsigned int dest_port = (unsigned int)ntohs(tcp_header->dest);
		
		//Firewall Rule 2: Block all SSH traffic (port 22) from outside the network
		if (dest_port == 22 && (src_ip != host1 || src_ip != host2 || src_ip != webserver)) {
			return NF_DROP; }

		//Firewall Rule 3: Block all HTTP traffic (port 80) from outside, except to the webserver. Local host allowed.
		else if (dest_port == 80 && dest_ip != webserver && (src_ip != host1 || src_ip != host2 || src_ip != webserver)) {
			return NF_DROP; }

		//Accept all other packets
		return NF_ACCEPT; 
	}
	

	//Check if the packet is ICMP packet, 1 is the protocol number for ICMP
	if (ip_header->protocol == 1) {

		//Using Socket Kernel Buffer to extract ICMP header
		struct icmphdr *icmp_header = icmp_hdr(skb);

		//Check if there is an ICMP (ICMP echo) request from outside
		if (icmp_header->type == ICMP_ECHO) {
	
			//Rule 1: Block all ICMP traffic from outside, except to the webserver. Localhost can ping anywhere
			if (dest_ip != webserver && (src_ip != host1 || src_ip != host2 || src_ip != webserver)) {
				return NF_DROP; }
		//Accept all other packets
		return NF_ACCEPT;
		}
	//Accept all other packets
	return NF_ACCEPT; }	

//Accept all other packets
return NF_ACCEPT; }

int init_module() {
	
	//Register the hook using nf_hook_ops structure
	netfilter.hook = main; //Attaching our hook function to register
	netfilter.pf = PF_INET; //Protocol Family, using IPv4
	netfilter.hooknum = NF_INET_PRE_ROUTING; //Apply our hook function at pre-routing chain
	netfilter.priority = NF_IP_PRI_FIRST; //Giving our hook function the highest priority when all the functions in the chain ar executed
	//Register the hook
	nf_register_hook(&netfilter);

	return 0;
}

void cleanup_module(){
	
	//for un-registering the hook
	nf_unregister_hook(&netfilter);
}
