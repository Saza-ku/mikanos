#include "arp.hpp"

#include "ethernet.hpp"
#include "mbuf.hpp"
#include "net_util.hpp"
#include "logger.hpp"
#include "net/nic/e1000.hpp"

namespace {
  #define ARP_TABLE_LENGTH 16
  net::arp_entry arp_table[ARP_TABLE_LENGTH];

  net::arp_entry *arp_lookup(ipaddr_t ip) {
    for (int i = 0; i < ARP_TABLE_LENGTH; i++) {
      net::arp_entry *e = &arp_table[i];
      if (e->in_use &&
          net::same_ipaddr(ip, e->ip)) {
        return e;
      }
    }
    return nullptr;
  }

    // TODO: 古いものから更新していくように最適化したい
  net::arp_entry *arp_add(ipaddr_t ip) {
    static int index = 0;

    net::arp_entry e;
    e.in_use = true;
    net::ipaddr_copy(e.ip, ip);
    // 同じ IP のエントリを置き換え
    for (int i = 0; i < ARP_TABLE_LENGTH; i++) {
      if (arp_table[i].in_use &&
          net::same_ipaddr(arp_table[i].ip, e.ip)) {
        arp_table[i] = e;
        return &arp_table[i];
      }
    }

    // 新しくエントリを追加
    arp_table[index] = e;
    int prev_index = index;
    index = (index + 1) % ARP_TABLE_LENGTH;
    return &arp_table[prev_index];
  }

  void receive_arp_reply(ipaddr_t ip, macaddr_t mac) {
    net::arp_entry *e = arp_lookup(ip);
    if (e) {
      e->resolved = true;
      net::macaddr_copy(e->mac, mac);
      e->send_packets();
    }
    else {
      Log(kError, "ARP: Received unknown reply\n");
    }
  }
  void receive_arp_request(ipaddr_t dest_ip, ipaddr_t ip, macaddr_t mac) {
    if (!net::same_ipaddr(dest_ip, net::e1000::nic->ipaddr)) {
      return;
    }

    net::arp_packet payload;
    net::ipaddr_copy(payload.dest_ip, ip);
    net::macaddr_copy(payload.dest_mac, mac);
    net::ipaddr_copy(payload.src_ip, net::e1000::nic->ipaddr);
    net::macaddr_copy(payload.src_mac, net::e1000::nic->macaddr);
    payload.opcode = net::hton16(2);

    net::mbuf *payload_mbuf = new net::mbuf(&payload, sizeof(payload));

    net::ethernet_header header;
    net::macaddr_copy(header.dest_address, mac);
    net::macaddr_copy(header.src_address, net::e1000::nic->macaddr);
    header.type = net::hton16(TYPE_ARP);

    net::mbuf *header_mbuf = new net::mbuf(&header, sizeof(header));
    header_mbuf->append(payload_mbuf);

    net::e1000::nic->Send(header_mbuf);
  }
}

namespace net {
  void send_arp(ipaddr_t dest_ip) {
    arp_packet payload;
    ipaddr_copy(payload.src_ip, e1000::nic->ipaddr);
    macaddr_copy(payload.src_mac, e1000::nic->macaddr);
    ipaddr_copy(payload.dest_ip, dest_ip);
    // TODO: ハードコーディングやめたい
    payload.opcode = hton16(1);

    mbuf *payload_mbuf = new mbuf(&payload, sizeof(payload));

    ethernet_header header;
    macaddr_copy(header.dest_address, MACADDR_BROADCAST);
    macaddr_copy(header.src_address, e1000::nic->macaddr);
    header.type = hton16(TYPE_ARP);

    mbuf *header_mbuf = new mbuf(&header, sizeof(header));
    header_mbuf->append(payload_mbuf);

    e1000::nic->Send(header_mbuf);
  }

  void receive_arp(mbuf *mbuf) {
    arp_packet packet;
    size_t len = mbuf->read(&packet, sizeof(packet));
    if (len != sizeof(packet)) {
      Log(kError, "ARP: Received invalid packet\n");
    }

    switch(ntoh16(packet.opcode)) {
      case 1:
        Log(kError, "ARP: Received request\n");
        receive_arp_request(packet.dest_ip, packet.src_ip, packet.src_mac);
        break;
      case 2:
        Log(kError, "ARP: Received reply\n");
        Log(kError, "%d.%d.%d.%d - %x:%x:%x:%x:%x:%x\n",
            packet.src_ip[0],
            packet.src_ip[1],
            packet.src_ip[2],
            packet.src_ip[3],
            packet.src_mac[0],
            packet.src_mac[1],
            packet.src_mac[2],
            packet.src_mac[3],
            packet.src_mac[4],
            packet.src_mac[5]
        );
        receive_arp_reply(packet.src_ip, packet.src_mac);
        break;
      default:
        Log(kError, "ARP: Received unknown packet\n");
        break;
    }
  }

  bool arp_resolve(ipaddr_t ip, macaddr_t mac) {
    if (same_ipaddr(ip, IPV4_ADDR_BROADCAST)) {
      macaddr_copy(mac, MACADDR_BROADCAST);
      return true;
    }

    arp_entry *e = arp_lookup(ip);
    if (!e || !e->resolved) {
      return false;
    }

    macaddr_copy(mac, e->mac);
    return true;
  }

  void arp_entry::send_packets() {
    net::mbuf_list *node = packets_ptr;
    while(node) {
      send_ethernet(node->buf, node->packet_type, ip);
      net::mbuf_list *old_node = node;
      node = node->next;
      delete old_node;
    }
  }

  void arp_entry::add_packet(mbuf *packet, uint16_t type) {
    // packets_ptr で持っているリストに prepend する
    mbuf_list *new_node = new mbuf_list();
    new_node->buf = packet;
    new_node->packet_type = type;
    new_node->next = packets_ptr;
    packets_ptr = new_node;
  }

  void arp_enqueue(mbuf *packet, uint16_t type, ipaddr_t ip) {
    arp_entry *e = arp_lookup(ip);
    if (!e) {
      e = arp_add(ip);
    }
    e->add_packet(packet, type);
  }
}
