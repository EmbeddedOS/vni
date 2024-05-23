from socket import *
def send_raw(src, dst, eth_type, payload, interface = "vni0"):
  assert(len(src) == len(dst) == 6) # 48-bit ethernet addresses
  assert(len(eth_type) == 2) # 16-bit ethernet type
  s = socket(AF_PACKET, SOCK_RAW)
  s.bind((interface, 0))
  return s.send(src + dst + eth_type + payload)

if __name__ == "__main__":
  print("Sent %d-byte Ethernet packet on eth0" %
  send_raw("\xFE\xED\xFA\xCE\xBE\xEF",
          "\xFE\xED\xFA\xCE\xBE\xEF",
          "\x7A\x05",
          "hello"))