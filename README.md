# VNI

- Virtual Network Interface.

## What is this?

- Create very simple network interface kernel module, a transmitter that transfer raw socket layer 2, and a receiver that capture all packet on the interface.

- Build & install the module:

```bash
make && sudo insmod vni.ko && sudo ifconfig vni0 up
```

- Run the layer 2 socket packet receiver:

```bash
sudo ./receiver.o vni0
```

- Send raw packet layer 2:

```bash
sudo ./transmitter.o vni0
```
